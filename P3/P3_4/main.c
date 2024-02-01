/*
	Autores: Iván Isabel Sierra
					 José Perandrés García de la Serrana
	Grupo: L05L06X03X04B
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Driver_SPI.h"

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern ARM_DRIVER_SPI Driver_SPI1; //driver del protocolo SPI
static GPIO_InitTypeDef GPIO_InitStruct; //tipo gpio para inicializar los pines
static TIM_HandleTypeDef htim7; //timer 7
static ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1; //interfaz del driver
//Buffer que escribe una A
static const unsigned char buffer[] = {0xFE,0x09,0x09,0x09,0x09,0x09,0x09,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
																0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
																0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
																0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);
static void LCD_reset(void);
static void initPIN_RESET(void);
static void initPIN_A0(void);
static void initPIN_CS(void);
static void initTIMER7(void);
void delay(volatile uint32_t n_microsegundos);
static void LCD_wr_data(unsigned char data);
static void LCD_wr_cmd(unsigned char cmd);
static void LCD_Init(void);
static void LCD_update(void);


/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  HAL_Init();

  /* Configure the system clock to 168 MHz */
  SystemClock_Config();
  SystemCoreClockUpdate();
	
	//Iniciamos el Timer 7
	initTIMER7();
	
	//Reseteamos el LCD
  LCD_reset();
  
	//Iniciamos el LCD
	LCD_Init();
	
	//Actualizamos el LCD
	LCD_update();
	
  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1)
  {
  }
}

/**
  * @brief  Configura el pin CS que se corresponde con el pin D14
  * @param  None
  * @retval None
  */
static void initPIN_CS(void){ 
  __HAL_RCC_GPIOD_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin = GPIO_PIN_14;
  
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/**
  * @brief  Configura el pin AO que se corresponde con el pin F13
  * @param  None
  * @retval None
  */
static void initPIN_A0(void){ 
  __HAL_RCC_GPIOF_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin = GPIO_PIN_13;

	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}

/**
  * @brief  Configura el pin RESET que se corresponde con el pin A6
  * @param  None
  * @retval None
  */
static void initPIN_RESET(void){ 
  __HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin = GPIO_PIN_6;
  
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
  * @brief  Configura los parámetros del timer 7
  * @param  None
	* 
  * @retval None
  */
static void initTIMER7(void){
	__HAL_RCC_TIM7_CLK_ENABLE();
	
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 83;
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 0xFFFF;
  HAL_TIM_Base_Init(&htim7);
	
}

/**
  * @brief  Configura el delay
	* @param  n_microsegundos -> tiempo establecido para realizar la pausa del delay; su valor se acota entre: 0-65534
  * @retval None
  */
void delay(volatile uint32_t n_microsegundos)
{
	//Iniciamos el timer 7
  HAL_TIM_Base_Start(&htim7);

	//Esperamos a que se consuma el tiempo que hemos establecido en segundos
	while( __HAL_TIM_GET_COUNTER(&htim7) < n_microsegundos ){
		if (__HAL_TIM_GET_FLAG(&htim7, TIM_FLAG_UPDATE)) {
            __HAL_TIM_CLEAR_FLAG(&htim7, TIM_FLAG_UPDATE);
    } 
  }
	
	//Paramos el timer 7
	HAL_TIM_Base_Stop(&htim7);
	//Reiniciamos el contador del timer 7
	__HAL_TIM_SET_COUNTER(&htim7, 0);
}

/**
  * @brief  Inicialización del LCD
	* @param  None
  * @retval None
  */
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

/**
  * @brief  Escribe un dato LCD
	* @param  None
  * @retval None
  */
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

/**
  * @brief  Escribe un comando en el LCD
	* @param  None
  * @retval None
  */
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
 
/**
  * @brief  Operaciones de iniciación del LCD
	* @param  None
  * @retval None
  */
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

/**
  * @brief  Copia la información de un array de datos global tipo unsigned char de 512 elementos a la memoria del display LCD
	* @param  None
  * @retval None
  */
static void LCD_update(void){
	int i;//variable auxiliar para los for
	
	LCD_wr_cmd(0x00);	//4 bits de la parte baja de la dirección a 0
	LCD_wr_cmd(0x10);	//4 bits de la parte alta de la dirección a 0
	LCD_wr_cmd(0xB0);	//Página 0
	
	//Lo escribimos en el LCD
	for(i=0; i<128; i++){
		LCD_wr_data(buffer[i]);
	}
	
	LCD_wr_cmd(0x00);	//4 bits de la parte baja de la dirección a 0
	LCD_wr_cmd(0x10);	//4 bits de la parte alta de la dirección a 0
	LCD_wr_cmd(0xB1);	//Página 1
	
	//Lo escribimos en el LCD
	for(i=128; i<256; i++){
		LCD_wr_data(buffer[i]);
	}
	
	LCD_wr_cmd(0x00); //4 bits de la parte baja de la dirección a 0
	LCD_wr_cmd(0x10); //4 bits de la parte alta de la dirección a 0
	LCD_wr_cmd(0xB2);	//Página 2
	
	//Lo escribimos en el LCD
	for(i=256; i<384; i++){
		LCD_wr_data(buffer[i]);
	}
	
	LCD_wr_cmd(0x00); //4 bits de la parte baja de la dirección a 0
	LCD_wr_cmd(0x10); //4 bits de la parte alta de la dirección a 0
	LCD_wr_cmd(0xB3);	//Página 3
	
	//Lo escribimos en el LCD
	for(i=384; i<512; i++){
		LCD_wr_data(buffer[i]);
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
