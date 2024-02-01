#ifndef _MSQUEUE_H

	#include "stm32f4xx_hal.h"
	#include "cmsis_os2.h"

	#define _MSQUEUE_H
		void Thread_MsgQueue (void *argument); //hilo de la cola
		int Init_MsgQueue (void); //funcion donde se crea la cola y el hilo asociado a esta
		void almacenarObjetoCola(uint8_t valor);
		void representacionGesto(void);
		
		extern osThreadId_t thled1; // id del hilo verde
		extern osThreadId_t thled2; // id del hilo azul
		extern osThreadId_t thled3; // id del hilo rojo
		
		#define S_PUL 0x00000001U //flag para cuando se active el joystick
		#define S_UP 0x00000002U //flag para cuando se active el joystick
#endif
