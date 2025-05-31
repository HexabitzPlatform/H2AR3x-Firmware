/*
 BitzOS (BOS) V0.4.0 - Copyright (C) 2017-2025 Hexabitz
 All rights reserved

 File Name     : H2AR3_adc.h
 Description   : Header file contains Peripheral ADC setup.
 */

/* Define to prevent recursive inclusion ***********************************/
#ifndef H2AR3_ADC_H_
#define H2AR3_ADC_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ****************************************************************/
#include "BOS.h"

/* Exported Variables ******************************************************/
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc;

/* Exported Functions ******************************************************/
//void MX_ADC_Init(void);
void SelectAmpereADCChannel(void);
void DeselectAmpereADCChannel(void);
void SelectVoltADCChannel(void);
void DeselectVoltADCChannel(void);

#ifdef __cplusplus
}
#endif

#endif /* H2AR3_ADC_H_ */

/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
