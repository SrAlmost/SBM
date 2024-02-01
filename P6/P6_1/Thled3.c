//#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Thled3.h"

/*----------------------------------------------------------------------------
 *      Thread 3 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 

int Init_Thled3 (void) {
	//Configuramos el pin del led rojo
  InitLed3();
  
  //Iniciamos el hilo asociado al led rojo
  thled3 = osThreadNew(Thled3, (void *)&led3, NULL);
  if (thled3 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thled3 (void *argument) {
 
	//Iniciamos el pin del led
  mygpio *gpio = (mygpio *)argument;
	HAL_GPIO_Init(gpio->port, &(gpio->gpio) );
  
	//Esperamos a que pasen 3 segundos
  osThreadFlagsWait(S_LEDS2, osFlagsWaitAll, osWaitForever);
	//Encendemos el led
  HAL_GPIO_TogglePin(gpio->port, gpio->gpio.Pin);
  
  while (1) {
    // Insert thread code here...
    osThreadYield();                            // suspend thread
  }
}

void InitLed3(void){
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
	
	//Configuración de los pines
	led3.gpio.Mode = GPIO_MODE_OUTPUT_PP;
	led3.gpio.Pull = GPIO_NOPULL;
	led3.gpio.Speed = GPIO_SPEED_FREQ_LOW;
	led3.gpio.Pin = GPIO_PIN_14;
	
	led3.port = GPIOB;
}
