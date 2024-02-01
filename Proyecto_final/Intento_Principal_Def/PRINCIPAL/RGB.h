#ifdef _RTE_
	#include "RTE_Components.h"             // Componentes
	#include "stm32f4xx_hal.h"
#endif
#ifdef RTE_CMSIS_RTOS2             
	#include "cmsis_os2.h"                  // CMSIS:RTOS2
#endif

#ifndef _RGB_H
	#define _RGB_H
		
		int Init_ThRGB(void);
		
		extern osMessageQueueId_t mid_MsgQueueLeds;

		//Header file only contains datatypes and functions that we made available for other modules
		typedef struct {                               
			uint8_t LD1;
			uint8_t LD2;
			uint8_t LD3;
		} MSGQUEUE_LEDS_t;
	
#endif
