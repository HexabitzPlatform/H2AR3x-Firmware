/*
 BitzOS (BOS)V0.2.7 - Copyright (C) 2017-2022 Hexabitz
 All rights reserved

 File Name     : H2AR3.c
 Description   : Source code for module H2AR3 AC current and voltage sensor.

 Required MCU resources :

 >> USARTs 2,3,4,5,6 for module ports.
 >> PA6 for HX711 RATE control.
 >> PA9 for HX711 PD_SCK.
 >> PA10 for HX711 DOUT.
 >> Gain of ch1 is fixed at 128.
 >> Gain of ch2 is fixed at 32.
*/
/* Includes ------------------------------------------------------------------*/
#include "BOS.h"
#include "H2AR3_inputs.h"
/* Define UART variables */
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart6;

/* Module exported parameters ------------------------------------------------*/
module_param_t modParam[NUM_MODULE_PARAMS] = { { .paramPtr = NULL,
		.paramFormat = FMT_FLOAT, .paramName = "" } };

/* Exported variables */
extern FLASH_ProcessTypeDef pFlash;
extern uint8_t numOfRecordedSnippets;

//uint32_t Thermo_buffer[1] = { 0 };
volatile uint32_t raw_adc, tmp_adc;
float _volt;

uint32_t Volt_buffer[1] = { 0 };
uint32_t Amp_buffer[1] = { 0 };

TIM_HandleTypeDef htim3;

TaskHandle_t VoltAmpHandle = NULL;
TimerHandle_t xTimer = NULL;
uint8_t startMeasurementRanging = STOP_MEASUREMENT_RANGING;


float DATA_To_SEND = 0.0f;     //float
//STM32CubeMonitor variable
float DATA_To_SEND1 __attribute__((section(".mySection")));

float measured_volt, measured_amp;

/* Private variables ---------------------------------------------------------*/

#define Volt 1
#define Amp 2
#define VRef 1.65
#define voltRatio 533.33333 //Amplifier ratio (150 / 4M) * 50
#define shuntResistor 0.1
#define ampTranRatio 1

#define IDLE_CASE                0
#define STREAM_CLI_CASE          1
#define STREAM_PORT_CASE         2
#define STREAM_BUFFER_CASE       3
#define STREAM_CLI_VERBOSE_CASE  4
#define SAMPLE_CLI_CASE          6
#define SAMPLE_PORT_CASE         7
#define SAMPLE_BUFFER_CASE       8
#define SAMPLE_CLI_VERBOSE_CASE  9

uint8_t global_port, global_module, global_mode, unit = Volt;
uint32_t global_period, global_timeout;
float volt_buffer;
float amp_buffer;
float *ptr_read_buffer;

uint8_t H2AR3_DATA_FORMAT = FMT_FLOAT;
float H2AR3_Read_V;
float H2AR3_Read_A;


/* Private function prototypes -----------------------------------------------*/
uint8_t SendResults(float message, uint8_t Mode, uint8_t unit, uint8_t Port,uint8_t Module, float *Buffer);
float CalculationVolt(void);
float CalculationAmp(void);
uint8_t SampleV(float *temp);
uint8_t SampleA(float *temp);
uint8_t StreamVToPort(uint8_t Port, uint8_t Module, uint32_t Period, uint32_t Timeout);
uint8_t StreamAToPort(uint8_t Port, uint8_t Module, uint32_t Period, uint32_t Timeout);
void VoltAmpTask(void *argument);
void TimerTask(void *argument);
static void CheckForEnterKey(void);
static void HandleTimeout(TimerHandle_t xTimer);

/* Create CLI commands --------------------------------------------------------*/

static portBASE_TYPE sampleCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen,const int8_t *pcCommandString);
static portBASE_TYPE streamCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen,const int8_t *pcCommandString);
//static portBASE_TYPE StreamCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);
static portBASE_TYPE stopCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen,const int8_t *pcCommandString);
static portBASE_TYPE unitCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen,const int8_t *pcCommandString);
static portBASE_TYPE demoCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen,const int8_t *pcCommandString);

/*-----------------------------------------------------------*/

/* CLI command structure : demo */
const CLI_Command_Definition_t demoCommandDefinition =
		{ (const int8_t*) "demo", /* The command string to type. */
				(const int8_t*) "demo:\r\n Run a demo program to test module functionality\r\n\r\n",
				demoCommand, /* The function to run. */
				0
		/* one parameter is expected. */
		};

/* CLI command structure : sample */
const CLI_Command_Definition_t sampleCommandDefinition =
		{ (const int8_t*) "sample", /* The command string to type. */
				(const int8_t*) "sample:\r\n Take one sample with the determinated unit \r\n\r\n",
				sampleCommand, /* The function to run. */
				0
		/* one parameter is expected. */
		};

/* CLI command structure : stream */
const CLI_Command_Definition_t streamCommandDefinition =
		{ (const int8_t*) "stream", /* The command string to type. */
				(const int8_t*) "stream:\r\n Stream temp to the CLI, buffer or port with period (ms) and total time (ms). \r\n\r\n",
				streamCommand, /* The function to run. */
				-1
		/* Multiparameters are expected. */
		};

/* CLI command structure : unit */
const CLI_Command_Definition_t unitCommandDefinition = { (const int8_t*) "unit", /* The command string to type. */

(const int8_t*) "unit:\r\n Set the measurement unit (V, A)\r\n\r\n",
		unitCommand, /* The function to run. */
		1
/* one parameter is expected. */
};

/* CLI command structure : stop */
const CLI_Command_Definition_t stopCommandDefinition =
		{ (const int8_t*) "stop", /* The command string to type. */
				(const int8_t*) "stop:\r\n Stop streaming \r\n\r\n",
				stopCommand, /* The function to run. */
				0
		/* No parameters are expected. */
		};



/* -----------------------------------------------------------------------
 |												 Private Functions	 														|
 -----------------------------------------------------------------------
 */

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = 16;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1
			| RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_USART3;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_HSE_DIV32;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	__SYSCFG_CLK_ENABLE()
	;

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

}

/*-----------------------------------------------------------*/
/* --- Save array topology and Command Snippets in Flash RO --- 
 */
uint8_t SaveToRO(void) {
	BOS_Status result = BOS_OK;
	HAL_StatusTypeDef FlashStatus = HAL_OK;
	uint16_t add = 2, temp = 0;
	uint8_t snipBuffer[sizeof(snippet_t) + 1] = { 0 };

	HAL_FLASH_Unlock();

	/* Erase RO area */
	FLASH_PageErase(RO_START_ADDRESS);
	FlashStatus = FLASH_WaitForLastOperation(
			(uint32_t) HAL_FLASH_TIMEOUT_VALUE);
	if (FlashStatus != HAL_OK) {
		return pFlash.ErrorCode;
	} else {
		/* Operation is completed, disable the PER Bit */
		CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
	}

	/* Save number of modules and myID */
	if (myID) {
		temp = (uint16_t) (N << 8) + myID;
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, RO_START_ADDRESS, temp);
		FlashStatus = FLASH_WaitForLastOperation(
				(uint32_t) HAL_FLASH_TIMEOUT_VALUE);
		if (FlashStatus != HAL_OK) {
			return pFlash.ErrorCode;
		} else {
			/* If the program operation is completed, disable the PG Bit */
			CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
		}

		/* Save topology */
		for (uint8_t i = 1; i <= N; i++) {
			for (uint8_t j = 0; j <= MaxNumOfPorts; j++) {
				if (array[i - 1][0]) {
					HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,
					RO_START_ADDRESS + add, array[i - 1][j]);
					add += 2;
					FlashStatus = FLASH_WaitForLastOperation(
							(uint32_t) HAL_FLASH_TIMEOUT_VALUE);
					if (FlashStatus != HAL_OK) {
						return pFlash.ErrorCode;
					} else {
						/* If the program operation is completed, disable the PG Bit */
						CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
					}
				}
			}
		}
	}

	// Save Command Snippets
	int currentAdd = RO_MID_ADDRESS;
	for (uint8_t s = 0; s < numOfRecordedSnippets; s++) {
		if (snippets[s].cond.conditionType) {
			snipBuffer[0] = 0xFE;		// A marker to separate Snippets
			memcpy((uint8_t*) &snipBuffer[1], (uint8_t*) &snippets[s],
					sizeof(snippet_t));
			// Copy the snippet struct buffer (20 x numOfRecordedSnippets). Note this is assuming sizeof(snippet_t) is even.
			for (uint8_t j = 0; j < (sizeof(snippet_t) / 2); j++) {
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, currentAdd,
						*(uint16_t*) &snipBuffer[j * 2]);
				FlashStatus = FLASH_WaitForLastOperation(
						(uint32_t) HAL_FLASH_TIMEOUT_VALUE);
				if (FlashStatus != HAL_OK) {
					return pFlash.ErrorCode;
				} else {
					/* If the program operation is completed, disable the PG Bit */
					CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
					currentAdd += 2;
				}
			}
			// Copy the snippet commands buffer. Always an even number. Note the string termination char might be skipped
			for (uint8_t j = 0; j < ((strlen(snippets[s].cmd) + 1) / 2); j++) {
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, currentAdd,
						*(uint16_t*) (snippets[s].cmd + j * 2));
				FlashStatus = FLASH_WaitForLastOperation(
						(uint32_t) HAL_FLASH_TIMEOUT_VALUE);
				if (FlashStatus != HAL_OK) {
					return pFlash.ErrorCode;
				} else {
					/* If the program operation is completed, disable the PG Bit */
					CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
					currentAdd += 2;
				}
			}
		}
	}

	HAL_FLASH_Lock();

	return result;
}

/* --- Clear array topology in SRAM and Flash RO --- 
 */
uint8_t ClearROtopology(void) {
	// Clear the array 
	memset(array, 0, sizeof(array));
	N = 1;
	myID = 0;

	return SaveToRO();
}
void initialValue(void)
{
	DATA_To_SEND1=0.0;
}


/* --- Trigger ST factory bootloader update for a remote module.
 */
void remoteBootloaderUpdate(uint8_t src,uint8_t dst,uint8_t inport,uint8_t outport){

	uint8_t myOutport =0, lastModule =0;
	int8_t *pcOutputString;

	/* 1. Get route to destination module */
	myOutport =FindRoute(myID,dst);
	if(outport && dst == myID){ /* This is a 'via port' update and I'm the last module */
		myOutport =outport;
		lastModule =myID;
	}
	else if(outport == 0){ /* This is a remote update */
		if(NumberOfHops(dst)== 1)
		lastModule = myID;
		else
		lastModule = route[NumberOfHops(dst)-1]; /* previous module = route[Number of hops - 1] */
	}

	/* 2. If this is the source of the message, show status on the CLI */
	if(src == myID){
		/* Obtain the address of the output buffer.  Note there is no mutual
		 exclusion on this buffer as it is assumed only one command console
		 interface will be used at any one time. */
		pcOutputString =FreeRTOS_CLIGetOutputBuffer();

		if(outport == 0)		// This is a remote module update
			sprintf((char* )pcOutputString,pcRemoteBootloaderUpdateMessage,dst);
		else
			// This is a 'via port' remote update
			sprintf((char* )pcOutputString,pcRemoteBootloaderUpdateViaPortMessage,dst,outport);

		strcat((char* )pcOutputString,pcRemoteBootloaderUpdateWarningMessage);
		writePxITMutex(inport,(char* )pcOutputString,strlen((char* )pcOutputString),cmd50ms);
		Delay_ms(100);
	}

	/* 3. Setup my inport and outport for bootloader update */
	SetupPortForRemoteBootloaderUpdate(inport);
	SetupPortForRemoteBootloaderUpdate(myOutport);


	/* 5. Build a DMA stream between my inport and outport */
	StartScastDMAStream(inport,myID,myOutport,myID,BIDIRECTIONAL,0xFFFFFFFF,0xFFFFFFFF,false);
}

/*-----------------------------------------------------------*/

/* --- Setup a port for remote ST factory bootloader update:
 - Set baudrate to 57600
 - Enable even parity
 - Set datasize to 9 bits
 */
void SetupPortForRemoteBootloaderUpdate(uint8_t port){
	UART_HandleTypeDef *huart =GetUart(port);

	huart->Init.BaudRate =57600;
	huart->Init.Parity = UART_PARITY_EVEN;
	huart->Init.WordLength = UART_WORDLENGTH_9B;
	HAL_UART_Init(huart);

	/* The CLI port RXNE interrupt might be disabled so enable here again to be sure */
	__HAL_UART_ENABLE_IT(huart,UART_IT_RXNE);
}
/* --- H2AR3 module initialization ---
 */
void Module_Peripheral_Init(void) {

	/* Array ports */
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();
	MX_USART4_UART_Init();
	MX_USART6_UART_Init();

	xTaskCreate(VoltAmpTask, (const char* ) "VoltAmpTask",
			(2*configMINIMAL_STACK_SIZE), NULL,
			osPriorityNormal - osPriorityIdle, &VoltAmpHandle);

	MX_ADC_Init();

}

/*-----------------------------------------------------------*/

/* --- H2AR3 message processing task. 
 */
Module_Status Module_MessagingTask(uint16_t code, uint8_t port, uint8_t src,
		uint8_t dst, uint8_t shift) {

	Module_Status result = H2AR3_OK;
	uint32_t period = 0;
	uint32_t timeout = 0;

	switch (code) {

	case (CODE_H2AR3_STREAM_PORT_V):
		period = ((uint32_t) cMessage[port - 1][2 + shift] )
				+ ((uint32_t) cMessage[port - 1][3 + shift] << 8)
				+ ((uint32_t) cMessage[port - 1][4 + shift] << 16)
				+ ((uint32_t)cMessage[port - 1][5 + shift] <<24);
		timeout = ((uint32_t) cMessage[port - 1][6 + shift] )
				+ ((uint32_t) cMessage[port - 1][7 + shift] << 8)
				+ ((uint32_t) cMessage[port - 1][8 + shift] << 16)
				+ ((uint32_t)cMessage[port - 1][9 + shift] <<24);
		StreamVToPort(cMessage[port - 1][1 + shift],
				cMessage[port - 1][shift], period, timeout);
		break;

	case (CODE_H2AR3_STREAM_PORT_A):
				period = ((uint32_t) cMessage[port - 1][2 + shift] )
						+ ((uint32_t) cMessage[port - 1][3 + shift] << 8)
						+ ((uint32_t) cMessage[port - 1][4 + shift] << 16)
						+ ((uint32_t)cMessage[port - 1][5 + shift] <<24);
				timeout = ((uint32_t) cMessage[port - 1][6 + shift] )
						+ ((uint32_t) cMessage[port - 1][7 + shift] << 8)
						+ ((uint32_t) cMessage[port - 1][8 + shift] << 16)
						+((uint32_t) cMessage[port - 1][9 + shift] <<24);
				StreamAToPort(cMessage[port - 1][1 + shift],
						cMessage[port - 1][shift], period, timeout);
		break;

	case (CODE_H2AR3_STOP):
		global_mode = IDLE_CASE;
		xTimerStop(xTimer, portMAX_DELAY);
		break;

	case (CODE_H2AR3_SAMPLE_V):
		SampleV(&H2AR3_Read_V);
		break;

	case (CODE_H2AR3_SAMPLE_A):
		SampleA(&H2AR3_Read_A);
		break;

	default:
		result = H2AR3_ERR_UnknownMessage;
		break;
	}

	return result;
}

/*-----------------------------------------------------------*/

/* --- Register this module CLI Commands 
 */
void RegisterModuleCLICommands(void) {

	FreeRTOS_CLIRegisterCommand(&sampleCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&streamCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&stopCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&unitCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&demoCommandDefinition);

}

/*-----------------------------------------------------------*/

/* --- Get the port for a given UART. 
 */
//uint8_t GetPort(UART_HandleTypeDef *huart) {
//	if (huart->Instance == USART4)
//		return P1;
//	else if (huart->Instance == USART2)
//		return P2;
//	else if (huart->Instance == USART6)
//		return P3;
//	else if (huart->Instance == USART3)
//		return P4;
//	else if (huart->Instance == USART1)
//		return P5;
//
//	return 0;
//}
uint8_t GetPort(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART2)
		return P1;
	else if (huart->Instance == USART6)
		return P2;
	else if (huart->Instance == USART3)
		return P3;
	return 0;
}

/*-----------------------------------------------------------*/

/* --- send results to x --- */

uint8_t SendResults(float message, uint8_t Mode, uint8_t Unit, uint8_t Port,
		uint8_t Module, float *Buffer) {
	float Raw_Msg = 0.0f;
	uint32_t RawMsgInt = 0;
	int8_t *pcOutputString;
	static const int8_t *pcWeightMsg = (int8_t*) "Read (%s): %f\r\n";
	static const int8_t *pcWeightVerboseMsg = (int8_t*) "%f\r\n";
	static const int8_t *pcWeightMsgUINT = (int8_t*) "Read (%s): %d\r\n";
	static const int8_t *pcWeightVerboseMsgUINT = (int8_t*) "%d\r\n";
	char *strUnit;
	static uint8_t temp[4];
	/* specify the unit */

	Raw_Msg = message;

	/* Get CLI output buffer */
	pcOutputString = FreeRTOS_CLIGetOutputBuffer();

	if (Mode != STREAM_CLI_VERBOSE_CASE && Mode != STREAM_PORT_CASE) {
		strUnit = malloc(6 * sizeof(char));
		memset(strUnit, 0, (6 * sizeof(char)));
		if (unit == Volt) {
			sprintf((char*) strUnit, "Volt");
		} else if (unit == Amp) {
			sprintf((char*) strUnit, "Amp");
		} else {
			sprintf((char*) strUnit, "Volt");
		}
	}

	// Send the value to appropriate outlet
	switch (Mode) {
	case SAMPLE_CLI_CASE:
	case STREAM_CLI_CASE:
		if (H2AR3_DATA_FORMAT == FMT_UINT32) {
			RawMsgInt = Raw_Msg * 10;
			sprintf((char*) pcOutputString, (char*) pcWeightMsgUINT, strUnit,
					RawMsgInt);
			writePxMutex(PcPort, (char*) pcOutputString,
					strlen((char*) pcOutputString), cmd500ms,
					HAL_MAX_DELAY);
			CheckForEnterKey();
		} else if (H2AR3_DATA_FORMAT == FMT_FLOAT) {
			sprintf((char*) pcOutputString, (char*) pcWeightMsg, strUnit,
					Raw_Msg);
			writePxMutex(PcPort, (char*) pcOutputString,
					strlen((char*) pcOutputString), cmd500ms,
					HAL_MAX_DELAY);
			CheckForEnterKey();
		}
		break;

	case SAMPLE_PORT_CASE:
	case STREAM_PORT_CASE:
		if (H2AR3_DATA_FORMAT == FMT_UINT32) {
			RawMsgInt = Raw_Msg * 10;
			if (Module == myID) {
				temp[0] = *((__IO uint8_t*) (&RawMsgInt) + 3);
				temp[1] = *((__IO uint8_t*) (&RawMsgInt) + 2);
				temp[2] = *((__IO uint8_t*) (&RawMsgInt) + 1);
				temp[3] = *((__IO uint8_t*) (&RawMsgInt) + 0);
				writePxMutex(Port, (char*) &temp, 4 * sizeof(uint8_t), 10, 10);
			} else {
				messageParams[0] = Port;
				messageParams[1] = *((__IO uint8_t*) (&RawMsgInt) + 3);
				messageParams[2] = *((__IO uint8_t*) (&RawMsgInt) + 2);
				messageParams[3] = *((__IO uint8_t*) (&RawMsgInt) + 1);
				messageParams[4] = *((__IO uint8_t*) (&RawMsgInt) + 0);
				SendMessageToModule(Module, CODE_PORT_FORWARD,
						sizeof(uint32_t) + 1);
			}

		} else if (H2AR3_DATA_FORMAT == FMT_FLOAT) {
			if (Module == myID) {
				temp[0] = *((__IO uint8_t*) (&Raw_Msg) + 3);
				temp[1] = *((__IO uint8_t*) (&Raw_Msg) + 2);
				temp[2] = *((__IO uint8_t*) (&Raw_Msg) + 1);
				temp[3] = *((__IO uint8_t*) (&Raw_Msg) + 0);
				writePxMutex(Port, (char*) &temp, 4 * sizeof(uint8_t), 10, 10);
			} else {
				messageParams[0] = Port;
				messageParams[1] = *((__IO uint8_t*) (&Raw_Msg) + 3);
				messageParams[2] = *((__IO uint8_t*) (&Raw_Msg) + 2);
				messageParams[3] = *((__IO uint8_t*) (&Raw_Msg) + 1);
				messageParams[4] = *((__IO uint8_t*) (&Raw_Msg) + 0);
				SendMessageToModule(Module, CODE_PORT_FORWARD,
						sizeof(float) + 1);
			}
		}

		break;

	case SAMPLE_BUFFER_CASE:
	case STREAM_BUFFER_CASE:
		memset(Buffer, 0, sizeof(float));
		memcpy(Buffer, &Raw_Msg, sizeof(float));
		break;
	}
	if (Mode != STREAM_CLI_VERBOSE_CASE && Mode != STREAM_PORT_CASE) {
		free(strUnit);
	}
	return (H2AR3_OK);
}



/*-----------------------------------------------------------*/

/* --- voltage a stream task
 */

void VoltAmpTask(void *argument) {

	HAL_ADC_Start_IT(&hadc);

	while (1) {

		uint32_t t0 = 0;

		switch (unit) {
		case Volt:
			DATA_To_SEND = CalculationVolt();
			break;
		case Amp:
			DATA_To_SEND = CalculationAmp();
			break;
		default:
			DATA_To_SEND = CalculationVolt();
			break;

		}

		switch (global_mode) {
		case STREAM_CLI_CASE:
			t0 = HAL_GetTick();
			SendResults(DATA_To_SEND, global_mode, unit, 0, 0, NULL);
			while (HAL_GetTick() - t0 < (global_period - 1)) {
				taskYIELD();
			}
			break;

		case STREAM_PORT_CASE:
			t0 = HAL_GetTick();
			SendResults(DATA_To_SEND, global_mode, unit, global_port,
					global_module, NULL);
			while (HAL_GetTick() - t0 < global_period) {
				taskYIELD();
			}
			break;

		case STREAM_BUFFER_CASE:
			t0 = HAL_GetTick();
			SendResults(DATA_To_SEND, global_mode, unit, 0, 0,ptr_read_buffer);
			while (HAL_GetTick() - t0 < global_period) {
				taskYIELD();
			}
			break;

		default:
			global_mode = IDLE_CASE;
			break;
		}
		taskYIELD();
	}
}
/* -----------------------------------------------------------------------
 |							private function
 -----------------------------------------------------------------------
 */
static uint32_t Adc_Calculation(uint8_t selected) {

	switch (selected) {
	case Amp:
		ADC_Select_CH7();
		HAL_ADC_Start(&hadc);
		HAL_ADC_PollForConversion(&hadc, 1000);
		tmp_adc = HAL_ADC_GetValue(&hadc);
		HAL_ADC_Stop(&hadc);
		ADC_Deselect_CH7();
		break;

	case Volt:
		ADC_Select_CH9();
		HAL_ADC_Start(&hadc);
		HAL_ADC_PollForConversion(&hadc, 1000);
		tmp_adc = HAL_ADC_GetValue(&hadc);
		HAL_ADC_Stop(&hadc);
		ADC_Deselect_CH9();
		break;

	default:
		break;
	}

	return tmp_adc;

}


float CalculationVolt(void) {

	raw_adc = Adc_Calculation(Volt);
	_volt = (float)raw_adc * (3.3 / 4095);// 12 bit resolution
	_volt = _volt - VRef;
	// the following condition is required in case of phase absence for 2LSB hystress
	//if((_volt<0.04)&&(_volt>-0.04)) {_volt=0;}// better than +-0.0016
	//measured_volt = _volt * (4000150/(50*150))+40/*voltRatio*/; /////////////////final volt calculation here
	// what is the voltage ratio in the order of fourty volts!!!??
	measured_volt = _volt * (4000150/(50*150));//measured_volt =0;533.3533
	return measured_volt;
}

float CalculationAmp(void) {

	raw_adc = Adc_Calculation(Amp);
	_volt = (float)raw_adc * (3.3 / 4095);
	_volt = _volt - VRef;
//	measured_amp = ((_volt * 100) / shuntResistor) * ampTranRatio; /////////////////final amp calculation here
//	measured_amp = (_volt / shuntResistor) * ampTranRatio * 100;
	//if ((_volt<0.04)&&(_volt>-0.04)) {_volt=0;}
	//measured_amp = _volt/0.0841;
	measured_amp =(_volt/0.009795);//2.5 we have to make average error of vref before load is switched on
	return measured_amp;
}

/*-----------------------------------------------------------*/
/* ---  Check for CLI stop key  --- */
static void CheckForEnterKey(void) {
	// Look for ENTER key to stop the stream
	for (uint8_t chr = 0; chr < MSG_RX_BUF_SIZE; chr++) {
		if (UARTRxBuf[PcPort - 1][chr] == '\r') {
			UARTRxBuf[PcPort - 1][chr] = 0;
			startMeasurementRanging = STOP_MEASUREMENT_RANGING;
			global_mode = IDLE_CASE;		      // Stop the streaming task
			xTimerStop(xTimer, 0);            // Stop the timeout timer
			break;
		}
	}
}

/*-----------------------------------------------------------*/

/* --- software timer callback function
 */
static void HandleTimeout(TimerHandle_t xTimer) {
	uint32_t tid = 0;

	/* Get Timer ID */
	tid = (uint32_t) pvTimerGetTimerID(xTimer);
	if (TIMERID_TIMEOUT_MEASUREMENT == tid) {
		global_mode = IDLE_CASE;		      // Stop the streaming task
		startMeasurementRanging = STOP_MEASUREMENT_RANGING; // stop streaming
	}
}

/* -----------------------------------------------------------------------
 |																APIs	 																 	|
 -----------------------------------------------------------------------
 */
uint8_t SampleV(float *volt) {

	*volt = CalculationVolt();
	return (H2AR3_OK);
}

uint8_t SampleA(float *curr) {
	*curr = CalculationAmp();
	return (H2AR3_OK);
}

uint8_t StreamVToPort(uint8_t Port, uint8_t Module, uint32_t Period,
		uint32_t Timeout) {
	global_port = Port;
	global_module = Module;
	global_period = Period;
	global_timeout = Timeout;
	global_mode = STREAM_PORT_CASE;
	unit = Volt;
	if ((global_timeout > 0) && (global_timeout < 0xFFFFFFFF)) {
		/* start software timer which will create event timeout */
		/* Create a timeout timer */
		xTimer = xTimerCreate("Timeout Measurement",
				pdMS_TO_TICKS(global_timeout), pdFALSE,
				(void*) TIMERID_TIMEOUT_MEASUREMENT, HandleTimeout);
		/* Start the timeout timer */
		xTimerStart(xTimer, portMAX_DELAY);
	}
	return (H2AR3_OK);
}

uint8_t StreamAToPort(uint8_t Port, uint8_t Module, uint32_t Period,
		uint32_t Timeout) {
	global_port = Port;
	global_module = Module;
	global_period = Period;
	global_timeout = Timeout;
	global_mode = STREAM_PORT_CASE;
	unit = Amp;
	if ((global_timeout > 0) && (global_timeout < 0xFFFFFFFF)) {
		/* start software timer which will create event timeout */
		/* Create a timeout timer */
		xTimer = xTimerCreate("Timeout Measurement",
				pdMS_TO_TICKS(global_timeout), pdFALSE,
				(void*) TIMERID_TIMEOUT_MEASUREMENT, HandleTimeout);
		/* Start the timeout timer */
		xTimerStart(xTimer, portMAX_DELAY);
	}
	return (H2AR3_OK);
}


uint8_t StreamVToBuffer(float *Buffer, uint32_t Period, uint32_t Timeout) {
	global_period = Period;
	global_timeout = Timeout;
	ptr_read_buffer = Buffer;
	global_mode = STREAM_BUFFER_CASE;
	unit = Volt;
	if ((global_timeout > 0) && (global_timeout < 0xFFFFFFFF)) {
		/* start software timer which will create event timeout */
		/* Create a timeout timer */
		xTimer = xTimerCreate("Measurement Timeout",
				pdMS_TO_TICKS(global_timeout), pdFALSE,
				(void*) TIMERID_TIMEOUT_MEASUREMENT, HandleTimeout);
		/* Start the timeout timer */
		xTimerStart(xTimer, portMAX_DELAY);
	}

	return (H2AR3_OK);
}

uint8_t StreamAToBuffer(float *Buffer, uint32_t Period, uint32_t Timeout) {
	global_period = Period;
	global_timeout = Timeout;
	ptr_read_buffer = Buffer;
	global_mode = STREAM_BUFFER_CASE;
	unit = Amp;
	if ((global_timeout > 0) && (global_timeout < 0xFFFFFFFF)) {
		/* start software timer which will create event timeout */
		/* Create a timeout timer */
		xTimer = xTimerCreate("Measurement Timeout",
				pdMS_TO_TICKS(global_timeout), pdFALSE,
				(void*) TIMERID_TIMEOUT_MEASUREMENT, HandleTimeout);
		/* Start the timeout timer */
		xTimerStart(xTimer, portMAX_DELAY);
	}

	return (H2AR3_OK);
}

/*------------------------------------------------*/

float Average(uint8_t samples) {

	static float avr = 0, sum = 0, DATA_To_AVR;

	for (int i = 0; i < samples; i++) {

		switch (unit) {
		case Volt:
			DATA_To_AVR = CalculationVolt();
			break;
		case Amp:
			DATA_To_AVR = CalculationAmp();
			break;
		default:
			DATA_To_AVR = CalculationVolt();
			break;
		}
		sum += DATA_To_AVR;
	}
	avr = sum / samples;
	return avr;
}



/*-----------------------------------------------------------*/

uint8_t Stop(void) {
	global_mode = IDLE_CASE;

	xTimerStop(xTimer, 0);

	return H2AR3_OK;
}



/* --- stream weightvalue from channel ch to CLI
*/
uint8_t StreamVToCLI(uint32_t Period, uint32_t Timeout)
{

	global_period=Period;
	global_timeout=Timeout;
	global_mode=STREAM_CLI_CASE;
	unit=Volt;
	if ((global_timeout > 0) && (global_timeout < 0xFFFFFFFF))
  {
		/* start software timer which will create event timeout */
		/* Create a timeout timer */
		xTimer = xTimerCreate( "Measurement Timeout", pdMS_TO_TICKS(global_timeout), pdFALSE, ( void * ) TIMERID_TIMEOUT_MEASUREMENT, HandleTimeout );
		/* Start the timeout timer */
		xTimerStart( xTimer, portMAX_DELAY );
	}
	if (global_timeout > 0)
	{
		startMeasurementRanging = START_MEASUREMENT_RANGING;
	}

	return (H2AR3_OK);
}

uint8_t StreamAToCLI(uint32_t Period, uint32_t Timeout)
{

	global_period=Period;
	global_timeout=Timeout;
	global_mode=STREAM_CLI_CASE;
	unit=Amp;
	if ((global_timeout > 0) && (global_timeout < 0xFFFFFFFF))
  {
		/* start software timer which will create event timeout */
		/* Create a timeout timer */
		xTimer = xTimerCreate( "Measurement Timeout", pdMS_TO_TICKS(global_timeout), pdFALSE, ( void * ) TIMERID_TIMEOUT_MEASUREMENT, HandleTimeout );
		/* Start the timeout timer */
		xTimerStart( xTimer, portMAX_DELAY );
	}
	if (global_timeout > 0)
	{
		startMeasurementRanging = START_MEASUREMENT_RANGING;
	}

	return (H2AR3_OK);
}

/* -----------------------------------------------------------------------
 |															Commands																 	|
 -----------------------------------------------------------------------
 */

static portBASE_TYPE unitCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen,
		const int8_t *pcCommandString) {
	Module_Status result = H2AR3_OK;
	int8_t *pcParameterString1;
	portBASE_TYPE xParameterStringLength1 = 0;
	static const int8_t *pcMessageWrongParam = (int8_t*) "Wrong parameter!\r\n";

	/* Remove compile time warnings about unused parameters, and check the
	 write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	 write buffer length is adequate, so does not check for buffer overflows. */
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* 1st parameter for naming of uart port: P1 to P6 */
	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
			&xParameterStringLength1);
	if (!strncmp((const char*) pcParameterString1, "v", 1)) {
		unit = Volt;
		strcpy((char*) pcWriteBuffer,
				(char*) "Used measurement unit: Volt\r\n");
	} else if (!strncmp((const char*) pcParameterString1, "a", 1)) {
		unit = Amp;
		strcpy((char*) pcWriteBuffer,
				(char*) "Used measurement unit: Ampere\r\n");
	} else {
		result = H2AR3_ERR_WrongParams;
	}

	/* Respond to the command */
	if (H2AR3_ERR_WrongParams == result) {
		strcpy((char*) pcWriteBuffer, (char*) pcMessageWrongParam);
	}

	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}

/*-----------------------------------------------------------*/

static portBASE_TYPE sampleCommand(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen, const int8_t *pcCommandString) {
	static const int8_t *pcMessageError = (int8_t*) "Wrong parameter\r\n";

	Module_Status result = H2AR3_OK;

	/* Remove compile time warnings about unused parameters, and check the
	 write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	 write buffer length is adequate, so does not check for buffer overflows. */
	(void) pcCommandString;
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	switch (unit) {
	case Volt:
		SampleV(&DATA_To_SEND1);
		break;
	case Amp:
		SampleA(&DATA_To_SEND1);
		break;
	default:
		SampleV(&DATA_To_SEND1);
	}

	global_mode = SAMPLE_CLI_CASE;
	SendResults(DATA_To_SEND1, global_mode, unit, 0, 0, NULL);

	if (result != H2AR3_OK)
		strcpy((char*) pcWriteBuffer, (char*) pcMessageError);
	/* clean terminal output */
	memset((char*) pcWriteBuffer, 0, configCOMMAND_INT_MAX_OUTPUT_SIZE);

	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}

/*-----------------------------------------------------------*/

static portBASE_TYPE stopCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen,
		const int8_t *pcCommandString) {

	/* Remove compile time warnings about unused parameters, and check the
	 write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	 write buffer length is adequate, so does not check for buffer overflows. */
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);
	volt_buffer = 0;
	amp_buffer = 0;
	Stop();

	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}

/*-----------------------------------------------------------*/


static bool streamCommandParser(const int8_t *pcCommandString, bool *pPortOrCLI, uint32_t *pPeriod, uint32_t *pTimeout, uint8_t *pPort, uint8_t *pModule)
{
	const char *pPeriodMSStr = NULL;
	const char *pTimeoutMSStr = NULL;

	portBASE_TYPE periodStrLen = 0;
	portBASE_TYPE timeoutStrLen = 0;

	const char *pPortStr = NULL;
	const char *pModStr = NULL;

	portBASE_TYPE portStrLen = 0;
	portBASE_TYPE modStrLen = 0;

	pPeriodMSStr = (const char *)FreeRTOS_CLIGetParameter(pcCommandString, 1, &periodStrLen);
	pTimeoutMSStr = (const char *)FreeRTOS_CLIGetParameter(pcCommandString, 2, &timeoutStrLen);

	// At least 3 Parameters are required!
	if ((pPeriodMSStr == NULL) || (pTimeoutMSStr == NULL))
		return false;

	// TODO: Check if Period and Timeout are integers or not!
	*pPeriod = atoi(pPeriodMSStr);
	*pTimeout = atoi(pTimeoutMSStr);
	*pPortOrCLI = true;

	pPortStr = (const char *)FreeRTOS_CLIGetParameter(pcCommandString, 3, &portStrLen);
	pModStr = (const char *)FreeRTOS_CLIGetParameter(pcCommandString, 4, &modStrLen);

	if ((pModStr == NULL) && (pPortStr == NULL))
		return true;
	if ((pModStr == NULL) || (pPortStr == NULL))	// If user has provided 4 Arguments.
		return false;

	*pPort = atoi(pPortStr);
	*pModule = atoi(pModStr);
	*pPortOrCLI = false;

	return true;
}

static portBASE_TYPE streamCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString)
{
//	const char *const temperatureCmdName = "temp";

	uint32_t period = 0;
	uint32_t timeout = 0;
	uint8_t port = 0;
	uint8_t module = 0;

	bool portOrCLI = true; // Port Mode => false and CLI Mode => true

	const char *pSensName = NULL;
	portBASE_TYPE sensNameLen = 0;

	// Make sure we return something
	*pcWriteBuffer = '\0';

	if (!streamCommandParser(pcCommandString,/* &pSensName, &sensNameLen, */&portOrCLI, &period, &timeout, &port, &module)) {
		snprintf((char *)pcWriteBuffer, xWriteBufferLen, "Invalid Arguments\r\n");
		return pdFALSE;
	}

	do {

            if(unit == Volt)
            {
			if (portOrCLI) {
				StreamVToCLI(period, timeout);

			} else {
				StreamVToPort(port, module, period, timeout);

			}
            }
            else if(unit == Amp)
            {
    		if (portOrCLI) {
    			StreamAToCLI(period, timeout);

    		} else {
    			StreamAToPort(port, module, period, timeout);

    		}
            }

		snprintf((char *)pcWriteBuffer, xWriteBufferLen, "\r\n");
		return pdFALSE;
	} while (0);

	snprintf((char *)pcWriteBuffer, xWriteBufferLen, "Error reading Sensor\r\n");
	return pdFALSE;
}
/*-----------------------------------------------------------*/

portBASE_TYPE demoCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen,
		const int8_t *pcCommandString) {
	static const int8_t *pcMessage =
			(int8_t*) "Streaming AC current and voltage reading: \r\n";
	static const int8_t *pcMessageError = (int8_t*) "Wrong parameter\r\n";
	portBASE_TYPE xParameterStringLength1 = 0;
	Module_Status result = H2AR3_OK;

	/* Remove compile time warnings about unused parameters, and check the
	 write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	 write buffer length is adequate, so does not check for buffer overflows. */
	(void) pcCommandString;
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);




	/* Respond to the command */

		strcpy((char*) pcWriteBuffer, (char*) pcMessage);
		writePxMutex(PcPort, (char*) pcWriteBuffer,
				strlen((char*) pcWriteBuffer), cmd50ms, HAL_MAX_DELAY);
		switch (unit) {
		case Volt:
			StreamVToCLI(500, 10000);
			break;
		case Amp:
			StreamAToCLI(500, 10000);
			break;
		default:
			StreamVToCLI(500, 10000);
		}

		/* Wait till the end of stream */
		while (startMeasurementRanging != STOP_MEASUREMENT_RANGING) {
			Delay_ms(1);
		};


	if (result != H2AR3_OK) {
		strcpy((char*) pcWriteBuffer, (char*) pcMessageError);
	}

	/* clean terminal output */
	memset((char*) pcWriteBuffer, 0, strlen((char*) pcWriteBuffer));

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;
}

/*-------------------------------------------*/


/*-------------------------------------------------------*/

