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
	
	LCD_limpiarBuffer();
	LCD_reset();
	LCD_Init();
	
  while (1) {
     // Insert thread code here...
		osThreadFlagsWait(S_MOVE, osFlagsWaitAny, osWaitForever);
		
		LCD_limpiarBuffer();
		symbolToLocalBuffer_L1('@', position);
		LCD_update();
		if(!final)
		{
			position++;
			final = (position < 109) ? 0 : 1;
		}else
		{
			position--;
			final = (position > 9) ? 1 : 0;
		}
		
    osThreadYield();                            // suspend thread
  }
}
