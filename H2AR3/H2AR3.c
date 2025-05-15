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
#include <math.h>

/* Exported Typedef ******************************************************/
/* Define UART variables */
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart6;
ADC_HandleTypeDef hadc1;

extern TIM_HandleTypeDef htim1; // TIM1 instance for 10 kHz sampling rate
/* Private Variables *******************************************************/
volatile uint8_t is_sampling_volt = 0;              /* Flag to indicate voltage sampling */
volatile uint8_t is_sampling_current = 0;           /* Flag to indicate current sampling (0 for voltage, 1 for current) */
volatile uint16_t sample_index_I = 0;               /* Sample index for current sampling */
volatile uint16_t sample_index_V = 0;               /* Sample index for voltage sampling */
static float current_te_by_r = 1.0f;                /* Pre-calculated Te/R for current */
float rms_buffer_I[SAMPLE_COUNT] = {0};             /* Buffer to store squared values for current RMS calculation */
float rms_buffer_V[SAMPLE_COUNT] = {0};             /* Buffer to store squared values for voltage RMS calculation */
float current_rms = 0.0f;                           /* RMS value for current */
float voltage_rms = 0.0f;                           /* RMS value for voltage */
static float rms_sum_I = 0.0f;                      /* Sum of squared values for current RMS calculation */
static float rms_sum_V = 0.0f;                      /* Sum of squared values for voltage RMS calculation */
static uint8_t initial_samples_collected_I = 0;      /* Flag to indicate if 200 current samples are collected */
static uint8_t initial_samples_collected_V = 0;      /* Flag to indicate if 200 voltage samples are collected */
static uint8_t voltage_sampling_started = 0;         /* Flag to indicate if voltage sampling has started */
static uint8_t current_sampling_started = 0;         /* Flag to indicate if current sampling has started */
uint16_t adc_val[2] = {0};                          /* Buffer to store ADC values */
AC ACC;                                             /* Structure to hold current and voltage data */

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
static float CalculateVoltageVolts(uint16_t adc_value); /* Calculates voltage from ADC reading */
static float CalculateCurrentAmps(uint16_t adc_value);  /* Calculates current from ADC reading */
static float CalculateCurrentRMS(float new_current);    /* Calculates RMS value for current */
static float CalculateVoltageRMS(float new_voltage);    /* Calculates RMS value for voltage */
/* Stream Functions */


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
}

/***************************************************************************/
/* H2AR3 message processing task */
Module_Status Module_MessagingTask(uint16_t code, uint8_t port, uint8_t src, uint8_t dst, uint8_t shift) {
	Module_Status result = H2AR3_OK;
	uint32_t Numofsamples;
	uint32_t timeout;

	switch (code) {

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

            break;

        /* Sample Current */
        case 2:

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

/*
 * @brief: Timer ISR callback to read ADC channel and update sample.
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	uint8_t channel_index = 0; /* Index for ADC channel iteration */
	float processed_value = 0.0f; /* Temporary value for calculated current or voltage */

	if (htim->Instance == TIM1) { /* Check if the interrupt is from TIM1 */
		/* Read ADC values for both channels */
		for (channel_index = 0; channel_index < 2; ++channel_index) { /* Iterate over two ADC channels */
			HAL_ADC_Start(&hadc1); /* Start ADC conversion */
			HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY); /* Wait for conversion to complete */
			adc_val[channel_index] = HAL_ADC_GetValue(&hadc1); /* Store ADC value */
		}

		/* Process current sampling */
		if (is_sampling_current) { /* Check if current sampling is active */
			processed_value = CalculateCurrentAmps(adc_val[0]); /* Calculate current from ADC value */
			current_rms = CalculateCurrentRMS(processed_value); /* Update RMS for current */
			if (++sample_index_I >= 1000) { /* Increment and check sample index */
				sample_index_I = 0; /* Reset index after 1000 samples */
				initial_samples_collected_I = 1; /* Mark initial samples as collected */
			}
		}

		/* Process voltage sampling */
		if (is_sampling_volt) { /* Check if voltage sampling is active */
			processed_value = CalculateVoltageVolts(adc_val[1]); /* Calculate voltage from ADC value */
			voltage_rms = CalculateVoltageRMS(processed_value); /* Update RMS for voltage */
			if (++sample_index_V >= 1000) { /* Increment and check sample index */
				sample_index_V = 0; /* Reset index after 1000 samples */
				initial_samples_collected_V = 1; /* Mark initial samples as collected */
			}
		}
	}

	HAL_ADC_Stop(&hadc1); /* Stop ADC */
}

/***************************************************************************/
/*
 * @brief: Calculates the voltage based on ADC reading.
 * @param adc_value: Raw ADC value to calculate voltage.
 * @retval: Calculated voltage (in volts), or -1.0f if error.
 */
static float CalculateVoltageVolts(uint16_t adc_value) {
	float adc_voltage = 0.0f; /* Voltage converted from ADC value */
	float resistor_voltage = 0.0f; /* Voltage after bias subtraction */
	float rounded_voltage = 0.0f; /* Rounded voltage value */
	float gain_adjusted_voltage = 0.0f; /* Voltage after gain adjustment */
	float input_voltage = 0.0f; /* Final input voltage */

	/* Convert ADC value to voltage (Vadc = (ADC_value / 4095) * 3.0) */
	adc_voltage = (adc_value * VREF) / ADC_MAX; /* Calculate ADC voltage using reference and max ADC value */

	/* Calculate resistor_voltage = (Vadc - Vbias) */
	resistor_voltage = (adc_voltage - VOLTAGE_VBIAS); /* Subtract bias voltage from ADC voltage */
	rounded_voltage = roundf(resistor_voltage * 100) / 100; /* Round to two decimal places */

	/* Calculate gain_adjusted_voltage = rounded_voltage / VOLTAGE_GAIN */
	gain_adjusted_voltage = rounded_voltage / VOLTAGE_GAIN; /* Divide by voltage gain */

	/* Calculate Vin = gain_adjusted_voltage * (sumR / Rv) */
	input_voltage = (gain_adjusted_voltage * (VOLTAGE_SUMR / VOLTAGE_RV)); /* Compute final voltage using resistor ratio */

	/* Return the calculated voltage */
	return input_voltage; /* Return the final voltage value */
}

/***************************************************************************/
/*
 * @brief: Calculates the current based on ADC reading.
 soluciones * @param adc_value: Raw ADC value to calculate current.
 * @retval: Calculated current (in amps), or -1.0f if error.
 */
static float CalculateCurrentAmps(uint16_t adc_value) {
	float adc_voltage = 0.0f; /* Voltage converted from ADC value */
	float resistor_voltage = 0.0f; /* Voltage after bias subtraction */
	float rounded_voltage = 0.0f; /* Rounded voltage value */
	float gain_adjusted_voltage = 0.0f; /* Voltage after gain adjustment */
	float input_current = 0.0f; /* Final input current */

	/* Convert ADC value to voltage (Vadc = (ADC_value / 4095) * 3.0) */
	adc_voltage = (adc_value * VREF) / ADC_MAX; /* Calculate ADC voltage using reference and max ADC value */

	/* Calculate resistor_voltage = (Vadc - CURRENT_VBIAS) */
	resistor_voltage = (adc_voltage - CURRENT_VBIAS); /* Subtract bias voltage from ADC voltage */
	rounded_voltage = roundf(resistor_voltage * 100) / 100; /* Round to two decimal places */

	/* Calculate gain_adjusted_voltage = rounded_voltage / CURRENT_GAIN */
	gain_adjusted_voltage = rounded_voltage / CURRENT_GAIN; /* Divide by current gain */

	/* Calculate Ir = gain_adjusted_voltage * (Te / R), where Te/R is pre-calculated as current_te_by_r */
	input_current = gain_adjusted_voltage * current_te_by_r; /* Compute final current using Te/R ratio */

	/* Return the calculated current */
	return input_current; /* Return the final current value */
}

/***************************************************************************/
/*
 * @brief: Calculates the RMS value of current samples.
 * @param new_current: Latest current value.
 * @retval: Calculated RMS value, or -1.0f if not enough samples.
 */
static float CalculateCurrentRMS(float new_current) {
	uint16_t buffer_index = sample_index_I % SAMPLE_COUNT; /* Calculate buffer index for current */
	float previous_current = 0.0f; /* Previous squared current value */
	float rms_value = 0.0f; /* Calculated RMS value */

	if (sample_index_I < SAMPLE_COUNT) { /* Check if fewer than 200 samples are collected */
		if (sample_index_I == SAMPLE_COUNT - 1
				&& initial_samples_collected_I == 0) { /* Check if 200th sample is reached */
			rms_sum_I += new_current * new_current; /* Add squared current to sum */

			/* Calculate RMS when 200 samples are collected */
			rms_value = sqrtf(rms_sum_I / SAMPLE_COUNT); /* Compute RMS using square root of average */
			ACC.cur = rms_value; /* Store RMS value in AC structure */
			return rms_value; /* Return calculated RMS */
		}
	} else if (initial_samples_collected_I) { /* Check if initial samples are collected */
		previous_current = rms_buffer_I[buffer_index]; /* Get previous squared value from buffer */
		rms_buffer_I[buffer_index] = new_current * new_current; /* Store new squared current value */
		rms_sum_I += rms_buffer_I[buffer_index]; /* Add new squared value to sum */
		rms_sum_I -= previous_current; /* Subtract previous squared value from sum */

		/* Calculate RMS */
		rms_value = sqrtf(rms_sum_I / SAMPLE_COUNT); /* Compute RMS using square root of average */
		ACC.cur = rms_value; /* Store RMS value in AC structure */
		return rms_value; /* Return calculated RMS */
	}

	return -1.0f; /* Return -1.0f until 200 samples are collected */
}

/***************************************************************************/
/*
 * @brief: Calculates the RMS value of voltage samples.
 * @param new_voltage: Latest voltage value.
 * @retval: Calculated RMS value, or -1.0f if not enough samples.
 */
static float CalculateVoltageRMS(float new_voltage) {
	uint16_t buffer_index = sample_index_V % SAMPLE_COUNT; /* Calculate buffer index for voltage */
	float previous_voltage = 0.0f; /* Previous squared voltage value */
	float rms_value = 0.0f; /* Calculated RMS value */

	if (sample_index_V < SAMPLE_COUNT) { /* Check if fewer than 200 samples are collected */
		if (sample_index_V == SAMPLE_COUNT - 1
				&& initial_samples_collected_V == 0) { /* Check if 200th sample is reached */
			rms_sum_V += new_voltage * new_voltage; /* Add squared voltage to sum */

			/* Calculate RMS when 200 samples are collected */
			rms_value = sqrtf(rms_sum_V / SAMPLE_COUNT); /* Compute RMS using square root of average */
			ACC.volt = rms_value; /* Store RMS value in AC structure */
			return rms_value; /* Return calculated RMS */
		}
	} else if (initial_samples_collected_V) { /* Check if initial samples are collected */
		previous_voltage = rms_buffer_V[buffer_index]; /* Get previous squared value from buffer */
		rms_buffer_V[buffer_index] = new_voltage * new_voltage; /* Store new squared voltage value */
		rms_sum_V += rms_buffer_V[buffer_index]; /* Add new squared value to sum */
		rms_sum_V -= previous_voltage; /* Subtract previous squared value from sum */

		/* Calculate RMS */
		rms_value = sqrtf(rms_sum_V / SAMPLE_COUNT); /* Compute RMS using square root of average */
		ACC.volt = rms_value; /* Store RMS value in AC structure */
		return rms_value; /* Return calculated RMS */
	}

	return -1.0f; /* Return -1.0f until 200 samples are collected */
}

/***************************************************************************/
/***************************** General Functions ***************************/
/***************************************************************************/
/*
 * @brief: Initiates sampling of voltage using ADC channel 16.
 * @param volt: Pointer to store the calculated voltage (in volts).
 * @retval: Module status indicating success or error.
 */
Module_Status SampleVoltage(float *volt) {
	Module_Status status = H2AR3_OK; /* Initialize status to success */

	if (!voltage_sampling_started) { /* Check if voltage sampling has not started */
		/* Set sampling mode to voltage */
		is_sampling_volt = 1; /* Enable voltage sampling flag */

		/* Reset RMS variables */
		sample_index_V = 0; /* Reset voltage sample index */
		rms_sum_V = 0.0f; /* Clear sum of squared values for voltage */
		initial_samples_collected_V = 0; /* Reset flag for initial samples */
		voltage_rms = 0.0f; /* Reset voltage RMS value */

		/* Start the timer only once */
		if ((htim1.Instance->CR1 & TIM_CR1_CEN) == 0) { /* Check if timer is not running */
			HAL_TIM_Base_Start_IT(&htim1); /* Start timer with interrupt */
		}

		voltage_sampling_started = 1; /* Mark voltage sampling as started */
	}

	/* Return latest RMS value */
	*volt = ACC.volt; /* Store the latest voltage RMS value in the provided pointer */
	return status; /* Return success status */
}

/***************************************************************************/
/*
 * @brief: Initiates sampling of current using ADC channel 6.
 * @param curr: Pointer to store the calculated current (in amps).
 * @param monitor_type: Enum defining the AC monitor type (CR8450_1000 or CR8401_1000).
 * @retval: Module status indicating success or error.
 */
Module_Status SampleCurrent(float *curr, AC_Monitor_Status monitor_type) {
	Module_Status status = H2AR3_OK; /* Initialize status to success */
	ADC_ChannelConfTypeDef channel_config = { 0 }; /* Initialize ADC channel configuration structure */
	float transformer_te = 0.0f; /* Variable to store transformer Te value */

	if (!current_sampling_started) { /* Check if current sampling has not started */
		/* Select Te value based on monitor type */
		switch (monitor_type) { /* Evaluate the monitor type */
		case CR8450_1000: /* CR8450-1000 transformer */
			transformer_te = TE_CR8450_1000; /* Set Te for CR8450-1000 */
			break;
		case CR8401_1000: /* CR8401-1000 transformer */
			transformer_te = TE_CR8401_1000; /* Set Te for CR8401-1000 */
			break;
		default: /* Invalid monitor type */
			return H2AR3_ERROR; /* Return error status for invalid type */
		}

		/* Calculate Te/R for current */
		current_te_by_r = transformer_te / CURRENT_R; /* Compute Te/R ratio for current calculation */

		/* Initialize sampling variables */
		is_sampling_current = 1; /* Enable current sampling flag */
		sample_index_I = 0; /* Reset current sample index */
		rms_sum_I = 0.0f; /* Clear sum of squared values for current */
		initial_samples_collected_I = 0; /* Reset flag for initial samples */
		current_rms = 0.0f; /* Reset current RMS value */

		/* Start the timer only once */
		if ((htim1.Instance->CR1 & TIM_CR1_CEN) == 0) { /* Check if timer is not running */
			HAL_TIM_Base_Start_IT(&htim1); /* Start timer with interrupt */
		}

		current_sampling_started = 1; /* Mark current sampling as started */
	}

	/* Return latest RMS value */
	*curr = ACC.cur; /* Store the latest current RMS value in the provided pointer */
	return status; /* Return success status */
}


/***************************************************************************/
/********************************* Commands ********************************/
/***************************************************************************/

/***************************************************************************/
/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
