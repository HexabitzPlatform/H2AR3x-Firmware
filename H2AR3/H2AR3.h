/*
 BitzOS (BOS) V0.3.4 - Copyright (C) 2017-2024 Hexabitz
 All rights reserved
 
 File Name     : H2AR3.h
 Description   : Header file for module H2AR3.
 (Description_of_module)

 (Description of Special module peripheral configuration):
 >>
 >>
 >>

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
#include "H2AR3_adc.h"
/* Exported definitions -------------------------------------------------------*/

#define	modulePN		_H2AR3

/* Port-related definitions */
#define	NumOfPorts			3

#define P_PROG 				P2						/* ST factory bootloader UART */
/* Define available ports */
#define _P1
#define _P2 
#define _P3 
#define _P4
//#define _P5
//#define _P6

/* Define available USARTs */
#define _Usart1 1
#define _Usart2 1
#define _Usart3 1
#define _Usart4 1
#define _Usart5 1
#define _Usart6	1

/* Port-UART mapping */

#define P1uart &huart4
#define P2uart &huart2
#define P3uart &huart6
#define P4uart &huart1
#define P5uart &huart5
#define P6uart &huart3

/* Port Definitions */
#define	USART1_TX_PIN		GPIO_PIN_9
#define	USART1_RX_PIN		GPIO_PIN_10
#define	USART1_TX_PORT		GPIOA
#define	USART1_RX_PORT		GPIOA
#define	USART1_AF			GPIO_AF1_USART1

#define	USART2_TX_PIN		GPIO_PIN_2
#define	USART2_RX_PIN		GPIO_PIN_3
#define	USART2_TX_PORT		GPIOA
#define	USART2_RX_PORT		GPIOA
#define	USART2_AF			GPIO_AF1_USART2

#define	USART3_TX_PIN		GPIO_PIN_10
#define	USART3_RX_PIN		GPIO_PIN_11
#define	USART3_TX_PORT		GPIOB
#define	USART3_RX_PORT		GPIOB
#define	USART3_AF			GPIO_AF4_USART6

#define	USART4_TX_PIN		GPIO_PIN_0
#define	USART4_RX_PIN		GPIO_PIN_1
#define	USART4_TX_PORT		GPIOA
#define	USART4_RX_PORT		GPIOA
#define	USART4_AF			GPIO_AF4_USART4

#define	USART5_TX_PIN		GPIO_PIN_3
#define	USART5_RX_PIN		GPIO_PIN_2
#define	USART5_TX_PORT		GPIOD
#define	USART5_RX_PORT		GPIOD
#define	USART5_AF			GPIO_AF3_USART5

#define	USART6_TX_PIN		GPIO_PIN_4
#define	USART6_RX_PIN		GPIO_PIN_5
#define	USART6_TX_PORT		GPIOA
#define	USART6_RX_PORT		GPIOA
#define	USART6_AF			GPIO_AF8_USART6

/* Module EEPROM Variables */
#define NUM_MODULE_PARAMS						1
// Module Addressing Space 500 - 599
#define _EE_MODULE							500		

#define Volt                     1
#define Amp                      2
#define VBAIS                    1.5            // VBAIS = 1.5 from Schematics
#define VREF                     3              // VREF  = 3  from Schematics
#define Resolution_12_Bit        4095           //  ADC Resolution
#define Offsite                  0.09633899     // Calculation Offsite (Offsite= volt - VREF) in case no inpout voltag
#define voltRatio                533.33333      // Amplifier ratio ( 150 R / 4M ) * 50
#define shuntResistor            0.1
#define ampTranRatio             1
#define IDLE_CASE                0
#define STREAM_CLI_CASE          1
#define STREAM_PORT_CASE         2
#define STREAM_BUFFER_CASE       3
#define STREAM_CLI_VERBOSE_CASE  4
#define SAMPLE_CLI_CASE          6
#define SAMPLE_PORT_CASE         7
#define SAMPLE_BUFFER_CASE       8
#define SAMPLE_CLI_VERBOSE_CASE  9
#define FILTER_DATA_TYPE         uint32_t
#define AVG_FILTER_ORDER_A       3
#define AVG_FILTER_ORDER_V       10

/* Module_Status Type Definition */
typedef enum {
	H2AR3_OK = 0,
	H2AR3_ERR_UnknownMessage,
	H2AR3_ERR_WrongColor,
	H2AR3_ERR_WrongIntensity,
	H2AR3_ERR_WrongMode,
	H2AR3_ERR_WrongParams,
	H2AR3_ERROR = 255
} Module_Status;

/* Indicator LED */
#define _IND_LED_PORT			GPIOA
#define _IND_LED_PIN			GPIO_PIN_15

/* Export UART variables */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart6;

/* Define UART Init prototypes */
extern void MX_USART1_UART_Init(void);
extern void MX_USART2_UART_Init(void);
extern void MX_USART3_UART_Init(void);
extern void MX_USART4_UART_Init(void);
extern void MX_USART5_UART_Init(void);
extern void MX_USART6_UART_Init(void);
extern void SystemClock_Config(void);
extern void ExecuteMonitor(void);

/* -----------------------------------------------------------------------
 |								  APIs							          |  																 	|
 /* -----------------------------------------------------------------------
 */
Module_Status SampleV(float *volt);
Module_Status SampleA(float *curr);

void SetupPortForRemoteBootloaderUpdate(uint8_t port);
void remoteBootloaderUpdate(uint8_t src, uint8_t dst, uint8_t inport,
		uint8_t outport);

/* -----------------------------------------------------------------------
 |								Commands							      |															 	|
 /* -----------------------------------------------------------------------
 */

#endif /* H2AR3_H */

/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
