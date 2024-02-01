#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Timer.h"
 
 
 int Init_Timers (void);
 static void Timer_Callback (void const *arg);
 static void Timeout_Callback (void const *arg);
 void Timer(void *argument);
 
/*----- Periodic Timer Example -----*/
osTimerId_t tim_id;                            // timer id
osTimerId_t timeout_id;
static uint32_t exec2;                          // argument for the timer call back function
extern osThreadId_t tid_Dibujar;
static osThreadId_t tid_Timer;
 
// Periodic Timer Function
static void Timer_Callback (void const *arg) {
  // add user code here
	osThreadFlagsSet(tid_Dibujar, S_DER);	
}

static void Timeout_Callback (void const *arg) {
  // add user code here
	osThreadFlagsSet(tid_Dibujar, S_ABA);	
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
  tim_id = osTimerNew((osTimerFunc_t)&Timer_Callback, osTimerOnce, &exec2, NULL);
	timeout_id = osTimerNew((osTimerFunc_t)&Timeout_Callback, osTimerOnce, NULL, NULL);
	
	while(1)
	{
		osThreadYield();
	}
}
