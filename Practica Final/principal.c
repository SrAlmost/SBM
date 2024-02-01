#include "principal.h"
#include "string.h"
#include <stdio.h>
/*-------------------------- VARIABLES ------------------------------*/	
	/*-------------------------- COLAS MENSAJES ------------------------------*/	
osMessageQueueId_t mid_MsgQueueJOY;                // message queue id 
osMessageQueueId_t mid_MsgQueueLCD;                // message queue id 
osMessageQueueId_t mid_MsgQueueTEM;                // message queue id 
osMessageQueueId_t mid_MsgQueuePOT;                // message queue id 
osMessageQueueId_t mid_MsgQueuePWM;                // message queue id
osMessageQueueId_t mid_MsgQueuePCTX;                // message queue id
osMessageQueueId_t mid_MsgQueuePCRX;                // message queue id
	/*-------------------------- LCD ------------------------------*/	
static texto Lineas; //lineas a escribir
	/*-------------------------- JOYSTICK ------------------------------*/	
static informacion_t pulsacion;
	/*------------------ TEMPERATURA ---------------------*/	
static double temREF = 25.0; //temperatura de referencia
static double temMED; //temperatura medida
	/*------------------ POTENCIOMETRO ---------------------*/	
static double potREF; //temperatura de referencia
static double potMED; //temperatura medida
static potenciometro potMedidas; // medidas sacadas del potenciometro
	/*------------------ PWM ---------------------*/	
static uint8_t PWM; //ciclo de trabajo	
	/*-------------------------- HILO ------------------------------*/	
static osThreadId_t tid_Thread; //id del hilo principal
static Modo modos = REPOSO; //modo de funcionamiento del sistema
static BufferCircular medidas; //buffer circular con las medidas
static uint8_t posicionBucle = 0; //posicion en el bucle circular para ir añadiendo elementos
	/*-------------------------- P&D ------------------------------*/	
static ProgDep PD = HORA; 
static uint8_t horasREF;
static uint8_t minutosREF;
static uint8_t segundosREF;
static double temREFERENCIA;
static Trama tramaRX;
static Trama tramaTX;
/*------------------ DECLARACION FUNCIONES NO GLOBALES ---------------------*/	
	/*------------------ COLAS DE MENSAJES ---------------------*/	
static int Init_MsgQueue (void) ;
	/*------------------ HILO PRINCIPAL ---------------------*/	
__NO_RETURN static void Thread (void *argument);                   // thread function
static void modoFuncionamiento(void); 
	/*------------------ MODOS FUNCIONAMIENTO ---------------------*/	
static void reposo(void);
static void activo(void);
static void test(void);
static void programacion(void);
static uint8_t generarPWM( double referencia, double medida );

static void inicializarBuffer(BufferCircular *buffer);
static int estaVacio(const BufferCircular *buffer);
static void escribirEnBuffer(BufferCircular *buffer, const MEDIDA medida) ;
static void obtenerTodasLasMedidas(const BufferCircular *buffer) ;

/*-------------------------- CODIGO FUNCIONES ------------------------------*/	 
static int Init_MsgQueue (void) {
  mid_MsgQueueJOY = osMessageQueueNew(2, sizeof(informacion_t), NULL); //Joystick
  if (mid_MsgQueueJOY == NULL) {    return -1;  }
	mid_MsgQueueLCD = osMessageQueueNew(2, sizeof(texto), NULL); //LCD
  if (mid_MsgQueueLCD == NULL) {    return -1;  }
	mid_MsgQueueTEM = osMessageQueueNew(2, sizeof(double), NULL); //Temperatura
  if (mid_MsgQueueTEM == NULL) {    return -1;  }
	mid_MsgQueuePOT = osMessageQueueNew(2, sizeof(potenciometro), NULL); //Potenciometro
  if (mid_MsgQueuePOT == NULL) {    return -1;  }
	mid_MsgQueuePWM = osMessageQueueNew(2, sizeof(uint8_t), NULL); //PWM
  if (mid_MsgQueuePWM == NULL) {    return -1;  }
	mid_MsgQueuePCRX = osMessageQueueNew(2, sizeof(Trama), NULL); // COM-PC RX
  if (mid_MsgQueuePCRX == NULL) {    return -1;  }
	mid_MsgQueuePCTX = osMessageQueueNew(2, sizeof(Trama), NULL); // COM-PC TX
  if (mid_MsgQueuePCTX == NULL) {    return -1;  }
	return 0;
} 
int Init_Principal (void) {
	//Creacion colas de mensajes
	if( Init_MsgQueue() != 0){ 		return -1;	}
	//Creacion hilo
  tid_Thread = osThreadNew(Thread, NULL, NULL);
  if (tid_Thread == NULL) {    return(-1);  }
  return(0);
}
__NO_RETURN static void Thread (void *argument) {
  while (1) {
		modoFuncionamiento();
		osMessageQueuePut( mid_MsgQueueLCD, &Lineas, 0U, osWaitForever);
    osThreadYield();                            // suspend thread
  }
}
static void modoFuncionamiento(void){
	
	if( ( osMessageQueueGet(mid_MsgQueueJOY,&pulsacion, NULL, 0U) == osOK ) & 
				(	(pulsacion.corta == 0) & ( strcmp( pulsacion.direccion, "Centro" ) ) )	){
		switch(modos){
			case REPOSO:
				modos = ACTIVO;						
				//Activamos el timer de la medida de la temperatura
				osTimerStart( tim_medida, 1000U);					
				//Activamos la señal PWM
				HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);
			break;						
			case ACTIVO:
				modos = TEST;
				//Desactivamos el timer de la medida de la temperatura
				osTimerStop( tim_medida );
				//Iniciamos el timer para medir las conversiones del ADC
				osTimerStart(tim_pot,1000U);
			break;
			case TEST:
				modos = PROGRAMACION;
				//Paramos el timer que controla la hora
				osTimerStop( id_clock );
				//Paramos el timer de las conversiones del ADC
				osTimerStop(tim_pot);
				//Paramos la señal PWM
				HAL_TIM_OC_Stop(&htim1, TIM_CHANNEL_1);
				//Establecemos el valor de las variables que queremos
				horasREF = horas; minutosREF = minutos; segundosREF = segundos;
				temREFERENCIA = temREF;
			break;
			case PROGRAMACION:
				modos = REPOSO;	
				//Volvemos a iniciar el timer de la hora
				osTimerStart( id_clock, 1000U);
			break;
		}
	} else {
		switch(modos){
			case REPOSO: reposo();	break;
			case ACTIVO: activo();	break;
			case TEST:   test();		break;
			case PROGRAMACION:	programacion(); 	break;
		}
	}
}

static void reposo( void ){
		snprintf( Lineas.linea1 , sizeof(Lineas.linea1), "SBM 2023" );
		snprintf( Lineas.linea2, sizeof(Lineas.linea2), "%d:%d:%d", segundos, minutos, horas);
}

static void activo( void ){
		if( osMessageQueueGet(mid_MsgQueueTEM,&temMED, NULL, 0U) == osOK) {
			MEDIDA med; //Medida a añadir en el bucle circular
			
			PWM = generarPWM(temMED, temREF); 
			
			med.temperaturaMedida = temMED; med.temperaturaReferencia = temREF;
			med.cicloTrabajo = PWM;
			snprintf( med.hora, sizeof(med.hora), "%d:%d:%d", horas, minutos, segundos); 
			
			escribirEnBuffer( &medidas ,med);
		}
		snprintf( Lineas.linea1 , sizeof(Lineas.linea1) , "ACT---%d:%d:%d---", segundos, minutos, horas);		
		snprintf( Lineas.linea2 , sizeof(Lineas.linea2) , "Tm:%1.f%cC-Tr:%1.f%cC-D:%d%%", (int)(temMED/0.5)*0.5, 176, (int)(temREF/0.5)*0.5, 176, PWM);
}
	

static void test( void ){
	if( osMessageQueueGet(mid_MsgQueuePOT,&potMedidas, NULL, 0U) == osOK ){
		potMED = potMedidas.temperaturaMED;
		potREF = potMedidas.temperaturaREF;
	}
	PWM = generarPWM( potMED, potREF); 
	snprintf( Lineas.linea1 , sizeof(Lineas.linea1) , "TEST---%d:%d:%d---", segundos, minutos, horas);
	snprintf( Lineas.linea2 , sizeof(Lineas.linea2) , "Tm:%1.f%cC-Tr:%1.f%cC-D:%d%%", (int)(potMED/0.5)*0.5, 176, (int)(potREF/0.5)*0.5, 176, PWM);
}

static uint8_t generarPWM( double referencia, double medida ){
	double diferencia = referencia - medida;
	uint8_t ciclo;
	if( diferencia < -5 ){											 				 	 ciclo = 0;	 }
	else if( (diferencia > -5) & (diferencia < 0) ){ 			 ciclo = 40; } 
	else if( (diferencia > 0)  & (diferencia < 5) ){ 	 		 ciclo = 80; }
	else{																		 							 ciclo = 100;}
	osMessageQueuePut( mid_MsgQueuePWM, &ciclo, 0U, osWaitForever);
	return ciclo;
}

static void programacion( void ){
	if( pulsacion.corta == 1){		
		if( strcmp(pulsacion.direccion,"Centro" ) ){
			switch(PD){
				case HORAS:	  horas = horasREF; 			break;
				case MINUTO:	minutos = minutosREF; 	break;
				case SEGUNDO:	segundos = segundosREF;	break;
				case TEMREF:	temREF = temREFERENCIA;	break;
			}
		} else if( strcmp(pulsacion.direccion,"Izquierda" ) ){
			switch(PD){
				case MINUTO:	PD = HORAS; 		break;
				case SEGUNDO:	PD = MINUTO;	break;
				case TEMREF:	PD = SEGUNDO;	break;
			}
		} else if( strcmp(pulsacion.direccion,"Derecha" ) ){
			switch(PD){
				case HORAS:		PD = MINUTO;	break;
				case MINUTO:	PD = SEGUNDO; break;
				case SEGUNDO:	PD = TEMREF;	break;
			}
		} else if( strcmp(pulsacion.direccion,"Arriba" ) ){
			switch(PD){
				case HORAS:		horasREF = (horasREF + 1) % 24 ;		break;
				case MINUTO:	minutos = (minutos + 1) % 60 ;			break;
				case SEGUNDO:	segundos = (segundos + 1) % 60 ;		break;
				case TEMREF:	temREFERENCIA = (temREFERENCIA == 30.0) ? 30.0: temREFERENCIA + 0.5;		break;
			}
		} else if( strcmp(pulsacion.direccion,"Abajo" ) ){
			switch(PD){
				case HORAS: 		horasREF = (horasREF - 1) % 24 ;		break;
				case MINUTO:	minutos = (minutos - 1) % 60 ;			break;
				case SEGUNDO:	segundos = (segundos - 1) % 60 ;		break;
				case TEMREF:	temREFERENCIA = (temREFERENCIA == 5.0) ? 5.0: temREFERENCIA - 0.5;			break;
			}
		} 
	}
	
	if( osMessageQueueGet( mid_MsgQueuePCRX, &tramaRX, 0U, 0U) == osOK){
		switch( tramaRX.c ){
			case HORA:
				horas = tramaRX.hora; minutos = tramaRX.minutos; segundos = tramaRX.segundos;
				tramaTX.hora = tramaRX.hora; tramaTX.minutos = tramaRX.minutos; tramaTX.segundos = tramaRX.segundos;
				
			break;
			
			case TEMPERATURA:
				temREF = tramaRX.temperaturaREF;
				tramaTX.temperaturaREF = tramaRX.temperaturaREF; 
			break;
			
			case MEDIDAS:
				tramaTX.c = MEDIDAS;
				obtenerTodasLasMedidas( &medidas );
			break;
			
			case BORRAR:
				tramaTX.c = BORRAR;
				inicializarBuffer( &medidas );
				osMessageQueuePut( mid_MsgQueuePCTX, &tramaTX, 0U, osWaitForever);
			break;
				
		}
		tramaTX.c = tramaRX.c; 
		osMessageQueuePut( mid_MsgQueuePCTX, &tramaTX, 0U, osWaitForever);
	}
		
	snprintf( Lineas.linea1 , sizeof(Lineas.linea1) , "---P&D---");
	
	switch(PD){
		case HORAS:
			snprintf( Lineas.linea2 , sizeof(Lineas.linea2) , "H: %d:%d:%d ---Tr: %1.f", horasREF, minutos, segundos,(int)(temMED/0.5)*0.5);
		break;	
		case MINUTO:
			snprintf( Lineas.linea2 , sizeof(Lineas.linea2) , "H: %d:%d:%d ---Tr: %1.f", horas, minutosREF, segundos,(int)(temMED/0.5)*0.5);
		break;
		case SEGUNDO:
			snprintf( Lineas.linea2 , sizeof(Lineas.linea2) , "H: %d:%d:%d ---Tr: %1.f", horas, minutos, segundosREF,(int)(temMED/0.5)*0.5);
		break;
		case TEMREF:
			snprintf( Lineas.linea2 , sizeof(Lineas.linea2) , "H: %d:%d:%d ---Tr: %1.f", horas, minutos, segundos, temREFERENCIA);
		break;
	}
}


	/*------------------ BUFFER CIRCULAR ---------------------*/
static void inicializarBuffer(BufferCircular *buffer) {
    buffer->lectura = 0;
    buffer->escritura = 0;
}

static int estaVacio(const BufferCircular *buffer) {
    return buffer->lectura == buffer->escritura;
}

static void escribirEnBuffer(BufferCircular *buffer, const MEDIDA medida) {
    // Copiamos la medida en el buffer
		buffer->datos[buffer->escritura] = medida;
	
    // Avanzar el puntero de escritura de manera circular
    buffer->escritura = (buffer->escritura + 1) % 10;

    // Si el puntero de escritura alcanza al de lectura, avanzar el de lectura también
    if (buffer->escritura == buffer->lectura) {
        buffer->lectura = (buffer->lectura + 1) % 10;
    }
}


static void obtenerTodasLasMedidas(const BufferCircular *buffer) {
		
		if (!estaVacio(buffer)) {
        uint8_t i = buffer->lectura;
        do {
						tramaTX.PWM = buffer->datos[i].cicloTrabajo;
						tramaTX.temperatura = buffer->datos[i].temperaturaMedida;
						tramaTX.temperaturaREF = buffer->datos[i].temperaturaReferencia;
						sscanf( buffer->datos[i].hora, "%d:%d:%d", &tramaTX.hora, &tramaTX.minutos, &tramaTX.segundos );
						
						osMessageQueuePut(mid_MsgQueuePCTX, &tramaTX, 0U, osWaitForever); 
					
            i = (i + 1) % 10;
						
        } while (i != ( (buffer->escritura+1)%10 ) );
				
    }
}
