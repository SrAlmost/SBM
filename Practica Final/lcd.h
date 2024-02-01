#ifndef _LCD_H
  #define _LCD_H
	
	#include "cmsis_os2.h"                  // CMSIS:RTOS2
	#include "Driver_SPI.h"
	#include <stdint.h>
	
	typedef struct{ char linea1[19]; char linea2[24]; } texto;
	extern ARM_DRIVER_SPI Driver_SPI1; //driver del protocolo SPI
	extern osMessageQueueId_t mid_MsgQueueLCD;                // message queue id
/*--------FUNCIONES--------*/		
    int Init_LCD (void); 

#endif
