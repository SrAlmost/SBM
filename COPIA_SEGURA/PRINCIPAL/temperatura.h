#ifdef _RTE_
	#include "RTE_Components.h"             // Componentes
#endif
#ifdef RTE_CMSIS_RTOS2                  
	#include "cmsis_os2.h"                  // CMSIS:RTOS2
#endif

#ifndef _TEMPERATURA_H
	#define _TEMPERATURA_H
		#include "Driver_I2C.h"
	
	/*------------------------- DEFINICIONES --------------------------*/	
		#define ADDRESS 0x48 //direccion del sensor de temperatura 
		#define S_MED 0x00000001U // flag para detectar las medidas del sensor
		
	/*---------------- DECLARACION VARIABLES EXTERNAS -----------------*/	
		extern osTimerId_t tim_medida; //id del timer para realizar la medida
		extern ARM_DRIVER_I2C Driver_I2C1; //driver para gestionar el bus I2C
		extern osMessageQueueId_t mid_MsgQueueTEM; //id de la cola donde meteremos la temperatura

	/*---------------- DECLARACION FUNCIONES GLOBALES -----------------*/
		/**
			* @brief  Configura el pin SDA que se corresponde con el pin PB9
			* @param  None
			* @retval None
		*/
		int8_t Init_Thtemperatura (void);

#endif
