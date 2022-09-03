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

int main(void)
{

	// Definimos los handler para los PIN que deseamos configurar
	GPIO_Handler_t handlerUserPin1 = {0};
	GPIO_Handler_t handlerUserPin2 = {0};

	// Deseamos trabajar con el puerto GPIOC
	handlerUserPin1.pGPIOx = GPIOC;
	handlerUserPin1.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
	handlerUserPin1.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerUserPin1.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserPin1.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerUserPin1.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserPin1.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						//Ninguna funcion

	handlerUserPin2.pGPIOx = GPIOC;
	handlerUserPin2.GPIO_PinConfig.GPIO_PinNumber			= PIN_10;
	handlerUserPin2.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerUserPin2.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserPin2.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUserPin2.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserPin2.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						//Ninguna funcion

	// Cargamos al configuracion del PIN_9 y PIN_10 especifico
	GPIO_Config(&handlerUserPin1);
	GPIO_Config(&handlerUserPin2);

	// Hacemos que el PIN_9 quede encendido
	GPIO_WritePin(&handlerUserPin1, SET);

	// Leemos el estado de varios pines para comprobar la funcion ReadPin

	estadoPin1 = GPIO_ReadPin(&handlerUserPin1);
	estadoPin2 = GPIO_ReadPin(&handlerUserPin2);


    /* Loop forever */
	while(1){

	}
}
