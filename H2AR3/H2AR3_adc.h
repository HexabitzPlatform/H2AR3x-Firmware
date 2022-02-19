/*
 BitzOS (BOS) V0.2.6 - Copyright (C) 2017-2022 Hexabitz
 All rights reserved

 File Name     : H2AR3_adc.h
 Description   : Header file contains Peripheral ADC setup.
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef H2AR3_ADC_H_
#define H2AR3_ADC_H_



/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

DMA_HandleTypeDef hdma_adc;
ADC_HandleTypeDef hadc;

#define volt_measure_input_Pin GPIO_PIN_1
#define volt_measure_input_GPIO_Port GPIOB

#define amp_measure_input_Pin GPIO_PIN_7
#define amp_measure_input_GPIO_Port GPIOA

#endif /* H2AR3_ADC_H_ */
