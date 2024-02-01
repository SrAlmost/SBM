#include "clock.h"
/*--------------------------- VARIABLES ------------------------------*/
	/*------- TIMER -------*/
osTimerId_t id_clock; // id del timer
	/*--------------------------- HILO -------------------------------*/
static osThreadId_t tid_clock; //id del hilo del timer
	/*--------------------------- RELOJ ------------------------------*/
uint8_t segundos; //variable global que nos mostrar� los segundos
uint8_t minutos; //variable global que nos mostrar� los minutos
uint8_t horas; //variable global que nos mostrar� las horas
/*----------------- DECLARACION FUNCIONES ----------------------------*/
static int8_t Init_Timer(void);
static void Timer_Callback (void);
__NO_RETURN static void Thclock (void *argument);
/*------------------------ CODIGO FUNCIONES ----------------------------*/
	/*------------------------ TIMER ----------------------------*/
static int8_t Init_Timer(void){
	//Creamos el timer periodico y le asignamos su callback
  id_clock = osTimerNew((osTimerFunc_t)&Timer_Callback, osTimerPeriodic, NULL, NULL);
	//Comprobamos que el timer se ha creado correctamente
	if (id_clock != NULL) {	return -1;   }
	//La creacion e inicializacion del timer se ha realizado correctamente
  return 0;
}
static void Timer_Callback (void) {
	//Actualiza las horas
  if(minutos == 59 && segundos == 59){   horas = (horas + 1) % 24 ;  }
	//Actualiza los minutos
  if(segundos == 59){ minutos = (minutos + 1) % 60 ;  }
	//Actualiza los segundos
  segundos = (segundos + 1) % 60 ;
}
	/*------------------------ INICIACIÓN HILO Y TIMER ----------------------------*/
int8_t Init_Clock (void) {
	//Iniciamos el timer periodico
	osStatus_t status = Init_Timer();
	//Iniciamos el hilo del timer periodico
  tid_clock = osThreadNew( Thclock, NULL, NULL);
	//Comprobamos que todas las creaciones se han realizado correctamente
  if (tid_clock != NULL & status == 0) {    return(0);   }
	//Ha habido un fallo al realizar alguna creacion
  return(-1);
}
	/*------------------------ HILO DEL RELOJ ----------------------------*/
__NO_RETURN static void Thclock (void *argument) {
	// Iniciamos el timer con un periodo de 1000 ms (1 s)
	osTimerStart(id_clock, 1000U);
  while(1){
    osThreadYield();
  }
}
