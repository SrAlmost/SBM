#ifndef _POT_H
	#define _POT_H
	#include "cmsis_os2.h"
	/*------------------------- DEFINICIONES --------------------------*/	
		#define S_POT 0x00000001U //flag para realizar las medidas de tensión de los potenciómetros 
	/*------------------------- DEFINICIONES --------------------------*/	
	typedef struct{ double temperaturaREF; double temperaturaMED; }potenciometro;
	/*---------------------- VARIABLES EXTERNAS -----------------------*/
		extern osTimerId_t tim_pot; //id del timer para realizar la medida 	
		extern osMessageQueueId_t mid_MsgQueuePOT; //cola de mensajes donde guardaremos la temperatura de referencia y medida
	/*---------------- DECLARACION FUNCIONES GLOBALES -----------------*/
		int8_t Init_Thpot (void);	
#endif
