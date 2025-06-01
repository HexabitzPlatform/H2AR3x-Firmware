/*
 BitzOS (BOS) V0.4.0 - Copyright (C) 2017-2025 Hexabitz
 All rights reserved

 File Name     : H2AR3_adc.c
 Description   : source file Contains Peripheral ADC setup .
 */

/* Includes ****************************************************************/
#include "H2AR3_adc.h"

/***************************************************************************/
/* Configure I2C ***********************************************************/
/***************************************************************************/

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
 void MX_ADC1_Init(void)
 {

	/* USER CODE BEGIN ADC1_Init 0 */

	/* USER CODE END ADC1_Init 0 */

	ADC_ChannelConfTypeDef sConfig = { 0 };

	/* USER CODE BEGIN ADC1_Init 1 */

	/* USER CODE END ADC1_Init 1 */

	/** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.ScanConvMode = ADC_SCAN_SEQ_FIXED;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc1.Init.LowPowerAutoWait = DISABLE;
	hadc1.Init.LowPowerAutoPowerOff = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DiscontinuousConvMode = ENABLE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_3CYCLES_5;
	hadc1.Init.OversamplingMode = DISABLE;
	hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
	HAL_ADC_Init(&hadc1);
	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_16;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	/* USER CODE BEGIN ADC1_Init 2 */

	/* USER CODE END ADC1_Init 2 */

}

/***************************************************************************/
void HAL_ADC_MspInit(ADC_HandleTypeDef *adcHandle) {

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };
	if (adcHandle->Instance == ADC1) {

		/** Initializes the peripherals clocks
		 */
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
		PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
		HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

		/* ADC1 clock enable */
		__HAL_RCC_ADC_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();

		/**ADC1 GPIO Configuration
		 PA6     ------> ADC1_IN6
		 PB12     ------> ADC1_IN16
		 */
		GPIO_InitStruct.Pin = AMP_ADC_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(AMP_ADC_GPIO_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = VOLT_ADC_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(VOLT_ADC_GPIO_PORT, &GPIO_InitStruct);

	}
}

/***************************************************************************/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *adcHandle) {

	if (adcHandle->Instance == ADC1) {

		/* Peripheral clock disable */
		__HAL_RCC_ADC_CLK_DISABLE();

		/**ADC1 GPIO Configuration
		 PA6     ------> ADC1_IN6
		 PB12     ------> ADC1_IN16
		 */
		HAL_GPIO_DeInit(AMP_ADC_GPIO_PORT, AMP_ADC_PIN);

		HAL_GPIO_DeInit(VOLT_ADC_GPIO_PORT, VOLT_ADC_PIN);
	}
}

/***************************************************************************/
void SelectAmpereADCChannel(void) {
	ADC_ChannelConfTypeDef sConfig = { 0 };

	sConfig.Channel = AMP_ADC_CHANNEL;
	sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	sConfig.SamplingTime = ADC_SAMPLETIME_39CYCLES_5;

	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

/***************************************************************************/
void DeselectAmpereADCChannel(void) {
	ADC_ChannelConfTypeDef sConfig = { 0 };

	sConfig.Channel = AMP_ADC_CHANNEL;
	sConfig.Rank = ADC_RANK_NONE;
	sConfig.SamplingTime = ADC_SAMPLETIME_39CYCLES_5;

	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

/***************************************************************************/
void SelectVoltADCChannel(void) {
	ADC_ChannelConfTypeDef sConfig = { 0 };

	sConfig.Channel = VOLT_ADC_CHANNEL;
	sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	sConfig.SamplingTime = ADC_SAMPLETIME_39CYCLES_5;

	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

/***************************************************************************/
void DeselectVoltADCChannel(void) {
	ADC_ChannelConfTypeDef sConfig = { 0 };

	sConfig.Channel = VOLT_ADC_CHANNEL;
	sConfig.Rank = ADC_RANK_NONE;
	sConfig.SamplingTime = ADC_SAMPLETIME_39CYCLES_5;

	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

/***************************************************************************/
/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
