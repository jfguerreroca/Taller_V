/*
 * main.c
 *
 *  Created on: 29 ago 2022
 *      Author: Felipe
 *
 *
 *
 */

#include <stdint.h>
#include "GPIOxDriver.h"
#include "stm32f411xx_hal.h"

uint8_t estadoPin1 = 0;
uint8_t estadoPin2 = 0;
uint8_t estadoBoton = 0;


int main(void)
{

	// Definimos los handler para los PIN que deseamos configurar
	GPIO_Handler_t handlerUserPin1 = {0};
	GPIO_Handler_t handlerUserPin2 = {0};

	// Deseamos trabajar con el puerto GPIOC
	handlerUserPin1.pGPIOx = GPIOC;
	handlerUserPin1.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	handlerUserPin1.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerUserPin1.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserPin1.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUserPin1.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserPin1.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						//Ninguna funcion

	handlerUserPin2.pGPIOx = GPIOC;
	handlerUserPin2.GPIO_PinConfig.GPIO_PinNumber			= PIN_12;
	handlerUserPin2.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerUserPin2.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserPin2.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUserPin2.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserPin2.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						//Ninguna funcion

	// Cargamos al configuracion del PIN_6 y PIN_12 especifico
	GPIO_Config(&handlerUserPin1);
	GPIO_Config(&handlerUserPin2);

	// Hacemos que el PIN_6 quede encendido
	GPIO_WritePin(&handlerUserPin1, SET);

	// Leemos el estado de varios pines para comprobar la funcion ReadPin

	estadoPin1 = GPIO_ReadPin(&handlerUserPin1);
	estadoPin2 = GPIO_ReadPin(&handlerUserPin2);

	// Punto 2

	//
	void GPIOxTogglePin(GPIO_Handler_t *pPinHandler){
		uint8_t estadoPin = GPIO_ReadPin(&*pPinHandler);
		if (estadoPin == 0){
			GPIO_WritePin(&*pPinHandler, SET);
		}
		else if (estadoPin == 1){
			GPIO_WritePin(&*pPinHandler, RESET);
		}
	}

	GPIOxTogglePin(&handlerUserPin1);
	GPIOxTogglePin(&handlerUserPin2);

	estadoPin1 = GPIO_ReadPin(&handlerUserPin1);
	estadoPin2 = GPIO_ReadPin(&handlerUserPin2);

	//Punto 3

	GPIO_Handler_t handlerUserBoton = {0};

	handlerUserBoton.pGPIOx = GPIOC;
	handlerUserBoton.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	handlerUserBoton.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerUserBoton.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserBoton.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerUserBoton.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserBoton.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						//Ninguna funcion

	GPIO_Handler_t handlerUserLED = {0};

	handlerUserLED.pGPIOx = GPIOA;
	handlerUserLED.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerUserLED.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerUserLED.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerUserLED.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLED.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserLED.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;						//Ninguna funcion

	GPIO_Config(&handlerUserBoton);
	GPIO_Config(&handlerUserLED);



	while(1){
		if (!GPIO_ReadPin(&handlerUserBoton)){
			GPIOxTogglePin(&handlerUserLED);
			for (int i=0;(i<600000);i++){
				NOP();
			}
		}
	}


    /* Loop forever */
	while(1){

	}
}
