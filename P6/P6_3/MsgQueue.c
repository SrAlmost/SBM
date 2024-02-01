#include "MsQueue.h"
#include "stdio.h"
#include "LCD.h"
 
/*----------------------------------------------------------------------------
 *      Message Queue creation & usage
 *---------------------------------------------------------------------------*/
 
#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects
 

 
osMessageQueueId_t mid_MsgQueue;                // message queue id
osThreadId_t tid_Thread_MsgQueue;              // thread id 
extern osThreadId_t tid_ThLCD;
char frase[80];

int Init_MsgQueue (void) {
 
  mid_MsgQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(uint8_t), NULL);
  if (mid_MsgQueue == NULL) {
    // Message Queue object not created, handle failure
  }
 
  tid_Thread_MsgQueue = osThreadNew(Thread_MsgQueue, NULL, NULL);
  if (tid_Thread_MsgQueue == NULL) {
    return(-1);
  }
	
  return(0);
}
 
void Thread_MsgQueue (void *argument) {
  
 
  while (1) {
    // Esperamos a que se inserte una accion en la cola
    //osThreadFlagsWait(S_COLA, osFlagsWaitAny, osWaitForever);
		// Representamos en el LCD la accion
		representacionGesto();
    osThreadYield();                                            // suspend thread
  }
}

void representacionGesto(void){

  uint8_t gesto; //accion almacenada en la cola
	
	//Obtenemos la accion de la cola
	osMessageQueueGet(mid_MsgQueue,&gesto,NULL,osWaitForever);
	
  sprintf(frase,"                                                               "); 
	//Establecemos el gesto a realizar
	switch( gesto ){
		case 1:
			sprintf(frase, "Arriba");
		break;
		case 2:
			sprintf(frase, "Derecha");
		break;
		case 4:
			sprintf(frase, "Abajo");
		break;
		case 8:
			sprintf(frase, "Izquierda");
		break;
		case 16:
			sprintf(frase, "Centro");
		break;
	}
  reseteo();
  escribirLCD(frase);
	osThreadFlagsSet(tid_ThLCD, S_UP);
}

