#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include "Joystick.h"
 
extern osThreadId_t tid_Dibujar;
extern osThreadId_t tid_Rebotes;
extern osMessageQueueId_t mid_MsgQueue;
extern osTimerId_t tim_id;
extern osTimerId_t timeout_id;
osTimerId_t tim_id1; // timer id
static uint32_t contador=0; //veces que se pulsan los botones del joystick
static osThreadId_t tid_Joystick; // id del hilo del joystick 
typedef struct{
	GPIO_InitTypeDef gpio;
	GPIO_TypeDef *port;
} mygpio; //estructura para codificar los pines asociados al joystick
static mygpio joystickAD; //estructura del joystick arriba derecha
static mygpio joystickIBC; //estructura del joystick izquierda abajo centro
static mygpio boton;
 
typedef struct{
	mygpio ArribaDerecha;
	mygpio IzquierdaBajoCentro;
	mygpio boton;
}joystick_t;
static joystick_t joystick; //guarda todas las configuraciones de los joystick
static uint16_t gesto;

void Joystick (void *argument); // funcion del hilo
void EXTI15_10IRQHandler(void); //funcion de la interrupcion del joystick derecho
void InitJoy(void); //funcion para configurar todos los joysticks
void EXTI15_10_IRQHandler(void); //funcion asociada a la interrupcion del joystick derecho
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin); //funcion donde se almacena el codigo de la interrupcion del joystick derecho
int Init_Timer (void); //funcion que crea el timer para manejar los rebotes
void Timer_Callback (void const *arg);
void generarCodigo(uint16_t pin);

 /*----------------HILO JOYSTICK----------------------------------------------*/

int Init_Joystick (void) {
	
	//Crea el hilo del joystick
  tid_Joystick = osThreadNew(Joystick, (void *)&joystick, NULL);
  if (tid_Joystick == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Joystick (void *argument) {
	uint8_t msg;
	
	InitJoy();
	Init_Timer();
	
	//Inicializa los pines de los joysticks
  joystick_t *joy = (joystick_t *)argument;
	HAL_GPIO_Init(joy->ArribaDerecha.port, &(joy->ArribaDerecha.gpio) );
	HAL_GPIO_Init(joy->IzquierdaBajoCentro.port, &(joy->IzquierdaBajoCentro.gpio) );
	HAL_GPIO_Init(joy->boton.port, &(joy->boton.gpio) );
 
	//Activa las interrupciones de los joysticks
  HAL_NVIC_EnableIRQ( EXTI15_10_IRQn );
	
  while (1) {
    // Espera a que se pulse uno de los joystick
		osThreadFlagsWait(S_REB, osFlagsWaitAny ,osWaitForever);
		
		if(gesto == GPIO_PIN_13)
		{
			osTimerStart(tim_id, 200U);
			osTimerStart(timeout_id, 5000U);
		}
		
    osThreadYield();                            // suspend thread
  }
}

void InitJoy(void){
  __HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
  
	
	//Configuraci n de los pines
	joystickAD.gpio.Mode = GPIO_MODE_IT_RISING;
	joystickAD.gpio.Pull = GPIO_PULLDOWN;
	joystickAD.gpio.Pin = GPIO_PIN_10 | GPIO_PIN_11 ;
	
	joystickIBC.gpio.Mode = GPIO_MODE_IT_RISING;
	joystickIBC.gpio.Pull = GPIO_PULLDOWN;
	joystickIBC.gpio.Pin = GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15;
	
	boton.gpio.Mode = GPIO_MODE_IT_RISING;
	boton.gpio.Pull = GPIO_NOPULL;
	boton.gpio.Pin = GPIO_PIN_13;
	
	boton.port = GPIOC;
	
	joystickAD.port = GPIOB;
	joystickIBC.port = GPIOE;
	
	joystick.ArribaDerecha = joystickAD;
	joystick.IzquierdaBajoCentro = joystickIBC;
	joystick.boton = boton;
}

void EXTI15_10_IRQHandler(void){
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	osThreadFlagsSet(tid_Rebotes, S_REB);

	gesto = GPIO_Pin;
}



/*----------------TIMER----------------------------------------------*/

// One-Shoot Timer Function
void Timer_Callback (void const *arg) {
	osThreadFlagsSet(tid_Joystick, S_REB);

}

int Init_Timer (void) {
 
  // Create one-shoot timer
  tim_id1 = osTimerNew((osTimerFunc_t)&Timer_Callback, osTimerOnce, NULL, NULL);
  if (tim_id1 != NULL) {  // One-shot timer created 
    return -1;
  }
	
  return NULL;
}
