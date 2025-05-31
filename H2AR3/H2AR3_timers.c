/*
 BitzOS (BOS) V0.4.0 - Copyright (C) 2017-2025 Hexabitz
 All rights reserved

 File Name     : H2AR3_timers.c
 Description   : Peripheral timers setup source file.

 Required MCU resources :

 >> Timer 14 for micro-sec delay.
 >> Timer 15 for milli-sec delay.

 */

/* Includes ****************************************************************/
#include "BOS.h"

/* Exported Functions ******************************************************/
void TIM_USEC_Init(void);
void TIM_MSEC_Init(void);
void MX_IWDG_Init(void);

/* Exported Variables ******************************************************/
TIM_HandleTypeDef htim16; /* micro-second delay counter */
TIM_HandleTypeDef htim17; /* milli-second delay counter */
IWDG_HandleTypeDef hiwdg;
TIM_HandleTypeDef htim1;
/***************************************************************************/
/* Configure Timers ********************************************************/
/***************************************************************************/

 void MX_TIM1_Init(void)
 {

	/* USER CODE BEGIN TIM1_Init 0 */

	/* USER CODE END TIM1_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM1_Init 1 */

	/* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 64 - 1;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 100 - 1;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(&htim1);
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig);
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig);
	/* USER CODE BEGIN TIM1_Init 2 */

	/* USER CODE END TIM1_Init 2 */

}
/* IWDG init function */
void MX_IWDG_Init(void){

	/* Reload Value = [(Time * 32 KHz) / (4 * 2^(pr) * 1000)] - 1
	 * RL = [(500 mS * 32000) / (4 * 2^1 * 1000)]  - 1 = 2000 - 1 =1999
	 * timeout time = 500 mS
	 * Pre-scaler = 8
	 * Reload Value = 1999
	 *  */

	hiwdg.Instance = IWDG;
	hiwdg.Init.Prescaler = IWDG_PRESCALER_8;
	hiwdg.Init.Window = IWDG_WINDOW_DISABLE;
	hiwdg.Init.Reload =1999;

	HAL_IWDG_Init(&hiwdg);

}

/***************************************************************************/
/* Micro-seconds timebase init function - TIM16 (16-bit) */
void TIM_USEC_Init(void){
	__TIM16_CLK_ENABLE();

	htim16.Instance = TIM16;
	htim16.Init.Prescaler =47;
	htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim16.Init.Period =0XFFFF;
	htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim16.Init.RepetitionCounter =0;
	htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(&htim16);

	HAL_TIM_Base_Start(&htim16);

}

/***************************************************************************/
/* Milli-seconds timebase init function - TIM17 (16-bit) */
void TIM_MSEC_Init(void){

	__TIM17_CLK_ENABLE();

	htim17.Instance = TIM17;
	htim17.Init.Prescaler =47999;
	htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim17.Init.Period =0xFFFF;
	htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim17.Init.RepetitionCounter =0;
	htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(&htim17);

	HAL_TIM_Base_Start(&htim17);
}

/***************************************************************************/
/* Load and start micro-second delay counter */
void StartMicroDelay(uint16_t Delay){
	uint32_t t0 =0;
	
	portENTER_CRITICAL();
	
	if(Delay){
		t0 =htim16.Instance->CNT;
		
		while(htim16.Instance->CNT - t0 <= Delay){
		};
	}

	portEXIT_CRITICAL();
}

/***************************************************************************/
/* Load and start milli-second delay counter */
void StartMilliDelay(uint16_t Delay){
	uint32_t t0 =0;
	
	portENTER_CRITICAL();
	
	if(Delay){
		t0 =htim17.Instance->CNT;
		
		while(htim17.Instance->CNT - t0 <= Delay){
		};
	}

	portEXIT_CRITICAL();
}

/**
* @brief TIM_Base MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(htim_base->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspInit 0 */

  /* USER CODE END TIM1_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_TIM1;
    PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLKSOURCE_PCLK1;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    /* Peripheral clock enable */
    __HAL_RCC_TIM1_CLK_ENABLE();
    /* TIM1 interrupt Init */
    HAL_NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
  /* USER CODE BEGIN TIM1_MspInit 1 */

  /* USER CODE END TIM1_MspInit 1 */

  }

}

/**
* @brief TIM_Base MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspDeInit 0 */

  /* USER CODE END TIM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM1_CLK_DISABLE();

    /* TIM1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
  /* USER CODE BEGIN TIM1_MspDeInit 1 */

  /* USER CODE END TIM1_MspDeInit 1 */
  }

}

/***************************************************************************/
/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
