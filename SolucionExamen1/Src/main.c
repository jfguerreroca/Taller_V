/**
 ******************************************************************************
 * @file           : main.c
 * @author         : jfguerreroca
 * @name           : Juan Felipe Guerrero Cataño
 * @brief          : Examen
 ******************************************************************************
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "stm32f4xx.h"
#include "BasicTimer.h"
#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "AdcDriver.h"
#include "ExtiDriver.h"
#include "I2CDriver.h"
#include "PwmDriver.h"

// Definimos los handler para los perifericos necesarios en el proyecto

USART_Handler_t 		handlerUSART6			= {0};

BasicTimer_Handler_t 	handlerTimer2 			= {0};

GPIO_Handler_t 			handlerBlinkyLed 		= {0};
GPIO_Handler_t			handlerTxPin			= {0};
GPIO_Handler_t			handlerRxPin			= {0};

ADC_Config_t  			adcAXIS_XY     			= {0};

// Definimos las variables que utilizaremos

double PI = 0;
double angle = 0;

bool adcIsComplete 				= false;

int16_t adcData				 	= 0;
int16_t XY[2]					= {0};
uint8_t rxData 					= 0;
uint8_t XY_indicator			= 0;
uint8_t flagStatus 				= 0;
uint8_t flagAdc 				= 0;

char bufferData[64] 			= {0};

char greetingMsg[] = "USART funciona hehehe siuuuu \n" ;

// Definimos las funciones que vamos a utilizar:

//Funcion que retorna el angulo del servomotor

double ServoAngle(int16_t, int16_t);

//Funcion de estado con el User LED

void statusLED(void);

//Funcion para inicializar las configuraciones del sistema

void InitSystem(void);

// Funcion principal del programa

int main(void)
{
	/*
	 * Cargamos las configuraciones de cada periferico a utilizar:
	 *
	 * 		- 2 GPIO para la transmision Serial RXTX por el USART6
	 * 		- 1 GPIO para el control del Blinky
	 * 		- 1 timer, el TIM2 encargado del Blinky
	 * 		- 1 Usart encargado de la comunicacion serial (USART6)
	 * 		- 1 ADC, de manera multi canal, para la lectura de los ejes X y Y del joystick
	 *
	 */

	InitSystem();

	writeChar(&handlerUSART6, ' ');

	/* Ciclo infinito del main */
	while(1){

		if(flagStatus){
			flagStatus = 0;
			statusLED();
		}

		if(rxData != '\0') {
			writeChar(&handlerUSART6, rxData);
			if(rxData == 'm'){
				writeMsg(&handlerUSART6, greetingMsg);
			}
			if(rxData == 's'){
				startSingleADC();
			}
			rxData = '\0';
		}

		if(adcIsComplete == true){
			sprintf(bufferData, "ADC X = %d ADC Y = %d \n\r", XY[0], XY[1]);
			writeMsg(&handlerUSART6, bufferData);
			adcIsComplete = false;
		}else{
			__NOP();
		}
	}

	return 0;
}

void InitSystem(void){

	handlerBlinkyLed.pGPIOx = GPIOA;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinNumber 				= PIN_5;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyLed);

//	handlerTxPin.pGPIOx = GPIOA;
//	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber 					= PIN_11;
//	handlerTxPin.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
//	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
//	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
//	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
//	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode				= AF8;
//	GPIO_Config(&handlerTxPin);
//
//	handlerRxPin.pGPIOx = GPIOA;
//	handlerRxPin.GPIO_PinConfig.GPIO_PinNumber 					= PIN_12;
//	handlerRxPin.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
//	handlerRxPin.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
//	handlerRxPin.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
//	handlerRxPin.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
//	handlerRxPin.GPIO_PinConfig.GPIO_PinAltFunMode				= AF8;
//	GPIO_Config(&handlerRxPin);

	handlerTxPin.pGPIOx = GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber 					= PIN_2;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode				= AF7;
	GPIO_Config(&handlerTxPin);

	handlerRxPin.pGPIOx = GPIOA;
	handlerRxPin.GPIO_PinConfig.GPIO_PinNumber 					= PIN_3;
	handlerRxPin.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
	handlerRxPin.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerRxPin.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerRxPin.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerRxPin.GPIO_PinConfig.GPIO_PinAltFunMode				= AF7;
	GPIO_Config(&handlerRxPin);

	handlerTimer2.ptrTIMx 										= TIM2;
	handlerTimer2.TIMx_Config.TIMx_mode 						= BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed						= BTIMER_SPEED_100us;
	handlerTimer2.TIMx_Config.TIMx_period						= 2500;
	handlerTimer2.TIMx_Config.TIMx_interruptEnable 				= 1;
	BasicTimer_Config(&handlerTimer2);
	startTimer(&handlerTimer2);

	handlerUSART6.ptrUSARTx 									= USART2;
	handlerUSART6.USART_Config.USART_mode 						= USART_MODE_RXTX;
	handlerUSART6.USART_Config.USART_baudrate 					= USART_BAUDRATE_57600;
	handlerUSART6.USART_Config.USART_datasize 					= USART_DATASIZE_9BIT;
	handlerUSART6.USART_Config.USART_parity 					= USART_PARITY_ODD;
	USART_Config(&handlerUSART6);

	adcAXIS_XY.channels[0] 		 								= ADC_CHANNEL_0;
	adcAXIS_XY.channels[1] 		 								= ADC_CHANNEL_1;
	adcAXIS_XY.dataAlignment     								= ADC_ALIGNMENT_RIGHT;
	adcAXIS_XY.resolution        								= ADC_RESOLUTION_12_BIT;
	adcAXIS_XY.samplingPeriod    								= ADC_SAMPLING_PERIOD_28_CYCLES;
	ADC_ConfigMultichannel(&adcAXIS_XY, 2);

}

void BasicTimer2_Callback(void){
	flagStatus = 1;
}

void usart2Rx_Callback(void){
	rxData = getRxData();
}

void adcComplete_Callback(void){
	XY_indicator++;
	adcData = getADC();
	if(XY_indicator == 1){
		XY[0] = adcData;
	}else if(XY_indicator == 2){
		XY[1] = adcData;
		adcIsComplete = true;
		XY_indicator = 0;
	}
}

double ServoAngle(int16_t Xvalue, int16_t Yvalue){
	PI = acos(-1);
	if(Xvalue > 0 && Yvalue > 0){
		angle = atan(Yvalue/Xvalue)*(180/PI);
	}else if(Xvalue < 0 && Yvalue > 0){
		angle = 180 - (atan(Yvalue/(-Xvalue))*(180/PI));
	}else if(Xvalue < 0 && Yvalue <= 0){
		angle = 180 + (atan((-Yvalue)/(-Xvalue))*(180/PI));
	}else if(Xvalue > 0 && Yvalue <= 0){
		angle = 360 - (atan((-Yvalue)/(Xvalue))*(180/PI));
	}else if(Xvalue == 0 && Yvalue == 0){
		angle = 0;
	}else if(Xvalue == 0 && Yvalue != 0){
		angle = 90;
	}
	return angle;
}

void statusLED(void){
	GPIOxTogglePin(&handlerBlinkyLed);
}
