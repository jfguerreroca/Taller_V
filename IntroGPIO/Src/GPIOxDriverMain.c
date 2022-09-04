/*
 * GPIOxDriverMain.c
 *
 *  Created on: Aug 30, 2022
 *      Author: felipe
 */
#include<stdint.h>

#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"
//Función principal//
int main(void){
	GPIO_Handler_t handlerUserLedPin = {0}; //Inicio la estructura vacia

	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;

	GPIO_Config(&handlerUserLedPin);

	GPIO_Handler_t handlerUserButton = {0};

	handlerUserButton.pGPIOx = GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerUserButton.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_MEDIUM;
	handlerUserButton.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;

	GPIO_Config(&handlerUserButton);




}


