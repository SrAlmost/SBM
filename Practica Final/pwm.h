#ifndef _PWM_H
	#define _PWM_H
		#include "cmsis_os2.h"                  // CMSIS:RTOS2
		#include "stm32f4xx_hal.h"
		/*---------------------------JOYSTICK--------------------------------------------*/
			int8_t Init_pwm(void); //funcion para crear el hilo del joystick
			extern osMessageQueueId_t mid_MsgQueuePWM;                // message queue id 
			extern TIM_HandleTypeDef htim1;
#endif
