/*
    BitzOS (BOS) V0.2.9 - Copyright (C) 2017-2023 Hexabitz
    All rights reserved
		
    File Name     : H2AR3.h
    Description   : Header file for module H2AR3 AC current and voltage sensor.
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef H2AR3_H
#define H2AR3_H

/* Includes ------------------------------------------------------------------*/
#include "BOS.h"
#include "H2AR3_MemoryMap.h"
#include "H2AR3_uart.h"
#include "H2AR3_gpio.h"
#include "H2AR3_dma.h"
#include "H2AR3_inputs.h"
#include "H2AR3_eeprom.h"
	
/* Exported definitions -------------------------------------------------------*/

#define	modulePN			_H2AR3
#define _IND_LED_PORT		GPIOA
#define _IND_LED_PIN		GPIO_PIN_11


/* Port-related definitions */
#define	NumOfPorts			3
#define P_PROG 				P2						/* ST factory bootloader UART */

/* Define available ports */
#define _P1 
#define _P2 
#define _P3 


/* Define available USARTs */
#define _Usart1 1
#define _Usart2 1
#define _Usart3 1
#define _Usart4 1
#define _Usart6 1

/* Port-UART mapping */
#define P1uart &huart2
#define P2uart &huart6
#define P3uart &huart3

/* Port Definitions */
#define	USART2_TX_PIN		GPIO_PIN_2
#define	USART2_RX_PIN		GPIO_PIN_3
#define	USART2_TX_PORT		GPIOA
#define	USART2_RX_PORT		GPIOA
#define	USART2_AF			GPIO_AF1_USART2

#define	USART3_TX_PIN		GPIO_PIN_10
#define	USART3_RX_PIN		GPIO_PIN_11
#define	USART3_TX_PORT		GPIOB
#define	USART3_RX_PORT		GPIOB
#define	USART3_AF			GPIO_AF4_USART3

#define	USART6_TX_PIN		GPIO_PIN_4
#define	USART6_RX_PIN		GPIO_PIN_5
#define	USART6_TX_PORT		GPIOA
#define	USART6_RX_PORT		GPIOA
#define	USART6_AF			GPIO_AF5_USART6


#define NUM_MODULE_PARAMS		1

typedef enum  { STATE_OFF, STATE_ON, STATE_PWM } Relay_state_t; 


#define TIMERID_TIMEOUT_MEASUREMENT   0xFF
#define STOP_MEASUREMENT_RANGING      0
#define START_MEASUREMENT_RANGING     1

/* H01R0_Status Type Definition */  
typedef enum 
{
    H2AR3_OK = 0,
	H2AR3_ERR_UnknownMessage = 1,
	H2AR3_ERR_WrongParams = 2,
	H2AR3_ERROR = 255
} Module_Status;


/* Indicator LED */


/* Export UART variables */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart6;

/* Define ADC_DMA Init prototypes */
extern DMA_HandleTypeDef hdma_adc;
extern ADC_HandleTypeDef hadc;

/* Define UART Init prototypes */
extern void MX_USART1_UART_Init(void);
extern void MX_USART2_UART_Init(void);
extern void MX_USART3_UART_Init(void);
extern void MX_USART4_UART_Init(void);
extern void MX_USART6_UART_Init(void);

/* Define UART Init prototypes */
extern void MX_ADC_Init(void);
//extern void ADC_Channel_config(void);

extern void ADC_Select_CH7(void);
extern void ADC_Deselect_CH7(void);
extern void ADC_Select_CH9(void);
extern void ADC_Deselect_CH9(void);

extern uint32_t Volt_buffer[1];
extern uint32_t Amp_buffer[1];
/* -----------------------------------------------------------------------
	|																APIs	 																 	|
   ----------------------------------------------------------------------- 
*/

extern uint8_t SampleV(float * temp);
extern uint8_t SampleA(float * temp);
extern float CalculationVolt(void);
extern float CalculationAmp(void);
#endif



/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/


///*
//    BitzOS (BOS) V0.2.4 - Copyright (C) 2017-2021 Hexabitz
//    All rights reserved
//
//    File Name     : H2AR3.h
//    Description   : Header file for module H2AR3 AC current and voltage sensor.
//*/
///* Define to prevent recursive inclusion -------------------------------------*/
//#ifndef H2AR3_H
//#define H2AR3_H
//
///* Includes ------------------------------------------------------------------*/
//#include "BOS.h"
//#include "H2AR3_MemoryMap.h"
//#include "H2AR3_uart.h"
//#include "H2AR3_gpio.h"
//#include "H2AR3_dma.h"
//
//
///* Exported definitions -------------------------------------------------------*/
//
//#define	modulePN			_H2AR3
//#define _IND_LED_PORT		GPIOA
//#define _IND_LED_PIN		GPIO_PIN_11
//
//
///* Port-related definitions */
//#define	NumOfPorts			5
//#define P_PROG 				P2						/* ST factory bootloader UART */
//
///* Define available ports */
//#define _P1
//#define _P2
//#define _P3
//#define _P4
//#define _P5
//
///* Define available USARTs */
//#define _Usart1 1
//#define _Usart2 1
//#define _Usart3 1
//#define _Usart4 1
//#define _Usart6 1
//
///* Port-UART mapping */
//#define P1uart &huart4
//#define P2uart &huart2
//#define P3uart &huart6
//#define P4uart &huart3
//#define P5uart &huart1
//
///* Port Definitions */
//#define	USART1_TX_PIN		GPIO_PIN_9
//#define	USART1_RX_PIN		GPIO_PIN_10
//#define	USART1_TX_PORT		GPIOA
//#define	USART1_RX_PORT		GPIOA
//#define	USART1_AF			GPIO_AF1_USART1
//
//#define	USART2_TX_PIN		GPIO_PIN_2
//#define	USART2_RX_PIN		GPIO_PIN_3
//#define	USART2_TX_PORT		GPIOA
//#define	USART2_RX_PORT		GPIOA
//#define	USART2_AF			GPIO_AF1_USART2
//
//#define	USART3_TX_PIN		GPIO_PIN_10
//#define	USART3_RX_PIN		GPIO_PIN_11
//#define	USART3_TX_PORT		GPIOB
//#define	USART3_RX_PORT		GPIOB
//#define	USART3_AF			GPIO_AF4_USART3
//
//#define	USART4_TX_PIN		GPIO_PIN_0
//#define	USART4_RX_PIN		GPIO_PIN_1
//#define	USART4_TX_PORT		GPIOA
//#define	USART4_RX_PORT		GPIOA
//#define	USART4_AF			GPIO_AF4_USART4
//
//#define	USART5_TX_PIN		GPIO_PIN_3
//#define	USART5_RX_PIN		GPIO_PIN_4
//#define	USART5_TX_PORT	    GPIOB
//#define	USART5_RX_PORT		GPIOB
//#define	USART5_AF			GPIO_AF4_USART5
//
//#define	USART6_TX_PIN		GPIO_PIN_4
//#define	USART6_RX_PIN		GPIO_PIN_5
//#define	USART6_TX_PORT		GPIOA
//#define	USART6_RX_PORT		GPIOA
//#define	USART6_AF			GPIO_AF5_USART6
//
//
//#define NUM_MODULE_PARAMS		1
//
//typedef enum  { STATE_OFF, STATE_ON, STATE_PWM } Relay_state_t;
//
//
//#define TIMERID_TIMEOUT_MEASUREMENT   0xFF
//#define STOP_MEASUREMENT_RANGING      0
//#define START_MEASUREMENT_RANGING     1
//
///* H01R0_Status Type Definition */
//typedef enum
//{
//    H2AR3_OK = 0,
//	H2AR3_ERR_UnknownMessage = 1,
//	H2AR3_ERR_WrongParams = 2,
//	H2AR3_ERROR = 255
//} Module_Status;
//
//
///* Indicator LED */
//
//
///* Export UART variables */
//extern UART_HandleTypeDef huart1;
//extern UART_HandleTypeDef huart2;
//extern UART_HandleTypeDef huart3;
//extern UART_HandleTypeDef huart4;
//extern UART_HandleTypeDef huart6;
//
///* Define ADC_DMA Init prototypes */
//extern DMA_HandleTypeDef hdma_adc;
//extern ADC_HandleTypeDef hadc;
//
///* Define UART Init prototypes */
//extern void MX_USART1_UART_Init(void);
//extern void MX_USART2_UART_Init(void);
//extern void MX_USART3_UART_Init(void);
//extern void MX_USART4_UART_Init(void);
//extern void MX_USART6_UART_Init(void);
//
///* Define UART Init prototypes */
//extern void MX_ADC_Init(void);
////extern void ADC_Channel_config(void);
//
//extern void ADC_Select_CH7(void);
//extern void ADC_Deselect_CH7(void);
//extern void ADC_Select_CH9(void);
//extern void ADC_Deselect_CH9(void);
//
//extern uint32_t Volt_buffer[1];
//extern uint32_t Amp_buffer[1];
///* -----------------------------------------------------------------------
//	|																APIs	 																 	|
//   -----------------------------------------------------------------------
//*/
//
//extern int SampleV(float * temp);
//extern int SampleA(float * temp);
//extern float CalculationVolt(void);
//extern float CalculationAmp(void);
//#endif
void SetupPortForRemoteBootloaderUpdate(uint8_t port);
void remoteBootloaderUpdate(uint8_t src,uint8_t dst,uint8_t inport,uint8_t outport);
///************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
