#ifndef _FUNCIONES_H

	#include "stm32f4xx_hal.h"
  #include "cmsis_os2.h"

#endif

#ifndef _LCD_H
  #define _LCD_H
/*--------FUNCIONES--------*/		
		/**
			* @brief Crea el hilo del timer
			*
		*/
    int Init_LCD (void);
		void LCD_modificarByteBuffer(uint8_t pos, uint8_t page, char byte);
		
		typedef struct{                                // object data type
			uint8_t linea;
			unsigned char inf[256];
		} escribir_t;
		
		extern osMessageQueueId_t mid_MsgQueueLCD; //id de la cola donde sacaremos la linea y la informacion a escribir

#endif
