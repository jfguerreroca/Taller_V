/*
 * GPIOxDriver.h
 *
 *  Created on: Aug 29, 2022
 *      Author: felipe
 * Este programa se encarga de las definiciones relacionadas únicamente con el debido manejo y control del
 * periférico GPIOx (funciones del controlador)
 */

#ifndef GPIOXDRIVER_H_
#define GPIOXDRIVER_H_

#include "stm32f411xx_hal.h" /*Incluyo el header file y también el correspondiente al GPIOx*/
typedef struct
{
	uint8_t GPIO_PinNumber;           // PIN con el que voy a trabajar
	uint8_t GPIO_PinMode;             // Modo de la configuración: entrada, salida, análogo, f. alternativa
	uint8_t GPIO_PinSpeed;            // Velocidad de respuesta del pin
	uint8_t GPIO_PinPuPdControl;      // Si la salida es pull-up, pull-down o libre
	uint8_t GPIO_PinOPType;           // Selecciona salida PUPD o OpenDrain
	uint8_t GPIO_PinAltFunMode;      // Seleeciona cuál es la función alternativa que se esta configurando

}GPIO_PinConfig_t;

/* La estructura a continuación contiene dos elementos:
 * 1. La address del puerto que se está utilizando
 * 2. La configuración específica del PIN que se está utilizando
 */

typedef struct{
	GPIOx_RegDef_t                  *pGPIOx;         /*Dirección del puerto al que el PIN corresponde*/
	GPIO_PinConfig_t                GPIO_PinConfig;  /*Configuración del PIN*/

}GPIO_Handler_t;

/* Definición de las cabeceras de las funciones del GPIOxDriver */
void GPIO_Config (GPIO_Handler_t *pGPIOHandler);                   //Función para configurar
void GPIO_WritePin(GPIO_Handler_t *pPinHandler, uint8_t newState); //Función para escribir
uint32_t GPIO_ReadPin(GPIO_Handler_t *pPinHandler);                //Función para leer

#endif /* GPIOXDRIVER_H_ */
