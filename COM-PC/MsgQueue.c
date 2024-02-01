#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "com.h"
 
/*----------------------------------------------------------------------------
 *      Message Queue creation & usage
 *---------------------------------------------------------------------------*/
 
#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects
 
osMessageQueueId_t mid_MsgQueuePCRX;                // message queue id
osMessageQueueId_t mid_MsgQueuePCTX;                // message queue id
 
static osThreadId_t tid_Thread_MsgQueue1;              // thread id 1
 
void Thread_MsgQueue1 (void *argument);         // thread function 1
int Init_MsgQueue (void);
 
int Init_MsgQueue (void) {
 
  mid_MsgQueuePCRX = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(Trama), NULL);
  if (mid_MsgQueuePCRX == NULL) {
     // Message Queue object not created, handle failure
  }
	
	mid_MsgQueuePCTX = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(Trama), NULL);
  if (mid_MsgQueuePCTX == NULL) {
     // Message Queue object not created, handle failure
  }
 
  tid_Thread_MsgQueue1 = osThreadNew(Thread_MsgQueue1, NULL, NULL);
  if (tid_Thread_MsgQueue1 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread_MsgQueue1 (void *argument) {
  
	Trama t={TEMPERATURA,22.5,"15:20:14"};
 
  while (1) {
		osDelay(2000);
		osMessageQueuePut(mid_MsgQueuePCTX,&t,0U,osWaitForever);
		osDelay(1000);
		osMessageQueueGet(mid_MsgQueuePCRX,&t,0U,1000);
		
    osThreadYield();                                            // suspend thread
  }
}
