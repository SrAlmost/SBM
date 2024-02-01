#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Thprueba.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thprueba;                        // thread id
extern osMessageQueueId_t mid_MsgQueuePWM;                // message queue id 
 
void prueba (void *argument);                   // thread function
 
int Init_Thprueba (void) {
 
  tid_Thprueba = osThreadNew(prueba, NULL, NULL);
  if (tid_Thprueba == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void prueba (void *argument) {
	uint32_t msg = 10;
 
  while (1) {
    osDelay(1000);
    osMessageQueuePut(mid_MsgQueuePWM, &msg, 0U, 0U);
    msg = (msg < 100) ? (msg + 10) : 0;
		
    osThreadYield();                            // suspend thread
  }
}
