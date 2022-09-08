/*
 * GPIOxDriver.c
 *
 *  Created on: Aug 29, 2022
 *      Author: felipe
 *
 * Aquí escribiré el control para que sea lo más genérico posible, de forma independiente del puerto GPIO
 * y el pin seleccionado, para que el programa se ejecute y configure todos de forma correcta
 */

#include "GPIOxDriver.h"

/*Para cualquier periférico se debe tener:
 * 1. Activación de la señal del reloj principal hacia ese elemento específico (relacionado con
 * el periférico RCC)
 */

/*Configuración registro a registro del MCU*/

void GPIO_Config (GPIO_Handler_t  *pGPIOHandler){

	// Variable para seguir paso a paso
	uint32_t auxConfig = 0;
	uint32_t auxPosition = 0;

	//= Activar el periférico =//
	/*Verifico para GPIOA*/
	if(pGPIOHandler->pGPIOx == GPIOA) {
		//Escribo SET en la posición correspondiente a GPIOA
		RCC -> AHB1ENR |= (SET << RCC_AHB1ENR_GPIOAEN_Pos);
	}
	/*Verifico para GPIOB*/
	else if(pGPIOHandler->pGPIOx == GPIOB) {
		//Escribo SET en la posición correspondiente a GPIOB
		RCC -> AHB1ENR |= (SET << RCC_AHB1ENR_GPIOBEN_Pos);
	}
	/*Verifico para GPIOC*/
	else if(pGPIOHandler->pGPIOx == GPIOC) {
		//Escribo SET en la posición correspondiente a GPIOC
		RCC -> AHB1ENR |= (SET << RCC_AHB1ENR_GPIOCEN_Pos);
	}
	/*Verifico para GPIOD*/
	else if(pGPIOHandler->pGPIOx == GPIOD) {
		//Escribo SET en la posición correspondiente a GPIOD
		RCC -> AHB1ENR |= (SET << RCC_AHB1ENR_GPIODEN_Pos);
	}
	/*Verifico para GPIOE*/
	else if(pGPIOHandler->pGPIOx == GPIOE) {
		//Escribo SET en la posición correspondiente a GPIOE
		RCC -> AHB1ENR |= (SET << RCC_AHB1ENR_GPIOEEN_Pos);
	}
	/*Verifico para GPIOH*/
	else if(pGPIOHandler->pGPIOx == GPIOH) {
		//Escribo SET en la posición correspondiente a GPIOH
		RCC -> AHB1ENR |= (SET << RCC_AHB1ENR_GPIOHEN_Pos);
	}

	//=Ya está activado, ahora lo configuro=
	/* Configurando el registro GPIOx_MODER. Acá leo la configuración, moviendo "PinNumber"
	veces hacia la izquierda ese valor y todo se carga en la variable auxCOnfig*/
	auxConfig = (pGPIOHandler -> GPIO_PinConfig.GPIO_PinMode <<2  * pGPIOHandler ->GPIO_PinConfig.GPIO_PinNumber);

	//Antes de cargar el nuevo valor, limpio los bits específicos de ese registro (escribiendo 0b00)
	// para lo cual, aplico una máscara y una operación bitwise AND
	pGPIOHandler -> pGPIOx -> MODER &= ~(0b11 <<2 * pGPIOHandler -> GPIO_PinConfig.GPIO_PinNumber);

	//Cargo a auxConfig en el registro MODER
	pGPIOHandler ->pGPIOx->MODER |= auxConfig;

	// = Configuro el registro GPIOx_OTYPER
	/*Leo y muevo el valor PinNumber veces
	 */
	auxConfig = (pGPIOHandler ->GPIO_PinConfig.GPIO_PinOPType << pGPIOHandler -> GPIO_PinConfig.GPIO_PinNumber);

	//Limpio antes de cargar
	pGPIOHandler->pGPIOx ->OTYPER &= ~(SET << pGPIOHandler ->GPIO_PinConfig.GPIO_PinNumber);

	//Cargo el resultado sobre el registro adecuado
	pGPIOHandler->pGPIOx->OTYPER |=auxConfig;

	//= COnfiguro la velocidad =//
	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed << 2* pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	//Limpio
	pGPIOHandler->pGPIOx->OSPEEDR &= ~(0b11 <<2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	//Cargo el resultado
	pGPIOHandler->pGPIOx->OSPEEDR |= auxConfig;

	//= Configuro si deseos pull-up/pull-down o flotante =/
	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl <<2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	//Limpio antes de cargar
	pGPIOHandler->pGPIOx->PUPDR &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	//Cargo el resultado
	pGPIOHandler->pGPIOx->PUPDR |= auxConfig;


	//= Viene la parte de la configuración de las funciones alternativas=//
	if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN) {
		//Seleciono si el registro se usaraá bajo AFRL o AFRH
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber < 8 ){
			//Me encuentro en el registro AFRL, que controla los pines del 0 al 7
			auxPosition = 4 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber;

			//Limpio
			pGPIOHandler ->pGPIOx->AFR[0] &=~(0b1111 << auxPosition);

			//Cargo
			pGPIOHandler->pGPIOx->AFR[0] |= (pGPIOHandler->GPIO_PinConfig.GPIO_PinAltFunMode << auxPosition);
		}
		else {
			//Me encuentro en el registro AFRH, que controla los pines del 8 al 15
			auxPosition = 4 * (pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber -8);

			//Limpio
			pGPIOHandler ->pGPIOx->AFR[1] &=~(0b1111 << auxPosition);

			//Cargo
			pGPIOHandler->pGPIOx->AFR[1] |= (pGPIOHandler->GPIO_PinConfig.GPIO_PinAltFunMode << auxPosition);

		}


	}


	} //Fin del GPIO_config

/**Función utilizada para cambiar de estado el pin entregado en el handler, asignando el valor
 * entregado de la variable newState
 */

void GPIO_WritePin(GPIO_Handler_t * pPinHandler, uint8_t newState){
	//Limpio la posición
	if(newState== SET){
		//Trabajo con la parte baja del registro
		pPinHandler->pGPIOx->BSRR |= (SET << pPinHandler->GPIO_PinConfig.GPIO_PinNumber);
	}
	else{
		//Trabajo con la parte alta del registro
		pPinHandler->pGPIOx->BSRR |= (SET << (pPinHandler->GPIO_PinConfig.GPIO_PinNumber + 16));
	}
}
//Función para leer el estado de un pin específico

uint32_t GPIO_ReadPin(GPIO_Handler_t *pPinHandler){
	// Creamos una variable auxiliar la cual luego retornaremos
	uint32_t pinValue = 0;

	// Cargamos el valor del registro IDR, desplazado a derecha tantas veces como la ubicacion
	// del pin especifico

	//pinValue = (pPinHandler->pGPIOx->IDR >> pPinHandler->GPIO_PinConfig.GPIO_PinNumber);

	/**
	 * Punto 1:
	 *
	 * El error de este código está en que solo está moviendo el valor del PIN necesario a la
	 * posición 0, pero no está teniendo en cuenta que puede haber mas de un PIN en uso.
	 * Por esto, la solución propuesta (hay muchas posibilidades) es mover a la izquierda
	 * 31 menos el numero del PIN, de manera que el valor esperado quede en la posicion 31
	 * del arreglo de 32 bits. Luego movemos a la derecha 31 veces este mismo valor para que
	 * obtengamos el estado del numero del PIN que ingresó a la función, y el resto de valores
	 * estén en 0, donde podremos ver si el PIN está activado (pinValue = 0b1) o está
	 * desactivado (pinValue = 0b0).
	 */
	pinValue = pPinHandler->pGPIOx->IDR << (31 - (pPinHandler->GPIO_PinConfig.GPIO_PinNumber));
	pinValue >>= 31;

	return pinValue;
}



