#include "cmsis_os2.h"                          // CMSIS RTOS header file
 #include "Thjoy.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
int Init_Thjoy (void) {
	
	//Configura el pin del joystick derecho
  InitJoy();
  
	//Crea el hilo del joystick derecho
  tid_Thjoy = osThreadNew(Thjoy, (void *)&joystickDer, NULL);
  if (tid_Thjoy == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thjoy (void *argument) {
	
	//Inicializa el pin del joystick derecho
  mygpio *gpio = (mygpio *)argument;
	HAL_GPIO_Init(gpio->port, &(gpio->gpio) );
 
	//Activa las interrupciones del joystick
  HAL_NVIC_EnableIRQ( EXTI15_10_IRQn );
	
  while (1) {
    // Insert thread code here...
    osThreadYield();                            // suspend thread
  }
}

void InitJoy(void){
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
	
	//Configuración de los pines
	joystickDer.gpio.Mode = GPIO_MODE_IT_RISING;
	joystickDer.gpio.Pull = GPIO_PULLDOWN;
	joystickDer.gpio.Pin = GPIO_PIN_11;
	
	joystickDer.port = GPIOB;
  
  
}

void EXTI15_10_IRQHandler(void){
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	//Lanza el flag para resetear el joystick
  osThreadFlagsSet(tid_Thtimer, S_TIM);
}
