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
char Data[20]={"1da1w"};
uint8_t k[40];
/* User Task */
void UserTask(void *argument){

	ESP_BootMode();
	ESP_ResetMode();
	MX_USART3_UART_Init();
	HAL_Delay(1000);
	ESP_ServerMode("mreda");


	// put your code here, to run repeatedly.
	while(1){
		ESP_ReadFromServer(k);
		ESP_WriteToServer(Data);

	}
}

/*-----------------------------------------------------------*/
