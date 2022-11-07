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
GPIO_Handler_t			handlerPinPWM			= {0};

ADC_Config_t  			adcAXIS_XY     			= {0};

PWM_Handler_t 			handlerPwm				= {0};

// Definimos las variables que utilizaremos

bool adcIsComplete 				= false;

int16_t adcData				 	= 0;
int16_t XY[2]					= {0};

uint8_t rxData 					= 0;
uint8_t XY_indicator			= 0;
uint8_t flagStatus 				= 0;
uint8_t flagAdc 				= 0;
uint8_t angleValue 				= 0;
uint8_t counterReception 		= 0;
bool stringComplete 			= false;
char bufferReception[64] 		= {0};
uint16_t start 					= 0;
char cmd[64]; // Para el analisis de comandos
unsigned int firstParameter 	= 0;
unsigned int secondParameter 	= 0;
char userMsg[64] 				= {0};

char bufferData[64] 			= {0};

char greetingMsg[] = "USART funcionando: muchas gracias aficion, esto es para vosotros hehehe siuuuu \n\r" ;

// Definimos las funciones que vamos a utilizar:

//Funcion que retorna el angulo del servomotor

int16_t servoAngle(int16_t, int16_t);

//Funcion para los comandos de consola

void parseCommands (char *ptrBufferReception);

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

		if (rxData != '\0'){
			bufferReception[counterReception] = rxData;
			counterReception++;
			if (rxData == 'm'){
				writeMsg(&handlerUSART6, greetingMsg);
			}
			if (rxData == 's'){
				startSingleADC();
			}
			if (rxData == '@'){
				stringComplete = true;
				bufferReception[counterReception-1] = '\0';
				counterReception = 0;
				writeMsg(&handlerUSART6, bufferReception);
			}
			rxData = '\0';
		}

		// Se realiza un analisis de la cadena de datos obtenida.
		if (stringComplete){
			parseCommands(bufferReception);
			stringComplete = false;
		}

		if(flagAdc){
			if(XY_indicator){
				adcData = (getADC() - 2045) * (-1);
				XY_indicator = 0;
				XY[1] = adcData;
				adcIsComplete = true;
			}else{
				XY_indicator = 1;
				adcData = getADC() - 2045;
				XY[0] = adcData;
			}
			flagAdc = 0;
		}

		if(adcIsComplete == true){
			sprintf(bufferData, "ADC X = %d ADC Y = %d \n\r", (int) XY[0], (int) XY[1]);
			writeMsg(&handlerUSART6, bufferData);
			uint16_t duty = (60/7*servoAngle(XY[0], XY[1])) - (480/7);
			updateDuttyCycle(&handlerPwm, duty);
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

	handlerPinPWM.pGPIOx = GPIOB;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinNumber 		   		= PIN_4;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinAltFunMode				= AF2;
	GPIO_Config(&handlerPinPWM);

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
	handlerUSART6.USART_Config.USART_stopbits             		= USART_STOPBIT_1;
	handlerUSART6.USART_Config.USART_enableIntRX          		= USART_RX_INTERRUP_ENABLE;
	USART_Config(&handlerUSART6);

//	handlerUSART6.ptrUSARTx 									= USART6;
//	handlerUSART6.USART_Config.USART_mode 						= USART_MODE_RXTX;
//	handlerUSART6.USART_Config.USART_baudrate 					= USART_BAUDRATE_57600;
//	handlerUSART6.USART_Config.USART_datasize 					= USART_DATASIZE_9BIT;
//	handlerUSART6.USART_Config.USART_parity 					= USART_PARITY_ODD;
//	USART_Config(&handlerUSART6);

	adcAXIS_XY.channels[0] 		 								= ADC_CHANNEL_0;
	adcAXIS_XY.channels[1] 		 								= ADC_CHANNEL_1;
	adcAXIS_XY.dataAlignment     								= ADC_ALIGNMENT_RIGHT;
	adcAXIS_XY.resolution        								= ADC_RESOLUTION_12_BIT;
	adcAXIS_XY.samplingPeriod    								= ADC_SAMPLING_PERIOD_28_CYCLES;
	ADC_ConfigMultichannel(&adcAXIS_XY, 2);

	handlerPwm.config.channel		= PWM_CHANNEL_1;
	handlerPwm.config.prescaler 	= 1600;
	handlerPwm.config.periodo 		= 200;
	handlerPwm.config.duttyCicle 	= 1;
	handlerPwm.ptrTIMx = TIM3;
	pwm_Config(&handlerPwm);
	startPwmSignal(&handlerPwm);
}

void BasicTimer2_Callback(void){
	flagStatus = 1;
}

void usart2Rx_Callback(void){
	rxData = getRxData();
}

//void usart6Rx_Callback(void){
//	rxData = getRxData();
//}

void adcComplete_Callback(void){
	flagAdc = 1;
}

int16_t servoAngle(int16_t Xvalue,int16_t Yvalue){
	Xvalue = Xvalue * 100/2045;
	Yvalue = Yvalue * 100/2045;
	uint16_t angle = 0;
	if(Yvalue >= 0){
		angle = 180 * atan2(Yvalue,Xvalue) / (M_PI);
	}else{
		angle = 180 * atan2(-Yvalue,-Xvalue) / (M_PI);
		angle += 180;
	}
	return angle;
}

void parseCommands (char *ptrBufferReception){
	// Esta función lee la cadena de caracteres a lo que apunta el 'ptr' y la divide
	// y almacena en tres elementos diferentes: Un string llamado 'cmd', y dos números
	// integer llamados 'firstParameter' y 'secondparameter', de esta forma, es posible
	// introducir información al micro desde el puerto serial.
	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter, &secondParameter, userMsg);
	// Este primer comando imprime una lista con los otros comandos que tiene el equipo.
	if(strcmp(cmd, "help") == 0 || strcmp(cmd, "1") == 0){
		writeMsg(&handlerUSART6, "\n");
		writeMsg(&handlerUSART6, "Help menus CMDs:\n");
		writeMsg(&handlerUSART6, "1) help				----	print this menu\n");
		writeMsg(&handlerUSART6, "2) updateRGB #H #S	----	initialize the accelerometer\n");
		writeMsg(&handlerUSART6, "3) up					----	read data on the x-axis of the accelerometer\n");
		writeMsg(&handlerUSART6, "4) currentDate		----	read data on the y-axis of the accelerometer\n");
		writeMsg(&handlerUSART6, "5) readJoystick		----	read data on both axes of the accelerometer\n");
		writeMsg(&handlerUSART6, "6) readAccXYc			----	read data on both axes of the accelerometer continuously\n");
		writeMsg(&handlerUSART6, "7) stopReadAccXYc		----	stop read data on both axes of the accelerometer continuously\n");
		writeMsg(&handlerUSART6, "8) setDate			----	set the date that will be shown on the console\n");
		writeMsg(&handlerUSART6, "9) showDate			----	show date only once\n");
		writeMsg(&handlerUSART6, "10) showDateC			----	show date continuously\n");
		writeMsg(&handlerUSART6, "11) stopShowDateC		----	stop show date continuously\n");
		writeMsg(&handlerUSART6, "12) setPeriod #		----	change the state led period\n");
	}else if(strcmp(cmd, "dummy") == 0){
		writeMsg(&handlerUSART6, "CMD: dummy\n");
	}else if(strcmp(cmd, "up") == 0){
			startSingleADC();
	}
//	else if (strcmp(cmd, "currentDate") == 0){
//		read_date();
//		Hours = calendario[2];
//		Mins = calendario[1];
//		Segs = calendario[0];
//		sprintf(bufferTime, "Current time is: %uh: %um: %us: \n\r", (unsigned int) Hours,Mins,Segs );
//		writeMsg(&handlerUsart6, bufferTime);
}

void statusLED(void){
	GPIOxTogglePin(&handlerBlinkyLed);
}
