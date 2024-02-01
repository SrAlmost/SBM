#include "joystick.h"
#include "stdio.h"
#include <string.h>

 /*----------------VARIABLES JOYSTICK----------------------------------------------*/
typedef struct{
	GPIO_InitTypeDef gpio;
	GPIO_TypeDef *port;
} mygpio; //estructura para codificar los pines asociados al joystick

typedef struct{
	mygpio ArribaDerecha;
	mygpio IzquierdaBajoCentro;
}joystick_t; //estructura para codificar todo el conjunto de pines

osThreadId_t tid_Thjoy; // id del hilo del joystick 
static mygpio joystickAD; //estructura del joystick arriba/derecha
static mygpio joystickIBC; //estructura del joystick izquierda/abajo/centro 
static joystick_t joystick; //estructura que guarda todas las configuraciones de los joystick
static mygpio gesto; //joystick pulsado

/*----------------VARIABLES TIMERS----------------------------------------------*/
static informacion_t informacion; //estructura que guarda el tipo de pulsacion y la accion
static osTimerId_t tim_id1; // id del timer1
static osTimerId_t tim_id2; // id del timer2

/*--------------FUNCIONES JOYSTICK----------------------------------------------*/
static void Thjoy (void *argument); // funcion del hilo
static void InitJoy(void); //funcion para configurar todos los joysticks
void EXTI15_10_IRQHandler(void); //funcion asociada a la interrupciones del joystick

/*--------------FUNCIONES TIMERS-----------------------------------------------*/
static void Timer1_Callback (void *arg); //funcion que indica que ya se han gestionado los rebotes
static void Timer2_Callback (mygpio *pin); //funcion donde se mete en la cola el tipo de pulsacion y la accion
static int Init_Timers (void); //funcion que crea los timers para manejar los rebotes y el tipo de pulsacion

/*-----------------------JOYSTICK----------------------------------------------*/
int Init_Thjoy (void) {
	
	//Configura los pines de los joysticks
  InitJoy();
	osStatus_t status = Init_Timers();
  
	//Crea el hilo del joystick
  tid_Thjoy = osThreadNew(Thjoy, &joystick, NULL);
  
	//Comprueba que la creación se ha realizado correctamente
	if (tid_Thjoy == NULL | status == -1) {
    return(-1);
  }
 
  return(0);
}
 
static void Thjoy (void *argument){
	uint32_t flag = 0;
	
	//Inicializa los pines de los joysticks
	joystick_t *joy = argument;
	
	HAL_GPIO_Init(joy->ArribaDerecha.port, &(joy->ArribaDerecha.gpio) );
	HAL_GPIO_Init(joy->IzquierdaBajoCentro.port, &(joy->IzquierdaBajoCentro.gpio) );
 
	//Activa las interrupciones de los joysticks
  HAL_NVIC_EnableIRQ( EXTI15_10_IRQn );
	
  while (1) {
    // Espera a que se pulse uno de los joystick
		flag = osThreadFlagsWait(0x03, osFlagsWaitAny ,osWaitForever);
		if(flag == S_REB)
		{
			//Se inicia el timer de los rebotes
			osTimerStart(tim_id1,50U);
		}else if(flag == S_PULSE)
		{
			//Inicia el timer para comprobar si la pulsacion ha sido larga o no
			osTimerStart(tim_id2,1000U);
		}
			
    osThreadYield();                            // suspend thread
  }
}

static void InitJoy(void){
  __HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
  
	
	//Configuraci�n de los pines
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
	
  if( GPIO_Pin == (GPIO_PIN_10 | GPIO_PIN_11) ) {
    gesto.port = GPIOB;
    gesto.gpio.Pin=GPIO_Pin;
    
  } else{
    gesto.port = GPIOE;
    gesto.gpio.Pin=GPIO_Pin;
  }
}



/*----------------TIMER----------------------------------------------*/

static void Timer1_Callback (void *arg) {
	//Encvia el flag para informar que ya se han gestionado los rebotes
	osThreadFlagsSet(tid_Thjoy, S_PULSE);
}

static void Timer2_Callback (mygpio *pin) {	
	//Comprueba si la pulsacion ha sido larga o corta
	informacion.corta = !HAL_GPIO_ReadPin( pin->port , (uint16_t)pin->gpio.Pin );
  
	//Establece el tipo de accion para compartir en la cola
  switch(pin->gpio.Pin){
		case GPIO_PIN_10:
			strncpy( informacion.direccion, "Arriba", sizeof(informacion.direccion) - 1);
		break;
		case GPIO_PIN_11:
			strncpy( informacion.direccion, "Derecha", sizeof(informacion.direccion) - 1);
		break;
		case GPIO_PIN_12:
			strncpy( informacion.direccion, "Abajo", sizeof(informacion.direccion) - 1);
		break;
		case GPIO_PIN_14:
			strncpy( informacion.direccion, "Izquierda", sizeof(informacion.direccion) - 1);
		break;
		case GPIO_PIN_15:
			strncpy( informacion.direccion, "Centro", sizeof(informacion.direccion) - 1);
		break;
  }
	
	//Añadimos la accion y el tipo de pulsacion a la cola
  osMessageQueuePut(mid_MsgQueueJOY, &informacion, 0U, 0U);
}

static int Init_Timers (void) {
 
  // Creamos one-shoot timer 1
  tim_id1 = osTimerNew((osTimerFunc_t)&Timer1_Callback, osTimerOnce, NULL, NULL);
  
  // Creamos one-shoot timer 2
  tim_id2 = osTimerNew((osTimerFunc_t)&Timer2_Callback, osTimerOnce, &gesto, NULL);
  
	//Comprobamos que se han creado correctamente los timers
	if (tim_id1 == NULL | tim_id2 == NULL) {
    return -1;
  }
	
  return 0;
}
