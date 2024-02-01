#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "string.h"
#include "stdio.h"
#include "Thread.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/

extern osMessageQueueId_t mid_MsgQueue;
osThreadId_t tid_Thread;                        // thread id

typedef struct {                                // object data type
  uint8_t linea;
  char inf[256];
} MSGQUEUE_OBJ_t;
 
void Thread (void *argument);                   // thread function
 
int Init_Thread (void) {
 
  tid_Thread = osThreadNew(Thread, NULL, NULL);
  if (tid_Thread == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread (void *argument) {
 
  MSGQUEUE_OBJ_t msg;
  
  sprintf(msg.inf, "Frase de ejemplo 2");
  msg.linea = 2;
  
	
	//osMessageQueuePut(mid_MsgQueue, &msg, 0U, 0U);
  while (1) {
    osMessageQueuePut(mid_MsgQueue, &msg, 0U, 0U);
    osDelay(3000U);
    msg.linea = (msg.linea == 1) ? 2 : 1;
		sprintf(msg.inf, "Frase de arriba");
    
    osThreadYield();                            // suspend thread
  }
}
