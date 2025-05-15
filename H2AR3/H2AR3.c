/*
 BitzOS (BOS) V0.4.0 - Copyright (C) 2017-2025 Hexabitz
 All rights reserved

 File Name     : H2AR3.c
 Description   : Source code for module H2AR3.
 	 	 	 	 (Description_of_module)

(Description of Special module peripheral configuration):
>>
>>
>>
 */

/* Includes ****************************************************************/
#include "BOS.h"
#include "H2AR3_inputs.h"

/* Exported Typedef ******************************************************/
/* Define UART variables */
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart6;

Filter_t AMPFilter;
Filter_t VoltFilter;

//TaskHandle_t ACMonitorTaskHandle = NULL;

ADC_HandleTypeDef hadc1;

/* Private Variables *******************************************************/
uint32_t adcRawData =0;
float voltage =0.0f;
float current =0.0f;

/* Streaming variables */
static bool stopStream = false;
uint8_t flag ;
uint8_t StreamingDataMode;
uint8_t port1, module1,mode1;
uint8_t port2 ,module2,mode2;
uint8_t port3 ,module3,mode3;
uint32_t Numofsamples1 ,timeout1;
uint32_t Numofsamples3 ,timeout3;

/* Global variables for sensor data used in ModuleParam */
float H2AR3_voltage = 0.0f;
float H2AR3_current = 0.0f;

/* Module Parameters */
ModuleParam_t ModuleParam[NUM_MODULE_PARAMS] ={
    {.ParamPtr = &H2AR3_voltage, .ParamFormat = FMT_FLOAT, .ParamName = "voltage"},
    {.ParamPtr = &H2AR3_current, .ParamFormat = FMT_FLOAT, .ParamName = "current"}
};

/* Local Typedef related to stream functions */
typedef void (*SampleToString)(char *, size_t);
typedef void (*SampleToPort)(uint8_t, uint8_t);
typedef void (*SampleToBuffer)(float *buffer);

/* Private function prototypes *********************************************/
uint8_t ClearROtopology(void);
void Module_Peripheral_Init(void);
void SetupPortForRemoteBootloaderUpdate(uint8_t port);
void RemoteBootloaderUpdate(uint8_t src,uint8_t dst,uint8_t inport,uint8_t outport);
Module_Status Module_MessagingTask(uint16_t code,uint8_t port,uint8_t src,uint8_t dst,uint8_t shift);

/* Local function prototypes ***********************************************/
void ACMonitorTask(void *argument);
void AvarageLPF(uint32_t IN, uint32_t* OUT, Filter_t* FILTER_OBJ);
uint32_t ADCCalculation(uint8_t selected);
Module_Status CalculationAmp(float *measured_volt);
Module_Status CalculationVolt(float * measured_volt);

/* Stream Functions */
void SampleVToString(char *cstring, size_t maxLen);
void SampleAToString(char *cstring, size_t maxLen);

Module_Status Exporttoport(uint8_t module,uint8_t port,All_Data Mode);
Module_Status Exportstreamtoterminal(uint32_t Numofsamples, uint32_t timeout,uint8_t Port,All_Data function);
Module_Status Exportstreamtoport (uint8_t module,uint8_t port,All_Data function,uint32_t Numofsamples,uint32_t timeout);
static Module_Status StreamToCLI(uint32_t Numofsamples, uint32_t timeout, SampleToString function);
static Module_Status PollingSleepCLISafe(uint32_t period, long Numofsamples);

/* Create CLI commands *****************************************************/
static portBASE_TYPE SampleSensorCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);
static portBASE_TYPE StreamSensorCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);

/* CLI command structure ***************************************************/
/* CLI command structure : sample */
const CLI_Command_Definition_t SampleCommandDefinition = {
	(const int8_t *) "sample",
	(const int8_t *) "sample:\r\n Syntax: sample [AMP]/[VOLT].\r\n\r\n",
	SampleSensorCommand,
	1
};

/***************************************************************************/
/* CLI command structure : stream */
const CLI_Command_Definition_t StreamCommandDefinition = {
	(const int8_t *) "stream",
	(const int8_t *) "stream:\r\n Syntax: stream  [AMP]/[VOLT] (period in ms) (time in ms) [port] [module].\r\n\r\n",
	StreamSensorCommand,
	-1
};
void MX_TIM1_Init(void) ;
void MX_ADC1_Init(void);
/***************************************************************************/
/************************ Private function Definitions *********************/
/***************************************************************************/
/* @brief  System Clock Configuration
 *         This function configures the system clock as follows:
 *            - System Clock source            = PLL (HSE)
 *            - SYSCLK(Hz)                     = 64000000
 *            - HCLK(Hz)                       = 64000000
 *            - AHB Prescaler                  = 1
 *            - APB1 Prescaler                 = 1
 *            - HSE Frequency(Hz)              = 8000000
 *            - PLLM                           = 1
 *            - PLLN                           = 16
 *            - PLLP                           = 2
 *            - Flash Latency(WS)              = 2
 *            - Clock Source for UART1,UART2,UART3 = 16MHz (HSI)
 */
void SystemClock_Config(void){
	RCC_OscInitTypeDef RCC_OscInitStruct ={0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct ={0};

	/** Configure the main internal regulator output voltage */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

	/* Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE; // Enable both HSI and HSE oscillators
	RCC_OscInitStruct.HSEState = RCC_HSE_ON; // Enable HSE (External High-Speed Oscillator)
	RCC_OscInitStruct.HSIState = RCC_HSI_ON; // Enable HSI (Internal High-Speed Oscillator)
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1; // No division on HSI
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT; // Default calibration value for HSI
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON; // Enable PLL
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE; // Set PLL source to HSE
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1; // Prescaler for PLL input
	RCC_OscInitStruct.PLL.PLLN =16; // Multiplication factor for PLL
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; // PLLP division factor
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2; // PLLQ division factor
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2; // PLLR division factor
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	/** Initializes the CPU, AHB and APB buses clocks */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; // Select PLL as the system clock source
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; // AHB Prescaler set to 1
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1; // APB1 Prescaler set to 1

	HAL_RCC_ClockConfig(&RCC_ClkInitStruct,FLASH_LATENCY_2); // Configure system clocks with flash latency of 2 WS
}

/***************************************************************************/
/* enable stop mode regarding only UART1 , UART2 , and UART3 */
BOS_Status EnableStopModebyUARTx(uint8_t port){

	UART_WakeUpTypeDef WakeUpSelection;
	UART_HandleTypeDef *huart =GetUart(port);

	if((huart->Instance == USART1) || (huart->Instance == USART2) || (huart->Instance == USART3)){

		/* make sure that no UART transfer is on-going */
		while(__HAL_UART_GET_FLAG(huart, USART_ISR_BUSY) == SET);

		/* make sure that UART is ready to receive */
		while(__HAL_UART_GET_FLAG(huart, USART_ISR_REACK) == RESET);

		/* set the wake-up event:
		 * specify wake-up on start-bit detection */
		WakeUpSelection.WakeUpEvent = UART_WAKEUP_ON_STARTBIT;
		HAL_UARTEx_StopModeWakeUpSourceConfig(huart,WakeUpSelection);

		/* Enable the UART Wake UP from stop mode Interrupt */
		__HAL_UART_ENABLE_IT(huart,UART_IT_WUF);

		/* enable MCU wake-up by LPUART */
		HAL_UARTEx_EnableStopMode(huart);

		/* enter STOP mode */
		HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_STOPENTRY_WFI);
	}
	else
		return BOS_ERROR;

}

/***************************************************************************/
/* Enable standby mode regarding wake-up pins:
 * WKUP1: PA0  pin
 * WKUP4: PA2  pin
 * WKUP6: PB5  pin
 * WKUP2: PC13 pin
 * NRST pin
 *  */
BOS_Status EnableStandbyModebyWakeupPinx(WakeupPins_t wakeupPins){

	/* Clear the WUF FLAG */
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF);

	/* Enable the WAKEUP PIN */
	switch(wakeupPins){

		case PA0_PIN:
			HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1); /* PA0 */
			break;

		case PA2_PIN:
			HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN4); /* PA2 */
			break;

		case PB5_PIN:
			HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN6); /* PB5 */
			break;

		case PC13_PIN:
			HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2); /* PC13 */
			break;

		case NRST_PIN:
			/* do no thing*/
			break;
	}

	/* Enable SRAM content retention in Standby mode */
	HAL_PWREx_EnableSRAMRetention();

	/* Finally enter the standby mode */
	HAL_PWR_EnterSTANDBYMode();

	return BOS_OK;
}

/***************************************************************************/
/* Disable standby mode regarding wake-up pins:
 * WKUP1: PA0  pin
 * WKUP4: PA2  pin
 * WKUP6: PB5  pin
 * WKUP2: PC13 pin
 * NRST pin
 *  */
BOS_Status DisableStandbyModeWakeupPinx(WakeupPins_t wakeupPins){

	/* The standby wake-up is same as a system RESET:
	 * The entire code runs from the beginning just as if it was a RESET.
	 * The only difference between a reset and a STANDBY wake-up is that, when the MCU wakes-up,
	 * The SBF status flag in the PWR power control/status register (PWR_CSR) is set */
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET){
		/* clear the flag */
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);

		/* Disable  Wake-up Pinx */
		switch(wakeupPins){

			case PA0_PIN:
				HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1); /* PA0 */
				break;

			case PA2_PIN:
				HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN4); /* PA2 */
				break;

			case PB5_PIN:
				HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN6); /* PB5 */
				break;

			case PC13_PIN:
				HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2); /* PC13 */
				break;

			case NRST_PIN:
				/* do no thing*/
				break;
		}

		IND_blink(1000);

	}
	else
		return BOS_OK;

}

/***************************************************************************/
/* Save Command Topology in Flash RO */
uint8_t SaveTopologyToRO(void){

	HAL_StatusTypeDef flashStatus =HAL_OK;

	/* flashAdd is initialized with 8 because the first memory room in topology page
	 * is reserved for module's ID */
	uint16_t flashAdd =8;
	uint16_t temp =0;

	/* Unlock the FLASH control register access */
	HAL_FLASH_Unlock();

	/* Erase Topology page */
	FLASH_PageErase(FLASH_BANK_2,TOPOLOGY_PAGE_NUM);

	/* Wait for an Erase operation to complete */
	flashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);

	if(flashStatus != HAL_OK){
		/* return FLASH error code */
		return pFlash.ErrorCode;
	}

	else{
		/* Operation is completed, disable the PER Bit */
		CLEAR_BIT(FLASH->CR,FLASH_CR_PER);
	}

	/* Save module's ID and topology */
	if(myID){

		/* Save module's ID */
		temp =(uint16_t )(N << 8) + myID;

		/* Save module's ID in Flash memory */
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,TOPOLOGY_START_ADDRESS,temp);

		/* Wait for a Write operation to complete */
		flashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);

		if(flashStatus != HAL_OK){
			/* return FLASH error code */
			return pFlash.ErrorCode;
		}

		else{
			/* If the program operation is completed, disable the PG Bit */
			CLEAR_BIT(FLASH->CR,FLASH_CR_PG);
		}

		/* Save topology */
		for(uint8_t row =1; row <= N; row++){
			for(uint8_t column =0; column <= MAX_NUM_OF_PORTS; column++){
				/* Check the module serial number
				 * Note: there isn't a module has serial number 0
				 */
				if(Array[row - 1][0]){
					/* Save each element in topology Array in Flash memory */
					HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,TOPOLOGY_START_ADDRESS + flashAdd,Array[row - 1][column]);
					/* Wait for a Write operation to complete */
					flashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);
					if(flashStatus != HAL_OK){
						/* return FLASH error code */
						return pFlash.ErrorCode;
					}
					else{
						/* If the program operation is completed, disable the PG Bit */
						CLEAR_BIT(FLASH->CR,FLASH_CR_PG);
						/* update new flash memory address */
						flashAdd +=8;
					}
				}
			}
		}
	}
	/* Lock the FLASH control register access */
	HAL_FLASH_Lock();
}

/***************************************************************************/
/* Save Command Snippets in Flash RO */
uint8_t SaveSnippetsToRO(void){
	HAL_StatusTypeDef FlashStatus =HAL_OK;
	uint8_t snipBuffer[sizeof(Snippet_t) + 1] ={0};

	/* Unlock the FLASH control register access */
	HAL_FLASH_Unlock();
	/* Erase Snippets page */
	FLASH_PageErase(FLASH_BANK_2,SNIPPETS_PAGE_NUM);
	/* Wait for an Erase operation to complete */
	FlashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);

	if(FlashStatus != HAL_OK){
		/* return FLASH error code */
		return pFlash.ErrorCode;
	}
	else{
		/* Operation is completed, disable the PER Bit */
		CLEAR_BIT(FLASH->CR,FLASH_CR_PER);
	}

	/* Save Command Snippets */
	int currentAdd = SNIPPETS_START_ADDRESS;
	for(uint8_t index =0; index < NumOfRecordedSnippets; index++){
		/* Check if Snippet condition is true or false */
		if(Snippets[index].Condition.ConditionType){
			/* A marker to separate Snippets */
			snipBuffer[0] =0xFE;
			memcpy((uint32_t* )&snipBuffer[1],(uint8_t* )&Snippets[index],sizeof(Snippet_t));
			/* Copy the snippet struct buffer (20 x NumOfRecordedSnippets). Note this is assuming sizeof(Snippet_t) is even */
			for(uint8_t j =0; j < (sizeof(Snippet_t) / 4); j++){
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,currentAdd,*(uint64_t* )&snipBuffer[j * 8]);
				FlashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);
				if(FlashStatus != HAL_OK){
					return pFlash.ErrorCode;
				}
				else{
					/* If the program operation is completed, disable the PG Bit */
					CLEAR_BIT(FLASH->CR,FLASH_CR_PG);
					currentAdd +=8;
				}
			}
			/* Copy the snippet commands buffer. Always an even number. Note the string termination char might be skipped */
			for(uint8_t j =0; j < ((strlen(Snippets[index].CMD) + 1) / 4); j++){
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,currentAdd,*(uint64_t* )(Snippets[index].CMD + j * 4));
				FlashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);
				if(FlashStatus != HAL_OK){
					return pFlash.ErrorCode;
				}
				else{
					/* If the program operation is completed, disable the PG Bit */
					CLEAR_BIT(FLASH->CR,FLASH_CR_PG);
					currentAdd +=8;
				}
			}
		}
	}
	/* Lock the FLASH control register access */
	HAL_FLASH_Lock();
}

/***************************************************************************/
/* Clear Array topology in SRAM and Flash RO */
uint8_t ClearROtopology(void){
	/* Clear the Array */
	memset(Array,0,sizeof(Array));
	N =1;
	myID =0;
	
	return SaveTopologyToRO();
}

/***************************************************************************/
/* Trigger ST factory bootloader update for a remote module */
void RemoteBootloaderUpdate(uint8_t src,uint8_t dst,uint8_t inport,uint8_t outport){

	uint8_t myOutport =0, lastModule =0;
	int8_t *pcOutputString;

	/* 1. Get Route to destination module */
	myOutport =FindRoute(myID,dst);
	if(outport && dst == myID){ /* This is a 'via port' update and I'm the last module */
		myOutport =outport;
		lastModule =myID;
	}
	else if(outport == 0){ /* This is a remote update */
		if(NumberOfHops(dst)== 1)
		lastModule = myID;
		else
		lastModule = Route[NumberOfHops(dst)-1]; /* previous module = Route[Number of hops - 1] */
	}

	/* 2. If this is the source of the message, show status on the CLI */
	if(src == myID){
		/* Obtain the address of the output buffer.  Note there is no mutual
		 * exclusion on this buffer as it is assumed only one command console
		 * interface will be used at any one time. */
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

/***************************************************************************/
/* Setup a port for remote ST factory bootloader update:
 * Set baudrate to 57600
 * Enable even parity
 * Set datasize to 9 bits
 */
void SetupPortForRemoteBootloaderUpdate(uint8_t port){

	UART_HandleTypeDef *huart =GetUart(port);
	HAL_UART_DeInit(huart);
	huart->Init.Parity = UART_PARITY_EVEN;
	huart->Init.WordLength = UART_WORDLENGTH_9B;
	HAL_UART_Init(huart);

	/* The CLI port RXNE interrupt might be disabled so enable here again to be sure */
	__HAL_UART_ENABLE_IT(huart,UART_IT_RXNE);

}

/***************************************************************************/
/* H2AR3 module initialization */
void Module_Peripheral_Init(void) {
//	__HAL_RCC_GPIOB_CLK_ENABLE();
//	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* Array ports */
	MX_USART2_UART_Init();
	MX_USART1_UART_Init();
	MX_USART6_UART_Init();
	MX_ADC1_Init();
	MX_TIM1_Init();

	/* Circulating DMA Channels ON All Module */
	for (int i = 1; i <= NUM_OF_PORTS; i++) {
		if (GetUart(i) == &huart1) {
			dmaIndex[i - 1] = &(DMA1_Channel1->CNDTR);
		} else if (GetUart(i) == &huart2) {
			dmaIndex[i - 1] = &(DMA1_Channel2->CNDTR);
		} else if (GetUart(i) == &huart3) {
			dmaIndex[i - 1] = &(DMA1_Channel3->CNDTR);
		} else if (GetUart(i) == &huart4) {
			dmaIndex[i - 1] = &(DMA1_Channel4->CNDTR);
		} else if (GetUart(i) == &huart5) {
			dmaIndex[i - 1] = &(DMA1_Channel5->CNDTR);
		} else if (GetUart(i) == &huart6) {
			dmaIndex[i - 1] = &(DMA1_Channel6->CNDTR);
		}
	}

	/* Create module special task (if needed) */
//	if (ACMonitorTaskHandle == NULL)
//		xTaskCreate(ACMonitorTask, (const char*) "ACMonitorTask",
//		configMINIMAL_STACK_SIZE, NULL, osPriorityNormal - osPriorityIdle, &ACMonitorTaskHandle);
}

/***************************************************************************/
/* H2AR3 message processing task */
Module_Status Module_MessagingTask(uint16_t code, uint8_t port, uint8_t src, uint8_t dst, uint8_t shift) {
	Module_Status result = H2AR3_OK;
	uint32_t Numofsamples;
	uint32_t timeout;

	switch (code) {

	case CODE_H2AR3_SAMPLE_V:
		SampletoPort(cMessage[port - 1][shift], cMessage[port - 1][1 + shift],
				VOLT);
		break;

	case CODE_H2AR3_SAMPLE_A:
		SampletoPort(cMessage[port - 1][shift], cMessage[port - 1][1 + shift],
				AMP);
		break;

	case CODE_H2AR3_STREAM_V:
		Numofsamples = ((uint32_t) cMessage[port - 1][2 + shift])
				+ ((uint32_t) cMessage[port - 1][3 + shift] << 8)
				+ ((uint32_t) cMessage[port - 1][4 + shift] << 16)
				+ ((uint32_t) cMessage[port - 1][5 + shift] << 24);
		timeout = ((uint32_t) cMessage[port - 1][6 + shift])
				+ ((uint32_t) cMessage[port - 1][7 + shift] << 8)
				+ ((uint32_t) cMessage[port - 1][8 + shift] << 16)
				+ ((uint32_t) cMessage[port - 1][9 + shift] << 24);
		Exportstreamtoport(cMessage[port - 1][shift], cMessage[port - 1][1 + shift], VOLT, Numofsamples, timeout);
		break;

	case CODE_H2AR3_STREAM_A:
		Numofsamples = ((uint32_t) cMessage[port - 1][2 + shift])
				+ ((uint32_t) cMessage[port - 1][3 + shift] << 8)
				+ ((uint32_t) cMessage[port - 1][4 + shift] << 16)
				+ ((uint32_t) cMessage[port - 1][5 + shift] << 24);
		timeout = ((uint32_t) cMessage[port - 1][6 + shift])
				+ ((uint32_t) cMessage[port - 1][7 + shift] << 8)
				+ ((uint32_t) cMessage[port - 1][8 + shift] << 16)
				+ ((uint32_t) cMessage[port - 1][9 + shift] << 24);
		Exportstreamtoport(cMessage[port - 1][shift], cMessage[port - 1][1 + shift], AMP, Numofsamples, timeout);
		break;

	}

	return result;
}

/***************************************************************************/
/* Get the port for a given UART */
uint8_t GetPort(UART_HandleTypeDef *huart) {

	if (huart->Instance == USART2)
		return P1;
	else if (huart->Instance == USART6)
		return P2;
	else if (huart->Instance == USART1)
		return P3;
	else if (huart->Instance == USART4)
		return P4;
	else if (huart->Instance == USART5)
		return P5;
	else if (huart->Instance == USART3)
		return P6;

	return 0;
}

/***************************************************************************/
/* Register this module CLI Commands */
void RegisterModuleCLICommands(void){
	FreeRTOS_CLIRegisterCommand( &SampleCommandDefinition );
	FreeRTOS_CLIRegisterCommand( &StreamCommandDefinition );
}

/***************************************************************************/
/* This function is useful only for input (sensor) modules.
 * Samples a module parameter value based on parameter index.
 * paramIndex: Index of the parameter (1-based index).
 * value: Pointer to store the sampled float value.
 */
Module_Status GetModuleParameter(uint8_t paramIndex, float *value) {
    Module_Status status = BOS_OK;

    switch (paramIndex) {
        /* Sample Voltage */
        case 1:
            status = SampleVoltage(value);
            break;

        /* Sample Current */
        case 2:
//            status = SampleCurrent(value);
            break;

        /* Invalid parameter index */
        default:
            status = BOS_ERR_WrongParam;
            break;
    }

    return status;
}

/***************************************************************************/
/****************************** Local Functions ****************************/
/***************************************************************************/
/* AC Monitor Task function */
//void ACMonitorTask(void *argument) {
//
//	/* Infinite loop */
//	for (;;) {
//		CalculationVolt(&voltage);
//		CalculationAmp(&current);
//		/*  */
//		switch (StreamingDataMode) {
//
//		case STREAM_TO_PORT:
//			Exportstreamtoport(module1, port1, mode1, Numofsamples1, timeout1);
//			break;
//
//		case SAMPLE_TO_PORT:
//			Exporttoport(module2, port2, mode2);
//			break;
//
//		case STREAM_TO_Terminal:
//			Exportstreamtoterminal(Numofsamples3, timeout3, port3, mode3);
//			break;
//
//		default:
//			osDelay(10);
//			break;
//		}
//
//		taskYIELD();
//	}
//}

/***************************************************************************/
uint32_t ADCCalculation(uint8_t selected) {
	uint32_t adcTemp =0;
	uint32_t adcTempFiltered =0;

	switch (selected) {
	case AMP:
		AMPFilter.FilterOrder = AVG_FILTER_ORDER_A;
		SelectAmpereADCChannel();

		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1000);
		adcTemp = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);

		DeselectAmpereADCChannel();
		AvarageLPF(adcTemp, &adcTempFiltered, &AMPFilter);
		break;

	case VOLT:
		VoltFilter.FilterOrder = AVG_FILTER_ORDER_V;
		SelectVoltADCChannel();

		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1000);
		adcTemp = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);

		DeselectVoltADCChannel();
		AvarageLPF(adcTemp, &adcTempFiltered, &VoltFilter);
		break;

	default:
		break;
	}

	return adcTempFiltered;

}

/***************************************************************************/
void AvarageLPF(uint32_t IN, uint32_t *OUT, Filter_t *FILTER_OBJ) {
	uint32_t SUM = 0;
	uint16_t i = 0;

	/* Push The New Input To The History Buffer */
	FILTER_OBJ->DataBuffer[FILTER_OBJ->BufferIndex] = IN;
	FILTER_OBJ->BufferIndex++;
	if (FILTER_OBJ->BufferIndex == FILTER_OBJ->FilterOrder + 1) {
		FILTER_OBJ->BufferIndex = 0;
	}

	/* Calculate The Average For The Data In The Buffer */
	for (i = 0; i < FILTER_OBJ->FilterOrder + 1; i++) {
		SUM += FILTER_OBJ->DataBuffer[i];
	}

	*OUT = SUM / (FILTER_OBJ->FilterOrder + 1);
}

/***************************************************************************/
Module_Status CalculationVolt(float *measured_volt) {
//	Module_Status status = H2AR3_OK;
//	float _volt;
//
//	adcRawData = ADCCalculation(VOLT);
//
//	_volt = (float) (adcRawData * VREF) / ADC_RESOLUTION_12_BIT; /* 12 bit resolution */
//	_volt = (_volt - (VBAIS + VOLTAGE_OFFSET));
//	*measured_volt = _volt * VOLT_RATIO;              /* measured_volt =0;533.3533 */
//
//	return status;
}

/***************************************************************************/
Module_Status CalculationAmp(float *measured_amp) {
//	Module_Status status = H2AR3_OK;
//	float _volt;
//
//	adcRawData = ADCCalculation(AMP);
//
//	_volt = (float) (adcRawData * VREF) / 4095;
//	_volt = (_volt - (VBAIS + VOLTAGE_OFFSET));
//
//	/* 2.5 we have to make average error of vref before load is switched on */
//	*measured_amp = (_volt / 0.009795);
//
//	return status;
}

/***************************************************************************/
Module_Status Exportstreamtoport(uint8_t module, uint8_t port, All_Data function, uint32_t Numofsamples, uint32_t timeout) {
	Module_Status status = H2AR3_OK;
	uint32_t samples = 0;
	uint32_t period = 0;

	period = timeout / Numofsamples;

	if (timeout < MIN_PERIOD_MS || period < MIN_PERIOD_MS)
		return H2AR3_ERR_WRONGPARAMS;

	while (samples < Numofsamples) {
		status = Exporttoport(module, port, function);
		vTaskDelay(pdMS_TO_TICKS(period));
		samples++;
	}

	StreamingDataMode = DEFAULT;
	samples = 0;

	return status;
}

/***************************************************************************/
Module_Status Exporttoport(uint8_t module, uint8_t port, All_Data Mode) {
	Module_Status status = H2AR3_OK;
	float floatData = 0;
	static uint8_t temp[4] = { 0 };

	if (port == 0 && module == myID)
		return H2AR3_ERR_WRONGPARAMS;

	switch (Mode) {
	case VOLT:
		status = CalculationVolt(&floatData);
		if (module == myID || module == 0) {
			temp[0] = (uint8_t) ((*(uint32_t*) &floatData) >> 0);
			temp[1] = (uint8_t) ((*(uint32_t*) &floatData) >> 8);
			temp[2] = (uint8_t) ((*(uint32_t*) &floatData) >> 16);
			temp[3] = (uint8_t) ((*(uint32_t*) &floatData) >> 24);
			writePxITMutex(port, (char*) &temp[0], 4 * sizeof(uint8_t), 10);
		} else {
			if (H2AR3_OK == status)
				MessageParams[1] = BOS_OK;
			else
				MessageParams[1] = BOS_ERROR;

			MessageParams[0] = FMT_FLOAT;
			MessageParams[2] = 1;
			MessageParams[3] = (uint8_t) ((*(uint32_t*) &floatData) >> 0);
			MessageParams[4] = (uint8_t) ((*(uint32_t*) &floatData) >> 8);
			MessageParams[5] = (uint8_t) ((*(uint32_t*) &floatData) >> 16);
			MessageParams[6] = (uint8_t) ((*(uint32_t*) &floatData) >> 24);
			SendMessageToModule(module, CODE_READ_RESPONSE, sizeof(float) + 3);
		}
		break;

	case AMP:
		status = CalculationAmp(&floatData);
		if (module == myID || module == 0) {
			temp[0] = (uint8_t) ((*(uint32_t*) &floatData) >> 0);
			temp[1] = (uint8_t) ((*(uint32_t*) &floatData) >> 8);
			temp[2] = (uint8_t) ((*(uint32_t*) &floatData) >> 16);
			temp[3] = (uint8_t) ((*(uint32_t*) &floatData) >> 24);
			writePxITMutex(port, (char*) &temp[0], 4 * sizeof(uint8_t), 10);
		} else {
			if (H2AR3_OK == status)
				MessageParams[1] = BOS_OK;
			else
				MessageParams[1] = BOS_ERROR;

			MessageParams[0] = FMT_FLOAT;
			MessageParams[2] = 1;
			MessageParams[3] = (uint8_t) ((*(uint32_t*) &floatData) >> 0);
			MessageParams[4] = (uint8_t) ((*(uint32_t*) &floatData) >> 8);
			MessageParams[5] = (uint8_t) ((*(uint32_t*) &floatData) >> 16);
			MessageParams[6] = (uint8_t) ((*(uint32_t*) &floatData) >> 24);
			SendMessageToModule(module, CODE_READ_RESPONSE, sizeof(float) + 3);

			default:
			break;
		}
	}

	StreamingDataMode = DEFAULT;
	memset(&temp[0], 0, sizeof(temp));

	return status;
}

/***************************************************************************/
static Module_Status PollingSleepCLISafe(uint32_t period, long Numofsamples) {
	const unsigned DELTA_SLEEP_MS = 100; // milliseconds
	long numDeltaDelay = period / DELTA_SLEEP_MS;
	unsigned lastDelayMS = period % DELTA_SLEEP_MS;

	while (numDeltaDelay-- > 0) {
		vTaskDelay(pdMS_TO_TICKS(DELTA_SLEEP_MS));

		/* Look for ENTER key to stop the stream */
		for (uint8_t chr = 0; chr < MSG_RX_BUF_SIZE; chr++) {
			if (UARTRxBuf[pcPort - 1][chr] == '\r' && Numofsamples > 0) {
				UARTRxBuf[pcPort - 1][chr] = 0;
				flag = 1;
				return H2AR3_ERR_TERMINATED;
			}
		}

		if (stopStream)
			return H2AR3_ERR_TERMINATED;
	}

	vTaskDelay(pdMS_TO_TICKS(lastDelayMS));

	return H2AR3_OK;
}

/***************************************************************************/
Module_Status Exportstreamtoterminal(uint32_t Numofsamples, uint32_t timeout, uint8_t Port, All_Data function) {
	Module_Status status = H2AR3_OK;
	int8_t *pcOutputString = NULL;
	uint32_t period = timeout / Numofsamples;
	float floatData;
	static uint8_t temp[4] = { 0 };
	char cstring[100];
	long numTimes = timeout / period;
	if (period < MIN_MEMS_PERIOD_MS)
		return H2AR3_ERR_WRONGPARAMS;

	switch (function) {
	case VOLT:
		if (period > timeout)
			timeout = period;

		stopStream = false;

		while ((numTimes-- > 0) || (timeout >= MAX_MEMS_TIMEOUT_MS)) {
			pcOutputString = FreeRTOS_CLIGetOutputBuffer();
			CalculationVolt(&floatData);

			snprintf(cstring, 50, "CellVoltage | VOLT: %.2f\r\n", floatData);

			writePxMutex(Port, (char*) cstring, strlen((char*) cstring),
			cmd500ms, HAL_MAX_DELAY);
			if (PollingSleepCLISafe(period, Numofsamples) != H2AR3_OK)
				break;
		}

	case AMP:
		if (period > timeout)
			timeout = period;

		stopStream = false;

		while ((numTimes-- > 0) || (timeout >= MAX_MEMS_TIMEOUT_MS)) {
			pcOutputString = FreeRTOS_CLIGetOutputBuffer();
			CalculationAmp(&floatData);

			snprintf(cstring, 50, "CellVoltage | AMP: %.2f\r\n", floatData);

			writePxMutex(Port, (char*) cstring, strlen((char*) cstring),
			cmd500ms, HAL_MAX_DELAY);
			if (PollingSleepCLISafe(period, Numofsamples) != H2AR3_OK)
				break;
		}
		break;

	}

	StreamingDataMode = DEFAULT;

	return status;
}

/***************************************************************************/
void SampleVToString(char *cstring, size_t maxLen) {
	float volt = 0;

	SampleVoltage(&volt);;
	snprintf(cstring, maxLen, "volt: %.2f \r\n", volt);
}

/***************************************************************************/
void SampleAToString(char *cstring, size_t maxLen) {
	float AMP = 0;

//	SampleCurrent(&AMP);;
	snprintf(cstring, maxLen, "AMP: %.2f \r\n", AMP);
}

/***************************************************************************/
static Module_Status StreamToCLI(uint32_t Numofsamples, uint32_t timeout, SampleToString function) {
	Module_Status status = H2AR3_OK;
	int8_t *pcOutputString = NULL;
	uint32_t period = timeout / Numofsamples;

	if (period < MIN_MEMS_PERIOD_MS)
		return H2AR3_ERR_WRONGPARAMS;

	for (uint8_t chr = 0; chr < MSG_RX_BUF_SIZE; chr++) {
		if (UARTRxBuf[pcPort - 1][chr] == '\r') {
			UARTRxBuf[pcPort - 1][chr] = 0;
		}
	}

	if (1 == flag) {
		flag = 0;
		static char *pcOKMessage = (int8_t*) "Stop stream !\n\r";
		writePxITMutex(pcPort, pcOKMessage, strlen(pcOKMessage), 10);
		return status;
	}

	if (period > timeout)
		timeout = period;

	long numTimes = timeout / period;
	stopStream = false;

	while ((numTimes-- > 0) || (timeout >= MAX_MEMS_TIMEOUT_MS)) {
		pcOutputString = FreeRTOS_CLIGetOutputBuffer();
		function((char*) pcOutputString, 100);

		writePxMutex(pcPort, (char*) pcOutputString, strlen((char*) pcOutputString), cmd500ms, HAL_MAX_DELAY);
		if (PollingSleepCLISafe(period, Numofsamples) != H2AR3_OK)
			break;
	}

	memset((char*) pcOutputString, 0, configCOMMAND_INT_MAX_OUTPUT_SIZE);
	sprintf((char*) pcOutputString, "\r\n");

	return status;
}

/***************************************************************************/
/***************************** General Functions ***************************/
/***************************************************************************/
//Module_Status SampleVoltage(float *volt) {
//	Module_Status status = H2AR3_OK;
//
//	*volt = voltage;
//
//	return status;
//}

/***************************************************************************/
//Module_Status SampleCurrent(float *curr) {
//	Module_Status status = H2AR3_OK;
//
//	*curr = current;
//
//	return status;
//}

/***************************************************************************/
Module_Status StreamtoPort(uint8_t module, uint8_t port, All_Data function,	uint32_t Numofsamples, uint32_t timeout) {
	Module_Status status = H2AR3_OK;

	StreamingDataMode = STREAM_TO_PORT;
	port1 = port;
	module1 = module;
	Numofsamples1 = Numofsamples;
	timeout1 = timeout;
	mode1 = function;

	return status;

}

/***************************************************************************/
Module_Status StreamToTerminal(uint8_t port, All_Data function, uint32_t Numofsamples, uint32_t timeout) {
	Module_Status status = H2AR3_OK;

	StreamingDataMode = STREAM_TO_Terminal;
	port3 = port;
	Numofsamples3 = Numofsamples;
	timeout3 = timeout;
	mode3 = function;

	return status;
}

/***************************************************************************/
Module_Status SampletoPort(uint8_t module, uint8_t port, All_Data function) {
	Module_Status status = H2AR3_OK;

	StreamingDataMode = SAMPLE_TO_PORT;
	port2 = port;
	module2 = module;
	mode2 = function;

	return status;
}

/*********************************************************
 * *******************************************************
 * ************************************************************
 * **************************************************************
 *****************************************************************************
 **************************************************************************************************
 ***********************************************************************************************************
 *****************************************************************************************************************
 ******************************************************************************************************************
 *******************************************************************************************************************
 *******************************************************************************************************************
 *******************************************************************************************************************
 *******************************************************************************************************************
 *******************************************************************************************************************/
//float st,et,tt ;
//
//extern ADC_HandleTypeDef hadc1; // ADC1 instance for current sense
//extern TIM_HandleTypeDef htim1; // TIM1 instance for 10 kHz sampling rate
//
//// Global variables for sampling and processing
//float vr_buffer[SAMPLE_COUNT];  // Array to store calculated Vr values (in volts)
//float ir_buffer[SAMPLE_COUNT];  // Array to store calculated Ir values (in amps)
//float vadc_buffer[SAMPLE_COUNT];  // Array to store calculated Ir values (in amps)
//
//volatile uint16_t sample_count = 0; // Counter for number of samples collected
//volatile uint8_t reading_complete = 0; // Flag to indicate sampling completion
//static float gain_factor = 1.0f / GAIN; // Pre-calculated 1/GAIN to reduce division
//static float te_by_r; // Pre-calculated Te/R to reduce division
//extern uint8_t f;
//    /*
//     * @brief: Samples current (Ir) data from AC monitor.
//     * @param ir_buffer: Pointer to store calculated Ir values (in amps).
//     * @param monitor_type: Enum defining the AC monitor type (CR8450_1000 or CR8401_1000).
//     * @retval: Module status indicating success or error.
//     */
//Module_Status SampleVrIr(float *vr_buffer, float *ir_buffer, AC_Monitor_Status monitor_type)
//{
//
//    Module_Status status = H2AR3_OK;
//    float te, r;           // Variables for Te and R based on monitor type
//    float gain_factor = 1.0f / GAIN; // Pre-calculate 1/GAIN to reduce division
//   volatile uint16_t sample_count = 0; // Sample counter
//
//    // Select Te and R based on monitor type
//    if (monitor_type == CR8450_1000) {
//        te = TE_CR8450_1000;
//        r = R_CR8450_1000;
//    } else if (monitor_type == CR8401_1000) {
//        te = TE_CR8401_1000;
//        r = R_CR8401_1000;
//    } else {
//        return status = H2AR3_ERROR; // Invalid monitor type
//    }
//    te_by_r = te / r; // Calculate Te/R once
//
//    // Start the timer with interrupt
//    HAL_TIM_Base_Start_IT(&htim1);
//    st=HAL_GetTick();
//    // Timer interrupt callback will handle sampling
////    while (!reading_complete) {
////        // Wait for completion (non-blocking in real application, just for demo)
////    }
//
//    // Stop the timer after sampling
//
//    // Check if sampling was successful
////    if (sample_count != SAMPLE_COUNT) {
////        return status = H2AR3_ERROR;
////    }
//
//    // Reset reading_complete flag
////    reading_complete = 0;
//
//    return status;
//}
//char data[100];
//float ff;
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// {
//
//	if (htim->Instance == TIM1) {
//		if (sample_count < SAMPLE_COUNT) {
//			HAL_ADC_Start(&hadc1); // Start ADC manually
//			if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
//				uint16_t adc_value = HAL_ADC_GetValue(&hadc1); // Get raw ADC value
//				float vadc = (adc_value * VREF) / ADC_MAX; // Convert to voltage (0-3V)
//				float vr = (vadc - VBIAS) * gain_factor; // Calculate Vr = (Vadc - Vbias) / Gain
//				vr_buffer[sample_count] = vr; // Store Vr
//				vadc_buffer[sample_count] = vadc;
//				ir_buffer[sample_count] = vr * te_by_r; // Calculate Ir = Vr * (Te/R)
//				//  sprintf(gu8_MSG, "%d\t%.2f\t%.2f\t\n",uwTick,VoltRtest speed motoreeeeequest,OldFreq);
//
//				sample_count++; // Increment sample counter
//
//			}
//			HAL_ADC_Stop(&hadc1); // Stop ADC manually
//		} else {
//			et = HAL_GetTick();
//			tt = et - st;
////            reading_complete = 1; // Set completion flag
//			HAL_TIM_Base_Stop_IT(&htim1);
//			sample_count = 0;
//			f++;
////              sprintf(data, "2f\t%.\t\n",ir_buffer);
////
////            HAL_UART_Transmit(&huart2,data, sizeof(data), 100);
////            char buffer[200];
////            sprintf(buffer, "%.2f", ir_buffer[sample_count]);
////            HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
//		}
//	}
//}

#include <math.h> // For sqrtf


extern ADC_HandleTypeDef hadc1; // ADC1 instance
extern TIM_HandleTypeDef htim1; // TIM1 instance for 10 kHz sampling rate
// Global variables for sampling
volatile uint8_t is_sampling_volt = 0;
volatile uint8_t is_sampling_current = 0; // Flag to indicate current or voltage sampling (0 for voltage, 1 for current)
volatile uint16_t sample_index_I,sample_index_V = 0;  // Sample index for continuous sampling
static float current_te_by_r = 1.0f; // Pre-calculated Te/R for current
float rms_buffer_I[SAMPLE_COUNT] ,rms_buffer_V[SAMPLE_COUNT] = {0}; // Buffer to store squared values for RMS calculation
float current_rms = 0.0f;            // RMS value for current
float voltage_rms = 0.0f;            // RMS value for voltage
static float rms_sum_I,rms_sum_V = 0.0f;         // Sum of squared values for RMS calculation
static uint8_t initial_samples_collected_I, initial_samples_collected_V = 0; // Flag to indicate if 200 samples are collected
static uint8_t voltage_sampling_started = 0;
static uint8_t current_sampling_started = 0;
uint16_t adc_val[2] = {0};
AC ACC;
/*
 * @brief: Initiates sampling of voltage using ADC channel 16.
 * @param volt: Pointer to store the calculated voltage (in volts).
 * @retval: Module status indicating success or error.
 */
Module_Status SampleVoltage(float *volt) {
    Module_Status status = H2AR3_OK;

    if (!voltage_sampling_started) {

        // Set sampling mode to voltage
        is_sampling_volt = 1;

        // Reset RMS variables
        sample_index_V = 0;
        rms_sum_V = 0.0f;
        initial_samples_collected_V = 0;
        voltage_rms = 0.0f;

        // Start the timer only once
        if ((htim1.Instance->CR1 & TIM_CR1_CEN) == 0) {
            HAL_TIM_Base_Start_IT(&htim1);
        }

        voltage_sampling_started = 1;
    }

    // Always return latest RMS value
//    *volt = voltage_rms;
    *volt=ACC.volt;
    return status;
}


/*
 * @brief: Initiates sampling of current using ADC channel 6.
 * @param curr: Pointer to store the calculated current (in amps).
 * @param monitor_type: Enum defining the AC monitor type (CR8450_1000 or CR8401_1000).
 * @retval: Module status indicating success or error.
 */
Module_Status SampleCurrent(float *curr, AC_Monitor_Status monitor_type) {
	Module_Status status = H2AR3_OK;

	if (!current_sampling_started) {
		ADC_ChannelConfTypeDef sConfig = { 0 };
		float te = 0.0f;

		switch (monitor_type) {
		case CR8450_1000:
			te = TE_CR8450_1000;
			break;
		case CR8401_1000:
			te = TE_CR8401_1000;
			break;
		default:
			return H2AR3_ERROR;
		}

		current_te_by_r = te / CURRENT_R;

		is_sampling_current = 1;
		sample_index_I = 0;
		rms_sum_I = 0.0f;
		initial_samples_collected_I = 0;
		current_rms = 0.0f;

		if ((htim1.Instance->CR1 & TIM_CR1_CEN) == 0) {
			HAL_TIM_Base_Start_IT(&htim1);
		}

		current_sampling_started = 1;
	}

	*curr = ACC.cur;
	return status;
}

/*
 * @brief: Timer ISR callback to read ADC and update sample.
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM1) {
		for (int i = 0; i < 2; ++i) {
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
			adc_val[i] = HAL_ADC_GetValue(&hadc1);
		}
		float temp_value = 0.0f;

		if (is_sampling_current) {
			temp_value = CalculateCurrent(adc_val[0]);
			current_rms = CalculateIRMS(temp_value); // Update RMS for current
			if (++sample_index_I >= 1000) {
				sample_index_I = 0;
				initial_samples_collected_I = 1;
			}
		}
		if (is_sampling_volt) {
			temp_value = CalculateVoltage(adc_val[1]);
			voltage_rms = CalculateVRMS(temp_value); // Update RMS for voltage
			if (++sample_index_V >= 1000) {
				sample_index_V = 0;
				initial_samples_collected_V = 1;
			}
		}

	}
	HAL_ADC_Stop(&hadc1);

}

/*
 * @brief: Calculates the voltage based on ADC reading.
 * @param adc_value: Raw ADC value to calculate voltage.
 * @retval: Calculated voltage (in volts), or -1.0f if error.
 */
float CalculateVoltage(uint16_t adc_value) {
    // Convert ADC value to voltage (Vadc = (ADC_value / 4095) * 3.0)
    float vadc = (adc_value * VREF) / ADC_MAX;

    // Calculate Vrv = (Vadc - Vbias) / gain_v
//    float vrv = (vadc - VOLTAGE_VBIAS) / VOLTAGE_GAIN;
    float vr = (vadc - VOLTAGE_VBIAS) ;
    float ff=roundf(vr*100)/100;

    float e= ff/VOLTAGE_GAIN;
    // Calculate Vin = Vrv * (sumR / Rv)
    float vin = (e * (VOLTAGE_SUMR / VOLTAGE_RV)) ;

    // Return the calculated voltage
    return vin;
}
/*
 * @brief: Calculates the current based on ADC reading.
 * @param adc_value: Raw ADC value to calculate current.
 * @retval: Calculated current (in amps), or -1.0f if error.
 */
float CalculateCurrent(uint16_t adc_value) {
    // Convert ADC value to voltage (Vadc = (ADC_value / 4095) * 3.0)
    float vadc = (adc_value * VREF) / ADC_MAX;

//vadc =0.12345;
    // Calculate Vr = (Vadc - Vbias) / Gain
    float vr = (vadc - CURRENT_VBIAS) ;
    float ff=roundf(vr*100)/100;

    float e= ff/CURRENT_GAIN;

    // Calculate Ir = Vr * (Te / R), where Te/R is pre-calculated as current_te_by_r
    float ir = e * current_te_by_r;

    // Return the calculated current
    return ir;
}

/*
 * @brief: Calculates the RMS value of samples.
 * @param new_ir: Latest Ir value.
 */
float CalculateIRMS(float new_ir) {
    uint16_t idx = sample_index_I % SAMPLE_COUNT;

    if (sample_index_I < SAMPLE_COUNT) {

        if (sample_index_I == SAMPLE_COUNT - 1 && initial_samples_collected_I == 0) {
        	rms_sum_I += new_ir * new_ir;

			float rms = sqrtf(rms_sum_I / SAMPLE_COUNT);
			ACC.cur = rms;
			return rms;
		}
    } else if (initial_samples_collected_I) {
        float old_ir = rms_buffer_I[idx];
        rms_buffer_I[idx] = new_ir * new_ir; // Store the new squared value
        rms_sum_I += rms_buffer_I[idx] ;
        rms_sum_I -=old_ir ;// Updatold_ire sum with new and old squared values
        float rms = sqrtf(rms_sum_I / SAMPLE_COUNT);
    	ACC.cur = rms;
        return rms;
    }
    return -1.0f; // Return -1.0f until 200 samples are collected
}


/*
 * @brief: Calculates the RMS value of samples.
 * @param new_ir: Latest Ir value.
 */
float CalculateVRMS(float new_V) {
    uint16_t idx = sample_index_V % SAMPLE_COUNT;

    if (sample_index_V < SAMPLE_COUNT) {

        if (sample_index_V == SAMPLE_COUNT - 1 && initial_samples_collected_V == 0) {
        	rms_sum_V += new_V * new_V;

			float rms = sqrtf(rms_sum_V / SAMPLE_COUNT);
			ACC.cur = rms;
			return rms;
		}
    } else if (initial_samples_collected_V) {
        float old_ir = rms_buffer_V[idx];
        rms_buffer_V[idx] = new_V * new_V; // Store the new squared value
        rms_sum_V += rms_buffer_V[idx] ;
        rms_sum_V -=old_ir ;// Updatold_ire sum with new and old squared values
        float rms = sqrtf(rms_sum_V / SAMPLE_COUNT);
    	ACC.volt = rms;
        return rms;
    }
    return -1.0f; // Return -1.0f until 200 samples are collected
}

/***************************************************************************/
/********************************* Commands ********************************/
/***************************************************************************/
static portBASE_TYPE SampleSensorCommand(int8_t *pcWriteBuffer,	size_t xWriteBufferLen, const int8_t *pcCommandString) {
	const char *const AMPCmdName = "amp";
	const char *const VOLTCmdName = "volt";
	const char *pSensName = NULL;
	portBASE_TYPE sensNameLen = 0;

	// Make sure we return something
	*pcWriteBuffer = '\0';

	pSensName = (const char*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
			&sensNameLen);

	if (pSensName == NULL) {
		snprintf((char*) pcWriteBuffer, xWriteBufferLen,
				"Invalid Arguments\r\n");
		return pdFALSE;
	}

	do {
		if (!strncmp(pSensName, AMPCmdName, strlen(AMPCmdName))) {
			SampleAToString((char*) pcWriteBuffer, xWriteBufferLen);

		} else if (!strncmp(pSensName, VOLTCmdName, strlen(VOLTCmdName))) {
			SampleVToString((char*) pcWriteBuffer, xWriteBufferLen);
		} else {
			snprintf((char*) pcWriteBuffer, xWriteBufferLen,
					"Invalid Arguments\r\n");
		}

		return pdFALSE;
	} while (0);

	snprintf((char*) pcWriteBuffer, xWriteBufferLen,
			"Error reading Sensor\r\n");

	return pdFALSE;
}

/***************************************************************************/
/* Port Mode => false and CLI Mode => true */
static bool StreamCommandParser(const int8_t *pcCommandString, const char **ppSensName, portBASE_TYPE *pSensNameLen,
		bool *pPortOrCLI, uint32_t *pPeriod, uint32_t *pTimeout, uint8_t *pPort, uint8_t *pModule) {

	const char *pPortStr = NULL;
	const char *pModStr = NULL;
	const char *pPeriodMSStr = NULL;
	const char *pTimeoutMSStr = NULL;

	portBASE_TYPE periodStrLen = 0;
	portBASE_TYPE timeoutStrLen = 0;
	portBASE_TYPE portStrLen = 0;
	portBASE_TYPE modStrLen = 0;

	*ppSensName = (const char*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
			pSensNameLen);
	pPeriodMSStr = (const char*) FreeRTOS_CLIGetParameter(pcCommandString, 2,
			&periodStrLen);
	pTimeoutMSStr = (const char*) FreeRTOS_CLIGetParameter(pcCommandString, 3,
			&timeoutStrLen);

	// At least 3 Parameters are required!
	if ((*ppSensName == NULL) || (pPeriodMSStr == NULL) || (pTimeoutMSStr == NULL))
		return false;

	// TODO: Check if Period and Timeout are integers or not!
	*pPeriod = atoi(pPeriodMSStr);
	*pTimeout = atoi(pTimeoutMSStr);
	*pPortOrCLI = true;

	pPortStr = (const char*) FreeRTOS_CLIGetParameter(pcCommandString, 4, &portStrLen);
	pModStr = (const char*) FreeRTOS_CLIGetParameter(pcCommandString, 5, &modStrLen);

	if ((pModStr == NULL) && (pPortStr == NULL))
		return true;

	if ((pModStr == NULL) || (pPortStr == NULL))// If user has provided 4 Arguments.
		return false;

	*pPort = atoi(pPortStr);
	*pModule = atoi(pModStr);
	*pPortOrCLI = false;

	return true;
}

/***************************************************************************/
static portBASE_TYPE StreamSensorCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString) {
	const char *const AMPCmdName = "amp";
	const char *const VOLTCmdName = "volt";
	const char *pSensName = NULL;
	uint32_t Numofsamples = 0;
	uint32_t timeout = 0;
	uint8_t port = 0;
	uint8_t module = 0;

	bool portOrCLI = true; // Port Mode => false and CLI Mode => true
	portBASE_TYPE sensNameLen = 0;

	*pcWriteBuffer = '\0';

	if (!StreamCommandParser(pcCommandString, &pSensName, &sensNameLen,
			&portOrCLI, &Numofsamples, &timeout, &port, &module)) {
		snprintf((char*) pcWriteBuffer, xWriteBufferLen,
				"Invalid Arguments\r\n");
		return pdFALSE;
	}

	do {
		if (!strncmp(pSensName, AMPCmdName, strlen(AMPCmdName))) {
			if (portOrCLI)
				StreamToCLI(Numofsamples, timeout, SampleAToString);
			else
				StreamtoPort(module, port, AMP, Numofsamples, timeout);

		} else if (!strncmp(pSensName, VOLTCmdName, strlen(VOLTCmdName))) {
			if (portOrCLI)
				StreamToCLI(Numofsamples, timeout, SampleVToString);
			else
				StreamtoPort(module, port, VOLT, Numofsamples, timeout);
		} else
			snprintf((char*) pcWriteBuffer, xWriteBufferLen,
					"Invalid Arguments\r\n");

		snprintf((char*) pcWriteBuffer, xWriteBufferLen, "\r\n");
		return pdFALSE;

	} while (0);

	snprintf((char*) pcWriteBuffer, xWriteBufferLen,
			"Error reading Sensor\r\n");

	return pdFALSE;
}

/***************************************************************************/
/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
