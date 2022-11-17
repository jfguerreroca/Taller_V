/**
 ******************************************************************************
 * @file           : main.c
 * @author         : jfguerreroca
 * @name           : Juan Felipe Guerrero Cataño
 * @brief          : Tarea 4
 ******************************************************************************
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "stm32f4xx.h"
#include "BasicTimer.h"
#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "AdcDriver.h"
#include "ExtiDriver.h"
#include "I2CDriver.h"
#include "PwmDriver.h"
#include "RTCxDriver.h"
#include "LCDxDriver.h"
#include "RCC100xDriver.h"

// Definimos los handler para los perifericos necesarios en el proyecto

USART_Handler_t 		handlerUSART2			= {0};

BasicTimer_Handler_t 	handlerTimer2 			= {0};

GPIO_Handler_t 			handlerBlinkyLed 		= {0};
GPIO_Handler_t			handlerTxPin			= {0};
GPIO_Handler_t			handlerRxPin			= {0};


// Definimos las variables que utilizaremos

uint8_t flagStatus 				= 0;
uint8_t rxData	 				= 0;


// Definimos las funciones que vamos a utilizar:

//Funcion de estado con el User LED

void statusLED(void);

//Funcion para inicializar las configuraciones del sistema

void InitSystem(void);

// Funcion principal del programa

int main(void)
{

	set100MHzRCC();

	InitSystem();

	configTimers();

	writeChar(&handlerUSART2, 'a');


	/* Ciclo infinito del main */
	while(1){


		// Condicional para la interrupcion del blinky
		if(flagStatus){
			flagStatus = 0;
			statusLED();
		}

	}

	return 0;
}

void InitSystem(void){

	handlerBlinkyLed.pGPIOx = GPIOA;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinNumber 				= PIN_5;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyLed);

	handlerTxPin.pGPIOx = GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber 					= PIN_2;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode				= AF7;
	GPIO_Config(&handlerTxPin);

	handlerRxPin.pGPIOx = GPIOA;
	handlerRxPin.GPIO_PinConfig.GPIO_PinNumber 					= PIN_3;
	handlerRxPin.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
	handlerRxPin.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerRxPin.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerRxPin.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerRxPin.GPIO_PinConfig.GPIO_PinAltFunMode				= AF7;
	GPIO_Config(&handlerRxPin);

	handlerTimer2.ptrTIMx 										= TIM2;
	handlerTimer2.TIMx_Config.TIMx_mode 						= BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed						= BTIMER_SPEED_RCC100_100us;
	handlerTimer2.TIMx_Config.TIMx_period						= 2500;
	handlerTimer2.TIMx_Config.TIMx_interruptEnable 				= 1;
	BasicTimer_Config(&handlerTimer2);
	startTimer(&handlerTimer2);

	handlerUSART2.ptrUSARTx 									= USART2;
	handlerUSART2.USART_Config.USART_mode 						= USART_MODE_RXTX;
	handlerUSART2.USART_Config.USART_baudrate 					= USART_BAUDRATE_57600;
	handlerUSART2.USART_Config.USART_datasize 					= USART_DATASIZE_9BIT;
	handlerUSART2.USART_Config.USART_parity 					= USART_PARITY_ODD;
	handlerUSART2.USART_Config.USART_stopbits             		= USART_STOPBIT_1;
	handlerUSART2.USART_Config.USART_enableIntRX          		= USART_RX_INTERRUP_ENABLE;
	handlerUSART2.USART_Config.USART_RCC						= USART_RCC_100MHZ;
	USART_Config(&handlerUSART2);
}

void BasicTimer2_Callback(void){
	flagStatus = 1;
}

void usart2Rx_Callback(void){
	rxData = getRxData();
}

void statusLED(void){
	GPIOxTogglePin(&handlerBlinkyLed);
}

