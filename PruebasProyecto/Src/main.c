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
#include "SysTickDriver.h"

// Definimos los handler para los perifericos necesarios en el proyecto

USART_Handler_t 				handlerUSART2			= {0};

BasicTimer_Handler_t 	        handlerTimer2 			= {0};

GPIO_Handler_t 					handlerBlinkyLed 		= {0};

GPIO_Handler_t 					handlerStepperA_X 		= {0};
GPIO_Handler_t 					handlerStepperB_X 		= {0};
GPIO_Handler_t 					handlerStepperC_X 		= {0};
GPIO_Handler_t 					handlerStepperD_X 		= {0};

GPIO_Handler_t 					handlerStepperA_Y1 		= {0};
GPIO_Handler_t 					handlerStepperB_Y1 		= {0};
GPIO_Handler_t 					handlerStepperC_Y1 		= {0};
GPIO_Handler_t 					handlerStepperD_Y1 		= {0};

GPIO_Handler_t 					handlerStepperA_Y2 		= {0};
GPIO_Handler_t 					handlerStepperB_Y2 		= {0};
GPIO_Handler_t 					handlerStepperC_Y2 		= {0};
GPIO_Handler_t 					handlerStepperD_Y2 		= {0};

// Definimos las variables que utilizaremos

uint8_t flagStatus 										= 0;

// Definimos las funciones que vamos a utilizar:

//Funcion para inicializar las configuraciones del sistema

void InitSystem(void);

void SMlowX(void);

void SMlowY1(void);

void SMlowY2(void);

void StepperMotorX(uint8_t);

void StepperMotorY1(uint8_t);

void StepperMotorY2(uint8_t);

// Funcion principal del programa

int main(void)
{
	InitSystem();

	/* Ciclo infinito del main */
	while(1){
		// Condicional para la interrupcion del blinky
		if(flagStatus){
			flagStatus = 0;
			GPIOxTogglePin(&handlerBlinkyLed);
		}

		//Secuencia Y1

		SMlowY1();
		SMlowY2();
		StepperMotorY1(1);
		StepperMotorY2(1);
		for (int i=0;(i<6000);i++){
			__NOP();
		}
		SMlowY1();
		SMlowY2();
		StepperMotorY1(2);
		StepperMotorY2(2);
		for (int i=0;(i<6000);i++){
			__NOP();
		}
		SMlowY1();
		SMlowY2();
		StepperMotorY1(3);
		StepperMotorY2(3);
		for (int i=0;(i<6000);i++){
			__NOP();
		}
		SMlowY1();
		SMlowY2();
		StepperMotorY1(4);
		StepperMotorY2(4);
		for (int i=0;(i<6000);i++){
			__NOP();
		}
	}

	return 0;
}

void InitSystem(void){

	handlerBlinkyLed.pGPIOx 										= GPIOA;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinNumber 					= PIN_5;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyLed);

//	handlerStepperA_X.pGPIOx 										= GPIOA;
//	handlerStepperA_X.GPIO_PinConfig.GPIO_PinNumber 				= PIN_11;
//	handlerStepperA_X.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
//	handlerStepperA_X.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
//	handlerStepperA_X.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_MEDIUM;
//	handlerStepperA_X.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
//	GPIO_Config(&handlerStepperA_X);
//	GPIO_WritePin(&handlerStepperA_X, RESET);
//
//	handlerStepperB_X.pGPIOx 										= GPIOA;
//	handlerStepperB_X.GPIO_PinConfig.GPIO_PinNumber 				= PIN_12;
//	handlerStepperB_X.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
//	handlerStepperB_X.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
//	handlerStepperB_X.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_MEDIUM;
//	handlerStepperB_X.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
//	GPIO_Config(&handlerStepperB_X);
//	GPIO_WritePin(&handlerStepperB_X, RESET);
//
//	handlerStepperC_X.pGPIOx 										= GPIOA;
//	handlerStepperC_X.GPIO_PinConfig.GPIO_PinNumber 				= PIN_9;
//	handlerStepperC_X.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
//	handlerStepperC_X.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
//	handlerStepperC_X.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_MEDIUM;
//	handlerStepperC_X.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
//	GPIO_Config(&handlerStepperC_X);
//	GPIO_WritePin(&handlerStepperC_X, RESET);
//
//	handlerStepperD_X.pGPIOx 										= GPIOA;
//	handlerStepperD_X.GPIO_PinConfig.GPIO_PinNumber 				= PIN_10;
//	handlerStepperD_X.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
//	handlerStepperD_X.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
//	handlerStepperD_X.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_MEDIUM;
//	handlerStepperD_X.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
//	GPIO_Config(&handlerStepperD_X);
//	GPIO_WritePin(&handlerStepperD_X, RESET);

	handlerStepperA_Y1.pGPIOx 										= GPIOA;
	handlerStepperA_Y1.GPIO_PinConfig.GPIO_PinNumber 				= PIN_6;
	handlerStepperA_Y1.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperA_Y1.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperA_Y1.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperA_Y1.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperA_Y1);
	GPIO_WritePin(&handlerStepperA_Y1, RESET);

	handlerStepperB_Y1.pGPIOx 										= GPIOA;
	handlerStepperB_Y1.GPIO_PinConfig.GPIO_PinNumber 				= PIN_7;
	handlerStepperB_Y1.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperB_Y1.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperB_Y1.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperB_Y1.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperB_Y1);
	GPIO_WritePin(&handlerStepperB_Y1, RESET);

	handlerStepperC_Y1.pGPIOx 										= GPIOA;
	handlerStepperC_Y1.GPIO_PinConfig.GPIO_PinNumber 				= PIN_8;
	handlerStepperC_Y1.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperC_Y1.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperC_Y1.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperC_Y1.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperC_Y1);
	GPIO_WritePin(&handlerStepperC_Y1, RESET);

	handlerStepperD_Y1.pGPIOx 										= GPIOA;
	handlerStepperD_Y1.GPIO_PinConfig.GPIO_PinNumber 				= PIN_9;
	handlerStepperD_Y1.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperD_Y1.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperD_Y1.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperD_Y1.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperD_Y1);
	GPIO_WritePin(&handlerStepperD_Y1, RESET);

	handlerStepperA_Y2.pGPIOx 										= GPIOB;
	handlerStepperA_Y2.GPIO_PinConfig.GPIO_PinNumber 				= PIN_1;
	handlerStepperA_Y2.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperA_Y2.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperA_Y2.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperA_Y2.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperA_Y2);
	GPIO_WritePin(&handlerStepperA_Y2, RESET);

	handlerStepperB_Y2.pGPIOx 										= GPIOB;
	handlerStepperB_Y2.GPIO_PinConfig.GPIO_PinNumber 				= PIN_2;
	handlerStepperB_Y2.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperB_Y2.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperB_Y2.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperB_Y2.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperB_Y2);
	GPIO_WritePin(&handlerStepperB_Y2, RESET);

	handlerStepperC_Y2.pGPIOx 										= GPIOB;
	handlerStepperC_Y2.GPIO_PinConfig.GPIO_PinNumber 				= PIN_14;
	handlerStepperC_Y2.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperC_Y2.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperC_Y2.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperC_Y2.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperC_Y2);
	GPIO_WritePin(&handlerStepperC_Y2, RESET);

	handlerStepperD_Y2.pGPIOx 										= GPIOB;
	handlerStepperD_Y2.GPIO_PinConfig.GPIO_PinNumber 				= PIN_15;
	handlerStepperD_Y2.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperD_Y2.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperD_Y2.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperD_Y2.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperD_Y2);
	GPIO_WritePin(&handlerStepperD_Y2, RESET);

	handlerTimer2.ptrTIMx 											= TIM2;
	handlerTimer2.TIMx_Config.TIMx_mode 							= BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed							= BTIMER_SPEED_100us;
	handlerTimer2.TIMx_Config.TIMx_period							= 2500;
	handlerTimer2.TIMx_Config.TIMx_interruptEnable 					= 1;
	BasicTimer_Config(&handlerTimer2);

	handlerUSART2.ptrUSARTx 										= USART2;
	handlerUSART2.USART_Config.USART_mode 							= USART_MODE_RXTX;
	handlerUSART2.USART_Config.USART_baudrate 						= USART_BAUDRATE_115200;
	handlerUSART2.USART_Config.USART_datasize 						= USART_DATASIZE_8BIT;
	handlerUSART2.USART_Config.USART_parity 						= USART_PARITY_NONE;
	handlerUSART2.USART_Config.USART_stopbits             			= USART_STOPBIT_1;
	handlerUSART2.USART_Config.USART_enableIntRX          			= USART_RX_INTERRUP_ENABLE;
	USART_Config(&handlerUSART2);
}

void StepperMotorX(uint8_t motor){
	if(motor == 1){
		GPIO_WritePin(&handlerStepperA_X, SET);
		GPIO_WritePin(&handlerStepperB_X, RESET);
		GPIO_WritePin(&handlerStepperC_X, RESET);
		GPIO_WritePin(&handlerStepperD_X, RESET);
	}else if(motor == 2){
		GPIO_WritePin(&handlerStepperA_X, RESET);
		GPIO_WritePin(&handlerStepperB_X, SET);
		GPIO_WritePin(&handlerStepperC_X, RESET);
		GPIO_WritePin(&handlerStepperD_X, RESET);
	}else if(motor == 3){
		GPIO_WritePin(&handlerStepperA_X, RESET);
		GPIO_WritePin(&handlerStepperB_X, RESET);
		GPIO_WritePin(&handlerStepperC_X, SET);
		GPIO_WritePin(&handlerStepperD_X, RESET);
	}else if(motor == 4){
		GPIO_WritePin(&handlerStepperA_X, RESET);
		GPIO_WritePin(&handlerStepperB_X, RESET);
		GPIO_WritePin(&handlerStepperC_X, RESET);
		GPIO_WritePin(&handlerStepperD_X, SET);
	}
}

void StepperMotorY1(uint8_t motor){
	if(motor == 1){
		GPIO_WritePin(&handlerStepperA_Y1, SET);
		GPIO_WritePin(&handlerStepperB_Y1, RESET);
		GPIO_WritePin(&handlerStepperC_Y1, RESET);
		GPIO_WritePin(&handlerStepperD_Y1, RESET);
	}else if(motor == 2){
		GPIO_WritePin(&handlerStepperA_Y1, RESET);
		GPIO_WritePin(&handlerStepperB_Y1, SET);
		GPIO_WritePin(&handlerStepperC_Y1, RESET);
		GPIO_WritePin(&handlerStepperD_Y1, RESET);
	}else if(motor == 3){
		GPIO_WritePin(&handlerStepperA_Y1, RESET);
		GPIO_WritePin(&handlerStepperB_Y1, RESET);
		GPIO_WritePin(&handlerStepperC_Y1, SET);
		GPIO_WritePin(&handlerStepperD_Y1, RESET);
	}else if(motor == 4){
		GPIO_WritePin(&handlerStepperA_Y1, RESET);
		GPIO_WritePin(&handlerStepperB_Y1, RESET);
		GPIO_WritePin(&handlerStepperC_Y1, RESET);
		GPIO_WritePin(&handlerStepperD_Y1, SET);
	}
}

void StepperMotorY2(uint8_t motor){
	if(motor == 1){
		GPIO_WritePin(&handlerStepperA_Y2, SET);
		GPIO_WritePin(&handlerStepperB_Y2, RESET);
		GPIO_WritePin(&handlerStepperC_Y2, RESET);
		GPIO_WritePin(&handlerStepperD_Y2, RESET);
	}else if(motor == 2){
		GPIO_WritePin(&handlerStepperA_Y2, RESET);
		GPIO_WritePin(&handlerStepperB_Y2, SET);
		GPIO_WritePin(&handlerStepperC_Y2, RESET);
		GPIO_WritePin(&handlerStepperD_Y2, RESET);
	}else if(motor == 3){
		GPIO_WritePin(&handlerStepperA_Y2, RESET);
		GPIO_WritePin(&handlerStepperB_Y2, RESET);
		GPIO_WritePin(&handlerStepperC_Y2, SET);
		GPIO_WritePin(&handlerStepperD_Y2, RESET);
	}else if(motor == 4){
		GPIO_WritePin(&handlerStepperA_Y2, RESET);
		GPIO_WritePin(&handlerStepperB_Y2, RESET);
		GPIO_WritePin(&handlerStepperC_Y2, RESET);
		GPIO_WritePin(&handlerStepperD_Y2, SET);
	}
}

void SMlowX(void){
	GPIO_WritePin(&handlerStepperA_X, RESET);
	GPIO_WritePin(&handlerStepperB_X, RESET);
	GPIO_WritePin(&handlerStepperC_X, RESET);
	GPIO_WritePin(&handlerStepperD_X, RESET);
}

void SMlowY1(void){
	GPIO_WritePin(&handlerStepperA_Y1, RESET);
	GPIO_WritePin(&handlerStepperB_Y1, RESET);
	GPIO_WritePin(&handlerStepperC_Y1, RESET);
	GPIO_WritePin(&handlerStepperD_Y1, RESET);
}

void SMlowY2(void){
	GPIO_WritePin(&handlerStepperA_Y2, RESET);
	GPIO_WritePin(&handlerStepperB_Y2, RESET);
	GPIO_WritePin(&handlerStepperC_Y2, RESET);
	GPIO_WritePin(&handlerStepperD_Y2, RESET);
}

void BasicTimer2_Callback(void){
	flagStatus = 1;
}
