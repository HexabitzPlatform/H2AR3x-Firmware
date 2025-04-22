/*
 BitzOS (BOS) V0.3.4 - Copyright (C) 2017-2024 Hexabitz
 All rights reserved

 File Name     : H2AR3.c
 Description   : Source code for module H2AR3.
 	 	 	 	 (Description_of_module)

(Description of Special module peripheral configuration):
>>
>>
>>

 */

/* Includes ------------------------------------------------------------------*/
#include "BOS.h"
#include "H2AR3_inputs.h"
/* Define UART variables */
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart6;


ADC_HandleTypeDef hadc1;

float H2AR3_voltage = 0.0f;
float H2AR3_current = 0.0f;

/* Module Parameters */
ModuleParam_t ModuleParam[NUM_MODULE_PARAMS] ={
    {.ParamPtr = &H2AR3_voltage, .ParamFormat = FMT_FLOAT, .ParamName = "voltage"},
    {.ParamPtr = &H2AR3_current, .ParamFormat = FMT_FLOAT, .ParamName = "current"}
};

/* Local functions */
Module_Status CalculationVolt( float * measured_volt) ;
Module_Status CalculationAmp(float *measured_volt) ;
Module_Status Exporttoport(uint8_t module,uint8_t port,All_Data Mode);
Module_Status Exportstreamtoterminal(uint32_t Numofsamples, uint32_t timeout,uint8_t Port,All_Data function);
Module_Status Exportstreamtoport (uint8_t module,uint8_t port,All_Data function,uint32_t Numofsamples,uint32_t timeout);

typedef void (*SampleToString)(char *, size_t);
typedef void (*SampleToPort)(uint8_t, uint8_t);
typedef void (*SampleToBuffer)(float *buffer);
/* Private variables ---------------------------------------------------------*/
TaskHandle_t ACMonitorTaskHandle = NULL;
uint32_t raw_adc, tmp_adc;
uint32_t adcTempFiltered;
uint8_t port1, module1,mode1;
uint32_t Numofsamples1 ,timeout1;
uint8_t port2 ,module2,mode2;
uint8_t port3 ,module3,mode3;
uint32_t Numofsamples3 ,timeout3;
uint8_t flag ;
uint8_t tofMode ;
FIR_Filter_cfg A_Filter;
FIR_Filter_cfg V_Filter;
float current_ , voltage ;
static bool stopStream = false;
float measured_volt, measured_amp;

/* Private function prototypes -----------------------------------------------*/
void ExecuteMonitor(void);
void FLASH_Page_Eras(uint32_t Addr );
void ACMonitorTask(void *argument);
static void AVG_FIR_LPF(FILTER_DATA_TYPE IN, FILTER_DATA_TYPE* OUT, FIR_Filter_cfg* FILTER_OBJ);

/* Create CLI commands --------------------------------------------------------*/
static portBASE_TYPE SampleSensorCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);
static portBASE_TYPE StreamSensorCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);

/* CLI command structure : sample */
const CLI_Command_Definition_t SampleCommandDefinition = {
	(const int8_t *) "sample",
	(const int8_t *) "sample:\r\n Syntax: sample [AMP]/[VOLT].\r\n\r\n",
	SampleSensorCommand,
	1
};
/* CLI command structure : stream */
const CLI_Command_Definition_t StreamCommandDefinition = {
	(const int8_t *) "stream",
	(const int8_t *) "stream:\r\n Syntax: stream  [AMP]/[VOLT] (period in ms) (time in ms) [port] [module].\r\n\r\n",
	StreamSensorCommand,
	-1
};


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
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	/* Array ports */

	MX_USART2_UART_Init();
	MX_USART1_UART_Init();
	MX_USART6_UART_Init();
	MX_ADC_Init();

	//Circulating DMA Channels ON All Module
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
	if (ACMonitorTaskHandle == NULL)
		xTaskCreate(ACMonitorTask, (const char*) "ACMonitorTask",
				configMINIMAL_STACK_SIZE, NULL,
				osPriorityNormal - osPriorityIdle, &ACMonitorTaskHandle);
}

/***************************************************************************/
/* This function is useful only for input (sensor) modules.
 * @brief: Samples a module parameter value based on parameter index.
 * @param paramIndex: Index of the parameter (1-based index).
 * @param value: Pointer to store the sampled float value.
 * @retval: Module_Status indicating success or failure.
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
            status = SampleCurrent(value);
            break;

        /* Invalid parameter index */
        default:
            status = BOS_ERR_WrongParam;
            break;
    }

    return status;
}

/*-----------------------------------------------------------*/
/* --- H2AR3 message processing task.
 */
Module_Status Module_MessagingTask(uint16_t code,uint8_t port,uint8_t src,uint8_t dst,uint8_t shift){
	Module_Status result =H2AR3_OK;
	  uint32_t Numofsamples;
		  uint32_t timeout;

		switch(code){
		case CODE_H2AR3_SAMPLE_V:
		{
			SampletoPort(cMessage[port-1][shift],cMessage[port-1][1+shift],VOLT);
			break;
		}

		case CODE_H2AR3_SAMPLE_A:
		{
			SampletoPort(cMessage[port-1][shift],cMessage[port-1][1+shift],AMP);
			break;
		}
		case CODE_H2AR3_STREAM_V:
			{
				Numofsamples = ((uint32_t) cMessage[port - 1][2 + shift] ) + ((uint32_t) cMessage[port - 1][3 + shift] << 8) + ((uint32_t) cMessage[port - 1][4 + shift] << 16) + ((uint32_t)cMessage[port - 1][5 + shift] << 24);
				timeout = ((uint32_t) cMessage[port - 1][6 + shift] ) + ((uint32_t) cMessage[port - 1][7 + shift] << 8) + ((uint32_t) cMessage[port - 1][8 + shift] << 16) + ((uint32_t)cMessage[port - 1][9 + shift] << 24);
				Exportstreamtoport(cMessage[port-1][shift] ,cMessage[port-1][1+shift],VOLT, Numofsamples, timeout);
				break;
			}

			case CODE_H2AR3_STREAM_A:
			{
				Numofsamples = ((uint32_t) cMessage[port - 1][2 + shift] ) + ((uint32_t) cMessage[port - 1][3 + shift] << 8) + ((uint32_t) cMessage[port - 1][4 + shift] << 16) + ((uint32_t)cMessage[port - 1][5 + shift] <<24);
				timeout = ((uint32_t) cMessage[port - 1][6 + shift] ) + ((uint32_t) cMessage[port - 1][7 + shift] << 8) + ((uint32_t) cMessage[port - 1][8 + shift] << 16) + ((uint32_t)cMessage[port - 1][9 + shift]<<24);
				Exportstreamtoport(cMessage[port-1][shift] ,cMessage[port-1][1+shift],AMP, Numofsamples, timeout);
				break;
			}
		}
		return result;

}
/* --- Get the port for a given UART. 
 */
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

/*-----------------------------------------------------------*/
/* AC Monitor Task function */

void ACMonitorTask(void *argument){

	/* Infinite loop */
	for(;;){
		CalculationVolt(&voltage);
		CalculationAmp(&current_);
		/*  */
		switch (tofMode) {

		case STREAM_TO_PORT:
			Exportstreamtoport(module1, port1, mode1, Numofsamples1, timeout1);
			break;
		case SAMPLE_TO_PORT:

			Exporttoport(module2, port2, mode2);
			break;
		case STREAM_TO_Terminal:
			Exportstreamtoterminal(Numofsamples3, timeout3, port3, mode3);

			break;

		default:
			osDelay(10);
			break;
		}

		taskYIELD();
	}

}
/* --- Register this module CLI Commands
 */
void RegisterModuleCLICommands(void){
	FreeRTOS_CLIRegisterCommand( &SampleCommandDefinition );
	FreeRTOS_CLIRegisterCommand( &StreamCommandDefinition );
}

/*-----------------------------------------------------------*/
uint32_t Adc_Calculation(uint8_t selected) {

	switch (selected) {
	case AMP:
		A_Filter.Filter_Order = AVG_FILTER_ORDER_A;
		ADC_Select_CH6();
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1000);
		tmp_adc = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
		ADC_Deselect_CH6();
		AVG_FIR_LPF(tmp_adc, &adcTempFiltered, &A_Filter);
		break;

	case VOLT:
		V_Filter.Filter_Order = AVG_FILTER_ORDER_V;
		ADC_Select_CH16();
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1000);
		tmp_adc = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
		ADC_Deselect_CH16();
		AVG_FIR_LPF(tmp_adc, &adcTempFiltered, &V_Filter);
		break;

	default:
		break;
	}

	return adcTempFiltered;

}
/*-----------------------------------------------------------*/
static void AVG_FIR_LPF(FILTER_DATA_TYPE IN, FILTER_DATA_TYPE* OUT, FIR_Filter_cfg* FILTER_OBJ)
 {
	FILTER_DATA_TYPE SUM = 0;
	uint16_t i = 0;

	// Push The New Input To The History Buffer
	FILTER_OBJ->Data_Buffer[FILTER_OBJ->Buffer_Index] = IN;
	FILTER_OBJ->Buffer_Index++;
	if (FILTER_OBJ->Buffer_Index == FILTER_OBJ->Filter_Order + 1) {
		FILTER_OBJ->Buffer_Index = 0;
	}

	// Calculate The Average For The Data In The Buffer
	for (i = 0; i < FILTER_OBJ->Filter_Order + 1; i++) {
		SUM += FILTER_OBJ->Data_Buffer[i];
	}

	*OUT = SUM / (FILTER_OBJ->Filter_Order + 1);
}

/*-----------------------------------------------------------*/
Module_Status CalculationVolt(float *measured_volt) {
	Module_Status status = H2AR3_OK;
	float _volt;
	raw_adc = Adc_Calculation(VOLT);
	_volt = (float) (raw_adc * VREF) / ADC_RESOLUTION_12_BIT;// 12 bit resolution
	_volt = (_volt - (VBAIS + VOLTAGE_OFFSET));
	*measured_volt = _volt * VOLTRATIO;              //measured_volt =0;533.3533
	return status;
}
/*-----------------------------------------------------------*/
Module_Status CalculationAmp(float *measured_amp) {
	Module_Status status = H2AR3_OK;
	float _volt;
	raw_adc = Adc_Calculation(AMP);
	_volt = (float) (raw_adc * VREF) / 4095;
	_volt = (_volt - (VBAIS + VOLTAGE_OFFSET));
	*measured_amp = (_volt / 0.009795); //2.5 we have to make average error of vref before load is switched on
	return status;
}

/*-----------------------------------------------------------*/
Module_Status Exportstreamtoport (uint8_t module,uint8_t port,All_Data function,uint32_t Numofsamples,uint32_t timeout)
 {
	Module_Status status = H2AR3_OK;
	uint32_t samples = 0;
	uint32_t period = 0;
	period = timeout / Numofsamples;

	if (timeout < MIN_PERIOD_MS || period < MIN_PERIOD_MS)
		return H2AR3_ERR_WrongParams;

	while (samples < Numofsamples) {
		status = Exporttoport(module, port, function);
		vTaskDelay(pdMS_TO_TICKS(period));
		samples++;
	}
	tofMode = DEFAULT;
	samples = 0;
	return status;
}
/*-----------------------------------------------------------*/
Module_Status Exporttoport(uint8_t module,uint8_t port,All_Data Mode)
 {
	float floatData = 0;
	static uint8_t temp[4] = { 0 };
	Module_Status status = H2AR3_OK;

	if (port == 0 && module == myID) {
		return H2AR3_ERR_WrongParams;
	}
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
	tofMode = DEFAULT;
	memset(&temp[0], 0, sizeof(temp));
	return status;
}
/*-----------------------------------------------------------*/

static Module_Status PollingSleepCLISafe(uint32_t period, long Numofsamples)
{
	const unsigned DELTA_SLEEP_MS = 100; // milliseconds
	long numDeltaDelay =  period / DELTA_SLEEP_MS;
	unsigned lastDelayMS = period % DELTA_SLEEP_MS;

	while (numDeltaDelay-- > 0) {
		vTaskDelay(pdMS_TO_TICKS(DELTA_SLEEP_MS));

		// Look for ENTER key to stop the stream
		for (uint8_t chr = 0; chr < MSG_RX_BUF_SIZE; chr++) {
			if (UARTRxBuf[pcPort - 1][chr] == '\r' && Numofsamples > 0) {
				UARTRxBuf[pcPort - 1][chr] = 0;
				flag=1;
				return H2AR3_ERR_TERMINATED;
			}
		}

		if (stopStream)
			return H2AR3_ERR_TERMINATED;
	}

	vTaskDelay(pdMS_TO_TICKS(lastDelayMS));
	return H2AR3_OK;
}
/*-----------------------------------------------------------*/
Module_Status Exportstreamtoterminal(uint32_t Numofsamples, uint32_t timeout,uint8_t Port,All_Data function)
 {
	Module_Status status = H2AR3_OK;
	int8_t *pcOutputString = NULL;
	uint32_t period = timeout / Numofsamples;
	float floatData;
	static uint8_t temp[4] = { 0 };
	char cstring[100];
	long numTimes = timeout / period;
	if (period < MIN_MEMS_PERIOD_MS)
		return H2AR3_ERR_WrongParams;

	// TODO: Check if CLI is enable or not
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

	tofMode = DEFAULT;
	return status;
 }

/*-----------------------------------------------------------*/
void SampleVToString(char *cstring, size_t maxLen)
 {
	float  volt = 0;
	SampleVoltage(&volt);;
	snprintf(cstring, maxLen, "volt: %.2f \r\n", volt);
 }
/*-----------------------------------------------------------*/
void SampleAToString(char *cstring, size_t maxLen)
 {
	float  AMP = 0;
	SampleCurrent(&AMP);;
	snprintf(cstring, maxLen, "AMP: %.2f \r\n", AMP);
 }

/*-----------------------------------------------------------*/

static Module_Status StreamToCLI(uint32_t Numofsamples, uint32_t timeout, SampleToString function)
{
	Module_Status status =H2AR3_OK;
	int8_t *pcOutputString = NULL;
	uint32_t period =timeout / Numofsamples;
	if(period < MIN_MEMS_PERIOD_MS)
		return H2AR3_ERR_WrongParams;

	// TODO: Check if CLI is enable or not
	for(uint8_t chr =0; chr < MSG_RX_BUF_SIZE; chr++){
		if(UARTRxBuf[pcPort - 1][chr] == '\r'){
			UARTRxBuf[pcPort - 1][chr] =0;
		}
	}
	if(1 == flag){
		flag =0;
		static char *pcOKMessage =(int8_t* )"Stop stream !\n\r";
		writePxITMutex(pcPort,pcOKMessage,strlen(pcOKMessage),10);
		return status;
	}
	if(period > timeout)
		timeout =period;

	long numTimes =timeout / period;
	stopStream = false;

	while((numTimes-- > 0) || (timeout >= MAX_MEMS_TIMEOUT_MS)){
		pcOutputString =FreeRTOS_CLIGetOutputBuffer();
		function((char* )pcOutputString,100);

		writePxMutex(pcPort,(char* )pcOutputString,strlen((char* )pcOutputString),cmd500ms,HAL_MAX_DELAY);
		if(PollingSleepCLISafe(period,Numofsamples) != H2AR3_OK)
			break;
	}

	memset((char* )pcOutputString,0,configCOMMAND_INT_MAX_OUTPUT_SIZE);
	sprintf((char* )pcOutputString,"\r\n");
	return status;
}

/* -----------------------------------------------------------------------
 |								  APIs							          | 																 	|
/* -----------------------------------------------------------------------
 */

Module_Status SampleVoltage(float *volt) {
	Module_Status status = H2AR3_OK;
	*volt = voltage;
	return status;
}
/*-----------------------------------------------------------*/
Module_Status SampleCurrent(float *curr) {
	Module_Status status = H2AR3_OK;
	*curr = current_;
	return status;
}
/*-----------------------------------------------------------*/
Module_Status StreamtoPort(uint8_t module,uint8_t port,All_Data function,uint32_t Numofsamples,uint32_t timeout)
{
	Module_Status status = H2AR3_OK;
	tofMode=STREAM_TO_PORT;
	port1 = port ;
	module1 =module;
	Numofsamples1=Numofsamples;
	timeout1=timeout;
	mode1= function;
	return status;

}
/*-----------------------------------------------------------*/
Module_Status StreamToTerminal(uint8_t port,All_Data function,uint32_t Numofsamples,uint32_t timeout)
{
	Module_Status status = H2AR3_OK;
	tofMode=STREAM_TO_Terminal;
	port3 = port ;
	Numofsamples3=Numofsamples;
	timeout3=timeout;
	mode3= function;
	return status;
}
/*-----------------------------------------------------------*/
Module_Status SampletoPort(uint8_t module,uint8_t port,All_Data function)
 {
	Module_Status status = H2AR3_OK;
	tofMode = SAMPLE_TO_PORT;
	port2 = port;
	module2 = module;
	mode2 = function;
	return status;
}
/* -----------------------------------------------------------------------
 |								Commands							      |
   -----------------------------------------------------------------------
 */
static portBASE_TYPE SampleSensorCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString)
{
	const char *const AMPCmdName = "amp";
	const char *const VOLTCmdName = "volt";


	const char *pSensName = NULL;
	portBASE_TYPE sensNameLen = 0;

	// Make sure we return something
	*pcWriteBuffer = '\0';

	pSensName = (const char *)FreeRTOS_CLIGetParameter(pcCommandString, 1, &sensNameLen);

	if (pSensName == NULL) {
		snprintf((char *)pcWriteBuffer, xWriteBufferLen, "Invalid Arguments\r\n");
		return pdFALSE;
	}

	do {
		if (!strncmp(pSensName, AMPCmdName, strlen(AMPCmdName))) {
			SampleAToString((char *)pcWriteBuffer, xWriteBufferLen);

		} else if (!strncmp(pSensName, VOLTCmdName, strlen(VOLTCmdName))) {
			SampleVToString((char *)pcWriteBuffer, xWriteBufferLen);
		}
		else {
			snprintf((char *)pcWriteBuffer, xWriteBufferLen, "Invalid Arguments\r\n");
		}

		return pdFALSE;
	} while (0);

	snprintf((char *)pcWriteBuffer, xWriteBufferLen, "Error reading Sensor\r\n");
	return pdFALSE;
}
/*-----------------------------------------------------------*/
// Port Mode => false and CLI Mode => true
static bool StreamCommandParser(const int8_t *pcCommandString, const char **ppSensName, portBASE_TYPE *pSensNameLen,
														bool *pPortOrCLI, uint32_t *pPeriod, uint32_t *pTimeout, uint8_t *pPort, uint8_t *pModule)
{
	const char *pPeriodMSStr = NULL;
	const char *pTimeoutMSStr = NULL;

	portBASE_TYPE periodStrLen = 0;
	portBASE_TYPE timeoutStrLen = 0;

	const char *pPortStr = NULL;
	const char *pModStr = NULL;

	portBASE_TYPE portStrLen = 0;
	portBASE_TYPE modStrLen = 0;

	*ppSensName = (const char *)FreeRTOS_CLIGetParameter(pcCommandString, 1, pSensNameLen);
	pPeriodMSStr = (const char *)FreeRTOS_CLIGetParameter(pcCommandString, 2, &periodStrLen);
	pTimeoutMSStr = (const char *)FreeRTOS_CLIGetParameter(pcCommandString, 3, &timeoutStrLen);

	// At least 3 Parameters are required!
	if ((*ppSensName == NULL) || (pPeriodMSStr == NULL) || (pTimeoutMSStr == NULL))
		return false;

	// TODO: Check if Period and Timeout are integers or not!
	*pPeriod = atoi(pPeriodMSStr);
	*pTimeout = atoi(pTimeoutMSStr);
	*pPortOrCLI = true;

	pPortStr = (const char *)FreeRTOS_CLIGetParameter(pcCommandString, 4, &portStrLen);
	pModStr = (const char *)FreeRTOS_CLIGetParameter(pcCommandString, 5, &modStrLen);

	if ((pModStr == NULL) && (pPortStr == NULL))
		return true;
	if ((pModStr == NULL) || (pPortStr == NULL))	// If user has provided 4 Arguments.
		return false;

	*pPort = atoi(pPortStr);
	*pModule = atoi(pModStr);
	*pPortOrCLI = false;

	return true;
}
/*-----------------------------------------------------------*/
static portBASE_TYPE StreamSensorCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString)
{
	const char *const AMPCmdName = "amp";
	const char *const VOLTCmdName = "volt";

	uint32_t Numofsamples = 0;
	uint32_t timeout = 0;
	uint8_t port = 0;
	uint8_t module = 0;

	bool portOrCLI = true; // Port Mode => false and CLI Mode => true

	const char *pSensName = NULL;
	portBASE_TYPE sensNameLen = 0;

	// Make sure we return something
	*pcWriteBuffer = '\0';

	if (!StreamCommandParser(pcCommandString, &pSensName, &sensNameLen, &portOrCLI, &Numofsamples, &timeout, &port, &module)) {
		snprintf((char *)pcWriteBuffer, xWriteBufferLen, "Invalid Arguments\r\n");
		return pdFALSE;
	}

	do {
		if (!strncmp(pSensName, AMPCmdName, strlen(AMPCmdName))) {
			if (portOrCLI) {

				StreamToCLI(Numofsamples, timeout, SampleAToString);
			} else {
				StreamtoPort(module, port,AMP, Numofsamples, timeout );

			}

		} else if (!strncmp(pSensName, VOLTCmdName, strlen(VOLTCmdName))) {
			if (portOrCLI) {
				StreamToCLI(Numofsamples, timeout, SampleVToString);

			} else {
				StreamtoPort(module, port,VOLT, Numofsamples, timeout);

			}




		}
		else {
			snprintf((char *)pcWriteBuffer, xWriteBufferLen, "Invalid Arguments\r\n");
		}

		snprintf((char *)pcWriteBuffer, xWriteBufferLen, "\r\n");
		return pdFALSE;
	} while (0);

	snprintf((char *)pcWriteBuffer, xWriteBufferLen, "Error reading Sensor\r\n");
	return pdFALSE;
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
