#ifdef _RTE_
	#include "RTE_Components.h"             // Componentes
#endif
#ifdef RTE_CMSIS_RTOS2                  
	#include "cmsis_os2.h"                  // CMSIS:RTOS2
#endif

#ifndef _PRINCIPAL_H
	#define _PRINCIPAL_H
		#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects
		
		typedef struct{ int8_t hora; double temperaturaMedida; double temperaturaReferencia; double cicloTrabajo; } medidas;
		
		typedef enum{ REPOSO, ACTIVO, TEST, PROGRAMACION } Modo;
		
		extern osThreadId_t tid_Thtemperatura;
		extern osTimerId_t tim_medida;
		extern osThreadId_t tid_ThcomTX;
		
		int Init_principal (void);
#endif
		