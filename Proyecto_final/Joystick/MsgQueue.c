#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "joystick.h"
 
/*----------------------------------------------------------------------------
 *      Message Queue creation & usage
 *---------------------------------------------------------------------------*/
 
#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects

osMessageQueueId_t mid_MsgQueueJOY;                // message queue id 
osThreadId_t tid_Thread_MsgQueue1;              // thread id 1
informacion_t msg;
 
void Thread_MsgQueue1 (void *argument);         // thread function 1
 
int Init_MsgQueue (void) {
 
  mid_MsgQueueJOY = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(informacion_t), NULL);
  if (mid_MsgQueueJOY == NULL) {
    // Message Queue object not created, handle failure
  }
 
  tid_Thread_MsgQueue1 = osThreadNew(Thread_MsgQueue1, NULL, NULL);
  if (tid_Thread_MsgQueue1 == NULL) {
    return(-1);
  }
 
  return(0);
}

void Thread_MsgQueue1 (void *argument) {
  
 
  while (1) {
    osMessageQueueGet(mid_MsgQueueJOY,&msg,NULL,osWaitForever);
    osThreadYield();                                            // suspend thread
  }
}
 

