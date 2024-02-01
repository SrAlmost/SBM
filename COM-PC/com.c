#include "com.h"
#include "string.h"

/*---------------------- CONSTANTES --------------------------*/	
static const uint8_t INI=0x01; //Inicio de una trama
static const uint8_t FIN=(uint8_t)0xFE; //Final de una trama



/*---------------- VARIABLES NO GLOBALES ---------------------*/	
static ARM_DRIVER_USART *USARTdrv = &Driver_USART3; //driver de la interfaz USART
static osThreadId_t tid_Thcom; //id del hilo que gestiona la temperatura 
	/*------ TRAMAS ------*/
static Estado receive = ESPERANDO_INICIO; //estado para cuando se recibe una trama
static Estado transfer = ESPERANDO_INICIO; //estado para cuando se debe enviar una trama
		/*------ TRAMA RECIBIDA ------*/
static Trama tramaRX; //comando e inforamacion relevante que se debe enviar en la cola de mensajes
static uint8_t recibido; //caracter recibido: INICIO|COMANDO|LONGITUD|FIN
static uint8_t comandoRX; //comando recibido
static char horaRX[8]; //hora recibida
static float temperaturaRX; //temperatura recibida
		/*------ TRAMA ENVIADA ------*/
static Trama tramaTX; //comando e inforamacion relevante que se recibe de la cola de mensajes
static uint8_t comandoTX; //comando a enviar		
static uint8_t longitudTX; //longitud a enviar
static bool tramaCompletaTX=false; //variable para concretar si se ha enviado la trama completa



/*---------- DECLARACION FUNCIONES NO GLOBALES ---------------*/	
static int8_t USART_Init(void);
static void USART_callback(uint32_t event);
__NO_RETURN static void Thcom (void *argument);
static void getInformacionCola(void);
static void putInforamcionCola(void);
static void procesarDatosRecibidos(void);
static void procesarDatosTransmitidos(void);



/*------------------ CODIGO FUNCIONES -----------------------*/	

static void USART_callback(uint32_t event){
	
	switch(event){
		
		//Datos recibidos correctamente del PC
		case ARM_USART_EVENT_RECEIVE_COMPLETE:
			//Cuando se reciben datos por parte del PC pasamos a procesar la trama que recibimos
			procesarDatosRecibidos();
			
		break;
			
		//Datos enviados correctamente al PC
		case ARM_USART_EVENT_SEND_COMPLETE:
			//Creamos la trama a enviar
			if( !tramaCompletaTX ){ //Si todavía la trama no está creada, seguimos enviandola
				procesarDatosTransmitidos();
			} else{ //la trama se ha enviado completa
				tramaCompletaTX = false;
			}
			
		break;
	}
	
	 
}


static int8_t USART_Init(void){
	
	if( USARTdrv->Initialize( USART_callback ) != ARM_DRIVER_OK //inicializamos la interfaz USART
		| USARTdrv->PowerControl(ARM_POWER_FULL) != ARM_DRIVER_OK //control de la alimentación del USART
		| USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS | //modo de comunicacion asincrono
			ARM_USART_DATA_BITS_8 | //datos de 8 bits
			ARM_USART_PARITY_NONE | //no hay paridad
			ARM_USART_STOP_BITS_1 | //1 bit de stop
			ARM_USART_FLOW_CONTROL_NONE,//no hay control de flujo en la comunicacion
			115200) != ARM_DRIVER_OK // la velocidad de transmision será de 115200 baudios
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
  tid_Thcom = osThreadNew(Thcom, NULL, NULL);
  if (tid_Thcom == NULL | status == -1) {
    return(-1);
  }
 
  return(0);
}


__NO_RETURN static void Thcom (void *argument){
	
	//Iniciamos la lectura de la información recibida por el PC
	USARTdrv->Receive(&recibido, 1);
	
	osThreadFlagsSet(tid_Thcom, S_GET);
	
	while(1){
		
		// Información a enviar al PC
		osThreadFlagsWait(S_GET, osFlagsWaitAny, osWaitForever);
		getInformacionCola();	
		
		
		osThreadYield(); //Hilo suspendido
	}
}


static void getInformacionCola(void){
	
	//Sacamos la informacion que debemos realizar de la cola
	osMessageQueueGet(mid_MsgQueuePCTX,&tramaTX,0U,osWaitForever);
	
	//Realizamos el complementario del comando ya que será lo que haya que mandar
	comandoTX = ~tramaTX.c;
	
	//Procedemos a enviar la informacion al PC
	procesarDatosTransmitidos();
}


static void putInforamcionCola(void){
	
	//Guardamos la información que debemos enviar
	tramaRX.c = comandoRX; //comando recibido
	if(comandoRX == HORA){ strcpy(tramaRX.hora, horaRX); } //hora a establecer recibida
	else if (comandoRX == TEMPERATURA){ tramaRX.temperatura = temperaturaRX; } //temperatura de referencia recibida
	
	//Metemos la información en la cola
	osMessageQueuePut(mid_MsgQueuePCRX,&tramaRX,0U,osWaitForever);
}


static void procesarDatosRecibidos(void){	
	
	switch(receive){
		
		case ESPERANDO_INICIO:
			
			//Comprobamos si lo recibido se corresponde con el caracter de inicio
			if(recibido == INI){
				receive = COMANDO;
			}
			
			//Seguimos buscando el caracter de inicio o buscamos el comando de la trama
			USARTdrv->Receive(&comandoRX,1);
			
		break;
			
		case COMANDO:
			
			//Comprobamos si lo recibido se corresponde con un comando
			if( comandoRX == (HORA|TEMPERATURA|MEDIDAS|BORRAR) ){
				
				//Pasamos a la longitud de la trama
				receive = LONGITUD;
				
			}else if(recibido != INI){ /*Si no se corresponde, la trama será erronea y volveremos a buscar el caracter de inicio 
																   (a no ser que se envie el caracter de inicio, en este caso seguimos buscando el comando)*/
				
				//Volvemos a buscar el principio de la trama
				receive = ESPERANDO_INICIO;
			
			}
			
			//Seguimos buscando el caracter de inicio, obtenemos la longitud del comando o volvemos a buscar el comando de la trama 
			USARTdrv->Receive(&recibido,1);
			
		break;
			
		case LONGITUD:
			
			//Comprobamos si lo recibido se corresponde con la longitud asociada al comando
			switch(comandoRX){
				
				case HORA:
					
					if(recibido == 0x0C ){ //longitud del comando: establecer hora
						//Obtenemos la hora 
						USARTdrv->Receive(&horaRX,8);
						
						//Pasamos a buscar la hora
						receive = DATOS;
					} else if(recibido == INI){ //Se ha enviado el caracter de inicio, por lo tanto procedemos reiniciamos la trama y buscamos el comando
				
						//Procedemos a buscar el comando
						receive = COMANDO;
						
						//Buscamos el comando
						USARTdrv->Receive(&recibido,1);						
						
					} else{ //Si no se corresponde con ningun caracter valido, la trama será erronea y volveremos a buscar el caracter de inicio 
						
						//Volvemos a buscar el principio de la trama
						receive = ESPERANDO_INICIO;
						
						//Buscamos el caracter de inicio 
						USARTdrv->Receive(&recibido,1);
					}
						
				break;
				
				case TEMPERATURA:
						
					if(recibido == 0x08 ){ //longitud del comando: establecer temperatura referencia
						//Obtenemos la temperatura
						USARTdrv->Receive(&temperaturaRX,4);
						
						//Pasamos a buscar la temperatura
						receive = DATOS;
					} else if(recibido == INI){ //Se ha enviado el caracter de inicio, por lo tanto procedemos reiniciamos la trama y buscamos el comando
				
						//Procedemos a buscar el comando
						receive = COMANDO;
						
						//Buscamos el comando
						USARTdrv->Receive(&recibido,1);						
						
					} else{ //Si no se corresponde con ningun caracter valido, la trama será erronea y volveremos a buscar el caracter de inicio 
						
						//Volvemos a buscar el principio de la trama
						receive = ESPERANDO_INICIO;
						
						//Buscamos el caracter de inicio 
						USARTdrv->Receive(&recibido,1);
					}
					
				break;
				
				default:
					
					if(recibido == 0x04 ){ //longitud del comando: solicitar/borrar medidas
						
						//Pasamos a buscar el caracter del final de la trama
						receive = TRAMA_COMPLETA;
						
						//Buscamos el caracter de final 
						USARTdrv->Receive(&recibido,1);
						
					}else if(recibido == INI){ //Se ha enviado el caracter de inicio, por lo tanto procedemos reiniciamos la trama y buscamos el comando
				
						//Procedemos a buscar el comando
						receive = COMANDO;
						
						//Buscamos el comando
						USARTdrv->Receive(&recibido,1);						
						
					} else{ //Si no se corresponde con ningun caracter valido, la trama será erronea y volveremos a buscar el caracter de inicio 
						
						//Volvemos a buscar el principio de la trama
						receive = ESPERANDO_INICIO;
						
						//Buscamos el caracter de inicio 
						USARTdrv->Receive(&recibido,1);
					}
					
				break;
			}
			
		break;
			
		case DATOS:
			
			//Pasamos a buscar el caracter del final de la trama
			receive = TRAMA_COMPLETA;
			
			//Buscamos el caracter de final 
			USARTdrv->Receive(&recibido,1);
		
		break;
		
		case TRAMA_COMPLETA:
			
			if(recibido == FIN){ //Si se recibe el caracter de fin pasamos a enviar la trama recibida en la cola de mensajes
				putInforamcionCola();
			}
			
			//Volvemos a buscar el principio de la trama
			receive = ESPERANDO_INICIO;
			
			//Buscamos el caracter de inicio 
			USARTdrv->Receive(&recibido,1);
		break;
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
			longitudTX = 	( tramaTX.c == HORA) ? 				0x0C :
										( tramaTX.c == TEMPERATURA) ? 0x08 :
																									0x04 ;
			
			//Enviamos la longitud de la trama
			USARTdrv->Send(&longitudTX,1);
		
			//Pasamos a enviar los datos o enviamos el final de trama
			transfer = ( ( tramaTX.c == HORA) | ( tramaTX.c == TEMPERATURA) ) ?		DATOS : //Pasamos a enviar los datos
																																									TRAMA_COMPLETA; //Enviamos el caracter de fin
		
		break;			
		
		case DATOS:
			
			//Enviamos los datos 
			if( tramaTX.c == HORA ){ //el comando nos pide la puesta en hora
				//Enviamos la hora a establecer
				USARTdrv->Send(&tramaTX.hora,8);
				
			} else if( tramaTX.c == TEMPERATURA){ //el comando nos pide establecer el valor de la temperatura de referencia
				//Enviamos la temperatura de referencia a establecer
				USARTdrv->Send(&tramaTX.temperatura,4);
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
			osThreadFlagsSet(tid_Thcom, S_GET);
		break;
	}
}
