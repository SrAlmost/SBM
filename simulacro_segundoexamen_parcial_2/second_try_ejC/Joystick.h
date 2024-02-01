#ifndef _JOYSTICK_

	#define _JOYSTICK_	

		int Init_Joystick(void); //funcion para crear el hilo del joystick
		
		#define S_DER 0x00000001U
		#define S_ABA 0x00000002U
		#define S_IZQ 0x00000004U
		#define S_ARR 0x00000008U
		#define S_REB 0x00000010U //flag para rebotes
		
#endif
