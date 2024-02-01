#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Timer.h"
 
/*----------------------------------------------------------------------------
 *      Timer: Sample timer functions
 *---------------------------------------------------------------------------*/
 

int Init_Timers (void) {
  
  //Crea el hilo del timer
  tid_Thtimer = osThreadNew(Thtimer, NULL, NULL);
  if (tid_Thtimer == NULL) {
    return(-1);
  }
 
  return(0);
}

void Thtimer (void *argument){
  
  
  osStatus_t status; // estado de las funciones que retornan
 
  // Crea un timer one-shoot
  exec1 = 1U;
  tim_id1 = osTimerNew((osTimerFunc_t)&Timer1_Callback, osTimerOnce, &exec1, NULL);
  if (tim_id1 != NULL) {  // One-shot timer creado
    // Empieza el timer
    osTimerStart(tim_id1, 3000U);
  }

  while(1){
		//Espera al flag que indica que se ha pulsado el joystick derecho
    osThreadFlagsWait(S_TIM, osFlagsWaitAny, osWaitForever); 
		//Resetea el timer
    osTimerStart(tim_id1, 3000U);
  }
}

static void Timer1_Callback (void const *arg) {
	//Lanza el flag para que se encienda el led rojo
  osThreadFlagsSet( thled3 , S_LEDS2);
	//Lanza el flag para que se apague el led verde
  osThreadFlagsSet( thled1 , S_LEDS2);
}

