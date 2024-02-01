#include "RGB.h"    

//These variables are only accessible from this module 
osThreadId_t tid_Th_RGB;

int Init_ThRGB(void);
void Thread_Leds(void *argument);

void initModLeds(void){
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  
  __HAL_RCC_GPIOD_CLK_ENABLE();

  
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13, GPIO_PIN_SET);
	
  GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

} 
int Init_ThRGB (void) {
 
  tid_Th_RGB = osThreadNew(Thread_Leds, NULL, NULL);
  if (tid_Th_RGB == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread_Leds (void *argument) {
	initModLeds();
	MSGQUEUE_LEDS_t localObject;
 
  while (1) {
		
		osMessageQueueGet(mid_MsgQueueLeds, &localObject, NULL, osWaitForever);
		
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, !localObject.LD1);
		
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, !localObject.LD2);
		
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, !localObject.LD3);
		
	}
}


