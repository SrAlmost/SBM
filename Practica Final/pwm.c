#include "pwm.h"
static osThreadId_t tid_pwm;                        // thread id
static TIM_OC_InitTypeDef sConfigOC;
TIM_HandleTypeDef htim1;
static uint8_t PWM;
 
__NO_RETURN static void pwm (void *argument);                   // thread function
static void initTim(void);
static void initPin(void);
static void setCicloTrabajo(uint8_t ciclo);
static void InitLeds( void );
static void encenderRGB( void );
 
int8_t Init_pwm (void) {
  tid_pwm = osThreadNew(pwm, NULL, NULL);
  if (tid_pwm == NULL) {    return(-1);  }
  return(0);
}
 
__NO_RETURN static void pwm (void *argument) {
	initPin();
	initTim();
	InitLeds();
  while (1) {
    osMessageQueueGet(mid_MsgQueuePWM, &PWM, NULL, osWaitForever);
		setCicloTrabajo(PWM);
		encenderRGB();
    osThreadYield();                            // suspend thread
  }
}

static void initTim(void)
{
	__HAL_RCC_TIM1_CLK_ENABLE();
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 499;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 999;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&htim1);
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0; //A esto no se le suma 1, poner 500 para 50% ciclo trabajo
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);
}

static void initPin(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOE_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;	
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

static void setCicloTrabajo(uint8_t ciclo)
{
	sConfigOC.Pulse = ciclo;
	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);
 	HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);
}

static void InitLeds(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	//Configuraci?n de los pines
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	//Inicializacion del pin Rojo
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	//Inicializacion del pin Verde
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	//Inicializacion del pin Azul
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

static void encenderRGB(){
	if( PWM == 0){
		HAL_GPIO_WritePin( GPIOD, GPIO_PIN_11, GPIO_PIN_SET); //encendemos led azul
		HAL_GPIO_WritePin( GPIOD, GPIO_PIN_12, GPIO_PIN_RESET); //apagamos led verde
		HAL_GPIO_WritePin( GPIOD, GPIO_PIN_13, GPIO_PIN_RESET); //apagamos led rojo
	} else if( PWM == 40){
		HAL_GPIO_WritePin( GPIOD, GPIO_PIN_11, GPIO_PIN_SET); //encendemos led azul
		HAL_GPIO_WritePin( GPIOD, GPIO_PIN_12, GPIO_PIN_SET); //encendemos led verde
		HAL_GPIO_WritePin( GPIOD, GPIO_PIN_13, GPIO_PIN_RESET); //apagamos led rojo
	} else if( PWM == 80){
		HAL_GPIO_WritePin( GPIOD, GPIO_PIN_13, GPIO_PIN_SET); //encendemos led rojo
		HAL_GPIO_WritePin( GPIOD, GPIO_PIN_12, GPIO_PIN_SET); //encendemos led verde
		HAL_GPIO_WritePin( GPIOD, GPIO_PIN_11, GPIO_PIN_RESET); //apagamos led azul
	} else if( PWM == 100){
		HAL_GPIO_WritePin( GPIOD, GPIO_PIN_13, GPIO_PIN_SET); //encendemos led rojo
		HAL_GPIO_WritePin( GPIOD, GPIO_PIN_12, GPIO_PIN_RESET); //apagamos led verde
		HAL_GPIO_WritePin( GPIOD, GPIO_PIN_11, GPIO_PIN_RESET); //apagamos led azul
	}
}
