#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Rebotes.h"
 
 
osThreadId_t tid_Rebotes;                        // thread id
extern osTimerId_t tim_id1;
 
void Rebotes (void *argument);                   // thread function
 
int Init_Rebotes (void) {
 
  tid_Rebotes = osThreadNew(Rebotes, NULL, NULL);
  if (tid_Rebotes == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Rebotes (void *argument) {
 
  while (1) {
    osThreadFlagsWait(S_REB, osFlagsWaitAny ,osWaitForever);
		osTimerStart(tim_id1,50U);
		
    osThreadYield();                            // suspend thread
  }
}
