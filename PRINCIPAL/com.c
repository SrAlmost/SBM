#include <stdio.h>
#include "com.h"
#include "string.h"

/*---------------------- CONSTANTES --------------------------*/	
static const char INI=0x01; //Inicio de una trama
static const char FIN=0xFE; //Final de una trama



/*---------------- VARIABLES NO GLOBALES ---------------------*/	
static ARM_DRIVER_USART *USARTdrv = &Driver_USART3; //driver de la interfaz USART
osThreadId_t tid_ThcomTX; //id del hilo que gestiona la temperatura 
static osThreadId_t tid_ThcomRX; //id del hilo que gestiona la temperatura 
	/*------ TRAMAS ------*/
static Estado receive = ESPERANDO_INICIO; //estado para cuando se recibe una trama
static Estado transfer = ESPERANDO_INICIO; //estado para cuando se debe enviar una trama
		/*------ TRAMA RECIBIDA ------*/
 Trama tramaRX; //comando e inforamacion relevante que se debe enviar en la cola de mensajes
static char recibidoRX[13]; //caracter recibido: INICIO|COMANDO|LONGITUD|FIN
static uint8_t comandoRX; //comando recibido
static char horaRX[8]; //hora recibida
static char temperaturaRX[4]; //temperatura recibida
		/*------ TRAMA ENVIADA ------*/
static Trama tramaTX; //comando e inforamacion relevante que se recibe de la cola de mensajes
static uint8_t comandoTX; //comando a enviar		
static char horaTX[9]; //hora a enviar (tama?o = 8 bytes a enviar + /0)
static char temperaturaTX[5]; //temperatura a enviar en formato texto (tama?o = 4 bytes a enviar + /0)
static char medidaTX[30]; //medida a pasar al PC
static uint8_t longitudTX; //longitud a enviar
static bool tramaCompletaTX=false; //variable para concretar si se ha enviado la trama completa
static char envio[40];
static uint8_t intTempRef; 
static uint8_t intTemp; 
static uint8_t intPWM;
/*---------- DECLARACION FUNCIONES NO GLOBALES ---------------*/	
static int8_t USART_Init(void);
static void USART_callback(uint32_t event);
__NO_RETURN static void ThcomTX (void *argument);
__NO_RETURN static void ThcomRX (void *argument);
static void getInformacionCola(void);
static void putInforamcionCola(void);
static void procesarDatosRecibidos( char recibido[12] );
static void procesarDatosTransmitidos(void);


int cuenta;
/*------------------ CODIGO FUNCIONES -----------------------*/	
//Creamos la trama a enviar
//			if( !tramaCompletaTX ){ //Si todav?a la trama no est? creada, seguimos enviandola
//				procesarDatosTransmitidos();
//			} else{ //la trama se ha enviado completa
//				tramaCompletaTX = false;
//			}
static void USART_callback(uint32_t event){
	if(event == ARM_USART_EVENT_SEND_COMPLETE)
	{
		osThreadFlagsSet(tid_ThcomTX, 0x01);
	}else if(event == ARM_USART_EVENT_RECEIVE_COMPLETE)
	{
		osThreadFlagsSet(tid_ThcomRX, 0x10);
	}
//	switch(event){
//		//Datos enviados correctamente al PC
//		case ARM_USART_EVENT_SEND_COMPLETE:
//			osThreadFlagsSet(tid_ThcomTX, 0x01);
//			break;
//		case ARM_USART_EVENT_RECEIVE_COMPLETE:
//			osThreadFlagsSet(tid_ThcomRX, 0x10);
//			break;
//	}
	
	 
}

static void controlarTramaRecibida(void){
	cuenta = USARTdrv->GetRxCount();
	
	while( USARTdrv->GetRxCount() < 2){
	}
	if(recibidoRX[1] == HORA){
		
		while( USARTdrv->GetRxCount() < 12){
		}
		
	}else if( recibidoRX[1] == TEMPERATURA ){
		
		while( USARTdrv->GetRxCount() < 8){
		}
		
		USARTdrv->Control(ARM_USART_ABORT_RECEIVE, 1);
	} else if( recibidoRX[1] == MEDIDAS | recibidoRX[1] == BORRAR){
		
		while( USARTdrv->GetRxCount() < 4){
		}
		
		USARTdrv->Control(ARM_USART_ABORT_RECEIVE, 1);
	}
	
	procesarDatosRecibidos(recibidoRX);
	memset(recibidoRX, 0 , sizeof(recibidoRX) );
}



static int8_t USART_Init(void){
	
	if( USARTdrv->Initialize( USART_callback ) != ARM_DRIVER_OK //inicializamos la interfaz USART
		| USARTdrv->PowerControl(ARM_POWER_FULL) != ARM_DRIVER_OK //control de la alimentaci?n del USART
		| USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS | //modo de comunicacion asincrono
			ARM_USART_DATA_BITS_8 | //datos de 8 bits
			ARM_USART_PARITY_NONE | //no hay paridad
			ARM_USART_STOP_BITS_1 | //1 bit de stop
			ARM_USART_FLOW_CONTROL_NONE,//no hay control de flujo en la comunicacion
			115200) != ARM_DRIVER_OK // la velocidad de transmision ser? de 115200 baudios
		| USARTdrv->Control(ARM_USART_CONTROL_TX,1) != ARM_DRIVER_OK //habilitamos la transmision de datos
		| USARTdrv->Control(ARM_USART_CONTROL_RX,1) != ARM_DRIVER_OK //habilitamos la recepcion de datos
	){
		return (-1);	
	}
	
	return 0;
}


int8_t Init_Thcom(void){
	
	int8_t status; //estado de las funciones para comprobar si se ha producido algun error;
	
	//Inicializamos el bus USART
	status = USART_Init();
		//Comprobamos que se ha inicializado correctamente el bus USART
	if( status == -1 ){
		return (-1);
	}
	
	//Creamos el hilo que gestiona el driver USART
  tid_ThcomRX = osThreadNew(ThcomRX, NULL, NULL);
  if (tid_ThcomRX == NULL | status == -1) {
    return(-1);
  }
	
	//Creamos el hilo que gestiona el driver USART
  tid_ThcomTX = osThreadNew(ThcomTX, NULL, NULL);
  if (tid_ThcomTX == NULL | status == -1) {
    return(-1);
  }
 
  return(0);
}


__NO_RETURN static void ThcomTX (void *argument){
	
	osThreadFlagsSet(tid_ThcomTX, S_GET);
	
	while(1){
		// Informaci?n a enviar al PC
		//osThreadFlagsWait(S_GET, osFlagsWaitAny, osWaitForever);
		getInformacionCola();			
		
		osThreadYield(); //Hilo suspendido
	}
}


__NO_RETURN static void ThcomRX (void *argument){
	
	//Iniciamos la lectura de la informaci?n recibida por el PC
	

	
	while(1){
		USARTdrv->Receive(&recibidoRX, 12);
		osThreadFlagsWait(0x10, osFlagsWaitAny, osWaitForever);
		controlarTramaRecibida();
		
		
		osThreadYield(); //Hilo suspendido
	}
}

static void getInformacionCola(void){
	
	//Sacamos la informacion que debemos realizar de la cola
	osMessageQueueGet(mid_MsgQueuePCTX,&tramaTX,0U,osWaitForever);
	
	//Realizamos el complementario del comando ya que ser? lo que haya que mandar
	comandoTX = ~tramaTX.c;
	
	//Procedemos a enviar la informacion al PC
	procesarDatosTransmitidos();
}


static void putInforamcionCola( void ){
	
	//Guardamos la informaci?n que debemos enviar
	tramaRX.c = comandoRX; //comando recibido
	if(comandoRX == HORA){ //hora a establecer recibida
		sscanf( horaRX, "%d:%d:%d", &tramaRX.hora, &tramaRX.minutos, &tramaRX.segundos);
	} 
	else if (comandoRX == TEMPERATURA){ //temperatura de referencia recibida
		sscanf( temperaturaRX, "%f", &tramaRX.temperaturaREF );
	} 
	
	//Metemos la informaci?n en la cola
	osMessageQueuePut(mid_MsgQueuePCRX,&tramaRX,0U,osWaitForever);
}


static void procesarDatosRecibidos( char recibido[12] ){	

	
	if( receive == ESPERANDO_INICIO){
		
		//Comprobamos si lo recibido se corresponde con el caracter de inicio
		if( recibido[0] == INI ){
			receive = COMANDO;
		}

	}
		
	if( receive == COMANDO){
		
		//Comprobamos si lo recibido se corresponde con un comando
		if( recibido[1] == HORA || recibido[1] == TEMPERATURA || recibido[1] == MEDIDAS || recibido[1] == BORRAR ){
			
			//Pasamos a la longitud de la trama
			receive = LONGITUD;
			
			//Guardamos el comando
			comandoRX = recibido[1];
			
		}else if(recibido[1] != INI){ /*Si no se corresponde, la trama ser? erronea y volveremos a buscar el caracter de inicio 
																 (a no ser que se envie el caracter de inicio, en este caso seguimos buscando el comando)*/
			
			//Volvemos a buscar el principio de la trama
			receive = ESPERANDO_INICIO;
		
		}
		
	}
		
	if(receive == LONGITUD){
		
		//Comprobamos si lo recibido se corresponde con la longitud asociada al comando
		switch(comandoRX){
			
			case HORA:
				
				if(recibido[2] == 0x0C ){ //longitud del comando: establecer hora
					
					//Pasamos a buscar la hora
					receive = DATOS;
				} else if(recibido[2] == INI){ //Se ha enviado el caracter de inicio, por lo tanto procedemos reiniciamos la trama y buscamos el comando
			
					//Procedemos a buscar el comando
					receive = COMANDO;
					
					
				} else{ //Si no se corresponde con ningun caracter valido, la trama ser? erronea y volveremos a buscar el caracter de inicio 
					
					//Volvemos a buscar el principio de la trama
					receive = ESPERANDO_INICIO;
					
				}
					
			break;
			
			case TEMPERATURA:
					
				if(recibido[2] == 0x08 ){ //longitud del comando: establecer temperatura referencia
					//Obtenemos la temperatura
					USARTdrv->Receive(&temperaturaRX,4);
					
					//Pasamos a buscar la temperatura
					receive = DATOS;
				} else if(recibido[2] == INI){ //Se ha enviado el caracter de inicio, por lo tanto procedemos reiniciamos la trama y buscamos el comando
			
					//Procedemos a buscar el comando
					receive = COMANDO;
					
					
				} else{ //Si no se corresponde con ningun caracter valido, la trama ser? erronea y volveremos a buscar el caracter de inicio 
					
					//Volvemos a buscar el principio de la trama
					receive = ESPERANDO_INICIO;
					
				}
				
			break;
			
			default:
				
				if(recibido[2] == 0x04 ){ //longitud del comando: solicitar/borrar medidas
					
					//Pasamos a buscar el caracter del final de la trama
					receive = TRAMA_COMPLETA;
					
				}else if(recibido[2] == INI){ //Se ha enviado el caracter de inicio, por lo tanto procedemos reiniciamos la trama y buscamos el comando
			
					//Procedemos a buscar el comando
					receive = COMANDO;
					
				} else{ //Si no se corresponde con ningun caracter valido, la trama ser? erronea y volveremos a buscar el caracter de inicio 
					
					//Volvemos a buscar el principio de la trama
					receive = ESPERANDO_INICIO;
					
				}
				
			break;
		}
	}
		
	if( receive == DATOS){
		
		if(comandoRX == HORA){
			memcpy(horaRX, recibido + 3, 8);
		}else if( comandoRX == TEMPERATURA){
			memcpy(temperaturaRX, recibido + 3, 4);
		}
		
		//Pasamos a buscar el caracter del final de la trama
		receive = TRAMA_COMPLETA;
	}
	
	
	if( receive == TRAMA_COMPLETA ){
		
		
		
		switch(comandoRX){
			case HORA:
				if( recibido[ 11 ] == FIN){ //Si se recibe el caracter de fin pasamos a enviar la trama recibida en la cola de mensajes
					putInforamcionCola();
					receive = ESPERANDO_INICIO;
			
				}else if( recibido[ 11 ] == INI){ //Se ha enviado el caracter de inicio, por lo tanto procedemos reiniciamos la trama y buscamos el comando
					
					//Procedemos a buscar el comando
					receive = COMANDO;
							
				} else{ //Si no se corresponde con ningun caracter valido, la trama ser? erronea y volveremos a buscar el caracter de inicio 
							
					//Volvemos a buscar el principio de la trama
					receive = ESPERANDO_INICIO;
							
				}
			break;
			
			case TEMPERATURA:
				if( recibido[ 7 ] == FIN){ //Si se recibe el caracter de fin pasamos a enviar la trama recibida en la cola de mensajes
					putInforamcionCola();
					receive = ESPERANDO_INICIO;
					
				}else if( recibido[ 7 ] == INI){ //Se ha enviado el caracter de inicio, por lo tanto procedemos reiniciamos la trama y buscamos el comando
					
					//Procedemos a buscar el comando
					receive = COMANDO;
							
				} else{ //Si no se corresponde con ningun caracter valido, la trama ser? erronea y volveremos a buscar el caracter de inicio 
							
					//Volvemos a buscar el principio de la trama
					receive = ESPERANDO_INICIO;
							
				}
			break;
			
			default:
				if( recibido[ 3 ] == FIN){ //Si se recibe el caracter de fin pasamos a enviar la trama recibida en la cola de mensajes
					putInforamcionCola();
					receive = ESPERANDO_INICIO;
					
				}else if( recibido[ 3 ] == INI){ //Se ha enviado el caracter de inicio, por lo tanto procedemos reiniciamos la trama y buscamos el comando
					
					//Procedemos a buscar el comando
					receive = COMANDO;
							
				} else{ //Si no se corresponde con ningun caracter valido, la trama ser? erronea y volveremos a buscar el caracter de inicio 
							
					//Volvemos a buscar el principio de la trama
					receive = ESPERANDO_INICIO;
							
				}
			break;
		}
		
		
		
	}

}


static void procesarDatosTransmitidos(void){
	switch(tramaTX.c)
	{
		case HORA:
			memset(envio, 0x00, sizeof(envio));
			envio[0] = INI;
			envio[1] = tramaTX.c;
			envio[2] = 0x0C;
			envio[3] = (tramaTX.hora/10) + '0';
			envio[4] = (tramaTX.hora%10) + '0';
			envio[5] = ':';
			envio[6] = (tramaTX.minutos/10) + '0';
			envio[7] = (tramaTX.minutos%10) + '0';
			envio[8] = ':';
			envio[9] = (tramaTX.segundos/10) + '0';
			envio[10] = (tramaTX.segundos%10) + '0';
			envio[11] = FIN;
			USARTdrv->Send(envio, envio[2]);
			osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
			break;
		case TEMPERATURA:
			memset(envio, 0x00, sizeof(envio));
			intTempRef = tramaTX.temperaturaREF;
			envio[0] = INI;
			envio[1] = tramaTX.c;
			envio[2] = 0x08;
			envio[3] = (intTempRef/10) + '0';
			envio[4] = (intTempRef%10) + '0';
			envio[5] = '.';
			envio[6] = ((tramaTX.temperaturaREF - intTempRef)*10) + '0';
			envio[7] = FIN;
			USARTdrv->Send(envio, envio[2]);
			osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
			break;
		case MEDIDAS:
			//snprintf(medidaTX,sizeof(medidaTX),"%d:%d:%d--Tm:%1.f--Tr:%1.f--D:%1.f%%", tramaTX.hora, tramaTX.minutos, tramaTX.segundos, (double)tramaTX.temperatura, (double)tramaTX.temperaturaREF,	(double)tramaTX.PWM);
			memset(envio, 0x00, sizeof(envio));
			intTempRef = tramaTX.temperaturaREF;
			intTemp = tramaTX.temperatura;
			intPWM = tramaTX.PWM;
			envio[0] = INI;
			envio[1] = tramaTX.c;
			envio[2] = 0x25;
			envio[3] = (tramaTX.hora/10) + '0';
			envio[4] = (tramaTX.hora%10) + '0';
			envio[5] = ':';
			envio[6] = (tramaTX.minutos/10) + '0';
			envio[7] = (tramaTX.minutos%10) + '0';
			envio[8] = ':';
			envio[9] = (tramaTX.segundos/10) + '0';
			envio[10] = (tramaTX.segundos%10) + '0';
			envio[11] = '-';
			envio[12] = '-';
			envio[13] = 'T';
			envio[14] = 'm';
			envio[15] = ':';
			envio[16] = (intTemp/10) + '0';
			envio[17] = (intTemp%10) + '0';
			envio[18] = '.';
			envio[19] = ((tramaTX.temperatura - intTemp)*10) + '0';
			envio[20] = '-';
			envio[21] = '-';
			envio[22] = 'T';
			envio[23] = 'r';
			envio[24] = ':';
			envio[25] = (intTempRef/10) + '0';
			envio[26] = (intTempRef%10) + '0';
			envio[27] = '.';
			envio[28] = ((tramaTX.temperaturaREF - intTempRef)*10) + '0';
			envio[29] = '-';
			envio[30] = '-';
			envio[31] = 'D';
			envio[32] = ':';
			envio[33] = (intPWM/10) + '0';
			envio[34] = (intPWM%10) + '0';
			envio[35] = '%';
			envio[36] = FIN;
			USARTdrv->Send(envio, envio[2]);
			osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
			break;
		case BORRAR:
			memset(envio, 0x00, sizeof(envio));
			envio[0] = INI;
			envio[1] = tramaTX.c;
			envio[2] = 0x04;
			envio[3] = FIN;
			USARTdrv->Send(envio, envio[2]);
			osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
			break;
	}
//	switch(transfer){
//		case ESPERANDO_INICIO:
//			
//			//Enviamos el inicio de la trama
//			USARTdrv->Send(&INI,1);
//		
//			//Pasamos a enviar el comando
//			transfer = COMANDO;
//		break;
//		
//		case COMANDO:
//			
//			//Enviamos el comando de la trama
//			USARTdrv->Send(&comandoTX,1);
//		
//			//Pasamos a enviar la longitud
//			transfer = LONGITUD;
//		break;
//		
//		case LONGITUD:
//			
//			//Establecemos la longitud que debemos enviar dependiendo del comando recibido
//			longitudTX = 	( tramaTX.c == HORA)        ?	0x0C :
//										( tramaTX.c == TEMPERATURA) ? 0x08 :
//										( tramaTX.c == MEDIDAS)     ?	0x25 :
//																									0x04 ;
//			
//			//Enviamos la longitud de la trama
//			USARTdrv->Send(&longitudTX,1);
//		
//			//Pasamos a enviar los datos o enviamos el final de trama
//			transfer = ( tramaTX.c == BORRAR ) ?		TRAMA_COMPLETA : //Enviamos el caracter de fin
//																							DATOS; //Pasamos a enviar los datos
//		
//		break;			
//		
//		case DATOS:
//			
//			//Enviamos los datos 
//			if( tramaTX.c == HORA ){ //el comando nos pide la puesta en hora	
//				//Convertimos la hora en una cadena de caracteres
//				snprintf( horaTX, sizeof(horaTX), "%d:%d:%d", tramaTX.hora, tramaTX.minutos, tramaTX.segundos);
//				
//				//Enviamos la hora a establecer
//				USARTdrv->Send(horaTX,8);
//				
//			} else if( tramaTX.c == TEMPERATURA){ //el comando nos pide establecer el valor de la temperatura de referencia
//				//Convertimos la temperatura 
//				snprintf(temperaturaTX,sizeof(temperaturaTX),"%.1f", tramaTX.temperatura);

//				//Enviamos la temperatura de referencia a establecer
//				USARTdrv->Send(temperaturaTX,4);
//			} else{
//				
//				//Convertimos la cadena de caracteres de la medida
//				snprintf(medidaTX,sizeof(medidaTX),"%d:%d:%d--Tm:%1.f--Tr:%1.f--D:%1.f%%", tramaTX.hora, tramaTX.minutos, 
//									tramaTX.segundos, (double)tramaTX.temperatura, (double)tramaTX.temperaturaREF,	(double)tramaTX.PWM);

//				//Enviamos la temperatura de referencia a establecer
//				USARTdrv->Send(medidaTX,30);
//			}
//			
//			//Pasamos a enviar el final de la trama
//			transfer = TRAMA_COMPLETA;
//		break;
//			
//		case TRAMA_COMPLETA:
//			
//			//Enviamos el final de la trama
//			USARTdrv->Send(&FIN,1);
//		
//			//Marcamos que la trama que debiamos enviar se ha completado
//			tramaCompletaTX = true;
//		
//			//Volvemos a esperar para la creacion de una nueva trama
//			transfer = ESPERANDO_INICIO;
//		
//			//Volvemos a esperar a recibir un nuevo comando
//			osThreadFlagsSet(tid_ThcomTX, S_GET);
//		break;
//	}
}