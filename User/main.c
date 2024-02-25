/*
 BitzOS (BOS) V0.2.9 - Copyright (C) 2017-2023 Hexabitz
 All rights reserved

 File Name     : main.c
 Description   : Main program body.
 */
/* Includes ------------------------------------------------------------------*/
#include "BOS.h"

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Main function ------------------------------------------------------------*/

int main(void){

	Module_Init();		//Initialize Module &  BitzOS

	//Don't place your code here.
	for(;;){}
}

/*-----------------------------------------------------------*/
//char ssi[50]={"DESKTOP-72Q66QF 7257"};
//char pas[20]={"00000000"};
uint8_t Data[60];

/* User Task */
void UserTask(void *argument){

	ESP_BootMode();
    ESP_ResetMode();
    HAL_Delay(2000);
	MX_USART3_UART_Init();
	ESP_ReadFromServer(Data);
	HAL_Delay(2000);
	ESP_ServerMode("mreda");



	// put your code here, to run repeatedly.
	while(1){

	}
}

/*-----------------------------------------------------------*/
