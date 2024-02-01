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
	//Variable empleada para la frecuencia de los leds
uint16_t periodo = 500;
/* Private function prototypes -----------------------------------------------*/
	//Reloj
static void SystemClock_Config(void);
static void Error_Handler(void);
	//Pines
void InitLeds(uint16_t GPIO_Pin);
void InitBoton(uint16_t GPIO_Pin);
	//Interrupción del boton
void EXTI15_10_IRQHandler(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);


int main(void)
{

  HAL_Init();

  /* Configure the system clock to 168 MHz */
  SystemClock_Config();
  SystemCoreClockUpdate();
	
	//Habilitamos interrupcion
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
	
	//Habilitamos los puertos de los pines
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	//Inicialización Boton
	InitBoton(GPIO_PIN_13);
	//Inicialización LD1 -> Rojo
	InitLeds(GPIO_PIN_13);
	//Inicialización LD2 -> Verde
	InitLeds(GPIO_PIN_12);
	//Inicialización LD3 -> Azul
	InitLeds(GPIO_PIN_11);

	//Apagamos los LEDs para que comiencen apagados
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);

  while (1)
  {
		//Cambia de estado los leds: LD1/LD2/LD3
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_11);
		HAL_Delay(periodo);
		//Cambia de estado el led: LD1
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
		HAL_Delay(periodo);
		//Cambia de estado los leds: LD1/LD2
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
		HAL_Delay(periodo);
		//Cambia de estado el led: LD1
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
		HAL_Delay(periodo);
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
  * @brief  Inicializa los perifericos que se asociaran a los pines para poder usarlos
  * @param  Pin del periferico que se va a configurar
  * @retval None
  */
void InitLeds(uint16_t GPIO_Pin){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	//Configuración de los pines
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pin = GPIO_Pin;
	
	//Inicialización de los pines
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/**
  * @brief  Inicializa el pin del boton para poder usarlo
  * @param  Pin del boton
  * @retval None
  */
void InitBoton(uint16_t GPIO_Pin){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	//Configuración del boton
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	
	//Inicialización del boton
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/**
  * @brief  Función a la que se llamará cuando salte la interrupcion del botón
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

/**
  * @brief  Función que contendrá el las acciones que se deben realizar cuando salte la interrupción del botón
  * @param  Pin debido al cual ha saltado la interrupcion
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	periodo = (periodo != 125) ? periodo/2 : 500;
}


/*****END OF FILE****/
