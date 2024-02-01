#include <stdio.h>
#include "com.h"
#include "string.h"

/*---------------------- CONSTANTES --------------------------*/	
static const char INI=0x01; //Inicio de una trama
static const char FIN=0xFE; //Final de una trama


/*---------------- VARIABLES NO GLOBALES ---------------------*/	
static ARM_DRIVER_USART *USARTdrv = &Driver_USART3; //driver de la interfaz USART
static osThreadId_t tid_ThcomTX; //id del hilo que gestiona la temperatura 
static osThreadId_t tid_ThcomRX; //id del hilo que gestiona la temperatura 
	/*------ TRAMAS ------*/
static Estado receive = ESPERANDO_INICIO; //estado para cuando se recibe una trama
static Estado transfer = ESPERANDO_INICIO; //estado para cuando se debe enviar una trama
		/*------ TRAMA RECIBIDA ------*/
static Trama tramaRX; //comando e inforamacion relevante que se debe enviar en la cola de mensajes
static volatile char recibidoRX[256]; //caracter recibido: INICIO|COMANDO|LONGITUD|FIN
static uint8_t posicion = 0; //posicion del array recibidoRX que se correspende con el inicio de la trama recibida 
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



/*---------- DECLARACION FUNCIONES NO GLOBALES ---------------*/	
static int8_t USART_Init(void);
static void USART_callback(uint32_t event);
__NO_RETURN static void ThcomTX (void *argument);
__NO_RETURN static void ThcomRX (void *argument);
static void getInformacionCola(void);
static void putInforamcionCola(void);
static void procesarDatosRecibidos( char recibido[12] );
static void procesarDatosTransmitidos(void);


/*------------------ CODIGO FUNCIONES -----------------------*/	

static void USART_callback(uint32_t event){
	
	switch(event){
		
		//Datos enviados correctamente al PC
		case ARM_USART_EVENT_SEND_COMPLETE:
			//Creamos la trama a enviar
			if( !tramaCompletaTX ){ //Si todav?a la trama no est? creada, seguimos enviandola
				procesarDatosTransmitidos();
			} else{ //la trama se ha enviado completa
				tramaCompletaTX = false;
			}
			
		break;
	}
	
	 
}

static void controlarTramaRecibida(void){

	char rec[13];
	
	while( USARTdrv->GetRxCount() < posicion + 2){
	}
	if(recibidoRX[posicion+1] == HORA){
		
		while( USARTdrv->GetRxCount() < posicion + 12){
		}
		strncpy(rec, recibidoRX + posicion, 12);
		posicion = posicion + 12;
		procesarDatosRecibidos(rec);
	}else if( recibidoRX[posicion + 1] == TEMPERATURA ){
		
		while( USARTdrv->GetRxCount() < posicion + 8){
		}
		strncpy(rec, recibidoRX + posicion, 8);
		posicion = posicion + 8;
		procesarDatosRecibidos(rec);
		
	} else if( recibidoRX[posicion + 1] == MEDIDAS || recibidoRX[posicion + 1] == BORRAR){
		
		while( USARTdrv->GetRxCount() < 4){
		}
		strncpy(rec, recibidoRX + posicion, 4);
		posicion = posicion + 4;
		procesarDatosRecibidos(rec);
	}
	
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
  tid_ThcomTX = osThreadNew(ThcomTX, NULL, NULL);
  if (tid_ThcomTX == NULL | status == -1) {
    return(-1);
  }
  
  //Creamos el hilo que gestiona el driver USART
  tid_ThcomRX = osThreadNew(ThcomRX, NULL, NULL);
  if (tid_ThcomRX == NULL | status == -1) {
    return(-1);
  }
 
  return(0);
}


__NO_RETURN static void ThcomTX (void *argument){
	
	osThreadFlagsSet(tid_ThcomTX, S_GET);
	
	while(1){
		// Informaci?n a enviar al PC
		osThreadFlagsWait(S_GET, osFlagsWaitAny, osWaitForever);
		getInformacionCola();			
		
		osThreadYield(); //Hilo suspendido
	}
}


__NO_RETURN static void ThcomRX (void *argument){
	
	
	//Iniciamos la lectura de la informaci?n recibida por el PC
	USARTdrv->Receive(&recibidoRX, 256);
	
	while(1){
		
		//osThreadFlagsWait(S_PUT, NULL, osWaitForever);
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
	switch(transfer){
		case ESPERANDO_INICIO:
			
			//Enviamos el inicio de la trama
			USARTdrv->Send(&INI,1);
		
			//Pasamos a enviar el comando
			transfer = COMANDO;
		break;
		
		case COMANDO:
			
			//Enviamos el comando de la trama
			USARTdrv->Send(&comandoTX,1);
		
			//Pasamos a enviar la longitud
			transfer = LONGITUD;
		break;
		
		case LONGITUD:
			
			//Establecemos la longitud que debemos enviar dependiendo del comando recibido
			longitudTX = 	( tramaTX.c == HORA)        ?	0x0C :
										( tramaTX.c == TEMPERATURA) ? 0x08 :
										( tramaTX.c == MEDIDAS)     ?	0x25 :
																									0x04 ;
			
			//Enviamos la longitud de la trama
			USARTdrv->Send(&longitudTX,1);
		
			//Pasamos a enviar los datos o enviamos el final de trama
			transfer = ( tramaTX.c == BORRAR ) ?		TRAMA_COMPLETA : //Enviamos el caracter de fin
																							DATOS; //Pasamos a enviar los datos
		
		break;			
		
		case DATOS:
			
			//Enviamos los datos 
			if( tramaTX.c == HORA ){ //el comando nos pide la puesta en hora	
				//Convertimos la hora en una cadena de caracteres
				snprintf( horaTX, sizeof(horaTX), "%d:%d:%d", tramaTX.hora, tramaTX.minutos, tramaTX.segundos);
				
				//Enviamos la hora a establecer
				USARTdrv->Send(horaTX,8);
				
			} else if( tramaTX.c == TEMPERATURA){ //el comando nos pide establecer el valor de la temperatura de referencia
				//Convertimos la temperatura 
				snprintf(temperaturaTX,sizeof(temperaturaTX),"%.1f", tramaTX.temperatura);

				//Enviamos la temperatura de referencia a establecer
				USARTdrv->Send(temperaturaTX,4);
			} else{
				
				//Convertimos la cadena de caracteres de la medida
				snprintf(medidaTX,sizeof(medidaTX),"%d:%d:%d--Tm:%1.f--Tr:%1.f--D:%1.f%%", tramaTX.hora, tramaTX.minutos, 
									tramaTX.segundos, (double)tramaTX.temperatura, (double)tramaTX.temperaturaREF,	(double)tramaTX.PWM);

				//Enviamos la temperatura de referencia a establecer
				USARTdrv->Send(medidaTX,30);
			}
			
			//Pasamos a enviar el final de la trama
			transfer = TRAMA_COMPLETA;
		break;
			
		case TRAMA_COMPLETA:
			
			//Enviamos el final de la trama
			USARTdrv->Send(&FIN,1);
		
			//Marcamos que la trama que debiamos enviar se ha completado
			tramaCompletaTX = true;
		
			//Volvemos a esperar para la creacion de una nueva trama
			transfer = ESPERANDO_INICIO;
		
			//Volvemos a esperar a recibir un nuevo comando
			osThreadFlagsSet(tid_ThcomTX, S_GET);
		break;
	}
}