#include "ThLCD.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_ThLCD;                        // thread id
 
void ThLCD(void *argument);                   // thread function
int Init_ThLCD (void);
 
int Init_ThLCD (void) {
 
  tid_ThLCD = osThreadNew(ThLCD, NULL, NULL);
  if (tid_ThLCD == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void ThLCD (void *argument) {
  
  //Iniciamos el Timer 7
	initTIMER7();
	
	//Reseteamos el LCD
  LCD_reset();
  
	//Iniciamos el LCD
	LCD_Init();
  
  while (1) {
    osThreadFlagsWait(S_UP, osFlagsWaitAny ,osWaitForever);
    LCD_update();
    osThreadYield();                            // suspend thread
  }
}
