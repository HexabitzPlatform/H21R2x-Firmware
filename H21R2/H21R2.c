/*
 BitzOS (BOS) V0.4.0 - Copyright (C) 2017-2025 Hexabitz
 All rights reserved

 File Name     : H21R2.c
 Description   : Source code for module H21R2.
 	 	 	 	 (Description_of_module)

(Description of Special module peripheral configuration):
>>
>>
>>
 */

/* Includes ****************************************************************/
#include "BOS.h"
#include "H21R2_inputs.h"

/* Exported Typedef ******************************************************/
/* Define UART variables */
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart6;

/* Private Variables *******************************************************/
uint8_t FullData [ESP_PACKET_LENGTH] = {0};
uint8_t wifiBuffer [WIFI_BUFF_SIZE] = {0};
uint16_t Length1 = 0;
uint16_t Length2 = 0;
uint64_t Time = 0;
uint64_t Timeout = 0;

/* Module Parameters */
ModuleParam_t ModuleParam[NUM_MODULE_PARAMS] ={0};

/* Create CLI commands *****************************************************/
portBASE_TYPE CLI_ESPResetModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
portBASE_TYPE CLI_ESPBootModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
portBASE_TYPE CLI_ESPBleServerModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
portBASE_TYPE CLI_ESPBleClientModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
portBASE_TYPE CLI_ESPWifiAccessPiontModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
portBASE_TYPE CLI_ESPWifiStationModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
portBASE_TYPE CLI_ESPWriteToBleServerModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
portBASE_TYPE CLI_ESPWriteToBleClientModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
portBASE_TYPE CLI_ESPReadFromBleServerModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
portBASE_TYPE CLI_ESPReadFromBleClientModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

/* CLI command structure ***************************************************/
/* CLI command structure : ESP Reset Mode */
const CLI_Command_Definition_t CLI_ESPResetModeCommandDefinition = {
	( const int8_t * ) "espreset", /* The command string to type. */
	( const int8_t * ) "espreset:\r\n To reset esp \r\n\r\n",
	CLI_ESPResetModeCommand, /* The function to run. */
	0 /* zero parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : ESP Boot Mode */
const CLI_Command_Definition_t CLI_ESPBootModeCommandDefinition = {
	( const int8_t * ) "espboot", /* The command string to type. */
	( const int8_t * ) "espboot:\r\n Booting Esp \r\n\r\n",
	CLI_ESPBootModeCommand, /* The function to run. */
	0 /* zero parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : To set ESP in Ble CLIENT Mode */
const CLI_Command_Definition_t CLI_ESPBleClientModeCommandDefinition = {
	( const int8_t * ) "bleclient", /* The command string to type. */
	( const int8_t * ) "bleclient:\r\n To Set ESP-BLE in Client Mode \r\n\r\n",
	CLI_ESPBleClientModeCommand, /* The function to run. */
	2 /* Two parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : To set ESP in Ble Server Mode */
const CLI_Command_Definition_t CLI_ESPBleServerModeCommandDefinition = {
	( const int8_t * ) "bleserver", /* The command string to type. */
	( const int8_t * ) "bleserver:\r\n To Set ESP-BLE in Server Mode  \r\n\r\n",
	CLI_ESPBleServerModeCommand, /* The function to run. */
	1 /* One parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : To set ESP in Wifi Access Piont Mode */
const CLI_Command_Definition_t CLI_ESPWifiAccessPiontCommandDefinition = {
	( const int8_t * ) "wifiaccesspoint", /* The command string to type. */
	( const int8_t * ) "wifiaccesspoint:\r\n To Set ESP-Wifi in Access Piont Mode \r\n\r\n",
	CLI_ESPWifiAccessPiontModeCommand, /* The function to run. */
	2 /* Two parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : To set ESP in Wifi Station Mode */
const CLI_Command_Definition_t CLI_ESPWifiStationModeCommandDefinition = {
	( const int8_t * ) "wifistation", /* The command string to type. */
	( const int8_t * ) "wifistation:\r\n To Set ESP-Wifi in Station Mode \r\n\r\n",
	CLI_ESPWifiStationModeCommand, /* The function to run. */
	2 /* Two parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : To Read From Ble in Server Mode  */
const CLI_Command_Definition_t CLI_ESPReadFromBleServerModeCommandDefinition = {
	( const int8_t * ) "readfromserver", /* The command string to type. */
	( const int8_t * ) "readfromserver:\r\n To Read From BLE in Server Mode \r\n\r\n",
	CLI_ESPReadFromBleServerModeCommand, /* The function to run. */
	0 /* zero parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : To Write From Ble in Server Mode  */
const CLI_Command_Definition_t CLI_ESPWriteFromBleServerModeCommandDefinition = {
	( const int8_t * ) "writetoserver", /* The command string to type. */
	( const int8_t * ) "writetoserver:\r\n To Write From BLE in Server Mode \r\n\r\n",
	CLI_ESPWriteToBleServerModeCommand, /* The function to run. */
	1 /* One parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : To Read From Ble in Client Mode  */
const CLI_Command_Definition_t CLI_ESPReadFromBleClientModeCommandDefinition = {
	( const int8_t * ) "readfromclient", /* The command string to type. */
	( const int8_t * ) "readfromclient:\r\n To Read From BLE in Client Mode \r\n\r\n",
	CLI_ESPReadFromBleClientModeCommand, /* The function to run. */
	0 /* zero parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : To Write From Ble in Client Mode  */
const CLI_Command_Definition_t CLI_ESPWriteFromBleclientModeCommandDefinition = {
	( const int8_t * ) "writetoclient", /* The command string to type. */
	( const int8_t * ) "writetoclient:\r\n To Write From BLE in Client Mode \r\n\r\n",
	CLI_ESPWriteToBleClientModeCommand, /* The function to run. */
	1 /* One parameters are expected. */
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
/* H21R2 module initialization */
void Module_Peripheral_Init(void) {

	/* Array ports */
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_USART4_UART_Init();
	MX_USART5_UART_Init();
	MX_USART6_UART_Init();

	/* Circulating DMA Channels ON All Module */
	for (int i = 1; i <= NUM_OF_PORTS; i++) {
		if (GetUart(i) == &huart1) {
			dmaIndex [i - 1] = &(DMA1_Channel1->CNDTR);
		} else if (GetUart(i) == &huart2) {
			dmaIndex [i - 1] = &(DMA1_Channel2->CNDTR);
		} else if (GetUart(i) == &huart3) {
			dmaIndex [i - 1] = &(DMA1_Channel3->CNDTR);
		} else if (GetUart(i) == &huart4) {
			dmaIndex [i - 1] = &(DMA1_Channel4->CNDTR);
		} else if (GetUart(i) == &huart5) {
			dmaIndex [i - 1] = &(DMA1_Channel5->CNDTR);
		} else if (GetUart(i) == &huart6) {
			dmaIndex [i - 1] = &(DMA1_Channel6->CNDTR);
		}
	}
}

/***************************************************************************/
/* H21R2 message processing task */
Module_Status Module_MessagingTask(uint16_t code, uint8_t port, uint8_t src, uint8_t dst, uint8_t shift) {
	Module_Status result = H21R2_OK;

	char ServerName2 [Length2];
	char ServerName [Length1];
	char ClientName [Length1];
	char Accessspoint [Length1];
	char Station [Length1];
	char Password [Length2];
	char Data2 [20];
	char Data [Length1];
	uint8_t SendData [Length1 + 2];
	uint16_t module;

	switch (code) {
	case CODE_H21R2_ESP_BOOT:
		ESP_Boot();
		break;

	case CODE_H21R2_ESP_RESET:
		ESP_Reset();
		break;

	case CODE_H21R2_ESP_SERVER:
		Length1 = (uint16_t) cMessage [port - 1] [shift];
		memcpy(ServerName, &cMessage [port - 1] [1 + shift], Length1);
		BLE_ServerMode(ServerName);
		break;

	case CODE_H21R2_ESP_CLIENT:
		Length1 = (uint16_t) cMessage [port - 1] [shift];
		Length2 = (uint16_t) cMessage [port - 1] [1 + shift];
		memcpy(ClientName, &cMessage [port - 1] [2 + shift], Length1);
		memcpy(ServerName2, &cMessage [port - 1] [Length1 + 2 + shift], Length2);
		BLE_ClientMode(ClientName, ServerName2);
		break;

	case CODE_H21R2_ESP_ACCESS_POINT:
		Length1 = (uint16_t) cMessage [port - 1] [shift];
		Length2 = (uint16_t) cMessage [port - 1] [1 + shift];
		memcpy(Accessspoint, &cMessage [port - 1] [2 + shift], Length1);
		memcpy(Password, &cMessage [port - 1] [Length1 + 2 + shift], Length2);
		WIFI_AccessPoint(Accessspoint, Password);
		break;

	case CODE_H21R2_ESP_STATION:
		Length1 = (uint16_t) cMessage [port - 1] [shift];
		Length2 = (uint16_t) cMessage [port - 1] [1 + shift];
		memcpy(Station, &cMessage [port - 1] [2 + shift], Length1);
		memcpy(Password, &cMessage [port - 1] [Length1 + 2 + shift], Length2);
		WIFI_AccessPoint(Station, Password);
		break;

	case CODE_H21R2_ESP_WRITE_TO_SERVER:
		Length1 = (uint16_t) cMessage [port - 1] [shift];
		memcpy(Data, &cMessage [port - 1] [1 + shift], Length1);
		SendData [0] = WRITE_TO_SERVER_MODE;
		SendData [1] = Length1;
		memcpy(&SendData [2], Data, Length1);
		if (1 == FullData [0]) {
			HAL_UART_Transmit(ESP32_UART_HANDEL, SendData, Length1 + 2, 0xff);
			FullData [0] = 0;
		}
		break;

	case CODE_H21R2_ESP_WRITE_TO_CLIENT:
		Length1 = (uint16_t) cMessage [port - 1] [shift];
		memcpy(Data, &cMessage [port - 1] [1 + shift], Length1);
		SendData [0] = WRITE_TO_CLIENT_MODE;
		SendData [1] = Length1;
		memcpy(&SendData [2], Data, Length1);
		if (1 == FullData [0]) {
			HAL_UART_Transmit(ESP32_UART_HANDEL, SendData, Length1 + 2, 0xff);
			FullData [0] = 0;
		}
		break;

	case CODE_H21R2_ESP_READ_FROM_SERVER:
		module = (uint16_t) cMessage [port - 1] [shift];
		BLE_Read(Data2, SERVER);
		memcpy(MessageParams, Data2, 20);
		SendMessageToModule(module, CODE_PORT_FORWARD, 20);
		break;

	case CODE_H21R2_ESP_READ_FROM_CLIENT:
		module = (uint16_t) cMessage [port - 1] [shift];
		BLE_Read(Data2, CLIENT);
		memcpy(MessageParams, Data2, 20);
		SendMessageToModule(module, CODE_PORT_FORWARD, 20);
		break;

	default:
		result = H21R2_ERR_UNKNOWNMESSAGE;
		break;
	}

	return result;
}

/***************************************************************************/
/* Get the port for a given UART */
uint8_t GetPort(UART_HandleTypeDef *huart) {

	if (huart->Instance == USART4)
		return P1;
	else if (huart->Instance == USART2)
		return P2;
	else if (huart->Instance == USART6)
		return P3;
	else if (huart->Instance == USART1)
		return P4;
	else if (huart->Instance == USART5)
		return P5;

	return 0;
}

/***************************************************************************/
/* Register this module CLI Commands */
void RegisterModuleCLICommands(void){
	  FreeRTOS_CLIRegisterCommand(&CLI_ESPResetModeCommandDefinition);
	  FreeRTOS_CLIRegisterCommand(&CLI_ESPBootModeCommandDefinition);
	  FreeRTOS_CLIRegisterCommand(&CLI_ESPBleServerModeCommandDefinition);
	  FreeRTOS_CLIRegisterCommand(&CLI_ESPBleClientModeCommandDefinition);
	  FreeRTOS_CLIRegisterCommand(&CLI_ESPWifiAccessPiontCommandDefinition);
	  FreeRTOS_CLIRegisterCommand(&CLI_ESPWifiStationModeCommandDefinition);
	  FreeRTOS_CLIRegisterCommand(&CLI_ESPReadFromBleServerModeCommandDefinition);
	  FreeRTOS_CLIRegisterCommand(&CLI_ESPWriteFromBleServerModeCommandDefinition);
	  FreeRTOS_CLIRegisterCommand(&CLI_ESPReadFromBleClientModeCommandDefinition);
	  FreeRTOS_CLIRegisterCommand(&CLI_ESPWriteFromBleclientModeCommandDefinition);
}

/***************************************************************************/
/* This functions is useful only for input (sensors) modules.
 * Samples a module parameter value based on parameter index.
 * paramIndex: Index of the parameter (1-based index).
 * value: Pointer to store the sampled float value.
 */
Module_Status GetModuleParameter(uint8_t paramIndex,float *value){
	Module_Status status =BOS_OK;

	switch(paramIndex){

		/* Invalid parameter index */
		default:
			status =BOS_ERR_WrongParam;
			break;
	}

	return status;
}

/***************************************************************************/
/***************************** General Functions ***************************/
/***************************************************************************/
/* Reset ESP32 MCU */
Module_Status ESP_Reset(void) {
	Module_Status Status = H21R2_OK;

	HAL_GPIO_WritePin(ESP32_RST_PORT, ESP32_RST_PIN, GPIO_PIN_RESET);
	HAL_Delay(5);
	HAL_GPIO_WritePin(ESP32_RST_PORT, ESP32_RST_PIN, GPIO_PIN_SET);
	HAL_Delay(1000);

	return Status;
}

/***************************************************************************/
/* Booting ESP32 MCU */
Module_Status ESP_Boot(void) {
	Module_Status Status = H21R2_OK;

	HAL_GPIO_WritePin(ESP32_BOOT_PORT, ESP32_BOOT_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ESP32_RST_PORT, ESP32_RST_PIN, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(ESP32_RST_PORT, ESP32_RST_PIN, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(ESP32_BOOT_PORT, ESP32_BOOT_PIN, GPIO_PIN_SET);

	return Status;
}

/***************************************************************************/
/* Initialize BLE CLIENT mode
 * ClientName: pointer to a ClientName
 * ServerName: pointer to a ServerName
 */
Module_Status BLE_ClientMode(char *ClientName, char *ServerName) {
	Module_Status Status = H21R2_OK;
	int LenClientName, LenServerName;

	UARTInitESP32();

	LenClientName = strlen(ClientName);
	LenServerName = strlen(ServerName);
	uint8_t Data [LenClientName + LenServerName + 3];

	Data [0] = CLIENT_MODE;
	Data [1] = LenClientName;
	Data [2] = LenServerName;
	memcpy(&Data [3], ClientName, LenClientName);
	memcpy(&Data [LenClientName + 3], ServerName, LenServerName);
	HAL_UART_Transmit(ESP32_UART_HANDEL, Data, LenClientName + LenServerName + 3, 0xff);
	HAL_UART_Receive_DMA(ESP32_UART_HANDEL, FullData, ESP_PACKET_LENGTH);

	return Status;
}

/***************************************************************************/
Module_Status BLE_ClientModeCLI(char *ClientName, char *ServerName, uint8_t LenClientName, uint8_t LenServerName) {
	Module_Status Status = H21R2_OK;
	uint8_t Data [LenClientName + LenServerName + 3];

	UARTInitESP32();

	Data [0] = CLIENT_MODE;
	Data [1] = LenClientName;
	Data [2] = LenServerName;
	memcpy(&Data [3], ClientName, LenClientName);
	memcpy(&Data [LenClientName + 3], ServerName, LenServerName);
	HAL_UART_Transmit(ESP32_UART_HANDEL, Data, LenClientName + LenServerName + 3, 0xff);
	HAL_UART_Receive_DMA(ESP32_UART_HANDEL, FullData, ESP_PACKET_LENGTH);

	return Status;
}

/***************************************************************************/
/* Initialize BLE Server mode
 * ServerName: pointer to a ServerName
 */
Module_Status BLE_ServerMode(char *ServerName) {
	Module_Status Status = H21R2_OK;

	int LenServerName;
	UARTInitESP32();

	LenServerName = strlen(ServerName);
	uint8_t Data [LenServerName + 2];

	Data [0] = SERVER_MODE;
	Data [1] = LenServerName;
	memcpy(&Data [2], ServerName, LenServerName);
	HAL_UART_Transmit(ESP32_UART_HANDEL, Data, LenServerName + 2, 0xff);
	HAL_UART_Receive_DMA(ESP32_UART_HANDEL, FullData, ESP_PACKET_LENGTH);

	return Status;
}

/***************************************************************************/
/* To read from BLE in SERVER / client mode
 * Data: pointer to a Data
 * function: BLE_MODE
 */
Module_Status BLE_Read(char *Data, BLE_MODE function) {
	Module_Status Status = H21R2_OK;
	uint8_t SendData;

	switch (function) {
	case SERVER:
		if ('H' == FullData [0] && 'Z' == FullData [1])
			memcpy(Data, &FullData [2], BLE_BUFF_SIZE);
		break;

	case CLIENT:
		SendData = WRITE_FROM_CLIENT_MODE;
		if (FullData [0] == 1) {
			HAL_UART_Transmit(ESP32_UART_HANDEL, &SendData, 1, 0xff);
			FullData [0] = 0;
		}
		HAL_Delay(10);
		if ('H' == FullData [0] && 'Z' == FullData [1]) {
			do {
				memcpy(Data, &FullData [2], BLE_BUFF_SIZE);
			} while (FullData [0] != 1);
			HAL_Delay(100);
		}
		break;
	}

	return Status;
}

/***************************************************************************/
/* To read from WIFI Socket mode
 * Data: pointer to a Data
 */
Module_Status WIFI_SocketRead(char *Data) {
	Module_Status Status = H21R2_ERROR;
	Status = H21R2_OK;

	if ('H' == wifiBuffer [0] && 'Z' == wifiBuffer [1])
		memcpy(Data, &wifiBuffer [2], 126);

	return Status;
}

/***************************************************************************/
/* To Write from BLE in SERVER mode   To Write from BLE in clien mode
 * Data: pointer to a Data
 * function: BLE_MODE
 * Size: data sent size
 */
Module_Status BLE_Write(char *Data, BLE_MODE function, uint16_t Size) {
	Module_Status Status = H21R2_ERROR;
	int LenData;
	LenData = Size;
	uint8_t SendData [LenData + 2];

	switch (function) {
	case SERVER:
		Status = H21R2_OK;
		SendData [0] = WRITE_TO_SERVER_MODE;
		SendData [1] = LenData;
		memcpy(&SendData [2], Data, LenData);
		if (FullData [0] == 1) {
			HAL_UART_Transmit(ESP32_UART_HANDEL, SendData, LenData + 2, 0xff);
			FullData [0] = 0;
		}
		break;

	case CLIENT:
		Status = H21R2_OK;
		SendData [0] = WRITE_TO_CLIENT_MODE;
		SendData [1] = LenData;
		memcpy(&SendData [2], Data, LenData);
		if (FullData [0] == 1) {
			HAL_UART_Transmit(ESP32_UART_HANDEL, SendData, LenData + 2, 0xff);
			FullData [0] = 0;
			do {

			} while (1 != FullData [0]);
		}
		HAL_Delay(100);
		break;
	}

	return Status;
}

/***************************************************************************/
/* To write to  WIFI Socket mode
 * Data: pointer to a Data size
 * Size: data sent size
 */
Module_Status WIFI_SocketWrite(char *Data, uint16_t Size) {
	Module_Status Status = H21R2_OK;
	int LenData;
	LenData = Size;
	uint8_t SendData [LenData + 2];

	SendData [0] = WRITE_SOCKET_MODE;
	SendData [1] = LenData;
	memcpy(&SendData [2], Data, LenData);
	if (wifiBuffer[0] == 1) {
		HAL_UART_Transmit(ESP32_UART_HANDEL, SendData, LenData + 2, 0xff);
		wifiBuffer [0] = 0;
	}

	return Status;
}

/***************************************************************************/
/* Initialize WIFI access point  mode
 * Ssid: pointer to a  Accesspoint name
 * Password: pointer to a  Password
 */
Module_Status WIFI_AccessPoint(char *Ssid, char *Password) {
	Module_Status Status = H21R2_OK;
	int LenSsid, LenPassword;

	UARTInitESP32();

	LenSsid = strlen(Ssid);
	LenPassword = strlen(Password);
	uint8_t Data [LenSsid + LenPassword + 3];

	Data [0] = WIFI_ACCESS_POINT_MODE;
	Data [1] = LenSsid;
	Data [2] = LenPassword;
	memcpy(&Data [3], Ssid, LenSsid);
	memcpy(&Data [LenSsid + 3], Password, LenPassword);
	HAL_UART_Transmit(ESP32_UART_HANDEL, Data, LenSsid + LenPassword + 3, 0xff);

	return Status;
}

/***************************************************************************/
/* Initialize WIFI Socket mode
 * Ssid: pointer to a  Accesspoint name
 * Password: pointer to a  Password
 */
Module_Status WIFI_Socket(char *Ssid, char *Password) {
	Module_Status Status = H21R2_OK;
	int LenSsid, LenPassword;

	UARTInitESP32();

	LenSsid = strlen(Ssid);
	LenPassword = strlen(Password);
	uint8_t Data [LenSsid + LenPassword + 3];
	Data [0] = WIFI_SOCKET_MODE;
	Data [1] = LenSsid;
	Data [2] = LenPassword;
	memcpy(&Data [3], Ssid, LenSsid);
	memcpy(&Data [LenSsid + 3], Password, LenPassword);
	HAL_UART_Transmit(ESP32_UART_HANDEL, Data, LenSsid + LenPassword + 3, 0xff);
	HAL_UART_Receive_DMA(ESP32_UART_HANDEL, wifiBuffer, WIFI_BUFF_SIZE);

	return Status;
}

/***************************************************************************/
Module_Status WIFI_AccessPointCLI(char *Ssid, char *Password, uint8_t lenSsid, uint8_t lenPassword) {
	Module_Status Status = H21R2_OK;
	uint8_t Data [lenSsid + lenPassword + 3];

	ESP_Boot();
	ESP_Reset();

	UARTInitESP32();

	Data [0] = WIFI_ACCESS_POINT_MODE;
	Data [1] = lenSsid;
	Data [2] = lenPassword;
	memcpy(&Data [3], Ssid, lenSsid);
	memcpy(&Data [lenSsid + 3], Password, lenPassword);
	HAL_Delay(1000);
	HAL_UART_Transmit(ESP32_UART_HANDEL, Data, lenSsid + lenPassword + 3, 0xff);

	return Status;
}

/***************************************************************************/
/* Initialize WIFI Station mode
 * Ssid: pointer to a  Station  name
 * Password: pointer to a  Password
 */
Module_Status WIFI_Station(char *Ssid, char *Password) {
	Module_Status Status = H21R2_OK;
	int LenSsid, LenPassword;

	UARTInitESP32();

	LenSsid = strlen(Ssid);
	LenPassword = strlen(Password);
	uint8_t Data [LenSsid + LenPassword + 3];
	Data [0] = WIFI_STATION_MODE;
	Data [1] = LenSsid;
	Data [2] = LenPassword;
	memcpy(&Data [3], Ssid, LenSsid);
	memcpy(&Data [LenSsid + 3], Password, LenPassword);
	HAL_UART_Transmit(ESP32_UART_HANDEL, Data, LenSsid + LenPassword + 3, 0xff);

	return Status;
}

/***************************************************************************/
Module_Status WIFI_StationCLI(char *Ssid, char *Password, uint8_t lenSsid, uint8_t lenPassword) {
	Module_Status Status = H21R2_OK;
	int LenSsid, LenPassword;

	ESP_Boot();
	ESP_Reset();

	UARTInitESP32();

	LenSsid = lenSsid;
	LenPassword = lenPassword;
	uint8_t Data [LenSsid + LenPassword + 3];
	Data [0] = WIFI_STATION_MODE;
	Data [1] = LenSsid;
	Data [2] = LenPassword;
	memcpy(&Data [3], Ssid, LenSsid);
	memcpy(&Data [LenSsid + 3], Password, LenPassword);
	HAL_UART_Transmit(ESP32_UART_HANDEL, Data, LenSsid + LenPassword + 3, 0xff);

	return Status;
}

/***************************************************************************/
/********************************* Commands ********************************/
/***************************************************************************/
portBASE_TYPE CLI_ESPResetModeCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString) {
	Module_Status status = H21R2_OK;
	static const int8_t *pcOKMessage = (int8_t*) "ESP Reset!\n\r";
	static const int8_t *pcErrorsMessage = (int8_t*) "Error Params!\n\r";

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	status = ESP_Reset();

	if (status == H21R2_OK)
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage);
	else if (status == H21R2_ERROR)
		strcpy((char*) pcWriteBuffer, (char*) pcErrorsMessage);

	return pdFALSE;
}

/***************************************************************************/
portBASE_TYPE CLI_ESPBootModeCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString) {
	Module_Status status = H21R2_OK;
	static const int8_t *pcOKMessage = (int8_t*) "ESP Booting!\n\r";
	static const int8_t *pcErrorsMessage = (int8_t*) "Error Params!\n\r";

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	status = ESP_Boot();

	if (status == H21R2_OK)
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage);
	else if (status == H21R2_ERROR)
		strcpy((char*) pcWriteBuffer, (char*) pcErrorsMessage);

	return pdFALSE;
}

/***************************************************************************/
portBASE_TYPE CLI_ESPBleClientModeCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString) {
	Module_Status status = H21R2_OK;

	static const int8_t *pcOKMessage = (int8_t*) "ESP Client Mode On\n\r";
	static const int8_t *pcErrorsMessage = (int8_t*) "Error Params!\n\r";

	static int8_t *pcParameterString1, *pcParameterString2;
	portBASE_TYPE xParameterStringLength1 = 0, xParameterStringLength2 = 0;

	(void) xWriteBufferLen;

	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength1);
	pcParameterString2 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameterStringLength2);

	status = BLE_ClientModeCLI(pcParameterString1, pcParameterString2, xParameterStringLength1,
			xParameterStringLength2);

	if (status == H21R2_OK)
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage);
	else if (status == H21R2_ERROR)
		strcpy((char*) pcWriteBuffer, (char*) pcErrorsMessage);

	return pdFALSE;
}

/***************************************************************************/
portBASE_TYPE CLI_ESPBleServerModeCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString) {
	Module_Status status = H21R2_OK;

	static const int8_t *pcOKMessage = (int8_t*) "ESP Server Mode On\n\r";
	static const int8_t *pcErrorsMessage = (int8_t*) "Error Params!\n\r";

	static int8_t *pcParameterString1;
	portBASE_TYPE xParameterStringLength1 = 0;

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength1);

	status = BLE_ServerMode(pcParameterString1);

	if (status == H21R2_OK)
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage);
	else if (status == H21R2_ERROR)
		strcpy((char*) pcWriteBuffer, (char*) pcErrorsMessage);

	return pdFALSE;
}

/***************************************************************************/
portBASE_TYPE CLI_ESPWifiAccessPiontModeCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen,
		const int8_t *pcCommandString) {
	Module_Status status = H21R2_OK;

	static const int8_t *pcOKMessage = (int8_t*) "ESP Wifi Access Point  Mode On\n\r";
	static const int8_t *pcErrorsMessage = (int8_t*) "Error Params!\n\r";

	static int8_t *pcParameterString1, *pcParameterString2;
	portBASE_TYPE xParameterStringLength1 = 0, xParameterStringLength2 = 0;

	(void) xWriteBufferLen;

	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength1);
	pcParameterString2 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameterStringLength2);

	status = WIFI_AccessPointCLI(pcParameterString1, pcParameterString2, xParameterStringLength1,
			xParameterStringLength2);

	if (status == H21R2_OK)
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage);
	else if (status == H21R2_ERROR)
		strcpy((char*) pcWriteBuffer, (char*) pcErrorsMessage);

	return pdFALSE;
}

/***************************************************************************/
portBASE_TYPE CLI_ESPWifiStationModeCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen,
		const int8_t *pcCommandString) {
	Module_Status status = H21R2_OK;

	static const int8_t *pcOKMessage = (int8_t*) "ESP Wifi Station  Mode On\n\r";
	static const int8_t *pcErrorsMessage = (int8_t*) "Error Params!\n\r";

	static int8_t *pcParameterString1, *pcParameterString2;
	portBASE_TYPE xParameterStringLength1 = 0, xParameterStringLength2 = 0;

	(void) xWriteBufferLen;

	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength1);
	pcParameterString2 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameterStringLength2);

	status = WIFI_StationCLI(pcParameterString1, pcParameterString2, xParameterStringLength1, xParameterStringLength2);

	if (status == H21R2_OK)
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage);
	else if (status == H21R2_ERROR)
		strcpy((char*) pcWriteBuffer, (char*) pcErrorsMessage);

	return pdFALSE;
}

/***************************************************************************/
portBASE_TYPE CLI_ESPWriteToBleServerModeCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen,
		const int8_t *pcCommandString) {
	Module_Status status = H21R2_OK;

	static const int8_t *pcOKMessage = (int8_t*) "ESP Write To Ble Server\n\r";
	static const int8_t *pcErrorsMessage = (int8_t*) "Error Params!\n\r";

	static int8_t *pcParameterString1;
	portBASE_TYPE xParameterStringLength1 = 0;

	(void) xWriteBufferLen;

	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength1);

	status = BLE_Write(pcParameterString1, SERVER, strlen(pcParameterString1));

	if (status == H21R2_OK)
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage);
	else if (status == H21R2_ERROR)
		strcpy((char*) pcWriteBuffer, (char*) pcErrorsMessage);

	return pdFALSE;
}

/***************************************************************************/
portBASE_TYPE CLI_ESPWriteToBleClientModeCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen,
		const int8_t *pcCommandString) {
	Module_Status status = H21R2_OK;

	static const int8_t *pcOKMessage = (int8_t*) "ESP Write To Ble Client\n\r";
	static const int8_t *pcErrorsMessage = (int8_t*) "Error Params!\n\r";

	static int8_t *pcParameterString1;
	portBASE_TYPE xParameterStringLength1 = 0;

	(void) xWriteBufferLen;

	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength1);

	status = BLE_Write(pcParameterString1, CLIENT, strlen(pcParameterString1));

	if (status == H21R2_OK)
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage);
	else if (status == H21R2_ERROR)
		strcpy((char*) pcWriteBuffer, (char*) pcErrorsMessage);

	return pdFALSE;
}

/***************************************************************************/
portBASE_TYPE CLI_ESPReadFromBleServerModeCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen,
		const int8_t *pcCommandString) {
	Module_Status status = H21R2_OK;

	static const uint16_t *pcOKMessage = (uint16_t*) "Read Data From Server :%s \n\r";
	static const uint16_t *pcErrorsMessage = (uint16_t*) "Error Params!\n\r";

	static char Data [18];

	(void) xWriteBufferLen;

	status = BLE_Read(Data, SERVER);

	if (status == H21R2_OK)
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage, Data);
	else if (status == H21R2_ERROR)
		strcpy((char*) pcWriteBuffer, (char*) pcErrorsMessage);

	return pdFALSE;
}

/***************************************************************************/
portBASE_TYPE CLI_ESPReadFromBleClientModeCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen,
		const int8_t *pcCommandString) {
	Module_Status status = H21R2_OK;

	static const uint16_t *pcOKMessage = (uint16_t*) "Read Data From Client :%s \n\r";
	static const uint16_t *pcErrorsMessage = (uint16_t*) "Error Params!\n\r";

	static char Data [18];

	(void) xWriteBufferLen;

	status = BLE_Read(Data, CLIENT);

	if (status == H21R2_OK)
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage, Data);
	else if (status == H21R2_ERROR)
		strcpy((char*) pcWriteBuffer, (char*) pcErrorsMessage);

	return pdFALSE;
}

/***************************************************************************/
/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
