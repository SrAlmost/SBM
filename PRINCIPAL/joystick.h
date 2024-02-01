#ifndef _FUNCIONES_H

	#include "stm32f4xx_hal.h"
  #include "cmsis_os2.h"

#endif

#ifndef _THJOY_H
	#define _THJOY_H
		/*---------------------------JOYSTICK--------------------------------------------*/
			int Init_Thjoy(void); //funcion para crear el hilo del joystick
			extern osThreadId_t tid_Thjoy; //id del hilo
		/*---------------------------TIMER--------------------------------------------*/		
			typedef struct{
				uint8_t direccion;
				uint8_t corta;
			}informacion_t; //estructura con la que se comunicaran 

			extern osMessageQueueId_t mid_MsgQueueJOY; //id de la cola donde meteremos la accion y el tipo de pulsacion
			
		#define ARRIBA 0x01 //flag para rebotes
		#define DERECHA 0x02 //flag para pulsaciones largas/cortas
		#define ABAJO 0x04 //flag para rebotes
		#define IZQUIERDA 0x08 //flag para pulsaciones largas/cortas
		#define CENTRO 0x10 //flag para rebotes
			
		#define S_REB 0x00000001U //flag para rebotes
		#define S_PULSE 0x00000002U //flag para pulsaciones largas/cortas

#endif
