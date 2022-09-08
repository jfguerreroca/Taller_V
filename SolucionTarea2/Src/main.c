/*
 * main.c
 *
 *  Created on: 29 ago 2022
 *      Author: Felipe Guerrero
 *      Mail: jfguerreroca@unal.edu.co
 *
 *
 *
 */

#include <stdint.h>
#include "GPIOxDriver.h"
#include "stm32f411xx_hal.h"

//Variables auxiliares para comprobar el funcionamiento del programa
uint8_t estadoPin1 = 0;
uint8_t estadoPin2 = 0;
uint8_t estadoBoton = 0;
uint8_t estadoBotonEx = 0;


int main(void)
{

	// Definimos los handler para los PIN que deseamos configurar para el Punto 1
	GPIO_Handler_t handlerUserPin1 = {0};
	GPIO_Handler_t handlerUserPin2 = {0};

	// Deseamos trabajar con el puerto GPIOC
	handlerUserPin1.pGPIOx = GPIOC;
	handlerUserPin1.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	handlerUserPin1.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerUserPin1.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserPin1.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUserPin1.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserPin1.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						//Ninguna funcion

	handlerUserPin2.pGPIOx = GPIOC;
	handlerUserPin2.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerUserPin2.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerUserPin2.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserPin2.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUserPin2.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserPin2.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						//Ninguna funcion

	// Cargamos al configuracion del PIN_2 y PIN_3 especifico
	GPIO_Config(&handlerUserPin1);
	GPIO_Config(&handlerUserPin2);

	// Hacemos que el PIN_2 quede encendido
	GPIO_WritePin(&handlerUserPin1, SET);

	// Leemos el estado de varios pines para comprobar la funcion ReadPin, donde estadoPin1 debe ser 1, y estadoPin2 debe ser 0

	estadoPin1 = GPIO_ReadPin(&handlerUserPin1);
	estadoPin2 = GPIO_ReadPin(&handlerUserPin2);

	/**
	  * Punto 2:
	  *
	  * Creamos la funcion TogglePin, y al recibir el handler correspondiente, lee el estado del PIN, y con base en el valor del estado, invierte el estado con
	  * la funcion WritePin
	  */

	void GPIOxTogglePin(GPIO_Handler_t *pPinHandler){
		uint8_t estadoPin = GPIO_ReadPin(&*pPinHandler);
		if (estadoPin == 0){
			GPIO_WritePin(&*pPinHandler, SET);
		}
		else if (estadoPin == 1){
			GPIO_WritePin(&*pPinHandler, RESET);
		}
	}

	// Comprobamos el funcionamiento de la funcion Toggle con los pines usados en el primer punto
	GPIOxTogglePin(&handlerUserPin1);
	GPIOxTogglePin(&handlerUserPin2);

	estadoPin1 = GPIO_ReadPin(&handlerUserPin1);
	estadoPin2 = GPIO_ReadPin(&handlerUserPin2);

	// Pines Punto 3, donde tenemos el UserButton y el UserLed

	GPIO_Handler_t handlerUserBoton = {0};

	handlerUserBoton.pGPIOx = GPIOC;
	handlerUserBoton.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	handlerUserBoton.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerUserBoton.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserBoton.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
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

	//Pines Punto 4, donde tenemos un boton externo y 3 LEDs

	GPIO_Handler_t handlerUserBotonEx = {0};

	handlerUserBotonEx.pGPIOx = GPIOC;
	handlerUserBotonEx.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	handlerUserBotonEx.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;
	handlerUserBotonEx.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserBotonEx.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLDOWN;
	handlerUserBotonEx.GPIO_PinConfig.GPIO_PinSpeed			    = GPIO_OSPEED_MEDIUM;
	handlerUserBotonEx.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						//Ninguna funcion

	GPIO_Handler_t handlerUserLED10 = {0};

	handlerUserLED10.pGPIOx = GPIOC;
	handlerUserLED10.GPIO_PinConfig.GPIO_PinNumber			= PIN_10;
	handlerUserLED10.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerUserLED10.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLED10.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUserLED10.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserLED10.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						//Ninguna funcion

	GPIO_Handler_t handlerUserLED11 = {0};

	handlerUserLED11.pGPIOx = GPIOC;
	handlerUserLED11.GPIO_PinConfig.GPIO_PinNumber			= PIN_11;
	handlerUserLED11.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerUserLED11.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLED11.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUserLED11.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserLED11.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						//Ninguna funcion

	GPIO_Handler_t handlerUserLED12 = {0};

	handlerUserLED12.pGPIOx = GPIOC;
	handlerUserLED12.GPIO_PinConfig.GPIO_PinNumber			= PIN_12;
	handlerUserLED12.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerUserLED12.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLED12.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUserLED12.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserLED12.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						//Ninguna funcion

	GPIO_Config(&handlerUserBotonEx);
	GPIO_Config(&handlerUserLED10);
	GPIO_Config(&handlerUserLED11);
	GPIO_Config(&handlerUserLED12);

	while(1){

		/**
		  * Punto 3:
		  * Sabemos que el boton en su estado de reposo tiene un estado con valor igual a 1, y al presionarlo toma el valor de 0, por
		  * lo que al tomar este ultimo valor, entra en un condicional que cambia el estado del Pin correspondiente al LED.
		  * Se agrega un for que no hace nada, para tener un retardo entre cada pulsacion.
		  */
		estadoBoton = GPIO_ReadPin(&handlerUserBoton);
		if (estadoBoton == 0){
			GPIOxTogglePin(&handlerUserLED);
			for (int i=0;(i<600000);i++){
				NOP();
			}
		}

		/**
		  * Punto 4:
		  * El boton externo, al configurarlo en Pull-down, tiene un comportamiento que toma 0 en reposo, y 1 presionado.
		  * Sabiendo esto, hacemos el condicional para que cuando sea pulsado encienda los LEDs al mismo tiempo, cambiando
		  * el estado de sus pines con Toggle, y utilizando ciclos for para los retardos de 5, 2 y 1 segundos, luego de cada
		  * uno de estos utilizamos Toggle de nuevo para ir apagando los LEDs.
		  */

		estadoBotonEx = GPIO_ReadPin(&handlerUserBotonEx);
		if (estadoBotonEx == 1){
			GPIOxTogglePin(&handlerUserLED10);
			GPIOxTogglePin(&handlerUserLED11);
			GPIOxTogglePin(&handlerUserLED12);
			for (int i=0;(i<6000000);i++){
				NOP();
			}
			GPIOxTogglePin(&handlerUserLED12);
			for (int i=0;(i<2400000);i++){
				NOP();
			}
			GPIOxTogglePin(&handlerUserLED11);
			for (int i=0;(i<1200000);i++){
				NOP();
			}
			GPIOxTogglePin(&handlerUserLED10);
		}
	}
}
