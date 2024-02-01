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
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
	//Periodo de inicio del timer para generar la señal cuadrada
uint32_t freq_inicio = 41999;
	//Periodo del timer para generar la señal cuadrada
uint32_t periodo = 41999;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);
static void initPIN_OUTPUT(void);
static void initTimer(void);
static void InitBoton(void);
void EXTI15_10_IRQHandler(void);

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
  initTimer();
  InitBoton();
  
  HAL_NVIC_EnableIRQ ( EXTI15_10_IRQn);
  
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

/**
  * @brief  Configura el pin PB11 para que salga por el una señal de onda cuadrada
  * @param  None
  * @retval None
  */
static void initPIN_OUTPUT(void){
	//Estructura para configurar el pin
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
	//Habilitamos el puerto correspondiente al pin del timer
  __HAL_RCC_GPIOB_CLK_ENABLE();
	
	//Configuramos el pin para que emplee su función alternativa como timer
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
	GPIO_InitStruct.Pin = GPIO_PIN_11;
  
	//Iniciamos el pin
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * @brief  Configura el timer 2 para 
  * @param  None
  * @retval None
  */
static void initTimer(void){
  
	//Estructura para configurar el timer
  TIM_HandleTypeDef tim2;
	//Estructura para configurar 
  TIM_OC_InitTypeDef TIM_Channel_InitStruct;
  
	//Habilitamos el puerto correspondiente al pin del timer
  __HAL_RCC_TIM2_CLK_ENABLE();
  
	//Configuramos el timer
		/*Apartado A
  tim2.Instance= TIM2;
  tim2.Init.Prescaler = 0;
  tim2.Init.Period = 41999;*/
  
		//Apartado B
  tim2.Instance = TIM2;
  tim2.Init.Prescaler = 0;
  tim2.Init.Period = periodo;
  
	//Iniciamos el timer
  HAL_TIM_OC_Init(&tim2);
  
  //Configuramos el canal por el que saldrá la señal del timer
  TIM_Channel_InitStruct.OCMode = TIM_OCMODE_TOGGLE;
  TIM_Channel_InitStruct.OCPolarity = TIM_OCPOLARITY_HIGH;
  TIM_Channel_InitStruct.OCFastMode = TIM_OCFAST_DISABLE;
  
	//Establecemos las configuraciones del timer
  HAL_TIM_OC_ConfigChannel( &tim2, &TIM_Channel_InitStruct, TIM_CHANNEL_4);
  
	//Iniciamos el canal
  HAL_TIM_OC_Start( &tim2, TIM_CHANNEL_4);
}

/**
  * @brief  Configura el pin del boton
  * @param  None
  * @retval None
  */
static void InitBoton(void){
	//Estructura para configurar el pin del boton
  GPIO_InitTypeDef GPIO_InitStruct;
	
	//Habilitamos el puerto donde se localiza su pin
  __HAL_RCC_GPIOC_CLK_ENABLE();
	
	//Configuramos el pin
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	
	//Iniciamos el pin
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/**
  * @brief  Tratará la interrupción del boton
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

/**
	* @brief  Contendrá las acciones que deberemos realizar cuando salte la interrupcion del boton: cambiar la frecuencia de la señal
  * @param  None
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	//Estructura para configurar el timer 
	TIM_HandleTypeDef tim2;
  
	//Cambiamos la frecuencia de la señal
	periodo = (periodo == freq_inicio) ? 16799 : freq_inicio;
	
	//Inicializamos otra vez el timer con su nueva frecuencia
	
  tim2.Instance = TIM2;
  tim2.Init.Prescaler = 0;
  tim2.Init.Period = periodo;
  
  HAL_TIM_OC_Init(&tim2);
  HAL_TIM_OC_Start( &tim2, TIM_CHANNEL_4);
}

/*****END OF FILE****/
