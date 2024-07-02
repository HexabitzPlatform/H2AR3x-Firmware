/*
 BitzOS (BOS) V0.2.9 - Copyright (C) 2017-2023 Hexabitz
 All rights reserved

 File Name     : H2AR3_adc.c
 Description   : source file Contains Peripheral ADC setup .
 */
/* Includes ------------------------------------------------------------------*/
#include "H2AR3_adc.h"

void ADC_Select_CH6(void) {
	ADC_ChannelConfTypeDef sConfig = { 0 };
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	sConfig.SamplingTime = ADC_SAMPLETIME_39CYCLES_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {

	}

}
/*-----------------------------------------------------------*/

void ADC_Deselect_CH6(void) {
	ADC_ChannelConfTypeDef sConfig = { 0 };
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = ADC_RANK_NONE;
	sConfig.SamplingTime = ADC_SAMPLETIME_39CYCLES_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {

	}
}


void ADC_Select_CH16(void) {
	ADC_ChannelConfTypeDef sConfig = { 0 };
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_16;
	sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	sConfig.SamplingTime = ADC_SAMPLETIME_39CYCLES_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {

	}

}
/*-----------------------------------------------------------*/

void ADC_Deselect_CH16(void) {
	ADC_ChannelConfTypeDef sConfig = { 0 };
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_16;
	sConfig.Rank = ADC_RANK_NONE;
	sConfig.SamplingTime = ADC_SAMPLETIME_39CYCLES_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {

	}
}
