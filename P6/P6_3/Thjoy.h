#ifndef _THJOY_H

	#include "stm32f4xx_hal.h"
	#include "cmsis_os2.h"

	#define _THJOY_H		
		int Init_Thjoy(void); //funcion para crear el hilo del joystick
		void Thjoy (void *argument); // funcion del hilo
		void EXTI15_10IRQHandler(void); //funcion de la interrupcion del joystick derecho
		void InitJoy(void); //funcion para configurar todos los joysticks
		void EXTI15_10_IRQHandler(void); //funcion asociada a la interrupcion del joystick derecho
		void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin); //funcion donde se almacena el codigo de la interrupcion del joystick derecho
		
		extern osTimerId_t tim_id1; //id del timer 
		extern osThreadId_t tid_Thtimer; //id del hilo del timer

	#define _TIMER_H
		int Init_Timer (void); //funcion que crea el timer para manejar los rebotes
		void generarCodigo(uint16_t pin); //funcion para generar el codigo de los leds
		void Timer1_Callback (void const *arg);
		
		extern osMessageQueueId_t mid_MsgQueue;
		extern osThreadId_t tid_Thread_MsgQueue;
	#define S_REB 0x00000001U //flag para rebotes
	#define S_MES 0x00000002U //flag para informar de que se ha almacenado un objeto en la cola
#endif
