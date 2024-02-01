#ifndef _LCD_
	
	#include "stm32f4xx_hal.h"
	#include "Driver_SPI.h"

	#include "stdio.h"

	#define _LCD_
		void initPIN_CS(void);
		void initPIN_A0(void);
		void initPIN_RESET(void);
		void initTIMER7(void);
		void delay(volatile uint32_t n_microsegundos);
		void LCD_reset(void);
		void LCD_wr_data(unsigned char data);
		void LCD_wr_cmd(unsigned char cmd);
		void LCD_Init(void);
		void LCD_update(void);
		void symbolToLocalBuffer_L1(uint8_t symbol);
		void symbolToLocalBuffer_L2(uint8_t symbol);
		void symbolToLocalBuffer(uint8_t line, uint8_t symbol);
		void escribirLCD(char frase[80]);
		void reseteo(void);
    
		extern ARM_DRIVER_SPI Driver_SPI1; //driver del protocolo SPI

#endif
