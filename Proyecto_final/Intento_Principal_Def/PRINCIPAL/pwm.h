#ifndef _FUNCIONES_H

	#include "stm32f4xx_hal.h"
  #include "cmsis_os2.h"

#endif

#ifndef _PWM_H
	#define _PWM_H
		/*---------------------------JOYSTICK--------------------------------------------*/
			int Init_pwm(void); //funcion para crear el hilo del joystick
			
			extern osMessageQueueId_t mid_MsgQueuePWM;                // message queue id
			
#endif
