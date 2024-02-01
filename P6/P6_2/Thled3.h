#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

#ifndef _THLED3_H
  #define _THLED3_H
		int Init_Thled3(void); //funcion donde se lanza el hilo del led
		void Thled3 (void *argument); // funcion del hilo
		void InitLed3(void); //funcion donde se configura el pin del led
		extern uint8_t codigo;
	#define S_L3ENC 0x00000004U //flag para cuando el led se debe encender
	#define S_L3APG 0x00000005U //flag para cuando el led se debe apagar
	
	#define S_PUL 0x00000001U //flag para cuando se active el joystick
		
#endif
