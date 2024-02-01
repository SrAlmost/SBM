//#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Thled1.h"


/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/

osThreadId_t thled1; // id del hilo
typedef struct{
	GPIO_InitTypeDef gpio;
	GPIO_TypeDef *port;
} mygpio; //estructura para configurar el pin del led
static mygpio led1; //estructura para el led verde

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
  mygpio *gpio1 = (mygpio *)argument;
	HAL_GPIO_Init(gpio1->port, &(gpio1->gpio) );
 
  
  while (1) {
    // Insert thread code here...
		osThreadFlagsWait(S_PUL, osFlagsWaitAny, osWaitForever);
		switch(codigo){
			//Tener en cuenta que los leds de la tarjeta se activan con un nivel bajo
			case 1:
					HAL_GPIO_WritePin(gpio1->port, (uint16_t)gpio1->gpio.Pin, GPIO_PIN_SET);
			break;
			case 2:
					HAL_GPIO_WritePin(gpio1->port, (uint16_t)gpio1->gpio.Pin, GPIO_PIN_RESET);
			break;
			case 3:
					HAL_GPIO_WritePin(gpio1->port, (uint16_t)gpio1->gpio.Pin, GPIO_PIN_SET);
			break;
			case 4:
					HAL_GPIO_WritePin(gpio1->port, (uint16_t)gpio1->gpio.Pin, GPIO_PIN_SET);
			break;
			case 5:
					HAL_GPIO_WritePin(gpio1->port, (uint16_t)gpio1->gpio.Pin, GPIO_PIN_RESET);
			break;
		}
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

