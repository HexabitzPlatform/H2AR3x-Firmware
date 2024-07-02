/*
 BitzOS (BOS) V0.2.9 - Copyright (C) 2017-2023 Hexabitz
 All rights reserved

 File Name     : H2AR3_adc.h
 Description   : Header file contains Peripheral ADC setup.
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef H2AR3_ADC_H_
#define H2AR3_ADC_H_



/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc;
//extern ADC_HandleTypeDef hadc;

void MX_ADC_Init(void);
void ADC_Select_CH6(void);
void ADC_Deselect_CH6(void);
void ADC_Select_CH16(void);
void ADC_Deselect_CH16(void);

#define volt_measure_input_Pin GPIO_PIN_1
#define volt_measure_input_GPIO_Port GPIOB

#define amp_measure_input_Pin GPIO_PIN_7
#define amp_measure_input_GPIO_Port GPIOA

#endif /* H2AR3_ADC_H_ */
