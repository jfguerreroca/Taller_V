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

USART_Handler_t 		handlerUSART2		= {0};

BasicTimer_Handler_t 	handlerTimer1 		= {0};
BasicTimer_Handler_t 	handlerTimer2 		= {0};

GPIO_Handler_t 			handlerBlinkyLed 	= {0};
GPIO_Handler_t			handlerUserButton	= {0};
GPIO_Handler_t			handlerTxPin		= {0};
GPIO_Handler_t			handlerData			= {0};
GPIO_Handler_t			handlerClock		= {0};

EXTI_Config_t			EXTI_Config			= {0};

// Definimos las variables que utilizaremos

uint8_t blinky = 0;
uint8_t data = 0;
uint8_t ButtonStatus = 0;
uint8_t flagStatus = 0;

// Definimos las funciones que vamos a utilizar

//Funcion de estado con el User LED

void statusLED(void);

// Funcion principal del programa

int main(void)
{
	/*
	 * Cargamos las configuraciones de cada periferico a utilizar:
	 * 		- Pin A5 del UserLed
	 * 		- Pin C13 del UserButton
	 * 		- Pin A9 para el USART1 (funcion alternativa AF7)
	 * 		- Timer 2 lanzando interrupciones cada 250ms
	 * 		- USART1 para transmision de datos
	 */

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
	handlerData.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerData.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerData.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;
	GPIO_Config(&handlerData);

	handlerClock.pGPIOx = GPIOA;
	handlerClock.GPIO_PinConfig.GPIO_PinNumber 				= PIN_8;
	handlerClock.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_IN;
	handlerClock.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerClock.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerClock.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	handlerClock.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;
	GPIO_Config(&handlerClock);

	handlerUserButton.pGPIOx = GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserButton.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerUserButton.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;
	GPIO_Config(&handlerUserButton);
	GPIO_WritePin(&handlerUserButton, SET);

	handlerTimer2.ptrTIMx 									= TIM2;
	handlerTimer2.TIMx_Config.TIMx_mode 					= BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed					= BTIMER_SPEED_100us;
	handlerTimer2.TIMx_Config.TIMx_period					= 2500;
	handlerTimer2.TIMx_Config.TIMx_interruptEnable 			= 1;
	BasicTimer_Config(&handlerTimer2);

	handlerUSART2.ptrUSARTx 								= USART2;
	handlerUSART2.USART_Config.USART_mode 					= USART_MODE_TX;
	handlerUSART2.USART_Config.USART_baudrate 				= USART_BAUDRATE_115200;
	handlerUSART2.USART_Config.USART_datasize 				= USART_DATASIZE_8BIT;
	handlerUSART2.USART_Config.USART_parity 				= USART_PARITY_NONE;
	USART_Config(&handlerUSART2);

	EXTI_Config.edgeType 									= EXTERNAL_INTERRUPT_FALLING_EDGE;
	EXTI_Config.pGPIOHandler 								= &handlerClock;
	extInt_Config(&EXTI_Config);


	/* Ciclo infinito del main */
	while(1){

		if(flagStatus){
			flagStatus = 0;
			statusLED();
		}



	}
	return 0;
}

/*
 * Definimos la funcion Callback para levantar la bandera (flag=1), para que en el ciclo while de la funcion main se sepa que
 * hay una interrupcion por atender.
 */

void BasicTimer2_Callback(void){
	flagStatus = 1;
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

