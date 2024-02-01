 #include "Thjoy.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
 /*----------------HILO JOYSTICK----------------------------------------------*/
static osThreadId_t tid_Thjoy; // id del hilo del joystick 
typedef struct{
	GPIO_InitTypeDef gpio;
	GPIO_TypeDef *port;
} mygpio; //estructura para codificar los pines asociados al joystick
static mygpio joystickAD; //estructura del joystick arriba derecha
static mygpio joystickIBC; //estructura del joystick izquierda abajo centro
 
typedef struct{
	mygpio ArribaDerecha;
	mygpio IzquierdaBajoCentro;
}joystick_t;
static joystick_t joystick; //guarda todas las configuraciones de los joystick
static uint16_t gesto;

int Init_Thjoy (void) {
	
	//Configura los pines de los joysticks
  InitJoy();
	Init_Timer();
  
	//Crea el hilo del joystick
  tid_Thjoy = osThreadNew(Thjoy, (void *)&joystick, NULL);
  if (tid_Thjoy == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thjoy (void *argument) {
	
	//Inicializa los pines de los joysticks
  joystick_t *joy = (joystick_t *)argument;
	HAL_GPIO_Init(joy->ArribaDerecha.port, &(joy->ArribaDerecha.gpio) );
	HAL_GPIO_Init(joy->IzquierdaBajoCentro.port, &(joy->IzquierdaBajoCentro.gpio) );
 
	//Activa las interrupciones de los joysticks
  HAL_NVIC_EnableIRQ( EXTI15_10_IRQn );
	
  while (1) {
    // Espera a que se pulse uno de los joystick
		osThreadFlagsWait(S_REB, osFlagsWaitAny ,osWaitForever);
		//Se inicia el timer de los rebotes
		osTimerStart(tim_id1,50U);
		
    osThreadYield();                            // suspend thread
  }
}

void InitJoy(void){
  __HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
  
	
	//Configuración de los pines
	joystickAD.gpio.Mode = GPIO_MODE_IT_RISING;
	joystickAD.gpio.Pull = GPIO_PULLDOWN;
	joystickAD.gpio.Pin = GPIO_PIN_10 | GPIO_PIN_11 ;
	
	joystickIBC.gpio.Mode = GPIO_MODE_IT_RISING;
	joystickIBC.gpio.Pull = GPIO_PULLDOWN;
	joystickIBC.gpio.Pin = GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15;
	
	joystickAD.port = GPIOB;
	joystickIBC.port = GPIOE;
	
	joystick.ArribaDerecha = joystickAD;
	joystick.IzquierdaBajoCentro = joystickIBC;
}

void EXTI15_10_IRQHandler(void){
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	//Activa el flag para gestionar los rebotes
	osThreadFlagsSet(tid_Thjoy, S_REB);
	
	gesto = GPIO_Pin;
	
}



/*----------------TIMER----------------------------------------------*/
osTimerId_t tim_id1; // timer id
static uint32_t contador=0; //veces que se pulsan los botones del joystick

// One-Shoot Timer Function
void Timer1_Callback (void const *arg) {
  // Suma las veces que se pulsa el joystick
	contador++;
	//Genera el codigo
	generarCodigo( *(uint16_t *)arg );
}

int Init_Timer (void) {
 
  // Create one-shoot timer
  tim_id1 = osTimerNew((osTimerFunc_t)&Timer1_Callback, osTimerOnce, &gesto, NULL);
  if (tim_id1 != NULL) {  // One-shot timer created 
    return -1;
  }
	
  return NULL;
}

/**
	codigo 1: LED VERDE->APG; LED AZUL->APG; LED ROJO->APG
  codigo 2: LED VERDE->ENC; LED AZUL->APG; LED ROJO->APG
  codigo 3: LED VERDE->APG; LED AZUL->ENC; LED ROJO->APG
  codigo 4: LED VERDE->APG; LED AZUL->APG; LED ROJO->ENC
  codigo 5: LED VERDE->ENC; LED AZUL->ENC; LED ROJO->ENC
*/
void generarCodigo(uint16_t pin){
  
	uint8_t mensaje;
	
	switch(pin){
		case GPIO_PIN_10:
			mensaje = 0b00001U;
		break;
		case GPIO_PIN_11:
			mensaje = 0b00010U;
		break;
		case GPIO_PIN_12:
			mensaje = 0b00100U;
		break;
		case GPIO_PIN_14:
			mensaje = 0b01000U;
		break;
		case GPIO_PIN_15:
			mensaje = 0b10000U;
		break;
	}
	
	osMessageQueuePut(mid_MsgQueue, &mensaje, 0U, 0U);
	//osThreadFlagsSet(tid_Thread_MsgQueue,S_MES);
}
