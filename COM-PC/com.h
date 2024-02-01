#ifdef _RTE_
	#include "RTE_Components.h"             // Componentes
#endif
#ifdef RTE_CMSIS_RTOS2                  
	#include "cmsis_os2.h"                  // CMSIS:RTOS2
#endif

#ifndef _COM_H
	#define _COM_H

		#include "Driver_USART.h"

		/*-------------------------- DEFINICIONES ------------------------------*/	
		#define S_GET 0x00000001U //flag para sacar información de la cola de mensajes para enviar al PC
		#define S_PUT 0x00000002U //flag para meter información en la cola de mensajes que se recibe del PC
		
		
		/*-------------------- TIPOS DE DATOS CREADOS --------------------------*/	
		typedef enum{ ESPERANDO_INICIO, COMANDO, LONGITUD, DATOS, TRAMA_COMPLETA} Estado; //formato de la trama de comunicacion
		typedef enum{ HORA=0x20, TEMPERATURA=0x25, MEDIDAS=0x55, BORRAR=0x60} Comandos; //comandos posibles que se pueden enviar
		typedef struct{ Comandos c;  float temperatura; char hora[8]; } Trama; //tipo de dato a recibir o enviar en la cola para la comunicación con el PC
		
		
		/*---------------------- VARIABLES EXTERNAS ---------------------------*/	
		extern ARM_DRIVER_USART Driver_USART3; //driver de la USART3
		extern osMessageQueueId_t mid_MsgQueuePCTX; //cola de mensajes donde se almacena la información a enviar al PC
		extern osMessageQueueId_t mid_MsgQueuePCRX; //cola de mensajes donde se almacena la información que se recibe del PC
		
		
		/*---------------- DECLARACION FUNCIONES GLOBALES ---------------------*/	
		int8_t Init_Thcom(void);
#endif
