#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Thled1.h"

/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t thled1;                        // thread id
 
void Thread (void *argument);                   // thread function
int Init_Thled1(void);
void InitLed(void);

typedef struct{
  GPIO_InitTypeDef gpio;
  GPIO_TypeDef *port;
} mygpio;
 
mygpio led1;
int Init_Thled1 (void) {
 
  InitLed();
  
  
  thled1 = osThreadNew(Thread, (void *)&led1, NULL);
  if (thled1 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread (void *argument) {
 
  mygpio *gpio = (mygpio *)argument;
	HAL_GPIO_Init(gpio->port, &(gpio->gpio) );
  
  while (1) {
    // Insert thread code here...
    HAL_GPIO_TogglePin(gpio->port, gpio->gpio.Pin);
    osDelay(200);
    HAL_GPIO_TogglePin(gpio->port, gpio->gpio.Pin);
    osDelay(800);
    
    osThreadYield();                            // suspend thread
  }
}

void InitLed(void){
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
	
	//Configuración de los pines
	led1.gpio.Mode = GPIO_MODE_OUTPUT_PP;
	led1.gpio.Pull = GPIO_NOPULL;
	led1.gpio.Speed = GPIO_SPEED_FREQ_LOW;
	led1.gpio.Pin = GPIO_PIN_0;
	
	led1.port = GPIOB;
}

