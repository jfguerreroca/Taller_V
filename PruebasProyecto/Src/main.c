/*
 *
 ******************************************************************************
 * @file           : main.c
 * @author         : jfguerreroca
 * @name           : Juan Felipe Guerrero Cataño
 * @brief          : Tarea 6
 ******************************************************************************
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "BasicTimer.h"
#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "PwmDriver.h"
#include "CaptureFrecDriver.h"

// Definimos los handler para los perifericos necesarios en el proyecto

USART_Handler_t 				handlerUSART2			= {0};

BasicTimer_Handler_t 	        handlerTimer2 			= {0};

GPIO_Handler_t 					handlerBlinkyLed 		= {0};
GPIO_Handler_t 					handlerStepperA 		= {0};
GPIO_Handler_t 					handlerStepperB 		= {0};
GPIO_Handler_t 					handlerStepperC 		= {0};
GPIO_Handler_t 					handlerStepperD 		= {0};


// Definimos las variables que utilizaremos

uint8_t flagStatus 										= 0;

// Definimos las funciones que vamos a utilizar:

//Funcion para inicializar las configuraciones del sistema

void InitSystem(void);

void SMlow(void);

void StepperMotor(uint8_t);

// Funcion principal del programa

int main(void)
{
	InitSystem();
	//1,4,2,3


	/* Ciclo infinito del main */
	while(1){
		// Condicional para la interrupcion del blinky
		if(flagStatus){
			flagStatus = 0;
			GPIOxTogglePin(&handlerBlinkyLed);
		}

		SMlow();
		StepperMotor(1);
		for (int i=0;(i<4500);i++){
			__NOP();
		}
		SMlow();
		StepperMotor(4);
		for (int i=0;(i<4500);i++){
			__NOP();
		}
		SMlow();
		StepperMotor(2);
		for (int i=0;(i<4500);i++){
			__NOP();
		}
		SMlow();
		StepperMotor(3);
		for (int i=0;(i<4500);i++){
			__NOP();
		}

	}

	return 0;
}

void InitSystem(void){

	handlerBlinkyLed.pGPIOx 									= GPIOA;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinNumber 				= PIN_5;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyLed);

	handlerStepperA.pGPIOx 										= GPIOA;
	handlerStepperA.GPIO_PinConfig.GPIO_PinNumber 				= PIN_11;
	handlerStepperA.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerStepperA.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperA.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperA.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperA);
	GPIO_WritePin(&handlerStepperA, RESET);

	handlerStepperB.pGPIOx 										= GPIOA;
	handlerStepperB.GPIO_PinConfig.GPIO_PinNumber 				= PIN_12;
	handlerStepperB.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerStepperB.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperB.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperB.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperB);
	GPIO_WritePin(&handlerStepperB, RESET);

	handlerStepperC.pGPIOx 										= GPIOA;
	handlerStepperC.GPIO_PinConfig.GPIO_PinNumber 				= PIN_9;
	handlerStepperC.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerStepperC.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperC.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperC.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperC);
	GPIO_WritePin(&handlerStepperC, RESET);

	handlerStepperD.pGPIOx 										= GPIOA;
	handlerStepperD.GPIO_PinConfig.GPIO_PinNumber 				= PIN_10;
	handlerStepperD.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerStepperD.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperD.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperD.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperD);
	GPIO_WritePin(&handlerStepperD, RESET);

	handlerTimer2.ptrTIMx 										= TIM2;
	handlerTimer2.TIMx_Config.TIMx_mode 						= BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed						= BTIMER_SPEED_100us;
	handlerTimer2.TIMx_Config.TIMx_period						= 2500;
	handlerTimer2.TIMx_Config.TIMx_interruptEnable 				= 1;
	BasicTimer_Config(&handlerTimer2);

	handlerUSART2.ptrUSARTx 									= USART2;
	handlerUSART2.USART_Config.USART_mode 						= USART_MODE_RXTX;
	handlerUSART2.USART_Config.USART_baudrate 					= USART_BAUDRATE_115200;
	handlerUSART2.USART_Config.USART_datasize 					= USART_DATASIZE_8BIT;
	handlerUSART2.USART_Config.USART_parity 					= USART_PARITY_NONE;
	handlerUSART2.USART_Config.USART_stopbits             		= USART_STOPBIT_1;
	handlerUSART2.USART_Config.USART_enableIntRX          		= USART_RX_INTERRUP_ENABLE;
	USART_Config(&handlerUSART2);
}

void StepperMotor(uint8_t motor){
	if(motor == 1){
		GPIO_WritePin(&handlerStepperA, SET);
		GPIO_WritePin(&handlerStepperB, RESET);
		GPIO_WritePin(&handlerStepperC, RESET);
		GPIO_WritePin(&handlerStepperD, RESET);
	}else if(motor == 2){
		GPIO_WritePin(&handlerStepperA, RESET);
		GPIO_WritePin(&handlerStepperB, SET);
		GPIO_WritePin(&handlerStepperC, RESET);
		GPIO_WritePin(&handlerStepperD, RESET);
	}else if(motor == 3){
		GPIO_WritePin(&handlerStepperA, RESET);
		GPIO_WritePin(&handlerStepperB, RESET);
		GPIO_WritePin(&handlerStepperC, SET);
		GPIO_WritePin(&handlerStepperD, RESET);
	}else if(motor == 4){
		GPIO_WritePin(&handlerStepperA, RESET);
		GPIO_WritePin(&handlerStepperB, RESET);
		GPIO_WritePin(&handlerStepperC, RESET);
		GPIO_WritePin(&handlerStepperD, SET);
	}
}

void SMlow(void){
	GPIO_WritePin(&handlerStepperA, RESET);
	GPIO_WritePin(&handlerStepperB, RESET);
	GPIO_WritePin(&handlerStepperC, RESET);
	GPIO_WritePin(&handlerStepperD, RESET);
}

void BasicTimer2_Callback(void){
	flagStatus = 1;
}
