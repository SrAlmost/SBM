#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

#ifndef _THled2_H
  #define _THled2_H
		int Init_Thled2(void); //funcion donde se lanza el hilo del led
		void Thled2 (void *argument); // funcion del hilo
		void Initled2(void); //funcion donde se configura el pin del led
		
		extern uint8_t codigo;
	#define S_L2ENC 0x0000002U//flag para cuando el led se debe encender
	#define S_L2APG 0x0000003U //flag para cuando el led se debe apagar
	
	#define S_PUL 0x00000001U //flag para cuando se active el joystick

#endif
