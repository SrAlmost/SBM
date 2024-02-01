//#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Thled2.h"

/*----------------------------------------------------------------------------
 *      Thread 2 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
 
typedef struct{
	GPIO_InitTypeDef gpio;
	GPIO_TypeDef *port;
} mygpio; //estructura para configurar el pin del led

static mygpio led2; //estructura para el led azul
osThreadId_t thled2; // id del hilo

int Init_Thled2 (void) {
	//Configuramos el pin del led azul
  Initled2();
  
  //Iniciamos el hilo asociado al led azul
  thled2 = osThreadNew(Thled2, (void *)&led2, NULL);
  if (thled2 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thled2 (void *argument) {
 
	//Iniciamos el pin del led
  mygpio *gpio2 = (mygpio *)argument;
	HAL_GPIO_Init(gpio2->port, &(gpio2->gpio) );
  
  while (1) {
    osThreadFlagsWait(S_PUL, osFlagsWaitAny, osWaitForever);
		switch(codigo){
			//Tener en cuenta que los leds de la tarjeta se activan con un nivel bajo
			case 1:
					HAL_GPIO_WritePin(gpio2->port, (uint16_t)gpio2->gpio.Pin, GPIO_PIN_SET);
			break;
			case 2:
					HAL_GPIO_WritePin(gpio2->port, (uint16_t)gpio2->gpio.Pin, GPIO_PIN_SET);
			break;
			case 3:
					HAL_GPIO_WritePin(gpio2->port, (uint16_t)gpio2->gpio.Pin, GPIO_PIN_RESET);
			break;
			case 4:
					HAL_GPIO_WritePin(gpio2->port, (uint16_t)gpio2->gpio.Pin, GPIO_PIN_SET);
			break;
			case 5:
					HAL_GPIO_WritePin(gpio2->port, (uint16_t)gpio2->gpio.Pin, GPIO_PIN_RESET);
			break;
		}
    osThreadYield();                            // suspend thread
  }
}

void Initled2(void){
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
	
	//Configuración de los pines
	led2.gpio.Mode = GPIO_MODE_OUTPUT_PP;
	led2.gpio.Pull = GPIO_NOPULL;
	led2.gpio.Speed = GPIO_SPEED_FREQ_LOW;
	led2.gpio.Pin = GPIO_PIN_0;
	
	led2.port = GPIOA;
}