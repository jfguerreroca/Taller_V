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

ADC_Config_t  			adcAXIS_XY     			= {0};

// Definimos las variables que utilizaremos

int16_t Xvalue = 0;
int16_t Yvalue = 0;
double PI = 0;
double angle = 0;
uint16_t zfeo = 0;

// Definimos las funciones que vamos a utilizar:

double ServoAngle(int16_t, int16_t);

//Funcion de estado con el User LED

void statusLED(void);

// Funcion principal del programa

int main(void)
{
	/*
	 * Cargamos las configuraciones de cada periferico a utilizar:
	 *
	 * 		-
	 *
	 */

	handlerBlinkyLed.pGPIOx = GPIOA;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinNumber 				= PIN_5;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyLed);

	handlerTxPin.pGPIOx = GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber 					= PIN_11;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode				= AF8;
	GPIO_Config(&handlerTxPin);

	handlerTimer2.ptrTIMx 										= TIM2;
	handlerTimer2.TIMx_Config.TIMx_mode 						= BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed						= BTIMER_SPEED_100us;
	handlerTimer2.TIMx_Config.TIMx_period						= 2500;
	handlerTimer2.TIMx_Config.TIMx_interruptEnable 				= 1;
	BasicTimer_Config(&handlerTimer2);
	startTimer(&handlerTimer2);

	handlerUSART6.ptrUSARTx 									= USART6;
	handlerUSART6.USART_Config.USART_mode 						= USART_MODE_TX;
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

	/* Ciclo infinito del main */
	while(1){

		Xvalue = -2000;
		Yvalue = 200;
		zfeo = ServoAngle(Xvalue,Yvalue);

	}

	return 0;
}

double ServoAngle(int16_t Xvalue, int16_t Yvalue){
	PI = acos(-1);
	if(Xvalue > 0 && Yvalue >= 0){
		angle = (atan(Yvalue/Xvalue))*180/PI;
	}else if(Xvalue < 0 && Yvalue >= 0){
		angle = ((PI) - atan(Yvalue/-Xvalue))*180/PI;
	}else if(Xvalue == 0 && Yvalue >= 0){
		angle = 90;
	}
	return angle;
}

void statusLED(void){
	GPIOxTogglePin(&handlerBlinkyLed);
}
