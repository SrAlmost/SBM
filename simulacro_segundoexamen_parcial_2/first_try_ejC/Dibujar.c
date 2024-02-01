#include "Dibujar.h"
#include "cmsis_os2.h"
#include "string.h"
#include "LCD.h"
#include "Timer.h"

osThreadId_t tid_Dibujar;                        // thread id
extern osMessageQueueId_t mid_MsgQueue;
 
void Dibujar (void *argument);                   // thread function
 
int Init_Dibujar (void) {
 
  tid_Dibujar = osThreadNew(Dibujar, NULL, NULL);
  if (tid_Dibujar == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Dibujar (void *argument) {
	uint16_t position = 9;
	uint8_t msg;
	
	symbolToLocalBuffer_L1('@', position);
	position++;
	
	LCD_limpiarBuffer();
	LCD_reset();
	LCD_Init();
	
  while (1) {
     // Insert thread code here...
		
		osMessageQueueGet(mid_MsgQueue, &msg, NULL, osWaitForever);   // wait for message
		
		switch(msg)
		{
			case 1:
				position = 109;
				break;
			case 2:
				position = (position < 109) ? (position+1) : position;
				break;
			case 4:
				position = 9;
				break;
			case 8:
				position = (position > 9) ? (position-1) : position;
				break;
		}
		
		LCD_limpiarBuffer();
		symbolToLocalBuffer_L1('@', position);
		LCD_update();
		
    osThreadYield();                            // suspend thread
  }
}
