/*
 BitzOS (BOS) V0.3.1 - Copyright (C) 2017-2024 Hexabitz
 All rights reserved

 File Name     : main.c
 Description   : Main program body.
 */
/* Includes ------------------------------------------------------------------*/
#include "BOS.h"

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Main function ------------------------------------------------------------*/
char Data[100];
int main(void){

	Module_Init();		//Initialize Module &  BitzOS

	//Don't place your code here.
	for(;;){}
}

/*-----------------------------------------------------------*/
extern int r ,i ;
/* User Task */
void UserTask(void *argument){

ESP_Boot();
ESP_Reset();
WIFI_AP_SOCKET("Redaa", "00000000");
//	 put your code here, to run repeatedly.
	while(1){
//		HAL_Delay(2000);
		SOCKET_WRITE("123456789qwertyuiopasdfghjklzxcvbnm");
		SOCKET_READ(Data);
	}
}

/*-----------------------------------------------------------*/
