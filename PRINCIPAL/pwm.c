#include "pwm.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects
 
osThreadId_t tid_pwm;                        // thread id
static TIM_OC_InitTypeDef sConfigOC;
static TIM_HandleTypeDef htim1;
 
void pwm (void *argument);                   // thread function
static void initTim(void);
static void initPin(void);
static void setCicloTrabajo(uint32_t ciclo);
 
int Init_pwm (void) {
 
  tid_pwm = osThreadNew(pwm, NULL, NULL);
  if (tid_pwm == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void pwm (void *argument) {
	uint16_t msg;
	
	initPin();
	initTim();
	
  while (1) {
    osMessageQueueGet(mid_MsgQueuePWM, &msg, NULL, osWaitForever);
		setCicloTrabajo(msg*10);
		
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
	
	HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);
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

static void setCicloTrabajo(uint32_t ciclo)
{
	sConfigOC.Pulse = ciclo*10;
	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);
 	HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);
}
