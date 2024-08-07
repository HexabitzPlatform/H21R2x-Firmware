/*
 BitzOS (BOS) V0.3.6 - Copyright (C) 2017-2024 Hexabitz
 All rights reserved

 File Name     : H21R2.c
 Description   : Source code for module H21R2.
 	 	 	 	 (Description_of_module)

(Description of Special module peripheral configuration):
>>
>>
>>

 */

/* Includes ------------------------------------------------------------------*/
#include "BOS.h"
#include "H21R2_inputs.h"
/* Define UART variables */
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart6;

/* Exported variables */
extern FLASH_ProcessTypeDef pFlash;
extern uint8_t numOfRecordedSnippets;

/* Module exported parameters ------------------------------------------------*/
module_param_t modParam[NUM_MODULE_PARAMS] ={{.paramPtr = NULL, .paramFormat =FMT_FLOAT, .paramName =""}};

/* Private variables ---------------------------------------------------------*/
uint8_t FullData[22];
uint64_t Timeout = 0;
uint64_t Time = 0;
uint16_t length1,length2;
uint8_t WIFIBUFF[128];

/* Private function prototypes -----------------------------------------------*/
void ExecuteMonitor(void);

/* Create CLI commands --------------------------------------------------------*/
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

/*-----------------------------------------------------------*/
/* CLI command structure : ESP Reset Mode */
const CLI_Command_Definition_t CLI_ESPResetModeCommandDefinition =
{
	( const int8_t * ) "espreset", /* The command string to type. */
	( const int8_t * ) "espreset:\r\n To reset esp \r\n\r\n",
	CLI_ESPResetModeCommand, /* The function to run. */
	0 /* zero parameters are expected. */
};
/*-----------------------------------------------------------*/
/* CLI command structure : ESP Boot Mode */
const CLI_Command_Definition_t CLI_ESPBootModeCommandDefinition =
{
	( const int8_t * ) "espboot", /* The command string to type. */
	( const int8_t * ) "espboot:\r\n Booting Esp \r\n\r\n",
	CLI_ESPBootModeCommand, /* The function to run. */
	0 /* zero parameters are expected. */
};
/*-----------------------------------------------------------*/
/* CLI command structure : To set ESP in Ble client Mode */
const CLI_Command_Definition_t CLI_ESPBleClientModeCommandDefinition =
{
	( const int8_t * ) "bleclient", /* The command string to type. */
	( const int8_t * ) "bleclient:\r\n To Set ESP-BLE in Client Mode \r\n\r\n",
	CLI_ESPBleClientModeCommand, /* The function to run. */
	2 /* zero parameters are expected. */
};
/*-----------------------------------------------------------*/
/* CLI command structure : To set ESP in Ble Server Mode */
const CLI_Command_Definition_t CLI_ESPBleServerModeCommandDefinition =
{
	( const int8_t * ) "bleserver", /* The command string to type. */
	( const int8_t * ) "bleserver:\r\n To Set ESP-BLE in Server Mode  \r\n\r\n",
	CLI_ESPBleServerModeCommand, /* The function to run. */
	1 /* zero parameters are expected. */
};
/*-----------------------------------------------------------*/
/* CLI command structure : To set ESP in Wifi Access Piont Mode */
const CLI_Command_Definition_t CLI_ESPWifiAccessPiontCommandDefinition =
{
	( const int8_t * ) "wifiaccesspoint", /* The command string to type. */
	( const int8_t * ) "wifiaccesspoint:\r\n To Set ESP-Wifi in Access Piont Mode \r\n\r\n",
	CLI_ESPWifiAccessPiontModeCommand, /* The function to run. */
	2 /* zero parameters are expected. */
};
/*-----------------------------------------------------------*/
/* CLI command structure : To set ESP in Wifi Station Mode */
const CLI_Command_Definition_t CLI_ESPWifiStationModeCommandDefinition =
{
	( const int8_t * ) "wifistation", /* The command string to type. */
	( const int8_t * ) "wifistation:\r\n To Set ESP-Wifi in Station Mode \r\n\r\n",
	CLI_ESPWifiStationModeCommand, /* The function to run. */
	2 /* zero parameters are expected. */
};
/*-----------------------------------------------------------*/
/* CLI command structure : To Read From Ble in Server Mode  */
const CLI_Command_Definition_t CLI_ESPReadFromBleServerModeCommandDefinition =
{
	( const int8_t * ) "readfromserver", /* The command string to type. */
	( const int8_t * ) "readfromserver:\r\n To Read From BLE in Server Mode \r\n\r\n",
	CLI_ESPReadFromBleServerModeCommand, /* The function to run. */
	0 /* zero parameters are expected. */
};
/*-----------------------------------------------------------*/
/* CLI command structure : To Write From Ble in Server Mode  */
const CLI_Command_Definition_t CLI_ESPWriteFromBleServerModeCommandDefinition =
{
	( const int8_t * ) "writetoserver", /* The command string to type. */
	( const int8_t * ) "writetoserver:\r\n To Write From BLE in Server Mode \r\n\r\n",
	CLI_ESPWriteToBleServerModeCommand, /* The function to run. */
	1 /* zero parameters are expected. */
};
/*-----------------------------------------------------------*/
/* CLI command structure : To Read From Ble in Client Mode  */
const CLI_Command_Definition_t CLI_ESPReadFromBleClientModeCommandDefinition =
{
	( const int8_t * ) "readfromclient", /* The command string to type. */
	( const int8_t * ) "readfromclient:\r\n To Read From BLE in Client Mode \r\n\r\n",
	CLI_ESPReadFromBleClientModeCommand, /* The function to run. */
	0 /* zero parameters are expected. */
};
/*-----------------------------------------------------------*/
/* CLI command structure : To Write From Ble in Client Mode  */
const CLI_Command_Definition_t CLI_ESPWriteFromBleclientModeCommandDefinition =
{
	( const int8_t * ) "writetoclient", /* The command string to type. */
	( const int8_t * ) "writetoclient:\r\n To Write From BLE in Client Mode \r\n\r\n",
	CLI_ESPWriteToBleClientModeCommand, /* The function to run. */
	1 /* zero parameters are expected. */
};


/*-----------------------------------------------------------*/

/* -----------------------------------------------------------------------
 |								 Private Function			              |
 -------------------------------------------------------------------------
 */

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow : 
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 48000000
 *            HCLK(Hz)                       = 48000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 1
 *            HSE Frequency(Hz)              = 8000000
 *            PREDIV                         = 1
 *            PLLMUL                         = 6
 *            Flash Latency(WS)              = 1
 * @param  None
 * @retval None
 */
void SystemClock_Config(void){
	  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	  /** Configure the main internal regulator output voltage
	  */
	  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
	  /** Initializes the RCC Oscillators according to the specified parameters
	  * in the RCC_OscInitTypeDef structure.
	  */
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
	  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
	  RCC_OscInitStruct.PLL.PLLN = 12;
	  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	  HAL_RCC_OscConfig(&RCC_OscInitStruct);

	  /** Initializes the CPU, AHB and APB buses clocks
	  */
	  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                              |RCC_CLOCKTYPE_PCLK1;
	  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

	  /** Initializes the peripherals clocks
	  */
	  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART2;
	  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);


	  HAL_NVIC_SetPriority(SysTick_IRQn,0,0);
	
}

/*-----------------------------------------------------------*/


/* --- Save array topology and Command Snippets in Flash RO --- 
 */
uint8_t SaveToRO(void){
	BOS_Status result =BOS_OK;
	HAL_StatusTypeDef FlashStatus =HAL_OK;
	uint16_t add =8;
    uint16_t temp =0;
	uint8_t snipBuffer[sizeof(snippet_t) + 1] ={0};
	
	HAL_FLASH_Unlock();
	/* Erase RO area */
	FLASH_PageErase(FLASH_BANK_1,RO_START_ADDRESS);
	FlashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);
	FLASH_PageErase(FLASH_BANK_1,RO_MID_ADDRESS);
	//TOBECHECKED
	FlashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);
	if(FlashStatus != HAL_OK){
		return pFlash.ErrorCode;
	}
	else{
		/* Operation is completed, disable the PER Bit */
		CLEAR_BIT(FLASH->CR,FLASH_CR_PER);
	}
	
	/* Save number of modules and myID */
	if(myID){
		temp =(uint16_t )(N << 8) + myID;
		//HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,RO_START_ADDRESS,temp);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,RO_START_ADDRESS,temp);
		//TOBECHECKED
		FlashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);
		if(FlashStatus != HAL_OK){
			return pFlash.ErrorCode;
		}
		else{
			/* If the program operation is completed, disable the PG Bit */
			CLEAR_BIT(FLASH->CR,FLASH_CR_PG);
		}
		
		/* Save topology */
		for(uint8_t i =1; i <= N; i++){
			for(uint8_t j =0; j <= MaxNumOfPorts; j++){
				if(array[i - 1][0]){

          	HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,RO_START_ADDRESS + add,array[i - 1][j]);
				 //HALFWORD 	//TOBECHECKED
					FlashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);
					if(FlashStatus != HAL_OK){
						return pFlash.ErrorCode;
					}
					else{
						/* If the program operation is completed, disable the PG Bit */
						CLEAR_BIT(FLASH->CR,FLASH_CR_PG);
						add +=8;
					}
				}
			}
		}
	}
	
	// Save Command Snippets
	int currentAdd = RO_MID_ADDRESS;
	for(uint8_t s =0; s < numOfRecordedSnippets; s++){
		if(snippets[s].cond.conditionType){
			snipBuffer[0] =0xFE;		// A marker to separate Snippets
			memcpy((uint32_t* )&snipBuffer[1],(uint8_t* )&snippets[s],sizeof(snippet_t));
			// Copy the snippet struct buffer (20 x numOfRecordedSnippets). Note this is assuming sizeof(snippet_t) is even.
			for(uint8_t j =0; j < (sizeof(snippet_t)/4); j++){
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,currentAdd,*(uint64_t* )&snipBuffer[j*8]);
				//HALFWORD
				//TOBECHECKED
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
			// Copy the snippet commands buffer. Always an even number. Note the string termination char might be skipped
			for(uint8_t j =0; j < ((strlen(snippets[s].cmd) + 1)/4); j++){
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,currentAdd,*(uint64_t* )(snippets[s].cmd + j*4 ));
				//HALFWORD
				//TOBECHECKED
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
	
	HAL_FLASH_Lock();
	
	return result;
}


/* --- Clear array topology in SRAM and Flash RO --- 
 */
uint8_t ClearROtopology(void){
	// Clear the array 
	memset(array,0,sizeof(array));
	N =1;
	myID =0;
	
	return SaveToRO();
}
/*-----------------------------------------------------------*/

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

/* --- H21R2 module initialization.
 */
void Module_Peripheral_Init(void){

	/* Array ports */
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
//	MX_USART3_UART_Init();             //initialization after ESP_Boot();
	MX_USART4_UART_Init();
	MX_USART5_UART_Init();
	MX_USART6_UART_Init();

	 //Circulating DMA Channels ON All Module
	 for(int i=1;i<=NumOfPorts;i++)
		{
		  if(GetUart(i)==&huart1)
				   { index_dma[i-1]=&(DMA1_Channel1->CNDTR); }
		  else if(GetUart(i)==&huart2)
				   { index_dma[i-1]=&(DMA1_Channel2->CNDTR); }
		  else if(GetUart(i)==&huart3)
				   { index_dma[i-1]=&(DMA1_Channel3->CNDTR); }
		  else if(GetUart(i)==&huart4)
				   { index_dma[i-1]=&(DMA1_Channel4->CNDTR); }
		  else if(GetUart(i)==&huart5)
				   { index_dma[i-1]=&(DMA1_Channel5->CNDTR); }
		  else if(GetUart(i)==&huart6)
				   { index_dma[i-1]=&(DMA1_Channel6->CNDTR); }
		}


	/* Create module special task (if needed) */
}

/*-----------------------------------------------------------*/
/* --- H21R2 message processing task.
 */
Module_Status Module_MessagingTask(uint16_t code, uint8_t port, uint8_t src,
		uint8_t dst, uint8_t shift) {
	Module_Status result = H21R2_OK;

	switch (code) {
	case CODE_H21R2_ESP_BOOT: {
		ESP_Boot();
		break;
	}
	case CODE_H21R2_ESP_RESET: {
		ESP_Reset();
		break;
	}
	case CODE_H21R2_ESP_SERVER: {
		length1 = (uint16_t) cMessage[port - 1][shift];
		char ServerName[length1];
		memcpy(ServerName, &cMessage[port - 1][1 + shift], length1);
		BLE_ServerMode(ServerName);
		break;
	}
	case CODE_H21R2_ESP_CLIENT: {
		length1 = (uint16_t) cMessage[port - 1][shift];
		length2 = (uint16_t) cMessage[port - 1][1 + shift];
		char ClientName[length1];
		char ServerName[length2];
		memcpy(ClientName, &cMessage[port - 1][2 + shift], length1);
		memcpy(ServerName, &cMessage[port - 1][length1 + 2 + shift], length2);
		BLE_ClientMode(ClientName, ServerName);
		break;
	}
	case CODE_H21R2_ESP_ACCESS_POINT: {
		length1 = (uint16_t) cMessage[port - 1][shift];
		length2 = (uint16_t) cMessage[port - 1][1 + shift];
		char Accessspoint[length1];
		char Password[length2];
		memcpy(Accessspoint, &cMessage[port - 1][2 + shift], length1);
		memcpy(Password, &cMessage[port - 1][length1 + 2 + shift], length2);
		WIFI_AccessPoint(Accessspoint, Password);
		break;
	}
	case CODE_H21R2_ESP_STATION: {
		length1 = (uint16_t) cMessage[port - 1][shift];
		length2 = (uint16_t) cMessage[port - 1][1 + shift];
		char Station[length1];
		char Password[length2];
		memcpy(Station, &cMessage[port - 1][2 + shift], length1);
		memcpy(Password, &cMessage[port - 1][length1 + 2 + shift], length2);
		WIFI_AccessPoint(Station, Password);
		break;
	}
	case CODE_H21R2_ESP_WRITE_TO_SERVER: {
		length1 = (uint16_t) cMessage[port - 1][shift];
		uint8_t SendData[length1 + 2];
		char Data[length1];
		memcpy(Data, &cMessage[port - 1][1 + shift], length1);
		SendData[0] = WRITE_TO_SERVER_MODE;
		SendData[1] = length1;
		memcpy(&SendData[2], Data, length1);
		if (1 == FullData[0]) {
			HAL_UART_Transmit(&huart3, SendData, length1 + 2, 0xff);
			FullData[0] = 0;
		}
		break;
	}
	case CODE_H21R2_ESP_WRITE_TO_CLIENT: {
		length1 = (uint16_t) cMessage[port - 1][shift];
		uint8_t SendData[length1 + 2];
		char Data[length1];
		memcpy(Data, &cMessage[port - 1][1 + shift], length1);
		SendData[0] = WRITE_TO_CLIENT_MODE;
		SendData[1] = length1;
		memcpy(&SendData[2], Data, length1);
		if (1 == FullData[0]) {
			HAL_UART_Transmit(&huart3, SendData, length1 + 2, 0xff);
			FullData[0] = 0;
		}
		break;
	}
	case CODE_H21R2_ESP_READ_FROM_SERVER: {
		uint16_t module;
		char Data[20];
		module = (uint16_t) cMessage[port - 1][shift];
		BLE_Read(Data, server);
		memcpy(messageParams, Data, 20);
		SendMessageToModule(module, CODE_PORT_FORWARD, 20);
		break;
	}
	case CODE_H21R2_ESP_READ_FROM_CLIENT: {
		uint16_t module;
		char Data[20];
		module = (uint16_t) cMessage[port - 1][shift];
		BLE_Read(Data, client);
		memcpy(messageParams, Data, 20);
		SendMessageToModule(module, CODE_PORT_FORWARD, 20);
		break;
	}
	default:
		result = H21R2_ERR_UnknownMessage;
		break;
	}

	return result;
}

/*-----------------------------------------------------------*/
/* --- Get the port for a given UART. 
 */
uint8_t GetPort(UART_HandleTypeDef *huart){

	if(huart->Instance == USART4)
		return P1;
	else if(huart->Instance == USART2)
		return P2;
	else if(huart->Instance == USART6)
		return P3;
	else if(huart->Instance == USART1)
		return P4;
	else if(huart->Instance == USART5)
		return P5;

	
	return 0;
}

/*-----------------------------------------------------------*/

/* --- Register this module CLI Commands
 */
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

/*-----------------------------------------------------------*/


/* Module special task function (if needed) */
//void Module_Special_Task(void *argument){
//
//	/* Infinite loop */
//	uint8_t cases; // Test variable.
//	for(;;){
//		/*  */
//		switch(cases){
//
//
//			default:
//				osDelay(10);
//				break;
//		}
//
//		taskYIELD();
//	}
//
//}


/*-----------------------------------------------------------*/



/*-----------------------------------------------------------*/

/* -----------------------------------------------------------------------
 |								  APIs							          | 																 	|
/* -----------------------------------------------------------------------
 */
/*
 * @brief: To reset esp
 * @retval: status
 */

Module_Status ESP_Reset(void)
{
	  /* RESET pin fpr esp32 */
	Module_Status Status = H21R2_OK;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_Delay(5);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_Delay(1000);
	return Status;
}

/*
 * @brief: Booting Esp
 * @retval: status
 */
Module_Status ESP_Boot(void)
{
	  /* BOOT pin for esp32 */
	Module_Status Status = H21R2_OK;
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
	return Status;
}

/*
 * @brief: Initialize BLE client mode
 * @param1: pointer to a ClientName
 * @param2: pointer to a ServerName
 * @retval: status
 */

Module_Status BLE_ClientMode(char* ClientName,char* ServerName)
 {
	Module_Status Status = H21R2_ERROR;
	MX_USART3_UART_Init();
	int LenClientName, LenServerName;
	LenClientName = strlen(ClientName);
	LenServerName = strlen(ServerName);
	uint8_t Data[LenClientName + LenServerName + 3];
	Data[0] = CLIENT_MODE;
	Data[1] = LenClientName;
	Data[2] = LenServerName;
	memcpy(&Data[3], ClientName, LenClientName);
	memcpy(&Data[LenClientName + 3], ServerName, LenServerName);
	HAL_UART_Transmit(&huart3, Data, LenClientName + LenServerName + 3, 0xff);
	HAL_UART_Receive_DMA(&huart3, FullData, SIZEBUF);
	Status = H21R2_OK;
	return Status;
}
Module_Status BLE_ClientModeCLI(char* ClientName,char* ServerName,uint8_t LenClientName,uint8_t LenServerName)
 {
	Module_Status Status = H21R2_ERROR;
	MX_USART3_UART_Init();
	uint8_t Data[LenClientName + LenServerName + 3];
	Data[0] = CLIENT_MODE;
	Data[1] = LenClientName;
	Data[2] = LenServerName;
	memcpy(&Data[3], ClientName, LenClientName);
	memcpy(&Data[LenClientName + 3], ServerName, LenServerName);
	HAL_UART_Transmit(&huart3, Data, LenClientName + LenServerName + 3, 0xff);
	HAL_UART_Receive_DMA(&huart3, FullData, SIZEBUF);
	Status = H21R2_OK;
	return Status;
}
/*
 * @brief: Initialize BLE Server mode
 * @param1: pointer to a ServerName
 * @retval: status
 */
Module_Status BLE_ServerMode(char* ServerName)
{
	Module_Status Status = H21R2_ERROR;
	MX_USART3_UART_Init();
	int LenServerName;
	LenServerName = strlen(ServerName);
	uint8_t Data[LenServerName+2];
  	Data[0] = SERVER_MODE;
	Data[1]=LenServerName;
	memcpy(&Data[2], ServerName, LenServerName);
	HAL_UART_Transmit(&huart3, Data, LenServerName+2, 0xff);
	HAL_UART_Receive_DMA(&huart3, FullData, SIZEBUF);
	Status = H21R2_OK;
	return Status;

}

/*
 * @brief: To read from BLE in server mode   To read from BLE in clien mode
 * @param1: pointer to a Data
 * @param2: BLE_MODE
 * @retval: status
 */

Module_Status BLE_Read(char * Data,BLE_MODE function )
 {
	Module_Status Status = H21R2_ERROR;
	switch (function) {
	case server:
		Status = H21R2_OK;
		if ('H' == FullData[0] && 'Z' == FullData[1]) {

			memcpy(Data, &FullData[2], SIZEBLEBUFF);

		}
		break;
	case client:
		Status = H21R2_OK;
		uint8_t SendData;
		SendData = WRITE_FROM_CLIENT_MODE;
		if (1 == FullData[0]) {
			HAL_UART_Transmit(&huart3, &SendData, 1, 0xff);
			FullData[0] = 0;
		}
		HAL_Delay(10);
		if ('H' == FullData[0] && 'Z' == FullData[1]) {
			do {
				memcpy(Data, &FullData[2], SIZEBLEBUFF);

			} while (1 != FullData[0]);
			HAL_Delay(100);
		}

		break;
	}
	return Status;
}
/*
 * @brief: To read from WIFI Socket mode
 * @param1: pointer to a Data
 * @retval: status
 */

Module_Status WIFI_SocketRead(char * Data)
 {

	Module_Status Status = H21R2_ERROR;
	Status = H21R2_OK;

	if ('H' == WIFIBUFF[0] && 'Z' == WIFIBUFF[1]) {

		memcpy(Data, &WIFIBUFF[2], 126);

	}

	return Status;

}

/*
 * @brief: To Write from BLE in server mode   To Write from BLE in clien mode
 * @param1: pointer to a Data
 * @param2: BLE_MODE
 * @retval: status
 */

Module_Status BLE_Write(char* Data,BLE_MODE function ,uint16_t Size)
 {
	Module_Status Status = H21R2_ERROR;
	int LenData;
	LenData =Size;
	uint8_t SendData[LenData + 2];
	switch (function) {
	case server:
		Status = H21R2_OK;
		SendData[0] = WRITE_TO_SERVER_MODE;
		SendData[1] = LenData;
		memcpy(&SendData[2], Data, LenData);
		if (1 == FullData[0]) {
			HAL_UART_Transmit(&huart3, SendData, LenData + 2, 0xff);
			FullData[0] = 0;
		}
		break;
	case client:
		Status = H21R2_OK;
		SendData[0] = WRITE_TO_CLIENT_MODE;
		SendData[1] = LenData;
		memcpy(&SendData[2], Data, LenData);
		if (1 == FullData[0]) {
			HAL_UART_Transmit(&huart3, SendData, LenData + 2, 0xff);
			FullData[0] = 0;
			do {

			} while (1 != FullData[0]);
		}
		HAL_Delay(100);
		break;
	}
	return Status;
}
/*
 * @brief: To write to  WIFI Socket mode
 * @param1: pointer to a Data
 * @retval: status
 */
Module_Status WIFI_SocketWrite(char * Data , uint16_t Size)
 {
	Module_Status Status = H21R2_ERROR;
	int LenData;
	LenData = Size;
	uint8_t SendData[LenData + 2];
	Status = H21R2_OK;
	SendData[0] = WRITE_SOCKET_MODE;
	SendData[1] = LenData;
	memcpy(&SendData[2], Data, LenData);
	if (1 == WIFIBUFF[0]) {
		HAL_UART_Transmit(&huart3, SendData, LenData + 2, 0xff);
		WIFIBUFF[0] = 0;
	}
	return Status;

}
/*
 * @brief: Initialize WIFI access point  mode
 * @param1: pointer to a  Accesspoint name
 * @param2: pointer to a  Password
 * @retval: status
 */

Module_Status WIFI_AccessPoint(char* Ssid,char* Password)
{
	Module_Status Status = H21R2_OK;
	MX_USART3_UART_Init();
	int LenSsid,LenPassword;
	LenSsid = strlen(Ssid);
	LenPassword = strlen(Password);
	uint8_t Data[LenSsid+LenPassword+3];
  	Data[0] = WIFI_ACCESS_POINT_MODE;
	Data[1]=LenSsid;
	Data[2]=LenPassword;
	memcpy(&Data[3], Ssid, LenSsid);
	memcpy(&Data[LenSsid+3], Password, LenPassword);
	HAL_UART_Transmit(&huart3, Data, LenSsid+LenPassword+3, 0xff);
	return Status;
}

/*
 * @brief:  Initialize WIFI Socket mode
 * @param1: pointer to a  Accesspoint name
 * @param2: pointer to a  Password
 * @retval: status
 */

Module_Status WIFI_Socket(char* Ssid,char* Password)
{
	Module_Status Status = H21R2_OK;
	MX_USART3_UART_Init();
	int LenSsid,LenPassword;
	LenSsid = strlen(Ssid);
	LenPassword = strlen(Password);
	uint8_t Data[LenSsid+LenPassword+3];
  	Data[0] = WIFI_SOCKET_MODE;
	Data[1]=LenSsid;
	Data[2]=LenPassword;
	memcpy(&Data[3], Ssid, LenSsid);
	memcpy(&Data[LenSsid+3], Password, LenPassword);
	HAL_UART_Transmit(&huart3, Data, LenSsid+LenPassword+3, 0xff);
	HAL_UART_Receive_DMA(&huart3, WIFIBUFF, SIZEWIFIBUFF);
	return Status;
}

Module_Status WIFI_AccessPointCLI(char* Ssid,char* Password,uint8_t lenSsid,uint8_t lenPassword)
{
	ESP_Boot();
 	ESP_Reset();
	Module_Status Status = H21R2_OK;
	MX_USART3_UART_Init();
//	int LenSsid,LenPassword;
//	LenSsid = lenSsid;
//	LenPassword = lenPassword;
	uint8_t Data[lenSsid+lenPassword+3];
  	Data[0] = WIFI_ACCESS_POINT_MODE;
	Data[1]=lenSsid;
	Data[2]=lenPassword;
	memcpy(&Data[3], Ssid, lenSsid);
	memcpy(&Data[lenSsid+3], Password, lenPassword);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart3, Data, lenSsid+lenPassword+3, 0xff);
	return Status;
}

/*
 * @brief:  Initialize WIFI Station mode
 * @param1: pointer to a  Station  name
 * @param2: pointer to a  Password
 * @retval: status
 */

Module_Status WIFI_Station(char* Ssid,char* Password)
{
	Module_Status Status = H21R2_OK;
	MX_USART3_UART_Init();
	int LenSsid,LenPassword;
	LenSsid = strlen(Ssid);
	LenPassword = strlen(Password);
	uint8_t Data[LenSsid+LenPassword+3];
  	Data[0] = WIFI_STATION_MODE;
	Data[1]=LenSsid;
	Data[2]=LenPassword;
	memcpy(&Data[3], Ssid, LenSsid);
	memcpy(&Data[LenSsid+3], Password, LenPassword);
	HAL_UART_Transmit(&huart3, Data, LenSsid+LenPassword+3, 0xff);
	return Status;
}
Module_Status WIFI_StationCLI(char* Ssid,char* Password,uint8_t lenSsid,uint8_t lenPassword)
{
	ESP_Boot();
 	ESP_Reset();
	Module_Status Status = H21R2_OK;
	MX_USART3_UART_Init();
	int LenSsid,LenPassword;
	LenSsid =lenSsid;
	LenPassword = lenPassword;
	uint8_t Data[LenSsid+LenPassword+3];
  	Data[0] = WIFI_STATION_MODE;
	Data[1]=LenSsid;
	Data[2]=LenPassword;
	memcpy(&Data[3], Ssid, LenSsid);
	memcpy(&Data[LenSsid+3], Password, LenPassword);
	HAL_UART_Transmit(&huart3, Data, LenSsid+LenPassword+3, 0xff);
	return Status;
}

/*-----------------------------------------------------------*/


/* -----------------------------------------------------------------------
 |								Commands							      |
   -----------------------------------------------------------------------
 */

portBASE_TYPE CLI_ESPResetModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString ){
	Module_Status status = H21R2_OK;
	static const int8_t *pcOKMessage=(int8_t* )"ESP Reset!\n\r";
	static const int8_t *pcErrorsMessage =(int8_t* )"Error Params!\n\r";

		(void )xWriteBufferLen;
		configASSERT(pcWriteBuffer);

	 	status=ESP_Reset();

	 if(status == H21R2_OK)
	 {
			 sprintf((char* )pcWriteBuffer,(char* )pcOKMessage);

	 }

	 else if(status == H21R2_ERROR)
			strcpy((char* )pcWriteBuffer,(char* )pcErrorsMessage);


	return pdFALSE;

}
/*-----------------------------------------------------------*/
portBASE_TYPE CLI_ESPBootModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString ){
	Module_Status status = H21R2_OK;
	static const int8_t *pcOKMessage=(int8_t* )"ESP Booting!\n\r";
	static const int8_t *pcErrorsMessage =(int8_t* )"Error Params!\n\r";

		(void )xWriteBufferLen;
		configASSERT(pcWriteBuffer);

	 	status=ESP_Boot();

	 if(status == H21R2_OK)
	 {
			 sprintf((char* )pcWriteBuffer,(char* )pcOKMessage);

	 }

	 else if(status == H21R2_ERROR)
			strcpy((char* )pcWriteBuffer,(char* )pcErrorsMessage);


	return pdFALSE;

}
portBASE_TYPE CLI_ESPBleClientModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString ){
	Module_Status status = H21R2_OK;

	static const int8_t *pcOKMessage=(int8_t* )"ESP Client Mode On\n\r";
	static const int8_t *pcErrorsMessage =(int8_t* )"Error Params!\n\r";

	static int8_t *pcParameterString1, *pcParameterString2;
	portBASE_TYPE xParameterStringLength1 =0, xParameterStringLength2 =0;

		(void )xWriteBufferLen;


		pcParameterString1 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,1,&xParameterStringLength1);
		pcParameterString2 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,2,&xParameterStringLength2);

	 	status=BLE_ClientModeCLI(pcParameterString1 , pcParameterString2,xParameterStringLength1,xParameterStringLength2);

	 if(status == H21R2_OK)
	 {
			 sprintf((char* )pcWriteBuffer,(char* )pcOKMessage);

	 }

	 else if(status == H21R2_ERROR)
			strcpy((char* )pcWriteBuffer,(char* )pcErrorsMessage);


	return pdFALSE;

}

portBASE_TYPE CLI_ESPBleServerModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString ){
	Module_Status status = H21R2_OK;

	static const int8_t *pcOKMessage=(int8_t* )"ESP Server Mode On\n\r";
	static const int8_t *pcErrorsMessage =(int8_t* )"Error Params!\n\r";

	static int8_t *pcParameterString1;
	portBASE_TYPE xParameterStringLength1 =0;

		(void )xWriteBufferLen;
		configASSERT(pcWriteBuffer);


		pcParameterString1 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,1,&xParameterStringLength1);


	 	status=BLE_ServerMode(pcParameterString1);

	 if(status == H21R2_OK)
	 {
			 sprintf((char* )pcWriteBuffer,(char* )pcOKMessage);

	 }

	 else if(status == H21R2_ERROR)
			strcpy((char* )pcWriteBuffer,(char* )pcErrorsMessage);


	return pdFALSE;

}
portBASE_TYPE CLI_ESPWifiAccessPiontModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString ){
	Module_Status status = H21R2_OK;

	static const int8_t *pcOKMessage=(int8_t* )"ESP Wifi Access Point  Mode On\n\r";
	static const int8_t *pcErrorsMessage =(int8_t* )"Error Params!\n\r";

	static int8_t *pcParameterString1, *pcParameterString2;
	portBASE_TYPE xParameterStringLength1 =0, xParameterStringLength2 =0;


		(void )xWriteBufferLen;


		pcParameterString1 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,1,&xParameterStringLength1);
		pcParameterString2 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,2,&xParameterStringLength2);

	 	status=WIFI_AccessPointCLI(pcParameterString1 , pcParameterString2,xParameterStringLength1,xParameterStringLength2);

	 if(status == H21R2_OK)
	 {
			 sprintf((char* )pcWriteBuffer,(char* )pcOKMessage);

	 }

	 else if(status == H21R2_ERROR)
			strcpy((char* )pcWriteBuffer,(char* )pcErrorsMessage);


	return pdFALSE;

}
portBASE_TYPE CLI_ESPWifiStationModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString ){
	Module_Status status = H21R2_OK;

	static const int8_t *pcOKMessage=(int8_t* )"ESP Wifi Station  Mode On\n\r";
	static const int8_t *pcErrorsMessage =(int8_t* )"Error Params!\n\r";

	static int8_t *pcParameterString1, *pcParameterString2;
	portBASE_TYPE xParameterStringLength1 =0, xParameterStringLength2 =0;

		(void )xWriteBufferLen;


		pcParameterString1 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,1,&xParameterStringLength1);
		pcParameterString2 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,2,&xParameterStringLength2);

	 	status=WIFI_StationCLI(pcParameterString1 , pcParameterString2,xParameterStringLength1,xParameterStringLength2);

	 if(status == H21R2_OK)
	 {
			 sprintf((char* )pcWriteBuffer,(char* )pcOKMessage);

	 }

	 else if(status == H21R2_ERROR)
			strcpy((char* )pcWriteBuffer,(char* )pcErrorsMessage);


	return pdFALSE;

}
portBASE_TYPE CLI_ESPWriteToBleServerModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString ){
	Module_Status status = H21R2_OK;

	static const int8_t *pcOKMessage=(int8_t* )"ESP Write To Ble Server\n\r";
	static const int8_t *pcErrorsMessage =(int8_t* )"Error Params!\n\r";

	static int8_t *pcParameterString1;
	portBASE_TYPE xParameterStringLength1 =0;

		(void )xWriteBufferLen;


		pcParameterString1 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,1,&xParameterStringLength1);


	 	status=BLE_Write(pcParameterString1, server,strlen(pcParameterString1));

	 if(status == H21R2_OK)
	 {
			 sprintf((char* )pcWriteBuffer,(char* )pcOKMessage);

	 }

	 else if(status == H21R2_ERROR)
			strcpy((char* )pcWriteBuffer,(char* )pcErrorsMessage);


	return pdFALSE;

}
portBASE_TYPE CLI_ESPWriteToBleClientModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString ){
	Module_Status status = H21R2_OK;

	static const int8_t *pcOKMessage=(int8_t* )"ESP Write To Ble Client\n\r";
	static const int8_t *pcErrorsMessage =(int8_t* )"Error Params!\n\r";

	static int8_t *pcParameterString1;
	portBASE_TYPE xParameterStringLength1 =0;

		(void )xWriteBufferLen;


		pcParameterString1 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,1,&xParameterStringLength1);


	 	status=BLE_Write(pcParameterString1, client,strlen(pcParameterString1));

	 if(status == H21R2_OK)
	 {
			 sprintf((char* )pcWriteBuffer,(char* )pcOKMessage);

	 }

	 else if(status == H21R2_ERROR)
			strcpy((char* )pcWriteBuffer,(char* )pcErrorsMessage);


	return pdFALSE;

}
portBASE_TYPE CLI_ESPReadFromBleServerModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString ){
	Module_Status status = H21R2_OK;

	static const uint16_t *pcOKMessage=(uint16_t* )"Read Data From Server :%s \n\r";
	static const uint16_t *pcErrorsMessage =(uint16_t* )"Error Params!\n\r";

	static char Data[18];


		(void )xWriteBufferLen;


	 	status=BLE_Read(Data, server);

	 if(status == H21R2_OK)
	 {
			 sprintf((char* )pcWriteBuffer,(char* )pcOKMessage,Data);

	 }

	 else if(status == H21R2_ERROR)
			strcpy((char* )pcWriteBuffer,(char* )pcErrorsMessage);


	return pdFALSE;

}
portBASE_TYPE CLI_ESPReadFromBleClientModeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString ){
	Module_Status status = H21R2_OK;

	static const uint16_t *pcOKMessage=(uint16_t* )"Read Data From Client :%s \n\r";
	static const uint16_t *pcErrorsMessage =(uint16_t* )"Error Params!\n\r";

	static char Data[18];


		(void )xWriteBufferLen;


	 	status=BLE_Read(Data, client);

	 if(status == H21R2_OK)
	 {
			 sprintf((char* )pcWriteBuffer,(char* )pcOKMessage,Data);

	 }

	 else if(status == H21R2_ERROR)
			strcpy((char* )pcWriteBuffer,(char* )pcErrorsMessage);


	return pdFALSE;

}

/*-----------------------------------------------------------*/

/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
