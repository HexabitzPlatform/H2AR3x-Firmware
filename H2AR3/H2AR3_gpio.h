/*
 BitzOS (BOS) V0.2.9 - Copyright (C) 2017-2023 Hexabitz
 All rights reserved

 File Name     : H2AR3_gpio.h
 Description   : header file Contains Peripheral GPIO setup .
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __H2AH3_gpio_H
#define __H2AR3_gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"


extern void GPIO_Init(void);
extern void IND_LED_Init(void);
extern void Relay_Init(void);


#ifdef __cplusplus
}
#endif
#endif /*__H2AR3_gpio_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
