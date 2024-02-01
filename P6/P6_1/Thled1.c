//#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Thled1.h"


/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 

int Init_Thled1 (void) {
	//Configuramos el pin del LED Verde
  InitLed1();
  
	//Creamos el hilo del LED Verde
  thled1 = osThreadNew(Thled1, (void *)&led1, NULL);
  if (thled1 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thled1 (void *argument) {
 
	//Iniciamos el led verde
  mygpio *gpio = (mygpio *)argument;
	HAL_GPIO_Init(gpio->port, &(gpio->gpio) );
 
	//Encendemos el led
  HAL_GPIO_TogglePin(gpio->port, gpio->gpio.Pin);
	//Esperamos a que el pasen 3 segundos
  osThreadFlagsWait(S_LEDS2, osFlagsWaitAll, osWaitForever);
	//Apagamos el led
  HAL_GPIO_TogglePin(gpio->port, gpio->gpio.Pin);
  
  while (1) {
    // Insert thread code here...
    osThreadYield();                            // suspend thread
  }
}

void InitLed1(void){
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
	
	//Configuración de los pines
	led1.gpio.Mode = GPIO_MODE_OUTPUT_PP;
	led1.gpio.Pull = GPIO_NOPULL;
	led1.gpio.Speed = GPIO_SPEED_FREQ_LOW;
	led1.gpio.Pin = GPIO_PIN_0;
	
	led1.port = GPIOB;
}

