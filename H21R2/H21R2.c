/*
 BitzOS (BOS) V0.4.0 - Copyright (C) 2017-2025 Hexabitz
 All rights reserved

 File Name  : H21R2.c
 Description: Main module file for ESP32 wireless communication.
 ESP32: Manages BLE (Server/Client) and WiFi (Station/Access Point) modes.
 CLI: Commands for ESP32 control (reset, boot, mode configuration).
 Messaging: Handles wireless communication protocols and data transfer.
 Special Config: UART3 for high-speed ESP32 communication (921600 baud).
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
uint8_t FullData[ESP_PACKET_LENGTH] = {0}; /* Buffer to store ESP32 communication data */
uint8_t wifiBuffer[WIFI_BUFF_SIZE] = {0};   /* Buffer to store WiFi communication data */
uint16_t Length1 = 0;                      /* Length of first data string */
uint16_t Length2 = 0;                      /* Length of second data string */
uint64_t Time = 0;                         /* Timestamp for operations */
uint64_t Timeout = 0;                      /* Timeout value for operations */
/* Module Parameters */
ModuleParam_t ModuleParam[NUM_MODULE_PARAMS] = {0}; /* Array to store module parameters */

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
/* CLI command structure: ESP Reset Mode */
const CLI_Command_Definition_t CLI_ESPResetModeCommandDefinition = {
    (const int8_t *)"espreset", /* Command string to type */
    (const int8_t *)"espreset:\r\n To reset esp \r\n\r\n",
    CLI_ESPResetModeCommand, /* Function to execute */
    0 /* Zero parameters expected */
};

/* CLI command structure: ESP Boot Mode */
const CLI_Command_Definition_t CLI_ESPBootModeCommandDefinition = {
    (const int8_t *)"espboot", /* Command string to type */
    (const int8_t *)"espboot:\r\n Booting Esp \r\n\r\n",
    CLI_ESPBootModeCommand, /* Function to execute */
    0 /* Zero parameters expected */
};

/* CLI command structure: Set ESP in BLE Client Mode */
const CLI_Command_Definition_t CLI_ESPBleClientModeCommandDefinition = {
    (const int8_t *)"bleclient", /* Command string to type */
    (const int8_t *)"bleclient:\r\n To Set ESP-BLE in Client Mode \r\n\r\n",
    CLI_ESPBleClientModeCommand, /* Function to execute */
    2 /* Two parameters expected */
};

/* CLI command structure: Set ESP in BLE Server Mode */
const CLI_Command_Definition_t CLI_ESPBleServerModeCommandDefinition = {
    (const int8_t *)"bleserver", /* Command string to type */
    (const int8_t *)"bleserver:\r\n To Set ESP-BLE in Server Mode \r\n\r\n",
    CLI_ESPBleServerModeCommand, /* Function to execute */
    1 /* One parameter expected */
};

/* CLI command structure: Set ESP in WiFi Access Point Mode */
const CLI_Command_Definition_t CLI_ESPWifiAccessPiontCommandDefinition = {
    (const int8_t *)"wifiaccesspoint", /* Command string to type */
    (const int8_t *)"wifiaccesspoint:\r\n To Set ESP-Wifi in Access Point Mode \r\n\r\n",
    CLI_ESPWifiAccessPiontModeCommand, /* Function to execute */
    2 /* Two parameters expected */
};

/* CLI command structure: Set ESP in WiFi Station Mode */
const CLI_Command_Definition_t CLI_ESPWifiStationModeCommandDefinition = {
    (const int8_t *)"wifistation", /* Command string to type */
    (const int8_t *)"wifistation:\r\n To Set ESP-Wifi in Station Mode \r\n\r\n",
    CLI_ESPWifiStationModeCommand, /* Function to execute */
    2 /* Two parameters expected */
};

/* CLI command structure: Read from BLE in Server Mode */
const CLI_Command_Definition_t CLI_ESPReadFromBleServerModeCommandDefinition = {
    (const int8_t *)"readfromserver", /* Command string to type */
    (const int8_t *)"readfromserver:\r\n To Read From BLE in Server Mode \r\n\r\n",
    CLI_ESPReadFromBleServerModeCommand, /* Function to execute */
    0 /* Zero parameters expected */
};

/* CLI command structure: Write to BLE in Server Mode */
const CLI_Command_Definition_t CLI_ESPWriteFromBleServerModeCommandDefinition = {
    (const int8_t *)"writetoserver", /* Command string to type */
    (const int8_t *)"writetoserver:\r\n To Write From BLE in Server Mode \r\n\r\n",
    CLI_ESPWriteToBleServerModeCommand, /* Function to execute */
    1 /* One parameter expected */
};

/* CLI command structure: Read from BLE in Client Mode */
const CLI_Command_Definition_t CLI_ESPReadFromBleClientModeCommandDefinition = {
    (const int8_t *)"readfromclient", /* Command string to type */
    (const int8_t *)"readfromclient:\r\n To Read From BLE in Client Mode \r\n\r\n",
    CLI_ESPReadFromBleClientModeCommand, /* Function to execute */
    0 /* Zero parameters expected */
};

/* CLI command structure: Write to BLE in Client Mode */
const CLI_Command_Definition_t CLI_ESPWriteFromBleclientModeCommandDefinition = {
    (const int8_t *)"writetoclient", /* Command string to type */
    (const int8_t *)"writetoclient:\r\n To Write From BLE in Client Mode \r\n\r\n",
    CLI_ESPWriteToBleClientModeCommand, /* Function to execute */
    1 /* One parameter expected */
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

/*
 * @brief: Processes messages for the H21R2 module.
 * @param code: Message code to process.
 * @param port: Port number receiving the message.
 * @param src: Source module ID.
 * @param dst: Destination module ID.
 * @param shift: Offset in the message buffer.
 * @retval: Module status indicating success or error.
 */
Module_Status Module_MessagingTask(uint16_t code, uint8_t port, uint8_t src, uint8_t dst, uint8_t shift) {
    Module_Status result = H21R2_OK; /* Initialize status to success */
    char ServerName2[Length2]; /* Buffer for second server name */
    char ServerName[Length1]; /* Buffer for server name */
    char ClientName[Length1]; /* Buffer for client name */
    char Accessspoint[Length1]; /* Buffer for WiFi access point name */
    char Station[Length1]; /* Buffer for WiFi station name */
    char Password[Length2]; /* Buffer for password */
    char Data2[20]; /* Buffer for read data */
    char Data[Length1]; /* Buffer for write data */
    uint8_t SendData[Length1 + 2]; /* Buffer for sending data */
    uint16_t module = 0; /* Module ID for message forwarding */

    switch (code) { /* Process message based on code */
    case CODE_H21R2_ESP_BOOT: /* ESP Boot command */
        ESP_Boot(); /* Trigger ESP32 boot */
        break;
    case CODE_H21R2_ESP_RESET: /* ESP Reset command */
        ESP_Reset(); /* Reset ESP32 */
        break;
    case CODE_H21R2_ESP_SERVER: /* Set BLE server mode */
        Length1 = (uint16_t)cMessage[port - 1][shift]; /* Get server name length */
        memcpy(ServerName, &cMessage[port - 1][1 + shift], Length1); /* Copy server name */
        BLE_ServerMode(ServerName); /* Initialize BLE server mode */
        break;
    case CODE_H21R2_ESP_CLIENT: /* Set BLE client mode */
        Length1 = (uint16_t)cMessage[port - 1][shift]; /* Get client name length */
        Length2 = (uint16_t)cMessage[port - 1][1 + shift]; /* Get server name length */
        memcpy(ClientName, &cMessage[port - 1][2 + shift], Length1); /* Copy client name */
        memcpy(ServerName2, &cMessage[port - 1][Length1 + 2 + shift], Length2); /* Copy server name */
        BLE_ClientMode(ClientName, ServerName2); /* Initialize BLE client mode */
        break;
    case CODE_H21R2_ESP_ACCESS_POINT: /* Set WiFi access point mode */
        Length1 = (uint16_t)cMessage[port - 1][shift]; /* Get access point name length */
        Length2 = (uint16_t)cMessage[port - 1][1 + shift]; /* Get password length */
        memcpy(Accessspoint, &cMessage[port - 1][2 + shift], Length1); /* Copy access point name */
        memcpy(Password, &cMessage[port - 1][Length1 + 2 + shift], Length2); /* Copy password */
        WIFI_AccessPoint(Accessspoint, Password); /* Initialize WiFi access point */
        break;
    case CODE_H21R2_ESP_STATION: /* Set WiFi station mode */
        Length1 = (uint16_t)cMessage[port - 1][shift]; /* Get station name length */
        Length2 = (uint16_t)cMessage[port - 1][1 + shift]; /* Get password length */
        memcpy(Station, &cMessage[port - 1][2 + shift], Length1); /* Copy station name */
        memcpy(Password, &cMessage[port - 1][Length1 + 2 + shift], Length2); /* Copy password */
        WIFI_Station(Station, Password); /* Initialize WiFi station mode */
        break;
    case CODE_H21R2_ESP_WRITE_TO_SERVER: /* Write to BLE server */
        Length1 = (uint16_t)cMessage[port - 1][shift]; /* Get data length */
        memcpy(Data, &cMessage[port - 1][1 + shift], Length1); /* Copy data */
        SendData[0] = WRITE_TO_SERVER_MODE; /* Set command code */
        SendData[1] = Length1; /* Set data length */
        memcpy(&SendData[2], Data, Length1); /* Copy data to send buffer */
        if (1 == FullData[0]) { /* Check if buffer is ready */
            HAL_UART_Transmit(ESP32_UART_HANDEL, SendData, Length1 + 2, 0xff); /* Transmit data */
            FullData[0] = 0; /* Clear buffer ready flag */
    }
        break;
    case CODE_H21R2_ESP_WRITE_TO_CLIENT: /* Write to BLE client */
        Length1 = (uint16_t)cMessage[port - 1][shift]; /* Get data length */
        memcpy(Data, &cMessage[port - 1][1 + shift], Length1); /* Copy data */
        SendData[0] = WRITE_TO_CLIENT_MODE; /* Set command code */
        SendData[1] = Length1; /* Set data length */
        memcpy(&SendData[2], Data, Length1); /* Copy data to send buffer */
        if (1 == FullData[0]) { /* Check if buffer is ready */
            HAL_UART_Transmit(ESP32_UART_HANDEL, SendData, Length1 + 2, 0xff); /* Transmit data */
            FullData[0] = 0; /* Clear buffer ready flag */
        }
        break;
    case CODE_H21R2_ESP_READ_FROM_SERVER: /* Read from BLE server */
        module = (uint16_t)cMessage[port - 1][shift]; /* Get module ID */
        BLE_Read(Data2, SERVER); /* Read data from BLE server */
        memcpy(MessageParams, Data2, 20); /* Copy read data to message parameters */
        SendMessageToModule(module, CODE_PORT_FORWARD, 20); /* Forward data to module */
        break;
    case CODE_H21R2_ESP_READ_FROM_CLIENT: /* Read from BLE client */
        module = (uint16_t)cMessage[port - 1][shift]; /* Get module ID */
        BLE_Read(Data2, CLIENT); /* Read data from BLE client */
        memcpy(MessageParams, Data2, 20); /* Copy read data to message parameters */
        SendMessageToModule(module, CODE_PORT_FORWARD, 20); /* Forward data to module */
        break;
    default: /* Unknown message code */
        result = H21R2_ERR_UNKNOWNMESSAGE; /* Set error status for unknown message */
        break;
    }

    return result; /* Return status */
}

/***************************************************************************/
/*
 * @brief: Gets the port number for a given UART handle.
 * @param huart: Pointer to UART handle.
 * @retval: Port number or 0 if invalid.
 */
uint8_t GetPort(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART4) /* Check if UART is USART4 */
        return P1; /* Return port 1 */
    else if (huart->Instance == USART2) /* Check if UART is USART2 */
        return P2; /* Return port 2 */
    else if (huart->Instance == USART6) /* Check if UART is USART6 */
        return P3; /* Return port 3 */
    else if (huart->Instance == USART1) /* Check if UART is USART1 */
        return P4; /* Return port 4 */
    else if (huart->Instance == USART5) /* Check if UART is USART5 */
        return P5; /* Return port 5 */

    return 0; /* Return 0 for invalid UART */
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

/*
 * @brief: Resets the ESP32 MCU.
 * @retval: Module status indicating success or error.
 */
Module_Status ESP_Reset(void) {
    Module_Status Status = H21R2_OK; /* Initialize status to success */

    /* Reset ESP32 by toggling reset pin */
    HAL_GPIO_WritePin(ESP32_RST_PORT, ESP32_RST_PIN, GPIO_PIN_RESET); /* Set reset pin low */
    HAL_Delay(5); /* Wait for 5 ms */
    HAL_GPIO_WritePin(ESP32_RST_PORT, ESP32_RST_PIN, GPIO_PIN_SET); /* Set reset pin high */
    HAL_Delay(1000); /* Wait for 1000 ms to complete reset */

    return Status; /* Return success status */
}

/***************************************************************************/
/*
 * @brief: Boots the ESP32 MCU.
 * @retval: Module status indicating success or error.
 */
Module_Status ESP_Boot(void) {
    Module_Status Status = H21R2_OK; /* Initialize status to success */

    /* Boot ESP32 by toggling boot and reset pins */
    HAL_GPIO_WritePin(ESP32_BOOT_PORT, ESP32_BOOT_PIN, GPIO_PIN_RESET); /* Set boot pin low */
    HAL_GPIO_WritePin(ESP32_RST_PORT, ESP32_RST_PIN, GPIO_PIN_RESET); /* Set reset pin low */
    HAL_Delay(100); /* Wait for 100 ms */
    HAL_GPIO_WritePin(ESP32_RST_PORT, ESP32_RST_PIN, GPIO_PIN_SET); /* Set reset pin high */
    HAL_Delay(100); /* Wait for 100 ms */
    HAL_GPIO_WritePin(ESP32_BOOT_PORT, ESP32_BOOT_PIN, GPIO_PIN_SET); /* Set boot pin high */

    return Status; /* Return success status */
}


/***************************************************************************/

/*
 * @brief: Initializes BLE client mode.
 * @param ClientName: Pointer to client name string.
 * @param ServerName: Pointer to server name string.
 * @retval: Module status indicating success or error.
 */
Module_Status BLE_ClientMode(char *ClientName, char *ServerName) {
    Module_Status Status = H21R2_OK; /* Initialize status to success */
    int LenClientName = 0; /* Length of client name */
    int LenServerName = 0; /* Length of server name */
    uint8_t Data[LenClientName + LenServerName + 3]; /* Buffer for data transmission */

    /* Initialize UART for ESP32 */
    UARTInitESP32(); /* Configure UART for ESP32 communication */

    /* Calculate lengths of names */
    LenClientName = strlen(ClientName); /* Get length of client name */
    LenServerName = strlen(ServerName); /* Get length of server name */

    /* Prepare data packet */
    Data[0] = CLIENT_MODE; /* Set mode to BLE client */
    Data[1] = LenClientName; /* Set client name length */
    Data[2] = LenServerName; /* Set server name length */
    memcpy(&Data[3], ClientName, LenClientName); /* Copy client name to buffer */
    memcpy(&Data[LenClientName + 3], ServerName, LenServerName); /* Copy server name to buffer */
    HAL_UART_Transmit(ESP32_UART_HANDEL, Data, LenClientName + LenServerName + 3, 0xff); /* Transmit data packet */
    HAL_UART_Receive_DMA(ESP32_UART_HANDEL, FullData, ESP_PACKET_LENGTH); /* Start DMA receive for response */

    return Status; /* Return success status */
}

/***************************************************************************/

/*
 * @brief: Initializes BLE client mode via CLI.
 * @param ClientName: Pointer to client name string.
 * @param ServerName: Pointer to server name string.
 * @param LenClientName: Length of client name.
 * @param LenServerName: Length of server name.
 * @retval: Module status indicating success or error.
 */
Module_Status BLE_ClientModeCLI(char *ClientName, char *ServerName, uint8_t LenClientName, uint8_t LenServerName) {
    Module_Status Status = H21R2_OK; /* Initialize status to success */
    uint8_t Data[LenClientName + LenServerName + 3]; /* Buffer for data transmission */

    /* Initialize UART for ESP32 */
    UARTInitESP32(); /* Configure UART for ESP32 communication */

    /* Prepare data packet */
    Data[0] = CLIENT_MODE; /* Set mode to BLE client */
    Data[1] = LenClientName; /* Set client name length */
    Data[2] = LenServerName; /* Set server name length */
    memcpy(&Data[3], ClientName, LenClientName); /* Copy client name to buffer */
    memcpy(&Data[LenClientName + 3], ServerName, LenServerName); /* Copy server name to buffer */
    HAL_UART_Transmit(ESP32_UART_HANDEL, Data, LenClientName + LenServerName + 3, 0xff); /* Transmit data packet */
    HAL_UART_Receive_DMA(ESP32_UART_HANDEL, FullData, ESP_PACKET_LENGTH); /* Start DMA receive for response */

    return Status; /* Return success status */
}

/***************************************************************************/
/*
 * @brief: Initializes BLE server mode.
 * @param ServerName: Pointer to server name string.
 * @retval: Module status indicating success or error.
 */
Module_Status BLE_ServerMode(char *ServerName) {
    Module_Status Status = H21R2_OK; /* Initialize status to success */
    int LenServerName = 0; /* Length of server name */
    uint8_t Data[LenServerName + 2]; /* Buffer for data transmission */

    /* Initialize UART for ESP32 */
    UARTInitESP32(); /* Configure UART for ESP32 communication */

    /* Calculate server name length */
    LenServerName = strlen(ServerName); /* Get length of server name */

    /* Prepare data packet */
    Data[0] = SERVER_MODE; /* Set mode to BLE server */
    Data[1] = LenServerName; /* Set server name length */
    memcpy(&Data[2], ServerName, LenServerName); /* Copy server name to buffer */
    HAL_UART_Transmit(ESP32_UART_HANDEL, Data, LenServerName + 2, 0xff); /* Transmit data packet */
    HAL_UART_Receive_DMA(ESP32_UART_HANDEL, FullData, ESP_PACKET_LENGTH); /* Start DMA receive for response */

    return Status; /* Return success status */
}

/***************************************************************************/
/*
 * @brief: Reads data from BLE in server or client mode.
 * @param Data: Pointer to store read data.
 * @param function: BLE mode (SERVER or CLIENT).
 * @retval: Module status indicating success or error.
 */
Module_Status BLE_Read(char *Data, BLE_MODE function) {
    Module_Status Status = H21R2_OK; /* Initialize status to success */
    uint8_t SendData; /* Variable for sending command */

    switch (function) { /* Check BLE mode */
    case SERVER: /* Server mode */
        if ('H' == FullData[0] && 'Z' == FullData[1]) /* Check for valid data marker */
            memcpy(Data, &FullData[2], BLE_BUFF_SIZE); /* Copy data from buffer */
        break;
    case CLIENT: /* Client mode */
        SendData = WRITE_FROM_CLIENT_MODE; /* Set command to write from client */
        if (FullData[0] == 1) { /* Check if buffer is ready */
            HAL_UART_Transmit(ESP32_UART_HANDEL, &SendData, 1, 0xff); /* Send command */
            FullData[0] = 0; /* Clear buffer ready flag */
        }
        HAL_Delay(10); /* Wait for 10 ms */
        if ('H' == FullData[0] && 'Z' == FullData[1]) { /* Check for valid data marker */
            do {
                memcpy(Data, &FullData[2], BLE_BUFF_SIZE); /* Copy data from buffer */
            } while (FullData[0] != 1); /* Wait until buffer is ready again */
            HAL_Delay(100); /* Wait for 100 ms */
        }
        break;
    }

    return Status; /* Return success status */
}

/***************************************************************************/
/*
 * @brief: Reads data from WiFi socket mode.
 * @param Data: Pointer to store read data.
 * @retval: Module status indicating success or error.
 */
Module_Status WIFI_SocketRead(char *Data) {
    Module_Status Status = H21R2_OK; /* Initialize status to success */

    if ('H' == wifiBuffer[0] && 'Z' == wifiBuffer[1]) /* Check for valid data marker */
        memcpy(Data, &wifiBuffer[2], 126); /* Copy data from WiFi buffer */

    return Status; /* Return success status */
}

/***************************************************************************/

/*
 * @brief: Writes data to BLE in server or client mode.
 * @param Data: Pointer to data to write.
 * @param function: BLE mode (SERVER or CLIENT).
 * @param Size: Size of data to write.
 * @retval: Module status indicating success or error.
 */
Module_Status BLE_Write(char *Data, BLE_MODE function, uint16_t Size) {
    Module_Status Status = H21R2_OK; /* Initialize status to success */
    int LenData = Size; /* Set data length */
    uint8_t SendData[LenData + 2]; /* Buffer for sending data */

    switch (function) { /* Check BLE mode */
    case SERVER: /* Server mode */
        SendData[0] = WRITE_TO_SERVER_MODE; /* Set command to write to server */
        SendData[1] = LenData; /* Set data length */
        memcpy(&SendData[2], Data, LenData); /* Copy data to send buffer */
        if (FullData[0] == 1) { /* Check if buffer is ready */
            HAL_UART_Transmit(ESP32_UART_HANDEL, SendData, LenData + 2, 0xff); /* Transmit data */
            FullData[0] = 0; /* Clear buffer ready flag */
        }
        break;
    case CLIENT: /* Client mode */
        SendData[0] = WRITE_TO_CLIENT_MODE; /* Set command to write to client */
        SendData[1] = LenData; /* Set data length */
        memcpy(&SendData[2], Data, LenData); /* Copy data to send buffer */
        if (FullData[0] == 1) { /* Check if buffer is ready */
            HAL_UART_Transmit(ESP32_UART_HANDEL, SendData, LenData + 2, 0xff); /* Transmit data */
            FullData[0] = 0; /* Clear buffer ready flag */
            do {
                /* Wait for buffer to be ready */
            } while (1 != FullData[0]); /* Loop until buffer is ready */
        }
        HAL_Delay(100); /* Wait for 100 ms */
        break;
    }

    return Status; /* Return success status */
}

/***************************************************************************/

/*
 * @brief: Writes data to WiFi socket mode.
 * @param Data: Pointer to data to write.
 * @param Size: Size of data to write.
 * @retval: Module status indicating success or error.
 */
Module_Status WIFI_SocketWrite(char *Data, uint16_t Size) {
    Module_Status Status = H21R2_OK; /* Initialize status to success */
    int LenData = Size; /* Set data length */
    uint8_t SendData[LenData + 2]; /* Buffer for sending data */

    /* Prepare data packet */
    SendData[0] = WRITE_SOCKET_MODE; /* Set command to write to socket */
    SendData[1] = LenData; /* Set data length */
    memcpy(&SendData[2], Data, LenData); /* Copy data to send buffer */
    if (wifiBuffer[0] == 1) { /* Check if WiFi buffer is ready */
        HAL_UART_Transmit(ESP32_UART_HANDEL, SendData, LenData + 2, 0xff); /* Transmit data */
        wifiBuffer[0] = 0; /* Clear buffer ready flag */
    }

    return Status; /* Return success status */
}

/***************************************************************************/

/*
 * @brief: Initializes WiFi access point mode.
 * @param Ssid: Pointer to access point name.
 * @param Password: Pointer to password.
 * @retval: Module status indicating success or error.
 */
Module_Status WIFI_AccessPoint(char *Ssid, char *Password) {
    Module_Status Status = H21R2_OK; /* Initialize status to success */
    int LenSsid = 0; /* Length of SSID */
    int LenPassword = 0; /* Length of password */
    uint8_t Data[LenSsid + LenPassword + 3]; /* Buffer for data transmission */

    /* Initialize UART for ESP32 */
    UARTInitESP32(); /* Configure UART for ESP32 communication */

    /* Calculate lengths */
    LenSsid = strlen(Ssid); /* Get length of SSID */
    LenPassword = strlen(Password); /* Get length of password */

    /* Prepare data packet */
    Data[0] = WIFI_ACCESS_POINT_MODE; /* Set mode to WiFi access point */
    Data[1] = LenSsid; /* Set SSID length */
    Data[2] = LenPassword; /* Set password length */
    memcpy(&Data[3], Ssid, LenSsid); /* Copy SSID to buffer */
    memcpy(&Data[LenSsid + 3], Password, LenPassword); /* Copy password to buffer */
    HAL_UART_Transmit(ESP32_UART_HANDEL, Data, LenSsid + LenPassword + 3, 0xff); /* Transmit data packet */

    return Status; /* Return success status */
}

/***************************************************************************/

/*
 * @brief: Initializes WiFi socket mode.
 * @param Ssid: Pointer to access point name.
 * @param Password: Pointer to password.
 * @retval: Module status indicating success or error.
 */
Module_Status WIFI_Socket(char *Ssid, char *Password) {
    Module_Status Status = H21R2_OK; /* Initialize status to success */
    int LenSsid = 0; /* Length of SSID */
    int LenPassword = 0; /* Length of password */
    uint8_t Data[LenSsid + LenPassword + 3]; /* Buffer for data transmission */

    /* Initialize UART for ESP32 */
    UARTInitESP32(); /* Configure UART for ESP32 communication */

    /* Calculate lengths */
    LenSsid = strlen(Ssid); /* Get length of SSID */
    LenPassword = strlen(Password); /* Get length of password */

    /* Prepare data packet */
    Data[0] = WIFI_SOCKET_MODE; /* Set mode to WiFi socket */
    Data[1] = LenSsid; /* Set SSID length */
    Data[2] = LenPassword; /* Set password length */
    memcpy(&Data[3], Ssid, LenSsid); /* Copy SSID to buffer */
    memcpy(&Data[LenSsid + 3], Password, LenPassword); /* Copy password to buffer */
    HAL_UART_Transmit(ESP32_UART_HANDEL, Data, LenSsid + LenPassword + 3, 0xff); /* Transmit data packet */
    HAL_UART_Receive_DMA(ESP32_UART_HANDEL, wifiBuffer, WIFI_BUFF_SIZE); /* Start DMA receive for response */

    return Status; /* Return success status */
}

/***************************************************************************/
/*
 * @brief: Initializes WiFi access point mode via CLI.
 * @param Ssid: Pointer to access point name.
 * @param Password: Pointer to password.
 * @param lenSsid: Length of SSID.
 * @param lenPassword: Length of password.
 * @retval: Module status indicating success or error.
 */
Module_Status WIFI_AccessPointCLI(char *Ssid, char *Password, uint8_t lenSsid, uint8_t lenPassword) {
    Module_Status Status = H21R2_OK; /* Initialize status to success */
    uint8_t Data[lenSsid + lenPassword + 3]; /* Buffer for data transmission */

    /* Prepare ESP32 for operation */
    ESP_Boot(); /* Boot ESP32 */
    ESP_Reset(); /* Reset ESP32 */

    /* Initialize UART for ESP32 */
    UARTInitESP32(); /* Configure UART for ESP32 communication */

    /* Prepare data packet */
    Data[0] = WIFI_ACCESS_POINT_MODE; /* Set mode to WiFi access point */
    Data[1] = lenSsid; /* Set SSID length */
    Data[2] = lenPassword; /* Set password length */
    memcpy(&Data[3], Ssid, lenSsid); /* Copy SSID to buffer */
    memcpy(&Data[lenSsid + 3], Password, lenPassword); /* Copy password to buffer */
    HAL_Delay(1000); /* Wait for 1000 ms */
    HAL_UART_Transmit(ESP32_UART_HANDEL, Data, lenSsid + lenPassword + 3, 0xff); /* Transmit data packet */

    return Status; /* Return success status */
}

/***************************************************************************/

/*
 * @brief: Initializes WiFi station mode.
 * @param Ssid: Pointer to station name.
 * @param Password: Pointer to password.
 * @retval: Module status indicating success or error.
 */
Module_Status WIFI_Station(char *Ssid, char *Password) {
    Module_Status Status = H21R2_OK; /* Initialize status to success */
    int LenSsid = 0; /* Length of SSID */
    int LenPassword = 0; /* Length of password */
    uint8_t Data[LenSsid + LenPassword + 3]; /* Buffer for data transmission */

    /* Initialize UART for ESP32 */
    UARTInitESP32(); /* Configure UART for ESP32 communication */

    /* Calculate lengths */
    LenSsid = strlen(Ssid); /* Get length of SSID */
    LenPassword = strlen(Password); /* Get length of password */

    /* Prepare data packet */
    Data[0] = WIFI_STATION_MODE; /* Set mode to WiFi station */
    Data[1] = LenSsid; /* Set SSID length */
    Data[2] = LenPassword; /* Set password length */
    memcpy(&Data[3], Ssid, LenSsid); /* Copy SSID to buffer */
    memcpy(&Data[LenSsid + 3], Password, LenPassword); /* Copy password to buffer */
    HAL_UART_Transmit(ESP32_UART_HANDEL, Data, LenSsid + LenPassword + 3, 0xff); /* Transmit data packet */

    return Status; /* Return success status */
}

/***************************************************************************/

/*
 * @brief: Initializes WiFi station mode via CLI.
 * @param Ssid: Pointer to station name.
 * @param Password: Pointer to password.
 * @param lenSsid: Length of SSID.
 * @param lenPassword: Length of password.
 * @retval: Module status indicating success or error.
 */
Module_Status WIFI_StationCLI(char *Ssid, char *Password, uint8_t lenSsid, uint8_t lenPassword) {
    Module_Status Status = H21R2_OK; /* Initialize status to success */
    uint8_t Data[lenSsid + lenPassword + 3]; /* Buffer for data transmission */

    /* Prepare ESP32 for operation */
    ESP_Boot(); /* Boot ESP32 */
    ESP_Reset(); /* Reset ESP32 */

    /* Initialize UART for ESP32 */
    UARTInitESP32(); /* Configure UART for ESP32 communication */

    /* Prepare data packet */
    Data[0] = WIFI_STATION_MODE; /* Set mode to WiFi station */
    Data[1] = lenSsid; /* Set SSID length */
    Data[2] = lenPassword; /* Set password length */
    memcpy(&Data[3], Ssid, lenSsid); /* Copy SSID to buffer */
    memcpy(&Data[lenSsid + 3], Password, lenPassword); /* Copy password to buffer */
    HAL_UART_Transmit(ESP32_UART_HANDEL, Data, lenSsid + lenPassword + 3, 0xff); /* Transmit data packet */

    return Status; /* Return success status */
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
