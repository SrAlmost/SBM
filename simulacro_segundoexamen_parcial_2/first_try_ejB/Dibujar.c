#include "Dibujar.h"
#include "cmsis_os2.h"
#include "string.h"
#include "LCD.h"
#include "Timer.h"

osThreadId_t tid_Dibujar;                        // thread id
 
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
	uint8_t final = 0;
	uint32_t flag;
	
	symbolToLocalBuffer_L1('@', position);
	position++;
	
	LCD_limpiarBuffer();
	LCD_reset();
	LCD_Init();
	
  while (1) {
     // Insert thread code here...
		flag = osThreadFlagsWait(S_MOVE, osFlagsWaitAny, osWaitForever);
		
		switch(flag)
		{
			case S_ARR:
				position = 109;
				break;
			case S_DER:
				position = (position < 109) ? (position+1) : position;
				break;
			case S_ABA:
				position = 9;
				break;
			case S_IZQ:
				position = (position > 9) ? (position-1) : position;
				break;
		}
		
		LCD_limpiarBuffer();
		symbolToLocalBuffer_L1('@', position);
		LCD_update();
		
//		LCD_limpiarBuffer();
//		symbolToLocalBuffer_L1('@', position);
//		LCD_update();
//		if(!final)
//		{
//			position++;
//			final = (position < 109) ? 0 : 1;
//		}else
//		{
//			position--;
//			final = (position > 9) ? 1 : 0;
//		}
		
    osThreadYield();                            // suspend thread
  }
}
