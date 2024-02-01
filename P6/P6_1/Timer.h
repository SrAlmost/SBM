#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

#ifndef _TIMER_H
  #define _TIMER_H
		osThreadId_t tid_Thtimer; // id del hilo del timer
		osTimerId_t tim_id1; // id del timer
		static uint32_t exec1; // argumento para la funcion callback del timer
		
		int Init_Timers(void); //inicia el hilo del timer
		extern osThreadId_t thled3; //hilo del led rojo
		extern osThreadId_t thled1; //hilo del led verde
		void Thtimer (void *argument); // funcion del hilo
		static void Timer1_Callback (void const *arg); //funcion callback del timer
		
  #define S_LEDS2 0x00000001U // flag para los leds
  #define S_TIM 0x00000002U //flags para el joystick derecho
#endif