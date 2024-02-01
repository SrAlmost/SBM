#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "lcd.h"
#include "Arial12x12.h"
 
static osThreadId_t tid_lcd;                        // thread id
static GPIO_InitTypeDef GPIO_InitStruct; //tipo gpio para inicializar los pines
static TIM_HandleTypeDef htim7; //timer 7
static ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1; //interfaz del driver
static unsigned char buffer[512]; //buffer donde se escribirá lo que se quiere mostrar por pantalla
static texto Lineas; //texto a mostrar por pantalla
static uint16_t positionL1 = 0; //posición de la primera letra de la primera línea
static uint16_t positionL2 = 0; //posición de la primera letra de la segunda línea
 
static void LCD (void *argument);                   // thread function
static void initPIN_CS(void);
static void initPIN_A0(void);
static void initPIN_RESET(void);
static void delay(volatile uint32_t n_microsegundos);
static void LCD_reset(void);
static void LCD_wr_data(unsigned char data);
static void LCD_wr_cmd(unsigned char cmd);
static void LCD_Init(void);
static void LCD_update(void);
static void symbolToLocalBuffer_L1(uint8_t symbol);
static void symbolToLocalBuffer_L2(uint8_t symbol);
static void symbolToLocalBuffer(uint8_t line, uint8_t symbol);
static void escribirLCD(uint8_t line, char frase[24] );
static void limpiarBuffer(void);

int Init_LCD (void) {
  tid_lcd = osThreadNew(LCD, NULL, NULL);
  if (tid_lcd == NULL) {    return(-1);  }
  return(0);
}
static void LCD (void *argument) {
  LCD_reset();
  LCD_Init(); 
  while (1) {
    osMessageQueueGet(mid_MsgQueueLCD, &Lineas, NULL, 0U);
		limpiarBuffer();
    escribirLCD( 1, Lineas.linea1);	escribirLCD( 2, Lineas.linea2);
    LCD_update();
    osThreadYield();                            // suspend thread
  }
}
 static void initPIN_CS(void){ 
  __HAL_RCC_GPIOD_CLK_ENABLE();
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}
static void initPIN_A0(void){ 
  __HAL_RCC_GPIOF_CLK_ENABLE();	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}
static void initPIN_RESET(void){ 
  __HAL_RCC_GPIOA_CLK_ENABLE();	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
static void delay(volatile uint32_t n_microsegundos)
{
	//Iniciamos el timer 7
  __HAL_RCC_TIM7_CLK_ENABLE();
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 83;
  htim7.Init.Period = n_microsegundos - 1;  
  HAL_TIM_Base_Init(&htim7);
  HAL_TIM_Base_Start(&htim7);
	//Esperamos a que se consuma el tiempo que hemos establecido en segundos
	__HAL_TIM_CLEAR_FLAG(&htim7, TIM_FLAG_UPDATE);
  while((__HAL_TIM_GET_FLAG(&htim7, TIM_FLAG_UPDATE) == RESET)){}
  __HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
	//Paramos el timer 7
	HAL_TIM_Base_Stop(&htim7);
	//Reiniciamos el contador del timer 7
	__HAL_TIM_SET_COUNTER(&htim7, 0);
}
static void LCD_reset(void)
{
	//Inicializamos el driver SPI
  SPIdrv->Initialize(NULL);
	//Configuramos el funcionamiento del consumo del driver SPI
  SPIdrv->PowerControl(ARM_POWER_FULL);
	//Configuramos el fucionamiento del driver SPI
  SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 20000000);
  SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);	
	//Configuro los pines de IO
	initPIN_RESET();
  initPIN_A0();
  initPIN_CS();
	//Ponemos los pines a nivel alto (por defecto)
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	//Generación señal de reset
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
  delay(1);
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
	delay(1000);
}
static void LCD_wr_data(unsigned char data){
	//Seleccionamos CS = 0 y A0 = 1
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);	
	//Escribimos el dato
	SPIdrv->Send(&data, sizeof(data));
	//Esperamos a que se libere el bus SPI
	while(SPIdrv->GetStatus().busy){}
	//Seleccionamos CS = 1
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}
static void LCD_wr_cmd(unsigned char cmd){
	//Seleccionamos CS = 0 y AO = 0
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);	
	//Escribimos el comando 
	SPIdrv->Send(&cmd, sizeof(cmd));
	//Esperamos a que se libere el bus SPI
	while(SPIdrv->GetStatus().busy){}
	//Seleccionamos CS = 1
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}
static void LCD_Init(void){
	LCD_wr_cmd(0xAE); //Display off
	LCD_wr_cmd(0xA2); //Tensión de polarización del LCD a 1/9
	LCD_wr_cmd(0xA0); //Direccionamiento de la RAM de datos del display es la normal
	LCD_wr_cmd(0xC8); //Scan en las salidas COM es el normal
	LCD_wr_cmd(0x22); //Resistencias interna a 2
	LCD_wr_cmd(0x2F); //Power on
	LCD_wr_cmd(0x40); //Display empieza en la linea 0
	LCD_wr_cmd(0xAF); //Display ON 
	LCD_wr_cmd(0x81); //Contraste
	LCD_wr_cmd(0x11); //Valor Contraste
	LCD_wr_cmd(0xA4); //Todos los puntos del display normales
	LCD_wr_cmd(0xA6); //LCD Display normal
}
static void LCD_update(void){
	int i;//variable auxiliar para los for	
	LCD_wr_cmd(0x00);	//4 bits de la parte baja de la dirección a 0
	LCD_wr_cmd(0x10);	//4 bits de la parte alta de la dirección a 0
	LCD_wr_cmd(0xB0);	//Página 0
	//Lo escribimos en el LCD
	for(i=0; i<128; i++){		LCD_wr_data(buffer[i]);	}
	LCD_wr_cmd(0x00);	//4 bits de la parte baja de la dirección a 0
	LCD_wr_cmd(0x10);	//4 bits de la parte alta de la dirección a 0
	LCD_wr_cmd(0xB1);	//Página 1
	//Lo escribimos en el LCD
	for(i=128; i<256; i++){		LCD_wr_data(buffer[i]);	}
	LCD_wr_cmd(0x00); //4 bits de la parte baja de la dirección a 0
	LCD_wr_cmd(0x10); //4 bits de la parte alta de la dirección a 0
	LCD_wr_cmd(0xB2);	//Página 2
	//Lo escribimos en el LCD
	for(i=256; i<384; i++){		LCD_wr_data(buffer[i]); }
	LCD_wr_cmd(0x00); //4 bits de la parte baja de la dirección a 0
	LCD_wr_cmd(0x10); //4 bits de la parte alta de la dirección a 0
	LCD_wr_cmd(0xB3);	//Página 3
	//Lo escribimos en el LCD
	for(i=384; i<512; i++){		LCD_wr_data(buffer[i]);	}
}
static void symbolToLocalBuffer_L1(uint8_t symbol){
	uint8_t i, value1, value2;
	uint16_t offset = 0; //espacio entre caracter y caracter		
	//Establecemos el offset que queremos
	offset = 25*(symbol - ' ');
	//Guardamos los caracteres que queramos escribir en la línea  en su posición en el buffer
	for(i=0; i<12; i++){
		value1 = Arial12x12[offset+i*2+1]; value2 = Arial12x12[offset+i*2+2];
		buffer[i+positionL1] = value1; buffer[i+128+positionL1] = value2;
	}
	//Actualizamos la siguiente posición del caracter
	positionL1 = positionL1 + Arial12x12[offset];
}
static void symbolToLocalBuffer_L2(uint8_t symbol){
	uint8_t i, value1, value2;
	uint16_t offset = 0; //espacio entre caracter y caracter
	//Establecemos el offset que queremos
	offset = 25*(symbol - ' ');	
	//Guardamos los caracteres que queramos escribir en la línea  en su posición en el buffer
	for(i=0; i<12; i++){
		value1 = Arial12x12[offset+i*2+1]; value2 = Arial12x12[offset+i*2+2];
		buffer[256+i+positionL2] = value1; buffer[i+384+positionL2] = value2;
	}
	//Actualizamos la siguiente posición del caracter
	positionL2 = positionL2 + Arial12x12[offset];
}
static void symbolToLocalBuffer(uint8_t line, uint8_t symbol){
	if(line == 1){
    symbolToLocalBuffer_L1(symbol); //Escribimos en el buffer la primera línea
  }else if(line == 2){
    symbolToLocalBuffer_L2(symbol); //Escribimos en el buffer la segunda línea 
  }
}
static void escribirLCD(uint8_t line, char frase[24] ){
	//Escribimos en el buffer lo que queremos mostrar por pantalla en cada línea
  if(line == 1)  {  for(int i = 0; frase[i] != '\0'; i++){ symbolToLocalBuffer(line,frase[i]);    }
  }else if(line == 2)  {    for(int i = 0; frase[i] != '\0'; i++){ symbolToLocalBuffer(line,frase[i]); } }
}
static void limpiarBuffer(void){
  memset(buffer, 0x00, sizeof(buffer));
}
