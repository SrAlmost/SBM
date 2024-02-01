/**
  ******************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @brief   STM32F4xx HAL API Template project 
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Driver_SPI.h"
#include "Arial12x12.h"

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#endif

#ifdef RTE_CMSIS_RTOS2_RTX5
/**
  * Override default HAL_GetTick function
  */
uint32_t HAL_GetTick (void) {
  static uint32_t ticks = 0U;
         uint32_t i;

  if (osKernelGetState () == osKernelRunning) {
    return ((uint32_t)osKernelGetTickCount ());
  }

  /* If Kernel is not running wait approximately 1 ms then increment 
     and return auxiliary tick counter value */
  for (i = (SystemCoreClock >> 14U); i > 0U; i--) {
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
  }
  return ++ticks;
}

#endif

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
extern ARM_DRIVER_SPI Driver_SPI1;
static GPIO_InitTypeDef GPIO_InitStruct;
static TIM_HandleTypeDef htim7;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;
unsigned char buffer[511];
static uint16_t positionL1;
	/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);
static void LCD_reset(void);
static void initPIN_RESET(void);
static void initPIN_A0(void);
static void initPIN_CS(void);
static void delay(uint32_t n_microsegundos);
static void LCD_wr_data(unsigned char data);
static void LCD_wr_cmd(unsigned char cmd);
static void LCD_Init(void);
static void LCD_update(void);
//static void symbolToLocalBuffer_L2(uint8_t symbol);
static void symbolToLocalBuffer(uint8_t line, uint8_t symbol);

//borrar
void LED_Init(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user 
             can eventually implement his proper time base source (a general purpose 
             timer for example or other time source), keeping in mind that Time base 
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
             handled in milliseconds basis.
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 168 MHz */
  SystemClock_Config();
  SystemCoreClockUpdate();

  /* Add your application code here
     */
	
  LCD_reset();
	LCD_Init();
//	symbolToLocalBuffer(1,'P');
//	symbolToLocalBuffer(1,'r');
//	symbolToLocalBuffer(1,'u');
//	symbolToLocalBuffer(1,'e');
//	symbolToLocalBuffer(1,'b');
//	symbolToLocalBuffer(1,'a');
//	symbolToLocalBuffer(1,' ');
//	symbolToLocalBuffer(1,'d');
//	symbolToLocalBuffer(1,'e');
//	symbolToLocalBuffer(1,' ');
//	symbolToLocalBuffer(1,'t');
//	symbolToLocalBuffer(1,'e');
//	symbolToLocalBuffer(1,'x');
//	symbolToLocalBuffer(1,'t');
//	symbolToLocalBuffer(1,'o');
//	symbolToLocalBuffer(1,' ');
//	symbolToLocalBuffer(1,'L');
//	symbolToLocalBuffer(1,'1');
	symbolToLocalBuffer(2,'P');
	symbolToLocalBuffer(2,'r');
	symbolToLocalBuffer(2,'u');
	symbolToLocalBuffer(2,'e');
	symbolToLocalBuffer(2,'b');
	symbolToLocalBuffer(2,'a');
	symbolToLocalBuffer(2,' ');
	symbolToLocalBuffer(2,'d');
	symbolToLocalBuffer(2,'e');
	symbolToLocalBuffer(2,' ');
	symbolToLocalBuffer(2,'t');
	symbolToLocalBuffer(2,'e');
	symbolToLocalBuffer(2,'x');
	symbolToLocalBuffer(2,'t');
	symbolToLocalBuffer(2,'o');
	symbolToLocalBuffer(2,' ');
	symbolToLocalBuffer(2,'L');
	symbolToLocalBuffer(2,'2');
	LCD_update();
  //LED_Init();

#ifdef RTE_CMSIS_RTOS2
  /* Initialize CMSIS-RTOS2 */
  osKernelInitialize ();

  /* Create thread functions that start executing, 
  Example: osThreadNew(app_main, NULL, NULL); */

  /* Start thread execution */
  osKernelStart();
#endif

  /* Infinite loop */
  while (1)
  {
		//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
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

void delay(uint32_t n_microsegundos)
{
	__HAL_RCC_TIM7_CLK_ENABLE();
  
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 83;
  htim7.Init.Period = (n_microsegundos - 1);
  HAL_TIM_Base_Init(&htim7);
  
  HAL_TIM_Base_Start(&htim7);
	
	__HAL_TIM_CLEAR_FLAG(&htim7, TIM_FLAG_UPDATE);
	while(__HAL_TIM_GET_FLAG(&htim7, TIM_FLAG_UPDATE) == RESET){}
	__HAL_TIM_CLEAR_FLAG(&htim7, TIM_FLAG_UPDATE);
		
	HAL_TIM_Base_Stop(&htim7);
	__HAL_TIM_SET_COUNTER(&htim7, 0);
}

static void LCD_reset(void){
	//Inicializaci�n y configuraci�n del driver SPI para gestionar el LCD
  //ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;
  SPIdrv->Initialize(NULL);
  SPIdrv->PowerControl(ARM_POWER_FULL);
  SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 20000000);

  //SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
	
	//Configuro los pines de IO y programo su valor por defecto
	initPIN_RESET();
  initPIN_A0();
  initPIN_CS();
	
	//Los ponemos a nivel alto inicialmente
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);	//PIN CS
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);	//PIN A0
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);	//PIN RESET
	
	//Generaci�n se�al de reset
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  delay(100);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
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
	LCD_wr_cmd(0xAE);
	LCD_wr_cmd(0xA2);
	LCD_wr_cmd(0xA0);
	LCD_wr_cmd(0xC8);
	LCD_wr_cmd(0x22);
	LCD_wr_cmd(0x2F);
	LCD_wr_cmd(0x40);
	LCD_wr_cmd(0xAF);
	LCD_wr_cmd(0x81);
	LCD_wr_cmd(0x11);
	LCD_wr_cmd(0xA4);
	LCD_wr_cmd(0xA6);
}

static void LCD_update(void){
	int i;
	LCD_wr_cmd(0x00);	//4 bits de la parte baja de la direcci�n a 0
	LCD_wr_cmd(0x10);	//4 bits de la parte alta de la direcci�n a 0
	LCD_wr_cmd(0xB0);	//P�gina 0
	
	for(i=0; i<128; i++){
		LCD_wr_data(buffer[i]);
	}
	
	LCD_wr_cmd(0x00);	//4 bits de la parte baja de la direcci�n a 0
	LCD_wr_cmd(0x10);	//4 bits de la parte alta de la direcci�n a 0
	LCD_wr_cmd(0xB1);	//P�gina 1
	
	for(i=128; i<256; i++){
		LCD_wr_data(buffer[i]);
	}
	
	LCD_wr_cmd(0x00);
	LCD_wr_cmd(0x10);
	LCD_wr_cmd(0xB2);	//P�gina 2
	
	for(i=256; i<384; i++){
		LCD_wr_data(buffer[i]);
	}
	
	LCD_wr_cmd(0x00);
	LCD_wr_cmd(0x10);
	LCD_wr_cmd(0xB3);	//P�gina 3
	
	for(i=384; i<512; i++){
		LCD_wr_data(buffer[i]);
	}
}

//static void symbolToLocalBuffer_L2(uint8_t symbol){
//	uint8_t i, value1, value2;
//	uint16_t offset = 0;
//	
//	offset = 25*(symbol - ' ');
//	
//	for(i=0; i<12; i++){
//		value1 = Arial12x12[offset+i*2+1];
//		value2 = Arial12x12[offset+i*2+2];
//		
//		buffer[128+i+positionL1] = value1;
//		buffer[i+256+positionL1] = value2;
//	}
//	positionL1 = positionL1 + Arial12x12[offset];
//}

static void symbolToLocalBuffer(uint8_t line, uint8_t symbol){
	uint8_t i, value1, value2;
	uint16_t offset = 0;
	
	if(line == 1){
		positionL1 = 0;
	}else if(line == 2){
		positionL1 = 256;
	}
	
	offset = 25*(symbol - ' ');
	
	for(i=0; i<12; i++){
		value1 = Arial12x12[offset+i*2+1];
		value2 = Arial12x12[offset+i*2+2];
		
		buffer[i+positionL1] = value1;
		buffer[i+128+positionL1] = value2;
	}
	positionL1 = positionL1 + Arial12x12[offset];
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
