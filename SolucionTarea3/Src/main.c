/**
 ******************************************************************************
 * @file           : main.c
 * @author         : jfguerreroca
 * @brief          : Configuración básica de un proyecto
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

/**
 * Función principal del programa
 * Esta función es el corazón del programa
 * */

// Definimos los handler para los perifericos necesarios en el proyecto

USART_Handler_t 		handlerUSART1		= {0};

BasicTimer_Handler_t 	handlerTimer2 		= {0};

GPIO_Handler_t 			handlerBlinkyLed 	= {0};
GPIO_Handler_t			handlerUserButton	= {0};
GPIO_Handler_t			handlerTxPin		= {0};

void TIM2_IRQHandler(void);

uint8_t blinky = 0;
uint8_t data = 0;
uint8_t ButtonStatus = 0;

int main(void)
{
	handlerBlinkyLed.pGPIOx = GPIOA;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyLed);

	handlerUserButton.pGPIOx = GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserButton.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerUserButton.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						//Ninguna funcion
	GPIO_Config(&handlerUserButton);
	GPIO_WritePin(&handlerUserButton, SET);

	handlerTxPin.pGPIOx = GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber 			= PIN_9;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;
	GPIO_Config(&handlerTxPin);
	GPIO_WritePin(&handlerTxPin, SET);

	handlerTimer2.ptrTIMx = TIM2;
	handlerTimer2.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed = BTIMER_SPEED_100us;
	handlerTimer2.TIMx_Config.TIMx_period = 2500;
	handlerTimer2.TIMx_Config.TIMx_interruptEnable = 1;

	BasicTimer_Config(&handlerTimer2);

	handlerUSART1.ptrUSARTx = USART1;
	handlerUSART1.USART_Config.USART_mode = USART_MODE_TX;
	handlerUSART1.USART_Config.USART_baudrate = USART_BAUDRATE_115200;
	handlerUSART1.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	handlerUSART1.USART_Config.USART_parity = USART_PARITY_NONE;

	USART_Config(&handlerUSART1);

	TIM2_IRQHandler();

	/* Loop forever */
	while(1){

	}

	return 0;
}

void BasicTimer2_Callback(void){
	blinky = !blinky;
	    if(blinky){
	        GPIO_WritePin(&handlerBlinkyLed, SET);
	    }
	    else{
	        GPIO_WritePin(&handlerBlinkyLed, RESET);
	    }
	data = 35;
	ButtonStatus = GPIO_ReadPin(&handlerUserButton);
	if (ButtonStatus == 0){
		data = data + 65;
	}
	writeChar(&handlerUSART1, data);
}
