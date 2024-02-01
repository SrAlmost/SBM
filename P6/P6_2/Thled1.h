#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

#ifndef _THLED1_H
  #define _THLED1_H
    int Init_Thled1(void); //funcion donde se lanza el hilo del led
		void Thled1 (void *argument); // funcion del hilo
		void InitLed1(void); //funcion donde se configura el pin del led
		
		extern uint8_t codigo;
	#define S_L1ENC 0x00000000U //flag para cuando el led se debe encender
	#define S_L1APG 0x00000000U //flag para cuando el led se debe apagar
	
	#define S_PUL 0x00000001U //flag para cuando se active el joystick

#endif
