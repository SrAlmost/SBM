#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

#ifndef _THLED3_H
  #define _THLED3_H
		int Init_Thled3(void); //funcion donde se lanza el hilo del led
		osThreadId_t thled3; // id del hilo
		void Thled3 (void *argument); // funcion del hilo
		void InitLed3(void); //funcion donde se configura el pin del led
		
		typedef struct{
			GPIO_InitTypeDef gpio;
			GPIO_TypeDef *port;
		} mygpio; //estructura para configurar el pin del led
		mygpio led3; //estructura para el led verde
		
	#define S_LEDS2 0x00000001U //flag para cuando el led se debe apagar
  
#endif