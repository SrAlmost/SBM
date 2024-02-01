#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Timer.h"
 
 
 int Init_Timers (void);
 static void Timer_Callback (void const *arg);
 void Timer(void *argument);
 
/*----- Periodic Timer Example -----*/
static osTimerId_t tim_id;                            // timer id
static uint32_t exec2;                          // argument for the timer call back function
extern osThreadId_t tid_Dibujar;
static osThreadId_t tid_Timer;
 
// Periodic Timer Function
static void Timer_Callback (void const *arg) {
  // add user code here
	osThreadFlagsSet(tid_Dibujar, S_MOVE);
}
 
// Example: Create and Start timers
int Init_Timers (void) {
	tid_Timer = osThreadNew(Timer, NULL, NULL);
	if(tid_Timer == NULL)
	{
		return (-1);
	}
	return (0);
}

void Timer(void *argument)
{
	osStatus_t status;                            // function return status
 
  // Create periodic timer
  exec2 = 2U;
  tim_id = osTimerNew((osTimerFunc_t)&Timer_Callback, osTimerPeriodic, &exec2, NULL);
  if (tim_id != NULL) {  // Periodic timer created
    // start timer with periodic 100ms interval
    status = osTimerStart(tim_id, 100U);            
  }
	
	while(1)
	{
		osThreadYield();
	}
}
