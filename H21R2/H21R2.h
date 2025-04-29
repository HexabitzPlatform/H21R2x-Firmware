/*
 BitzOS (BOS) V0.3.6 - Copyright (C) 2017-2024 Hexabitz
 All rights reserved
 
 File Name     : H21R2.h
 Description   : Header file for module H21R2.
 	 	 	 	 (Description_of_module)

(Description of Special module peripheral configuration):
>>
>>
>>

 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef H21R2_H
#define H21R2_H

/* Includes ------------------------------------------------------------------*/
#include "BOS.h"
#include "H21R2_MemoryMap.h"
#include "H21R2_uart.h"
#include "H21R2_gpio.h"
#include "H21R2_dma.h"
#include "H21R2_inputs.h"
#include "H21R2_eeprom.h"
/* Exported definitions -------------------------------------------------------*/
#define	MODULE_PN		_H21R2

/* Port-related definitions */
#define	NUM_OF_PORTS			5
#define P_PROG 				P2						/* ST factory bootloader UART */

/* Define Available Ports */
#define _P1
#define _P2
#define _P3
#define _P4
#define _P5

/* Define Available USARTs */
#define _USART1
#define _USART2
#define _USART4
#define _USART5
#define _USART6

/* Port-UART mapping */
#define UART_P1 &huart4
#define UART_P2 &huart2
#define UART_P3 &huart6
#define UART_P4 &huart1
#define UART_P5 &huart5

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

/* Module-specific Definitions */

/* Indicator LED */
#define _IND_LED_PORT			GPIOB
#define _IND_LED_PIN			GPIO_PIN_14

#define NUM_MODULE_PARAMS		1

/* Module GPIO Pinout */
#define ESP32_BOOT_PIN         GPIO_PIN_13
#define ESP32_BOOT_PORT        GPIOC
#define ESP32_RST_PIN          GPIO_PIN_0
#define ESP32_RST_PORT         GPIOD
#define ESP32_UART_HANDEL      huart3
#define USART3_RX_Pin          GPIO_PIN_9
#define USART3_TX_Pin          GPIO_PIN_8
/* Module special parameters */
#define SIZEBUF                    22
#define SIZEBLEBUFF                20
#define SIZEWIFIBUFF               128

#define CLIENT_MODE                1
#define SERVER_MODE                2
#define WIFI_ACCESS_POINT_MODE     3
#define WIFI_STATION_MODE          4
#define WRITE_TO_SERVER_MODE       5
#define WRITE_TO_CLIENT_MODE       6
#define WRITE_FROM_CLIENT_MODE     7
#define WIFI_SOCKET_MODE           8
#define WRITE_SOCKET_MODE          9


/* Module_Status Type Definition */
typedef enum {
	H21R2_OK =0,
	H21R2_ERR_UnknownMessage,
	H21R2_ERR_WrongParams,
	H21R2_ERROR =255
} Module_Status;

typedef enum {
	server=0,
	client,
}BLE_MODE;

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

/* -----------------------------------------------------------------------
 |								  APIs							          |  																 	|
/* -----------------------------------------------------------------------
 */
Module_Status ESP_Reset(void);
Module_Status ESP_Boot(void);
Module_Status BLE_ClientMode(char* Client_Name,char* Server_Name);
Module_Status BLE_ServerMode(char* ServerName);
Module_Status BLE_Read(char * Data,BLE_MODE function);
Module_Status BLE_Write(char* Data,BLE_MODE function ,uint16_t Size);
Module_Status WIFI_AccessPoint(char* Ssid,char* Password);
Module_Status WIFI_Station(char* Ssid,char* Password);
Module_Status WIFI_Socket(char* Ssid,char* Password);
Module_Status WIFI_SocketWrite(char * Data , uint16_t Size) ;
Module_Status WIFI_SocketRead(char * Data);


#endif /* H21R2_H */

/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
