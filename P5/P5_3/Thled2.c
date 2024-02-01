//#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Thled2.h"
#include "Thled1.h"

/*----------------------------------------------------------------------------
 *      Thread 2 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
 extern osThreadId_t thled3;
void Thled2 (void *argument);                   // thread function
int Init_Thled2(void);
void InitLed2(void);
osThreadId_t thled2;                        // thread id
uint8_t ciclosLED2 = 0;


typedef struct{
  GPIO_InitTypeDef gpio;
  GPIO_TypeDef *port;
} mygpio;
 
mygpio led2;
int Init_Thled2 (void) {
 
  InitLed2();
  
  
  thled2 = osThreadNew(Thled2, (void *)&led2, NULL);
  if (thled2 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thled2 (void *argument) {
 
  mygpio *gpio = (mygpio *)argument;
	HAL_GPIO_Init(gpio->port, &(gpio->gpio) );
  
  osThreadFlagsWait(S_LED1, osFlagsWaitAny, osWaitForever);
  
  while (1) {
    // Insert thread code here...
    HAL_GPIO_TogglePin(gpio->port, gpio->gpio.Pin);
    osDelay(137);
    ciclosLED2++;
    
    if(ciclosLED2 == 30){
      osThreadFlagsSet(thled3, S_LED3);
    }
    
    if(ciclosLED2 == 40){
      osThreadFlagsWait(S_LED2, osFlagsWaitAny, osWaitForever);
      ciclosLED2 = 0;
    }
    
    osThreadYield();                            // suspend thread
  }
}

void InitLed2(void){
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
	
	//Configuración de los pines
	led2.gpio.Mode = GPIO_MODE_OUTPUT_PP;
	led2.gpio.Pull = GPIO_NOPULL;
	led2.gpio.Speed = GPIO_SPEED_FREQ_LOW;
	led2.gpio.Pin = GPIO_PIN_7;
	
	led2.port = GPIOB;
}
