/**
 ******************************************************************************
 * @file           : main.c
 * @author         : jfguerreroca
 * @name		   : Juan Felipe Guerrero Cataño
 * @brief          : Tarea 3
 ******************************************************************************
 *
 ******************************************************************************
 */

/* Definimos las librerias que vamos a utilizar. En este caso agregamos una libreria llamada stdlib.h, para
 * utilizar la funcion rand(), que nos permitira generar numeros aleatorios.
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "BasicTimer.h"
#include "GPIOxDriver.h"
#include "USARTxDriver.h"

// Definimos los handler para los perifericos necesarios en el proyecto

USART_Handler_t 		handlerUSART1		= {0};

BasicTimer_Handler_t 	handlerTimer2 		= {0};

GPIO_Handler_t 			handlerBlinkyLed 	= {0};
GPIO_Handler_t			handlerUserButton	= {0};
GPIO_Handler_t			handlerTxPin		= {0};

// Definimos las variables que utilizaremos
uint8_t blinky = 0;
uint8_t data = 0;
uint8_t ButtonStatus = 0;
uint8_t flag = 0;

// Definimos las funciones que vamos a utilizar
void TIM2_IRQHandler(void);

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
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
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

	// Llamamos a la funcion que se encarga de atender las interrupciones

	TIM2_IRQHandler();

	/* Loop forever */
	while(1){

		/*
		 * Leemos el estado de la bandera definida en la funcion Callback (la cual es llamada por la funcion IRQHandler)
		 * y con esta definimos si mandamos (cuando la bandera está activa) o no (cuando la bandera está abajo)
		 * la variable data.
		 * Con el estado del boton decidimos si esta variable será "constante" (cuando no se presiona el boton), o un
		 * dato aleatorio (cuando dejamos presionado el boton).
		 * El dato aleatorio se definio utilizando la funcion rand(), la cual arroja un dato aleatorio entre 0 y N, donde
		 * para nosotros N=50.
		 *
		 * La funcion tiene la siguiente estructura:
		 * 		data = rand() % N+1; , donde N es el maximo del rango de numeros posibles.
		 *
		 * El valor se asigna a la variable data, la cual es enviada con la funcion writeChar, y tambien bajamos la bandera
		 * para indicar que la interrupcion ya fue atendida (en este caso, el envio de un dato cada 250ms).
		 */
		ButtonStatus = GPIO_ReadPin(&handlerUserButton);

		if (flag == 1){
			if (ButtonStatus == 0){
				data = rand() % 51;
			}else{
				data = 60;
			}
			writeChar(&handlerUSART1, data);
			flag = 0;
		}
	}
	return 0;
}

/*
 * Definimos la funcion Callback para que cada 250 ms el UserLed cambie de estado (Hacer "blinky"),
 * indicando que el programa está funcionando correctamente.
 * Ademas, levantamos la bandera (flag=1) para que en el ciclo while de la funcion main se sepa que
 * hay una interrupcion por atender.
 */

void BasicTimer2_Callback(void){
	blinky = !blinky;
	    if(blinky){
	        GPIO_WritePin(&handlerBlinkyLed, SET);
	    }
	    else{
	        GPIO_WritePin(&handlerBlinkyLed, RESET);
	    }
	flag = 1;

}
