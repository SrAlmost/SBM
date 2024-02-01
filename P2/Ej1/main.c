/*
	Autores: Iván Isabel Sierra
					 José Perandrés García de la Serrana
	Grupo: L05L06X03X04B
*/

/*APARTADO A
    *Frecuencia del reloj TIM7 -> 84MHz
		Corregimos el ejemplo para que la frecuencia del reloj coincida con la que utilizamos nosotros:
		htim7.Init.Prescaler = 41999;
		htim7.Init.Period = 999;
		
		*Frecuencia de encendido y apagado del led: 1 Hz -> 1 s ( 2 Hz -> 0,5 s apagado/encendido)
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
	//Estructura para configurar el TIM7
	TIM_HandleTypeDef htim7 = {0};
	
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);
void LED_Init(void);
void Timer_Init(void);
void TIM7_IRQHandler(void);

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

	//Iniciamos el pin del led
  LED_Init();
	
	//Iniciamos el timer 7
	Timer_Init();
 
	//Iniciamos el timer 7
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
  * @brief  Gestiona la interrupción del Timer 7
  * @param  None
  * @retval None
  */
void TIM7_IRQHandler(void){
  HAL_TIM_IRQHandler(&htim7);
}

/**
	* @brief  Contiene la accion: encender y apagar el led, que se deben realizar al recibir al interrupción del timer
  * @param  None
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef *htim){
	
  if( htim-> Instance == TIM7){
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
	}
	
}

/**
  * @brief  Inicializa el pin del led que vamos a encender
  * @param  None
  * @retval None
  */
void LED_Init(void){
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
	//Iniciamos el puerto donde se localiza el pin del led
  __HAL_RCC_GPIOB_CLK_ENABLE();
	
	//Configuramos el pin
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	
	//Iniciamos el pin
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Timer_Init(void){
	
	//Iniciamos el puerto correspondiente al timer 7
	__HAL_RCC_TIM7_CLK_ENABLE();
	
	//Configuramos el timer para que tenga una frecuencia de 2/3 Hz (1,5 s)
	htim7.Instance= TIM7;
  htim7.Init.Prescaler = 41999;
  htim7.Init.Period = 2999;
	
	//Activamos la interrupción del timer
	HAL_NVIC_EnableIRQ( TIM7_IRQn);
	
  //Iniciamos el timer
  HAL_TIM_Base_Init(&htim7);
  HAL_TIM_Base_Start_IT( &htim7);
}

/*****END OF FILE****/
