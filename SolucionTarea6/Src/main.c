/**
 ******************************************************************************
 * @file           : main.c
 * @author         : jfguerreroca
 * @name           : Juan Felipe Guerrero Cataño
 * @brief          : Tarea 6
 ******************************************************************************
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "BasicTimer.h"
#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "PwmDriver.h"
#include "CaptureFrecDriver.h"

// Definimos los handler para los perifericos necesarios en el proyecto

USART_Handler_t 				handlerUSART2			= {0};

BasicTimer_Handler_t 	        handlerTimer2 			= {0};

GPIO_Handler_t 					handlerBlinkyLed 		= {0};
GPIO_Handler_t					handlerTxPin			= {0};
GPIO_Handler_t					handlerRxPin			= {0};
GPIO_Handler_t					handlerPinPWM			= {0};

GPIO_Handler_t					handlerCaptureSignal 	= {0};

Capture_Handler_t				handlerCapture          = {0};
PWM_Handler_t 					handlerPwm 				= {0};

// Definimos las variables que utilizaremos

uint8_t flagStatus 										= 0;
uint8_t flagCap 										= 0;
uint8_t rxData	 										= 0;
uint8_t capCounter 										= 0;

uint32_t ts1 											= 0;
uint32_t ts2		 									= 0;
uint32_t finalPeriod									= 0;

char bufferData[64] 									= {0};

// Definimos las funciones que vamos a utilizar:

//Funcion para inicializar las configuraciones del sistema

void InitSystem(void);

// Funcion principal del programa

int main(void)
{
	InitSystem();

	/* Ciclo infinito del main */
	while(1){


		// Condicional para la interrupcion del blinky
		if(flagStatus){
			flagStatus = 0;
			GPIOxTogglePin(&handlerBlinkyLed);
		}

		/*
		 * El timer siempre esta encendido, y se realiza continuamente la captura de frecuencia,
		 * donde con un contador se toman los dos datos, se restan, y se les hace las correspondientes
		 * operaciones para que el dato se despliegue en ms.
		 * Antes de cada toma de los dos datos, se limpian los registros del counter del timer y del canal
		 * utilizado.
		 */

		if(flagCap){
			if(capCounter){
				ts2 = getTimeStamp(&handlerCapture);
				capCounter = 0;
				finalPeriod = getPeriodFrec(&handlerCapture, ts1, ts2);
				sprintf(bufferData, "\n\rTiempo 1: %u , Tiempo 2: %u ", (unsigned int) ts1, (unsigned int) ts2);
				writeMsg(&handlerUSART2, bufferData);
				sprintf(bufferData, "\n\rPeriodo en ms: %u", (unsigned int) finalPeriod);
				writeMsg(&handlerUSART2, bufferData);
			}else{
				cleanData(&handlerCapture);
				ts1 = getTimeStamp(&handlerCapture);
				capCounter = 1;
			}
			flagCap = 0;
		}
	}

	return 0;
}

void InitSystem(void){

	handlerBlinkyLed.pGPIOx 									= GPIOA;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinNumber 				= PIN_5;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyLed);

	handlerTxPin.pGPIOx 										= GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber 					= PIN_2;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode				= AF7;
	GPIO_Config(&handlerTxPin);

	handlerRxPin.pGPIOx 										= GPIOA;
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

	handlerUSART2.ptrUSARTx 									= USART2;
	handlerUSART2.USART_Config.USART_mode 						= USART_MODE_RXTX;
	handlerUSART2.USART_Config.USART_baudrate 					= USART_BAUDRATE_115200;
	handlerUSART2.USART_Config.USART_datasize 					= USART_DATASIZE_8BIT;
	handlerUSART2.USART_Config.USART_parity 					= USART_PARITY_NONE;
	handlerUSART2.USART_Config.USART_stopbits             		= USART_STOPBIT_1;
	handlerUSART2.USART_Config.USART_enableIntRX          		= USART_RX_INTERRUP_ENABLE;
	USART_Config(&handlerUSART2);

	handlerPinPWM.pGPIOx 										= GPIOB;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinNumber 				= PIN_6;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinAltFunMode				= AF2;
	GPIO_Config(&handlerPinPWM);

	handlerCaptureSignal.pGPIOx 								= GPIOA;
	handlerCaptureSignal.GPIO_PinConfig.GPIO_PinNumber 			= PIN_6;
	handlerCaptureSignal.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerCaptureSignal.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerCaptureSignal.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerCaptureSignal.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerCaptureSignal.GPIO_PinConfig.GPIO_PinAltFunMode		= AF2;
	GPIO_Config(&handlerCaptureSignal);

	// PWM
	handlerPwm.config.channel									= PWM_CHANNEL_1;
	handlerPwm.config.prescaler 								= 1600;
	handlerPwm.config.periodo 									= 500;
	handlerPwm.config.duttyCicle 								= 50;
	handlerPwm.ptrTIMx 											= TIM4;
	pwm_Config(&handlerPwm);
	startPwmSignal(&handlerPwm);

	// Capture Freq
	handlerCapture.ptrTIMx										= TIM3;
	handlerCapture.config.channel								= CAPTURE_CHANNEL_1;
	handlerCapture.config.edgeSignal							= CAPTURE_RISING_EDGE;
	handlerCapture.config.prescalerCapture						= CAPTURE_PREESCALER_8_1;
	handlerCapture.config.timerSpeed							= CAPTURE_TIMER_SPEED_1ms;
	capture_Config(&handlerCapture);

}

void BasicTimer2_Callback(void){
	flagStatus = 1;
}

void usart2Rx_Callback(void){
	rxData = getRxData();
}

void CaptureFrec1_Callback(void){
	flagCap = 1;
}
