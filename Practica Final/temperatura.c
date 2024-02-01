#include "temperatura.h"
/*------------- VARIABLES NO GLOBALES ----------------------*/
	/*------------- BUS I2C ----------------------*/
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C1; //interfaz del driver
	/*------------- TIMER ----------------------*/
osTimerId_t tim_medida; //id del timer para realizar la medida
	/*------------- SENSOR TEMPERATURA ----------------------*/
static uint8_t buffer[2]; //datos que recibiremos del sensor de temperatura
static double temperatura; //valor de la temperatura medido
	/*------------- HILO ----------------------*/
static osThreadId_t tid_Thtemperatura; //id del hilo que gestiona la temperatura 
/*----------- DECLARACION FUNCIONES NO GLOBALES -----------*/
static int8_t Init_Timers(void);
static void TimerMEDIDA_Callback(void *arg);
static int8_t I2C_Init(void);
static void I2C_Read(void);
__NO_RETURN static void Thtemperatura (void *argument);
static void medida(void);
/*---------------- CODIGO FUNCIONES ------------------------*/
	/*---------------- TIMER ------------------------*/
static int8_t Init_Timers(void){
	// Creamos el timer para realizar la medida
  tim_medida = osTimerNew((osTimerFunc_t)&TimerMEDIDA_Callback, osTimerPeriodic, NULL, NULL);
	//Comprobamos que el timer se ha creado correctamente
  if (tim_medida == NULL) { 
    return -1;
  } 
	return 0;
}
static void TimerMEDIDA_Callback(void *arg){
	//Flag que indicará que se debe realizar la medida de la temperatura
	osThreadFlagsSet(tid_Thtemperatura, S_MED);
}
	/*---------------- SENSOR DE TEMPERATURA ------------------------*/
static int8_t I2C_Init(void){
	//Iniciamos y configuramos el bus I2C
	if( (I2Cdrv->Initialize(NULL) != ARM_DRIVER_OK) //inicializa la interfaz I2C
			| (I2Cdrv->PowerControl(ARM_POWER_FULL) != ARM_DRIVER_OK) //controla la alimentacion de la interfaz I2C
			| (I2Cdrv->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST) != ARM_DRIVER_OK ) //establece la velocidad binaria del bus
			| (I2Cdrv->Control(ARM_I2C_OWN_ADDRESS, ADDRESS) != ARM_DRIVER_OK ) //establece su propia direccion
			| (I2Cdrv->Control(ARM_I2C_BUS_CLEAR, 0) != ARM_DRIVER_OK ) //borra el bus
			| (I2Cdrv->Control(ARM_I2C_BUS_CLEAR, 1) != ARM_DRIVER_OK) ){ //restauramos el bus a un estado normal
		return -1;
	}
	return 0;
}
static void I2C_Read(void){
	//Ordenamos al sensor de temperatura que queremos leer sus datos
	I2Cdrv->MasterTransmit (ADDRESS, 0x00, sizeof(0x00), true);
		//Esperamos hasta que se complete la transferencia
	while (I2Cdrv->GetStatus().busy){}
	//Recibimos los datos del sensor de temperatura
	I2Cdrv->MasterReceive (ADDRESS, buffer, sizeof(buffer), false);
		//Esperamos hasta que se complete la transferencia
	while (I2Cdrv->GetStatus().busy){}
}
	/*---------------- INICIACION BUS I2C, TIMER E HILO ------------------------*/
int8_t Init_Thtemperatura (void) {
	int8_t status; //estado de las funciones para comprobar si se ha producido algun error
	//Inicializamos el bus I2C
	status = I2C_Init();
		//Comprobamos que se ha inicializado correctamente el bus I2C
	if( status == -1 ){ return (-1); }
	//Creamos los timers que emplearemos
	status = Init_Timers();
		//Comprobamos que se ha creado correctamente
	if( status == -1 ){ return (-1);	}
	//Creamos el hilo que gestiona la temperatura
  tid_Thtemperatura = osThreadNew(Thtemperatura, NULL, NULL);
  if (tid_Thtemperatura == NULL | status == -1) { return(-1); }
  return(0);
}
	/*---------------- HILO ------------------------*/
__NO_RETURN static void Thtemperatura (void *argument) {
	while(1){
		//Esperamos a que salte el temporizador de 1 segundo
		osThreadFlagsWait(S_MED, osFlagsWaitAny, osWaitForever);
		//Realizamos la medida
		medida();		
		osThreadYield(); //Suspendemos el hilo
	}
}
	/*---------------- MEDIDA SENSOR ------------------------*/
static void medida(void){
	uint8_t MSByte,LSByte; //byte mas alto/bajo del buffer donde se almacenará la temperatura
	//Leemos la temperatura del bus I2C
	I2C_Read();
	//Diferenciamos entre los diferentes bytes de informacion que nos proporciona el sensor para calcular la temperatura
	MSByte = buffer[0]; //byte más alto recibido
	LSByte = buffer[1]; //byte más bajo recibido
	//Calculamos el valor de temperatura que nos proporciona el sensor
	uint16_t	t=( (uint16_t) ((( (MSByte & 0xFF) << 8) | (LSByte & 0xE0) ) >> 5) );
	//Calculamos la temperatura
	if( (MSByte>>7) == 0){ /*temperatura positiva*/ temperatura = t*0.125; } 
	else{ /*temperatura negativa*/	temperatura = (~t)*0.125*(-1);	}
	//Guardamos la temperatura en la cola de mensajes
	osMessageQueuePut(mid_MsgQueueTEM, &temperatura, 0U, 0U);
}
