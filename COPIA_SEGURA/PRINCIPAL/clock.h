#ifdef _RTE_
	#include "RTE_Components.h"             // Componentes
#endif
#ifdef RTE_CMSIS_RTOS2             
	#include "cmsis_os2.h"                  // CMSIS:RTOS2
#endif

#ifndef _CLOCK_H
  #define _CLOCK_H
/*--------VARIABLES--------*/
	extern uint8_t segundos;
  extern uint8_t minutos;
  extern uint8_t horas;
	/*------- TIMER -------*/
	extern osTimerId_t id_clock; // id del timer
	/*--------FUNCIONES--------*/		
		/**
			* @brief Crea el hilo del timer
			*
		*/
    int8_t Init_Clock (void); 

#endif

