#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Thled3.h"

/*----------------------------------------------------------------------------
 *      Thread 3 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t thled3;                        // thread id
 
void Thled3 (void *argument);                   // thread function
int Init_Thled3(void);
void InitLed3(void);

typedef struct{
  GPIO_InitTypeDef gpio;
  GPIO_TypeDef *port;
} mygpio;
 
mygpio led3;
int Init_Thled3 (void) {
 
  InitLed3();
  
  
  thled3 = osThreadNew(Thled3, (void *)&led3, NULL);
  if (thled3 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thled3 (void *argument) {
 
  mygpio *gpio = (mygpio *)argument;
	HAL_GPIO_Init(gpio->port, &(gpio->gpio) );
  
  while (1) {
    // Insert thread code here...
    HAL_GPIO_TogglePin(gpio->port, gpio->gpio.Pin);
    osDelay(287);
    
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
