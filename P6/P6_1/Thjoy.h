#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

#ifndef _THJOY_H
  #define _THJOY_H
		osThreadId_t tid_Thjoy; // id del hilo del joystick derecho
		typedef struct{
			GPIO_InitTypeDef gpio;
			GPIO_TypeDef *port;
		} mygpio; //estructura para codificar el pin asociado al joystick derecho
		mygpio joystickDer; //estructura del joystick derecho
		
		int Init_Thjoy(void); //funcion para crear el hilo del joystick
		void Thjoy (void *argument); // funcion del hilo
		void EXTI15_10IRQHandler(void); //funcion de la interrupcion del joystick derecho
		void InitJoy(void); //funcion para configurar el joystick derecho
		void EXTI15_10IRQHandler(void); //funcion asociada a la interrupcion del joystick derecho
		void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin); //funcion donde se almacena el codigo de la interrupcion del joystick derecho
		
		extern osTimerId_t tim_id1; //id del timer 
		extern osThreadId_t tid_Thtimer; //id del hilo del timer

  #define S_TIM 0x00000002U //flag para reiniciar el timer
#endif
