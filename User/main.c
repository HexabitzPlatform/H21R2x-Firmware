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
char d[8]={"abcdefg"};
uint8_t g[8] ;
/* User Task */
void UserTask(void *argument){

	ESP_Boot();
	ESP_Reset();
		 MX_USART3_UART_Init();
	HAL_Delay(1000);
	g[0]=2;
	memcpy(&g[1], &d[0], 8);

	 HAL_UART_Transmit(&huart3, &g, 8, 0xff);
	// put your code here, to run repeatedly.
	while(1){

	}
}

/*-----------------------------------------------------------*/
