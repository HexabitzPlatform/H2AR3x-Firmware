/*
 BitzOS (BOS) V0.2.9 - Copyright (C) 2017-2023 Hexabitz
 All rights reserved

 File Name     : main.c
 Description   : Main program body.
 */
/* Includes ------------------------------------------------------------------*/
#include "BOS.h"
volatile float V;
volatile float I;
/* Private variables ---------------------------------------------------------*/
float timeout=6000;
/* Private function prototypes -----------------------------------------------*/

/* Main function ------------------------------------------------------------*/

int main(void){

	Module_Init();		//Initialize Module &  BitzOS

	//Don't place your code here.
	for(;;){}
}

/*-----------------------------------------------------------*/

/* User Task */
void UserTask(void *argument){
	     V=7;
	   //  BOSMessaging.trace=TRACE_NONE;

		 AddBOSvar(FMT_FLOAT, (uint32_t)&V);
	//	 AddBOSvar(FMT_FLOAT, (uint32_t)&I);
		// WriteRemote(2, (uint32_t) &V, 1, FMT_FLOAT,0);
	while(1){
//	V=5;
//	WriteRemote(2, (uint32_t) &V, 1, FMT_FLOAT,0);
//    Delay_ms(1000);
//    SendMessageToModule(2, CODE_H2AR3_SAMPLE_A, 0);
//    V=10;
//    WriteRemote(2, (uint32_t) &V, 1, FMT_FLOAT,0);
//    Delay_ms(1000);
	}
}

/*-----------------------------------------------------------*/
