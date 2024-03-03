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
char d[50]={"DESKTOP-72Q66QF 7257"};
char e[10]={"00000000"};
uint8_t k[40];
int x ;
/* User Task */
void UserTask(void *argument){


	ESP_BootMode();
 	ESP_ResetMode();
//ESP_WifiAccessPoint("reda", "12345678");
//HAL_Delay(5000);
ESP_WifiStation(d, e);
//	 put your code here, to run repeatedly.
	while(1){

	}
}

/*-----------------------------------------------------------*/
