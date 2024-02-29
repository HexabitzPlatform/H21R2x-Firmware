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

int main(void){

	Module_Init();		//Initialize Module &  BitzOS

	//Don't place your code here.
	for(;;){}
}

/*-----------------------------------------------------------*/
char Data[10]={"reda1239"};
uint8_t k[40];
int x ;
/* User Task */
void UserTask(void *argument){

	ESP_BootMode();
 	ESP_ResetMode();

    ESP_ClientMode("Reda", "Reda");
//	ESP_ServerMode("Reda");
//	 put your code here, to run repeatedly.
	while(1){
		ESP_BleWrite(Data, client);
		ESP_BleRead(k, client);
//		ESP_BleWrite(Data, server);
//		ESP_BleRead(k, server);
	}
}

/*-----------------------------------------------------------*/
