#include "Joystick.h"
 

static osThreadId_t tid_Joystick;                        // thread id
extern osThreadId_t tid_Dibujar;
 
void Joystick (void *argument);                   // thread function
 
int Init_Joystick (void) {
 
  tid_Joystick = osThreadNew(Joystick, NULL, NULL);
  if (tid_Joystick == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Joystick (void *argument) {
 
  while (1) {
     // Insert thread code here...
    osThreadYield();                            // suspend thread
  }
}
