#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

#ifndef _THLED1_H
  #define _THLED1_H
    int Init_Thled1(void); //funcion donde se lanza el hilo del led
		osThreadId_t thled1; // id del hilo
		void Thled1 (void *argument); // funcion del hilo
		void InitLed1(void); //funcion donde se configura el pin del led
		
		typedef struct{
			GPIO_InitTypeDef gpio;
			GPIO_TypeDef *port;
		} mygpio; //estructura para configurar el pin del led
		mygpio led1; //estructura para el led verde
		
	#define S_LEDS2 0x00000001U //flag para cuando el led se debe apagar
#endif
