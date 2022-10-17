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
#include <stdbool.h>
#include "stm32f4xx.h"
#include "BasicTimer.h"
#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "AdcDriver.h"
#include "ExtiDriver.h"

// Definimos los handler para los perifericos necesarios en el proyecto

USART_Handler_t 		handlerUSART2			= {0};

BasicTimer_Handler_t 	handlerTimer2 			= {0};
BasicTimer_Handler_t 	handlerTimer4 			= {0};

GPIO_Handler_t 			handlerBlinkyLed 		= {0};
GPIO_Handler_t			handlerEncoderButton	= {0};
GPIO_Handler_t			handlerTxPin			= {0};
GPIO_Handler_t			handlerData				= {0};
GPIO_Handler_t			handlerClock			= {0};

GPIO_Handler_t			handlerA				= {0};
GPIO_Handler_t			handlerB				= {0};
GPIO_Handler_t			handlerC				= {0};
GPIO_Handler_t			handlerD				= {0};
GPIO_Handler_t			handlerE				= {0};
GPIO_Handler_t			handlerF				= {0};
GPIO_Handler_t			handlerG				= {0};

GPIO_Handler_t			handlerSS1				= {0};
GPIO_Handler_t			handlerSS2				= {0};

EXTI_Config_t			EXTI_Data				= {0};
EXTI_Config_t			EXTI_Button				= {0};

// Definimos las variables que utilizaremos

uint8_t blinky 			= 0;
uint8_t data 			= 0;
uint8_t dataSS 			= 0;
uint8_t clock 			= 0;
uint8_t buttonStatus 	= 0;
uint8_t flagStatus 		= 0;
uint8_t flagButton 		= 0;
uint8_t flagClock		= 0;
uint8_t flagSS 			= 0;
uint8_t number 			= 0;
uint8_t num1 			= 0;
uint8_t num2 			= 0;
uint8_t position 		= 0;
char bufferData [64] 	= {0};

// Definimos las funciones que vamos a utilizar

//Funcion de numeros del 7 segmentos

void numberSS(int);

//Funcion de estado con el User LED

void statusLED(void);

// Funcion principal del programa

int main(void)
{
	/*
	 * Cargamos las configuraciones de cada periferico a utilizar:
	 *
	 */

	handlerA.pGPIOx = GPIOC;
	handlerA.GPIO_PinConfig.GPIO_PinNumber 					= PIN_4;
	handlerA.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerA.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerA.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerA.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerA);

	handlerB.pGPIOx = GPIOB;
	handlerB.GPIO_PinConfig.GPIO_PinNumber 					= PIN_4;
	handlerB.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerB.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerB.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerB.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerB);

	handlerC.pGPIOx = GPIOB;
	handlerC.GPIO_PinConfig.GPIO_PinNumber 					= PIN_5;
	handlerC.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerC.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerC.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerC.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerC);

	handlerD.pGPIOx = GPIOA;
	handlerD.GPIO_PinConfig.GPIO_PinNumber 					= PIN_10;
	handlerD.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerD.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerD.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerD.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerD);

	handlerE.pGPIOx = GPIOB;
	handlerE.GPIO_PinConfig.GPIO_PinNumber 					= PIN_10;
	handlerE.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerE.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerE.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerE.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerE);

	handlerF.pGPIOx = GPIOA;
	handlerF.GPIO_PinConfig.GPIO_PinNumber 					= PIN_11;
	handlerF.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerF.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerF.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerF.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerF);

	handlerG.pGPIOx = GPIOA;
	handlerG.GPIO_PinConfig.GPIO_PinNumber 					= PIN_12;
	handlerG.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerG.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerG.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerG.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerG);

	handlerSS1.pGPIOx = GPIOC;
	handlerSS1.GPIO_PinConfig.GPIO_PinNumber 				= PIN_7;
	handlerSS1.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerSS1.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerSS1.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerSS1.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerSS1);
	GPIO_WritePin(&handlerSS1, SET);

	handlerSS2.pGPIOx = GPIOB;
	handlerSS2.GPIO_PinConfig.GPIO_PinNumber 				= PIN_9;
	handlerSS2.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerSS2.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerSS2.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerSS2.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerSS2);

	handlerTxPin.pGPIOx = GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber 				= PIN_2;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_ALTFN;
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerTxPin);
	GPIO_WritePin(&handlerTxPin, SET);

	handlerBlinkyLed.pGPIOx = GPIOA;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinNumber 			= PIN_5;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyLed);

	handlerData.pGPIOx = GPIOA;
	handlerData.GPIO_PinConfig.GPIO_PinNumber 				= PIN_7;
	handlerData.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_IN;
	handlerData.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerData.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerData.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerData.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;
	GPIO_Config(&handlerData);

	handlerClock.pGPIOx = GPIOA;
	handlerClock.GPIO_PinConfig.GPIO_PinNumber 				= PIN_8;
	handlerClock.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_IN;
	handlerClock.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerClock.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerClock.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	handlerClock.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;
	GPIO_Config(&handlerClock);

	handlerEncoderButton.pGPIOx = GPIOA;
	handlerEncoderButton.GPIO_PinConfig.GPIO_PinNumber			= PIN_0;
	handlerEncoderButton.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerEncoderButton.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerEncoderButton.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerEncoderButton.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerEncoderButton.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;
	GPIO_Config(&handlerEncoderButton);

	handlerTimer2.ptrTIMx 									= TIM2;
	handlerTimer2.TIMx_Config.TIMx_mode 					= BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed					= BTIMER_SPEED_100us;
	handlerTimer2.TIMx_Config.TIMx_period					= 2500;
	handlerTimer2.TIMx_Config.TIMx_interruptEnable 			= 1;
	BasicTimer_Config(&handlerTimer2);
	startTimer(&handlerTimer2);

	handlerTimer4.ptrTIMx 									= TIM4;
	handlerTimer4.TIMx_Config.TIMx_mode 					= BTIMER_MODE_UP;
	handlerTimer4.TIMx_Config.TIMx_speed					= BTIMER_SPEED_100us;
	handlerTimer4.TIMx_Config.TIMx_period					= 100;
	handlerTimer4.TIMx_Config.TIMx_interruptEnable 			= 1;
	BasicTimer_Config(&handlerTimer4);
	startTimer(&handlerTimer4);

	handlerUSART2.ptrUSARTx 								= USART2;
	handlerUSART2.USART_Config.USART_mode 					= USART_MODE_TX;
	handlerUSART2.USART_Config.USART_baudrate 				= USART_BAUDRATE_115200;
	handlerUSART2.USART_Config.USART_datasize 				= USART_DATASIZE_8BIT;
	handlerUSART2.USART_Config.USART_parity 				= USART_PARITY_NONE;
	USART_Config(&handlerUSART2);

	EXTI_Data.edgeType 										= EXTERNAL_INTERRUPT_FALLING_EDGE;
	EXTI_Data.pGPIOHandler 									= &handlerClock;
	extInt_Config(&EXTI_Data);

	EXTI_Button.edgeType 									= EXTERNAL_INTERRUPT_RISING_EDGE;
	EXTI_Button.pGPIOHandler 								= &handlerEncoderButton;
	extInt_Config(&EXTI_Button);

	/* Ciclo infinito del main */
	while(1){

		if(flagStatus){
			flagStatus = 0;
			statusLED();
		}
		if(flagClock){
			flagClock = 0;
			data = GPIO_ReadPin(&handlerData);
			if(data){
				if (position>=0 && position<50){
					position++;
				}
				sprintf(bufferData, "CW -> %u \n\r",  position);
				writeMsg(&handlerUSART2, bufferData);
			}else{
				if (position>0 && position<=50){
					position--;
				}
				sprintf(bufferData, "CCW -> %u \n\r",  position);
				writeMsg(&handlerUSART2, bufferData);
			}
		}

		num1 = position % 10;
		num2 = position / 10;
		num2 = num2 % 10;

		if(flagSS){
			flagSS = 0;
			dataSS = GPIO_ReadPin(&handlerSS1);
			if(dataSS){
				GPIO_WritePin(&handlerSS1, RESET);
				GPIO_WritePin(&handlerSS2, SET);
				numberSS(num2);
			}else{
				GPIO_WritePin(&handlerSS2, RESET);
				GPIO_WritePin(&handlerSS1, SET);
				numberSS(num1);
			}
		}
		if(flagButton){
			flagButton = 0;
			writeMsg(&handlerUSART2, "Buenos días profe \n\r");
		}
	}
	return 0;

}

/*
 * Definimos las funciones Callback para levantar banderas, para que en el ciclo while de la funcion main se sepa que
 * hay unas interrupciones por atender, en este caso, del timer y del EXTI.
 */

void BasicTimer2_Callback(void){
	flagStatus = 1;
}

void BasicTimer4_Callback(void){
	flagSS = 1;
}

void callback_extInt0(void){
	flagButton = 1;
}

void callback_extInt8(void){
	flagClock = 1;
}

/*
 * Funcion para los numeros del 0 a 9 en el 7 segmentos
 */

void numberSS(int number){
	if(number == 0){
		GPIO_WritePin(&handlerA, SET);
		GPIO_WritePin(&handlerB, SET);
		GPIO_WritePin(&handlerC, SET);
		GPIO_WritePin(&handlerD, SET);
		GPIO_WritePin(&handlerE, SET);
		GPIO_WritePin(&handlerF, SET);
		GPIO_WritePin(&handlerG, RESET);

	}else if(number == 1){
		GPIO_WritePin(&handlerA, RESET);
		GPIO_WritePin(&handlerB, SET);
		GPIO_WritePin(&handlerC, SET);
		GPIO_WritePin(&handlerD, RESET);
		GPIO_WritePin(&handlerE, RESET);
		GPIO_WritePin(&handlerF, RESET);
		GPIO_WritePin(&handlerG, RESET);

	}else if(number == 2){
		GPIO_WritePin(&handlerA, SET);
		GPIO_WritePin(&handlerB, SET);
		GPIO_WritePin(&handlerC, RESET);
		GPIO_WritePin(&handlerD, SET);
		GPIO_WritePin(&handlerE, SET);
		GPIO_WritePin(&handlerF, RESET);
		GPIO_WritePin(&handlerG, SET);

	}else if(number == 3){
		GPIO_WritePin(&handlerA, SET);
		GPIO_WritePin(&handlerB, SET);
		GPIO_WritePin(&handlerC, SET);
		GPIO_WritePin(&handlerD, SET);
		GPIO_WritePin(&handlerE, RESET);
		GPIO_WritePin(&handlerF, RESET);
		GPIO_WritePin(&handlerG, SET);

	}else if(number == 4){
		GPIO_WritePin(&handlerA, RESET);
		GPIO_WritePin(&handlerB, SET);
		GPIO_WritePin(&handlerC, SET);
		GPIO_WritePin(&handlerD, RESET);
		GPIO_WritePin(&handlerE, RESET);
		GPIO_WritePin(&handlerF, SET);
		GPIO_WritePin(&handlerG, SET);

	}else if(number == 5){
		GPIO_WritePin(&handlerA, SET);
		GPIO_WritePin(&handlerB, RESET);
		GPIO_WritePin(&handlerC, SET);
		GPIO_WritePin(&handlerD, SET);
		GPIO_WritePin(&handlerE, RESET);
		GPIO_WritePin(&handlerF, SET);
		GPIO_WritePin(&handlerG, SET);

	}else if(number == 6){
		GPIO_WritePin(&handlerA, SET);
		GPIO_WritePin(&handlerB, RESET);
		GPIO_WritePin(&handlerC, SET);
		GPIO_WritePin(&handlerD, SET);
		GPIO_WritePin(&handlerE, SET);
		GPIO_WritePin(&handlerF, SET);
		GPIO_WritePin(&handlerG, SET);

	}else if(number == 7){
		GPIO_WritePin(&handlerA, SET);
		GPIO_WritePin(&handlerB, SET);
		GPIO_WritePin(&handlerC, SET);
		GPIO_WritePin(&handlerD, RESET);
		GPIO_WritePin(&handlerE, RESET);
		GPIO_WritePin(&handlerF, RESET);
		GPIO_WritePin(&handlerG, RESET);

	}else if(number == 8){
		GPIO_WritePin(&handlerA, SET);
		GPIO_WritePin(&handlerB, SET);
		GPIO_WritePin(&handlerC, SET);
		GPIO_WritePin(&handlerD, SET);
		GPIO_WritePin(&handlerE, SET);
		GPIO_WritePin(&handlerF, SET);
		GPIO_WritePin(&handlerG, SET);

	}else if(number == 9){
		GPIO_WritePin(&handlerA, SET);
		GPIO_WritePin(&handlerB, SET);
		GPIO_WritePin(&handlerC, SET);
		GPIO_WritePin(&handlerD, SET);
		GPIO_WritePin(&handlerE, RESET);
		GPIO_WritePin(&handlerF, SET);
		GPIO_WritePin(&handlerG, SET);

	}

}

/* 
 * Definimos la funcion de estado de LED, para que el UserLed cambie de estado (Hacer "blinky"),
 * indicando que el programa está funcionando correctamente.
 * En nuestro caso, como el timer está funcionando cada 250ms, el estado del LED cambiará con
 * este periodo.
 */		

void statusLED(void){
	blinky = !blinky;
	    if(blinky){
	        GPIO_WritePin(&handlerBlinkyLed, SET);
	    }
	    else{
	        GPIO_WritePin(&handlerBlinkyLed, RESET);
	    }
}

