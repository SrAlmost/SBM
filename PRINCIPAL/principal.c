#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "principal.h"

#include "joystick.h"
#include "lcd.h"
#include "clock.h"
#include "pwm.h"
#include "temperatura.h"
#include "RGB.h"
#include "pot.h"
#include "com.h"

#include "string.h"
#include <stdio.h>

/*-------------------------- VARIABLES ------------------------------*/	
	/*-------------------------- COLAS MENSAJES ------------------------------*/	
osMessageQueueId_t mid_MsgQueueJOY;                // message queue id 
osMessageQueueId_t mid_MsgQueueLCD;                // message queue id 
osMessageQueueId_t mid_MsgQueuePWM;                // message queue id 
osMessageQueueId_t mid_MsgQueueTEM;                // message queue id 
osMessageQueueId_t mid_MsgQueuePOT;                // message queue id 
osMessageQueueId_t mid_MsgQueueLeds;
osMessageQueueId_t mid_MsgQueuePCTX;
osMessageQueueId_t mid_MsgQueuePCRX;
	/*-------------------------- JOYSTICK ------------------------------*/	
static informacion_t pulsacion;
	/*------------------ TEMPERATURA ---------------------*/	
static double temREF = 25.0; //temperatura de referencia
static double temMED; //temperatura medida
	/*------------------ PWM ---------------------*/	
static uint16_t cicloPWM; //ciclo de trabajo	
	/*------------------ POTENCIOMETRO ---------------------*/	
static potenciometro potMedidas; // medidas sacadas del potenciometro
	/*-------------------------- HILO ------------------------------*/	
osThreadId_t tid_principal;                        // thread id
Modo modos = REPOSO; //modo de funcionamiento del sistema
static uint8_t pos = 29; //posicion seleccion en modo Programacion
static uint8_t varMod = 0x80; //variable que estoy modificando
static uint8_t horasMod;
static uint8_t minutosMod;
static uint8_t segundosMod;
static double temRefMod;
static Trama recived;
static char medidasBuffer[10][40];
static uint8_t numMedida = 0;
static uint8_t intTemMod;
static double doubTemMod;
 
/*------------------ DECLARACION FUNCIONES NO GLOBALES ---------------------*/	
	/*------------------ COLAS DE MENSAJES ---------------------*/	
static int Init_MsgQueue (void) ;
	/*------------------ HILO PRINCIPAL ---------------------*/	
void principal (void *argument); 									// thread function
int Init_principal (void);
static void modoFuncionamiento(void); 
static void setLeds(uint8_t LED1, uint8_t LED2, uint8_t LED3);
	/*------------------ MODOS FUNCIONAMIENTO ---------------------*/	
static void reposo(void);
static void activo(void);
static void test(void);
static void programacion();
                  
 
/*-------------------------- CODIGO FUNCIONES ------------------------------*/	 
static int Init_MsgQueue (void) {
 
  mid_MsgQueueJOY = osMessageQueueNew(4, sizeof(informacion_t), NULL);
  if (mid_MsgQueueJOY == NULL) {
    return -1;
  }
	
	mid_MsgQueueLCD = osMessageQueueNew(2, sizeof(escribir_t), NULL);
  if (mid_MsgQueueLCD == NULL) {
    return -1;
  }
	
	mid_MsgQueuePWM = osMessageQueueNew(1, sizeof(uint16_t), NULL);
  if (mid_MsgQueuePWM == NULL) {
    return -1;
  }
	
	mid_MsgQueueTEM = osMessageQueueNew(1, sizeof(double), NULL);
  if (mid_MsgQueueTEM == NULL) {
    return -1;
  }
	
	mid_MsgQueueLeds = osMessageQueueNew(1, sizeof(MSGQUEUE_LEDS_t), NULL);
  if (mid_MsgQueueLeds == NULL) {
    return -1;
  }
	
	mid_MsgQueuePOT = osMessageQueueNew(1, sizeof(potenciometro), NULL);
  if (mid_MsgQueuePOT == NULL) {
    return -1;
  }
	
	mid_MsgQueuePCTX = osMessageQueueNew(10, sizeof(Trama), NULL);
  if (mid_MsgQueuePCTX == NULL) {
    return -1;
  }
	
	mid_MsgQueuePCRX = osMessageQueueNew(1, sizeof(Trama), NULL);
  if (mid_MsgQueuePCRX == NULL) {
    return -1;
  }
	
	return 0;
} 
 
int Init_principal (void) {
	//Creacion colas de mensajes
	if( Init_MsgQueue() != 0){
		return -1;
	}
 
  tid_principal = osThreadNew(principal, NULL, NULL);
  if (tid_principal == NULL) {
    return(-1);
  }
 
  return(0);
}

static void setLeds(uint8_t LED1, uint8_t LED2, uint8_t LED3){
	MSGQUEUE_LEDS_t ledsRGB;
	ledsRGB.LD3=LED3;
	ledsRGB.LD2=LED2;
	ledsRGB.LD1=LED1;
	osMessageQueuePut(mid_MsgQueueLeds, &ledsRGB, 0U, 0U);
}

static void reposo(void){
	escribir_t infoReposo;
	infoReposo.linea = 1;
	sprintf(infoReposo.inf, "     SBM 2023");
	osMessageQueuePut(mid_MsgQueueLCD, &infoReposo, 0U, 0U);
	infoReposo.linea = 2;
	sprintf(infoReposo.inf, "      %d%d:%d%d:%d%d", horas/10, horas%10, minutos/10, minutos%10, segundos/10, segundos%10);
	osMessageQueuePut(mid_MsgQueueLCD, &infoReposo, 0U, 0U);
}

static void activo(void){
	osMessageQueueGet(mid_MsgQueueTEM,&temMED, NULL, 1000U);
	double x = temREF - temMED;
	if(5 <= x)
	{
		cicloPWM = 100;
		osMessageQueuePut(mid_MsgQueuePWM, &cicloPWM, 0U, 0U);
		setLeds(0,0,1);
	}else if(0 <= x && x < 5)
	{
		cicloPWM = 80;
		osMessageQueuePut(mid_MsgQueuePWM, &cicloPWM, 0U, 0U);
		setLeds(0,1,1);
	}else if(-5 < x && x < 0)
	{
		cicloPWM = 40;
		osMessageQueuePut(mid_MsgQueuePWM, &cicloPWM, 0U, 0U);
		setLeds(1,1,0);
	}else
	{
		cicloPWM = 0;
		osMessageQueuePut(mid_MsgQueuePWM, &cicloPWM, 0U, 0U);
		setLeds(1,0,0);
	}
	
	sprintf(medidasBuffer[numMedida], "%d%d:%d%d:%d%d--Tm:%0.1f--Tr:%0.1f--D:%d%d%%", horas/10, horas%10, minutos/10, minutos%10, segundos/10, segundos%10, temMED, temREF, cicloPWM/10, cicloPWM%10);
	numMedida = (numMedida == 9) ? 0 : (numMedida + 1);
	
	escribir_t infoReposo;
	infoReposo.linea = 1;
	sprintf(infoReposo.inf, "   ACT---%d%d:%d%d:%d%d---", horas/10, horas%10, minutos/10, minutos%10, segundos/10, segundos%10);
	osMessageQueuePut(mid_MsgQueueLCD, &infoReposo, 0U, 0U);
	infoReposo.linea = 2;
	sprintf(infoReposo.inf, "Tm:%0.1f'-Tr:%0.1f'-D:%d%d%%", temMED, temREF, cicloPWM/10, cicloPWM%10);
	osMessageQueuePut(mid_MsgQueueLCD, &infoReposo, 0U, 0U);
}

static void test(void){
	osMessageQueueGet(mid_MsgQueuePOT,&potMedidas, NULL, 1000U);
	double x = potMedidas.temperaturaREF - potMedidas.temperaturaMED;
	if(5 <= x)
	{
		cicloPWM = 100;
		osMessageQueuePut(mid_MsgQueuePWM, &cicloPWM, 0U, 0U);
		setLeds(0,0,1);
	}else if(0 <= x && x < 5)
	{
		cicloPWM = 80;
		osMessageQueuePut(mid_MsgQueuePWM, &cicloPWM, 0U, 0U);
		setLeds(0,1,1);
	}else if(-5 < x && x < 0)
	{
		cicloPWM = 40;
		osMessageQueuePut(mid_MsgQueuePWM, &cicloPWM, 0U, 0U);
		setLeds(1,1,0);
	}else
	{
		cicloPWM = 0;
		osMessageQueuePut(mid_MsgQueuePWM, &cicloPWM, 0U, 0U);
		setLeds(1,0,0);
	}
	
	escribir_t infoReposo;
	infoReposo.linea = 1;
	sprintf(infoReposo.inf, "   TEST---%d%d:%d%d:%d%d---", horas/10, horas%10, minutos/10, minutos%10, segundos/10, segundos%10);
	osMessageQueuePut(mid_MsgQueueLCD, &infoReposo, 0U, 0U);
	infoReposo.linea = 2;
	sprintf(infoReposo.inf, "Tm:%0.1f'-Tr:%0.1f'-D:%d%d%%", potMedidas.temperaturaMED, potMedidas.temperaturaREF, cicloPWM/10, cicloPWM%10);
	osMessageQueuePut(mid_MsgQueueLCD, &infoReposo, 0U, 0U);
}

static void programacion(){
	intTemMod = temRefMod;
	doubTemMod = temRefMod - intTemMod;
	escribir_t infoReposo;
	infoReposo.linea = 1;
	sprintf(infoReposo.inf, "      ---P&D---");
	osMessageQueuePut(mid_MsgQueueLCD, &infoReposo, 0U, 0U);
	infoReposo.linea = 2;
	sprintf(infoReposo.inf, "   H:%d%d:%d%d:%d%d---Tr:%d%0.1f'", horasMod/10, horasMod%10, minutosMod/10, minutosMod%10, segundosMod/10, segundosMod%10, intTemMod/10, (intTemMod%10 + doubTemMod));
	osMessageQueuePut(mid_MsgQueueLCD, &infoReposo, 0U, 0U);
	osMessageQueueGet(mid_MsgQueuePCRX,&recived, 0U, 0U);
	switch(recived.c)
	{
		case HORA:
			horas = recived.hora;
			minutos = recived.minutos;
			segundos = recived.segundos;
			osMessageQueuePut(mid_MsgQueuePCTX, &recived, 0U, 0U);
			recived.c = 0x00;
			break;
		case TEMPERATURA:
			temREF = recived.temperaturaREF;
			osMessageQueuePut(mid_MsgQueuePCTX, &recived, 0U, 0U);
			recived.c = 0x00;
			break;
		case MEDIDAS:
			for(int i = 0; i < 10; i++)
			{
				
			}
			osMessageQueuePut(mid_MsgQueuePCTX, &recived, 0U, 0U);
			recived.c = 0x00;
			break;
		case BORRAR:
			for(int i = 0; i < 10; i++)
			{
				memset(medidasBuffer[i], 0x00, sizeof(medidasBuffer[i]));
			}
			osMessageQueuePut(mid_MsgQueuePCTX, &recived, 0U, 0U);
			recived.c = 0x00;
			break;
	}
	osMessageQueueGet(mid_MsgQueueJOY,&pulsacion, 0U, 0U);
	if((pulsacion.corta == 1))
	{
		switch(pulsacion.direccion)
		{
			case DERECHA:
				if(varMod != 0x0A)
				{
					varMod = (varMod == 0x01) ? 0x0A : varMod >> 1;
					if(varMod == 0x20 || varMod == 0x08 || varMod == 0x0A)
					{
						pos = pos + 8;
					}else if(varMod == 0x02)
					{
						pos = pos + 28;
					}else{
						pos = pos + 6;
					}
					pulsacion.direccion = 0x00;
				}
				break;
			case IZQUIERDA:
				if(varMod != 0x80)
				{
					varMod = (varMod == 0x0A) ? 0x01 : varMod << 1;
					if(varMod == 0x40 || varMod == 0x10 || varMod == 0x0A)
					{
						pos = pos - 8;
					}else if(varMod == 0x04)
					{
						pos = pos - 28;
					}else{
						pos = pos - 6;
					}
				}
				pulsacion.direccion = 0x00;
				break;
			case ARRIBA:
				switch(varMod)
				{
					case 0x80:
						if(horasMod > 14 && horasMod < 20)
						{
							horasMod = horasMod - 10;
						}else if(horasMod >= 20 && horasMod <= 24)
						{
							horasMod = horasMod - 20;
						}else
						{
							horasMod = horasMod + 10;
						}
						break;
					case 0x40:
						horasMod = (horasMod == 24) ? 0 : (horasMod + 1);
						break;
					case 0x20:
						minutosMod = (minutosMod > 49) ? (minutosMod - 50) : (minutosMod + 10);
						break;
					case 0x10:
						minutosMod = (minutosMod == 59) ? 0 : (minutosMod + 1);
						break;
					case 0x08:
						segundosMod = (segundosMod > 49) ? (segundosMod - 50) : (segundosMod + 10);
						break;
					case 0x04:
						segundosMod = (segundosMod == 59) ? 0 : (segundosMod + 1);
						break;
					case 0x02:
						temRefMod = (temRefMod > 20.0) ? (temRefMod - 20) : (temRefMod + 10);
						break;
					case 0x01:
						temRefMod = (temRefMod == 30.0) ? 5 : (temRefMod + 1);
						break;
					case 0x0A:
						temRefMod = (temRefMod == 30.0) ? 5 : (temRefMod + 0.5);
						break;
				}
				pulsacion.direccion = 0x00;
				break;
			case ABAJO:
				switch(varMod)
				{
					case 0x80:
						horasMod = (horasMod < 10) ? (horasMod + 20) : (horasMod - 10);
						break;
					case 0x40:
						horasMod = (horasMod == 0) ? 24 : (horasMod - 1);
						break;
					case 0x20:
						minutosMod = (minutosMod < 10) ? (minutosMod + 50) : (minutosMod - 10);
						break;
					case 0x10:
						minutosMod = (minutosMod == 0) ? 59 : (minutosMod - 1);
						break;
					case 0x08:
						segundosMod = (segundosMod < 10) ? (segundosMod + 50) : (segundosMod - 10);
						break;
					case 0x04:
						segundosMod = (segundosMod == 0) ? 59 : (segundosMod - 1);
						break;
					case 0x02:
						temRefMod = (temRefMod < 10.0) ? (temRefMod + 20) : (temRefMod - 10);
						break;
					case 0x01:
						temRefMod = (temRefMod <= 5.5) ? 30.0 : (temRefMod - 1);
						break;
					case 0x0A:
						temRefMod = (temRefMod == 5.0) ? 30.0 : (temRefMod - 0.5);
						break;
				}
				pulsacion.direccion = 0x00;
				break;
			case CENTRO:
				horas = horasMod;
				minutos = minutosMod;
				segundos = segundosMod;
				temREF = temRefMod;
				pulsacion.direccion = 0x00;
				break;
		}
	}
	for(int i = 0; i < 8; i++)
	{
		LCD_modificarByteBuffer((pos+i), 3, 0x04);
	}
}
 
static void modoFuncionamiento(void){
	
	switch(modos){
		case REPOSO:
			
			reposo();
			osMessageQueueGet(mid_MsgQueueJOY,&pulsacion, NULL, 1000U);
			if((pulsacion.corta == 0) && (pulsacion.direccion == CENTRO))
			{
				pulsacion.direccion = 0x00;
				modos = ACTIVO;
			}
			break;
					
		case ACTIVO:
			
			osTimerStart(tim_medida,1000U);
			activo();
			osMessageQueueGet(mid_MsgQueueJOY,&pulsacion, NULL, 1000U);
			if((pulsacion.corta == 0) && (pulsacion.direccion == CENTRO))
			{
				pulsacion.direccion = 0x00;
				modos = TEST;
			}
			break;
		
		case TEST:
			
			osTimerStop(tim_medida);
			osTimerStart(tim_pot,1000U);
			test();
			osMessageQueueGet(mid_MsgQueueJOY,&pulsacion, NULL, 1000U);
			if((pulsacion.corta == 0) && (pulsacion.direccion == CENTRO))
			{
				horasMod = horas;
				minutosMod = minutos;
				segundosMod = segundos;
				temRefMod = temREF;
				pulsacion.direccion = 0x00;
				modos = PROGRAMACION;
			}
			break;
		
		case PROGRAMACION:
			
			setLeds(0,0,0);
			osTimerStop(tim_pot);
			programacion();
			osMessageQueueGet(mid_MsgQueueJOY,&pulsacion, NULL, 1000U);
			if((pulsacion.corta == 0) && (pulsacion.direccion == CENTRO))
			{
				pos = 29;
				varMod = 0x80;
				pulsacion.direccion = 0x00;
				modos = REPOSO;
			}
			break;

	}
}

void principal (void *argument) {
	pulsacion.direccion = 0x00;
	
  while (1) {
		
		modoFuncionamiento();
    
    osThreadYield();                            // suspend thread
  }
}
