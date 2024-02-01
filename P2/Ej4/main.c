/*
	Autores: Iván Isabel Sierra
					 José Perandrés García de la Serrana
	Grupo: L05L06X03X04B
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#endif

/* Private typedef -----------------------------------------------------------*/
static TIM_HandleTypeDef tim3;
static TIM_HandleTypeDef tim2;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t periodo = 41999;
static volatile uint32_t valor1_IC = 0;
static volatile uint32_t valor2_IC = 0;
static volatile uint32_t frecuencia = 0;
static volatile uint32_t diferencia = 0;
static volatile uint8_t capturaHecha = 0;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);
static void initPIN_OUTPUT(void);
static void initPIN_INPUT(void);
static void TIM3_Init(void);
static void TIM2_Init(void);
static void InitBoton(void);
void EXTI15_10_IRQHandler(void);
void TIM3_IRQHandler(void);

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

  initPIN_OUTPUT();
	initPIN_INPUT();
  TIM3_Init();
	TIM2_Init();
  InitBoton();
	HAL_TIM_IC_Start_IT(&tim3, TIM_CHANNEL_1);
  
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  
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
  *            PLL_M                          = 4
  *            PLL_N                          = 168
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

static void initPIN_OUTPUT(void){
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
	GPIO_InitStruct.Pin = GPIO_PIN_11;
  
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void initPIN_INPUT(void){
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
	GPIO_InitStruct.Pin = GPIO_PIN_6;
  
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void TIM2_Init(void)
{
  TIM_OC_InitTypeDef TIM_Channel_InitStruct;
  
  __HAL_RCC_TIM2_CLK_ENABLE();
  
  tim2.Instance = TIM2;
  tim2.Init.Prescaler = 0;
  tim2.Init.Period = periodo;
  HAL_TIM_OC_Init(&tim2);
  
  TIM_Channel_InitStruct.OCMode = TIM_OCMODE_TOGGLE;
  TIM_Channel_InitStruct.OCPolarity = TIM_OCPOLARITY_HIGH;
  TIM_Channel_InitStruct.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_OC_ConfigChannel( &tim2, &TIM_Channel_InitStruct, TIM_CHANNEL_4);
  
  HAL_TIM_OC_Start( &tim2, TIM_CHANNEL_4);
}

static void TIM3_Init(void){
  TIM_IC_InitTypeDef sConfigIC;
  
  __HAL_RCC_TIM3_CLK_ENABLE();
  
  tim3.Instance = TIM3;
  tim3.Init.Prescaler = 1;
  tim3.Init.Period = 0xffff;
	tim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  HAL_TIM_IC_Init(&tim3);
  
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sConfigIC.ICSelection = 	TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 0;
  HAL_TIM_IC_ConfigChannel(&tim3, &sConfigIC, TIM_CHANNEL_1);
	
}

static void InitBoton(void){
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

//Definicion Funciones IRQ y Callback
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if( periodo == 41999)
    periodo = 16799;
  else
    periodo = 41999;
	
  tim2.Instance = TIM2;
  tim2.Init.Prescaler = 0;
  tim2.Init.Period = periodo;
  
  HAL_TIM_OC_Init(&tim2);
  HAL_TIM_OC_Start( &tim2, TIM_CHANNEL_4);
}

void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&tim3);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		if(capturaHecha == 0)
		{
			valor1_IC = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			capturaHecha = 1;
		}
		else if(capturaHecha == 1)
		{
			valor2_IC = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			if(valor2_IC >= valor1_IC)
				diferencia = valor2_IC - valor1_IC;
			else
				diferencia = (tim3.Instance->ARR - valor1_IC) + valor2_IC;
			
			frecuencia = 2*HAL_RCC_GetPCLK1Freq() / (tim3.Instance->PSC + 1);
			frecuencia = frecuencia / diferencia;
			
			capturaHecha = 0;
		}
		
	}
}

/******END FILE******/
