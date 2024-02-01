#ifndef _PRINCIPAL_H
	#define _PRINCIPAL_H
		#include <stdio.h>
		#include "cmsis_os2.h"  
		#include "stm32f4xx_hal.h"
		
		/*------- CLOCK -------*/
		extern uint8_t segundos;
		extern uint8_t minutos;
		extern uint8_t horas;
		extern osTimerId_t id_clock; // id del timer
		
		/*------- LCD -------*/
		typedef struct{ char linea1[19]; char linea2[24]; } texto;
		
		/*------- POT -------*/
		typedef struct{ double temperaturaREF; double temperaturaMED; }potenciometro;
		extern osTimerId_t tim_pot; //id del timer para realizar la medida 	
		
		/*------- JOYSTICK -------*/
		typedef struct{
				char direccion[10];
				uint8_t corta;
		}informacion_t; //estructura con la que se comunicaran 
		
		/*------ TEMPERATURA -------*/
		extern osTimerId_t tim_medida; //id del timer para realizar la medida
		
		/*------ PWM -------*/
		extern TIM_HandleTypeDef htim1;
		
		/*------ COM-PC ------*/
		typedef enum{ HORA=0x20, TEMPERATURA=0x25, MEDIDAS=0x55, BORRAR=0x60} Comandos; //comandos posibles que se pueden enviar
		typedef struct{ 
			Comandos c; 
			uint8_t hora; uint8_t minutos; uint8_t segundos;
			double temperatura; double temperaturaREF; double PWM;
		} Trama; //tipo de dato a recibir o enviar en la cola para la comunicación con el PC
		
		
		/*------ HILO ------*/
		typedef struct{ char hora[9]; double temperaturaMedida; double temperaturaReferencia; double cicloTrabajo; } MEDIDA;
		typedef enum{ REPOSO, ACTIVO, TEST, PROGRAMACION } Modo;
		typedef enum{ HORAS, MINUTO, SEGUNDO, TEMREF } ProgDep;
		
		typedef struct {
			MEDIDA datos[10];
			uint8_t lectura;  // Puntero de lectura
			uint8_t escritura; // Puntero de escritura
		} BufferCircular;



		
		int Init_Principal (void);
#endif
