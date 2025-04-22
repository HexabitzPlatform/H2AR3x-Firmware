/*
 BitzOS (BOS) V0.4.0 - Copyright (C) 2017-2025 Hexabitz
 All rights reserved
 
 File Name     : H2AR3.h
 Description   : Header file for module H2AR3.
 (Description_of_module)

 (Description of Special module peripheral configuration):
 >>
 >>
 >>
 */

/* Define to prevent recursive inclusion ***********************************/
#ifndef H2AR3_H
#define H2AR3_H

/* Includes ****************************************************************/
#include "BOS.h"
#include "H2AR3_MemoryMap.h"
#include "H2AR3_uart.h"
#include "H2AR3_gpio.h"
#include "H2AR3_dma.h"
#include "H2AR3_inputs.h"
#include "H2AR3_eeprom.h"
#include "H2AR3_adc.h"

/* Exported Macros *********************************************************/
#define	MODULE_PN		_H2AR3

/* Port-related Definitions */
#define	NUM_OF_PORTS	3
#define P_PROG 			P2		/* ST factory bootloader UART */

/* Define available ports */
#define _P1
#define _P2
#define _P3

/* Define available USARTs */
#define _USART1
#define _USART2
#define _USART6

/* Port-UART mapping */
#define UART_P1 &huart2
#define UART_P2 &huart6
#define UART_P3 &huart1

/* Module-specific Hardware Definitions ************************************/
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
#define	USART6_AF			GPIO_AF3_USART6

/* ADC Pin Definition */
#define VOLT_ADC_PIN        GPIO_PIN_12
#define VOLT_ADC_GPIO_PORT  GPIOB
#define VOLT_ADC_CHANNEL    ADC_CHANNEL_16

#define AMP_ADC_PIN         GPIO_PIN_6
#define AMP_ADC_GPIO_PORT   GPIOA
#define AMP_ADC_CHANNEL     ADC_CHANNEL_6

/* Indicator LED */
#define _IND_LED_PORT	    GPIOA
#define _IND_LED_PIN		GPIO_PIN_15

/* Module-specific Macro Definitions ***************************************/
/* ADC special parameters */
#define VBAIS                    1.5            // VBAIS = 1.5 from Schematics
#define VREF                     3              // VREF  = 3  from Schematics
#define ADC_RESOLUTION_12_BIT    4095           //  ADC Resolution
#define VOLTAGE_OFFSET           0.09633899     // Calculation Offsite (Offsite= volt - VREF) in case no inpout voltag
#define VOLT_RATIO               533.33333      // Amplifier ratio ( 150 R / 4M ) * 50

/* LPF special parameters */
#define AVG_FILTER_ORDER_A       3
#define AVG_FILTER_ORDER_V       10

#define NUM_MODULE_PARAMS		 2

/* Streaming parameters */
#define MIN_PERIOD_MS		     100
#define MIN_MEMS_PERIOD_MS		 100
#define MAX_MEMS_TIMEOUT_MS		 0xFFFFFFFF
#define SAMPLE_TO_PORT           1
#define STREAM_TO_PORT           2
#define STREAM_TO_Terminal       3
#define DEFAULT                  4

/* Module-specific Type Definition *****************************************/
/* Module-status Type Definition */
typedef enum {
	H2AR3_OK = 0,
	H2AR3_ERR_WRONGPARAMS,
	H2AR3_ERR_TERMINATED,
	H2AR3_ERROR = 255
} Module_Status;

/* AC monitor status type definitions */
typedef enum {
	VOLT = 0,
	AMP
} All_Data;

/* Filter type definitions */
typedef struct {
	uint16_t FilterOrder;
	uint16_t BufferIndex;
	uint32_t DataBuffer[512];
	float *FilterCoeffecients;
} Filter_t;

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

/***************************************************************************/
/***************************** General Functions ***************************/
/***************************************************************************/
Module_Status SampleVoltage(float *volt);
Module_Status SampleCurrent(float *curr);

Module_Status SampletoPort(uint8_t module,uint8_t port,All_Data function);
Module_Status StreamToTerminal(uint8_t port,All_Data function,uint32_t Numofsamples,uint32_t timeout);
Module_Status StreamtoPort(uint8_t module,uint8_t port,All_Data function,uint32_t Numofsamples,uint32_t timeout);

#endif /* H2AR3_H */

/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
