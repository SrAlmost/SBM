#include "Driver_USART.h"
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include <stdio.h>
#include <string.h>
#include "com.h"
 
osThreadId_t tid_com;                        // thread id
extern ARM_DRIVER_USART Driver_USART3;
static uint8_t cmd;
static unsigned char tramaRX[TRAMA_SIZE];
static ARM_DRIVER_USART * USARTdrv = &Driver_USART3;

static volatile uint8_t i = 0;
 
static void USART_Init(void);
void com (void *argument);                   // thread function
void myUSART_callback(uint32_t event);
void processByteRX(uint8_t byte);
 
int Init_com (void) {
	USART_Init();
 
  tid_com = osThreadNew(com, NULL, NULL);
  if (tid_com == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void myUSART_callback(uint32_t event)
{
  switch(event)
	{
		case ARM_USART_EVENT_SEND_COMPLETE:
			osThreadFlagsSet(tid_com, 0x01);
			break;
		case ARM_USART_EVENT_RECEIVE_COMPLETE:
			osThreadFlagsSet(tid_com, 0x01);
			break;
	}
}

static void USART_Init(void){
	ARM_DRIVER_VERSION     version;
	ARM_USART_CAPABILITIES drv_capabilities;
	
 
  #ifdef DEBUG
    version = USARTdrv->GetVersion();
    if (version.api < 0x200)   /* requires at minimum API version 2.00 or higher */
    {                          /* error handling */
        return;
    }
    drv_capabilities = USARTdrv->GetCapabilities();
    if (drv_capabilities.event_tx_complete == 0)
    {                          /* error handling */
        return;
    }
  #endif
 
	/*Initialize the USART driver */
	USARTdrv->Initialize(myUSART_callback);
	/*Power up the USART peripheral */
	USARTdrv->PowerControl(ARM_POWER_FULL);
	/*Configure the USART to 4800 Bits/sec */
	USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
										ARM_USART_DATA_BITS_8 |
										ARM_USART_PARITY_NONE |
										ARM_USART_STOP_BITS_1 |
										ARM_USART_FLOW_CONTROL_NONE, 115200);
	 
	/* Enable Receiver and Transmitter lines */
	USARTdrv->Control (ARM_USART_CONTROL_TX, 1);
	USARTdrv->Control (ARM_USART_CONTROL_RX, 1);
}

void processByteRX(uint8_t byte)
{
	tramaRX[i] = byte;
	i++;
//	switch(byte)
//	{
//		
//	}
}

void com (void *argument) {
	USARTdrv->Send("\nPress Enter to receive a message", 34);
	osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
	 
	while (1)
	{
		USARTdrv->Receive(&cmd, 1);
		osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
		processByteRX(cmd);
		
//		USARTdrv->Receive(&cmd, 1);          /* Get byte from UART */
//		osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
//		if (cmd == 13)                       /* CR, send greeting  */
//		{
//			memset(&cmd, 0x00, sizeof(cmd));
//			USARTdrv->Send("\nHello World!", 12);
//			osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
//		}

			osThreadYield();                            // suspend thread
	}
}
