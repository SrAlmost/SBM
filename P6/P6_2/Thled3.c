//#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Thled3.h"

/*----------------------------------------------------------------------------
 *      Thread 3 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t thled3; // id del hilo 
typedef struct{
	GPIO_InitTypeDef gpio;
	GPIO_TypeDef *port;
} mygpio; //estructura para configurar el pin del led
static mygpio led3; //estructura para el led rojo

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
  mygpio *gpio3 = (mygpio *)argument;
	HAL_GPIO_Init(gpio3->port, &(gpio3->gpio) );
  
  while (1) {
		osThreadFlagsWait(S_PUL, osFlagsWaitAny, osWaitForever);
		switch(codigo){
			//Tener en cuenta que los leds de la tarjeta se activan con un nivel bajo
			case 1:
					HAL_GPIO_WritePin(gpio3->port, (uint16_t)gpio3->gpio.Pin, GPIO_PIN_SET);
			break;
			case 2:
					HAL_GPIO_WritePin(gpio3->port, (uint16_t)gpio3->gpio.Pin, GPIO_PIN_SET);
			break;
			case 3:
					HAL_GPIO_WritePin(gpio3->port, (uint16_t)gpio3->gpio.Pin, GPIO_PIN_SET);
			break;
			case 4:
					HAL_GPIO_WritePin(gpio3->port, (uint16_t)gpio3->gpio.Pin, GPIO_PIN_RESET);
			break;
			case 5:
					HAL_GPIO_WritePin(gpio3->port, (uint16_t)gpio3->gpio.Pin, GPIO_PIN_RESET);
			break;
		}
		
    osThreadYield();                            // suspend thread
  }
}

void InitLed3(void){
  __HAL_RCC_GPIOE_CLK_ENABLE();
  
	
	//Configuración de los pines
	led3.gpio.Mode = GPIO_MODE_OUTPUT_PP;
	led3.gpio.Pull = GPIO_NOPULL;
	led3.gpio.Speed = GPIO_SPEED_FREQ_LOW;
	led3.gpio.Pin = GPIO_PIN_0;
	
	led3.port = GPIOE;
}
