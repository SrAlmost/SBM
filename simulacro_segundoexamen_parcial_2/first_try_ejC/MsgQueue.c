#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "MsgQueue.h"
 
#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects
 
osMessageQueueId_t mid_MsgQueue;                // message queue id
 
static osThreadId_t tid_Thread_MsgQueue;              // thread id 
 
void Thread_MsgQueue (void *argument);         // thread function 
 
int Init_MsgQueue (void) {

  tid_Thread_MsgQueue = osThreadNew(Thread_MsgQueue, NULL, NULL);
  if (tid_Thread_MsgQueue == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread_MsgQueue (void *argument) {
 
	mid_MsgQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(uint8_t), NULL);
	
  while (1) {
     // Insert thread code here...
    osThreadYield();                                            // suspend thread
  }
}
