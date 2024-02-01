#include "Dibujar.h"
#include "cmsis_os2.h"
#include "string.h"
#include "LCD.h"
#include "Timer.h"

osThreadId_t tid_Dibujar;                        // thread id
extern osMessageQueueId_t mid_MsgQueue;
extern osTimerId_t tim_id;
 
void Dibujar (void *argument);                   // thread function
 
int Init_Dibujar (void) {
 
  tid_Dibujar = osThreadNew(Dibujar, NULL, NULL);
  if (tid_Dibujar == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Dibujar (void *argument) {
	unsigned char figura[511];
	uint16_t posicion = 0;
	uint32_t flag;
	uint8_t linea = 1;
	
	LCD_reset();
	LCD_Init();
	LCD_update();
	
  while (1) {
     // Insert thread code here...
		osTimerStart(tim_id, 200U);
		flag = osThreadFlagsWait(S_DER, osFlagsWaitAny, osWaitForever);
		
		memset(figura, 0x00,  sizeof(figura));
		
		for(int i = 0; i < 128; i++)
		{
			switch(linea)
			{
				case 1:
					figura[i+posicion] = 0x01;
					break;
				case 2:
					figura[i+posicion] = 0x02;
					break;
				case 3:
					figura[i+posicion] = 0x04;
					break;
				case 4:
					figura[i+posicion] = 0x08;
					break;
				case 5:
					figura[i+posicion] = 0x10;
					break;
				case 6:
					figura[i+posicion] = 0x20;
					break;
				case 7:
					figura[i+posicion] = 0x40;
					break;
				case 8:
					figura[i+posicion] = 0x80;
					break;
			}
		}
		
		LCD_escribirBuffer(figura, sizeof(figura));
		LCD_update();
		
		if(linea == 8)
			posicion = (posicion<384) ? posicion + 128 : 0;
		linea = (linea == 8) ? 1 : (linea+1);
			
    osThreadYield();                            // suspend thread
  }
}
