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
#include "RTCxDriver.h"
#include "LCDxDriver.h"

// Definimos los handler para los perifericos necesarios en el proyecto

USART_Handler_t 		handlerUSART6			= {0};

BasicTimer_Handler_t 	handlerTimer2 			= {0};
BasicTimer_Handler_t 	handlerTimer4 			= {0};

GPIO_Handler_t 			handlerBlinkyLed 		= {0};
GPIO_Handler_t			handlerTxPin			= {0};
GPIO_Handler_t			handlerRxPin			= {0};
GPIO_Handler_t			handlerPinPWM			= {0};
GPIO_Handler_t			handlerSCL				= {0};
GPIO_Handler_t			handlerSDA				= {0};

ADC_Config_t  			adcAXIS_XY     			= {0};

PWM_Handler_t 			handlerPwm				= {0};

RTC_Handler_t handlerRTC = {0};

I2C_Handler_t handlerLCD = {0};

// Definimos las variables que utilizaremos

bool adcIsComplete 				= false;

int16_t adcData				 	= 0;
int16_t XY[2]					= {0};

uint8_t rxData 					= 0;
uint8_t XY_indicator			= 0;
uint8_t flagStatus 				= 0;
uint8_t flagAdc 				= 0;
uint8_t flagTimer 				= 0;
uint8_t counterReception 		= 0;
bool stringComplete 			= false;
char bufferReception[64] 		= {0};
char cmd[64]; // Para el analisis de comandos
char bufferData[64] 			= {0};
char greetingMsg[] 				= "USART funcionando: muchas gracias aficion, esto es para vosotros hehehe siuuuu \n\r" ;
char userMsg[64] 				= {0};
char dataLCD[64] 				= {0};

unsigned int firstParameter 	= 0;
unsigned int secondParameter 	= 0;

uint8_t hours = 0;
uint8_t mins = 0;
uint8_t segs = 0;
uint8_t day = 0;
uint8_t month = 0;
uint8_t year = 0;
uint16_t *date = 0;

// Definimos las funciones que vamos a utilizar:

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

	LCD_Clear(&handlerLCD);
	delay_10();
	LCD_Init(&handlerLCD);
	delay_10();
	sprintf(dataLCD, "Hola, soy JFGC");
	LCD_setCursor(&handlerLCD,0,0);
	LCD_sendSTR(&handlerLCD,dataLCD);
	sprintf(dataLCD, "Parcial Taller V");
	LCD_setCursor(&handlerLCD,1,0);
	LCD_sendSTR(&handlerLCD,dataLCD);
	sprintf(dataLCD, "Me voy a matar");
	LCD_setCursor(&handlerLCD,2,0);
	LCD_sendSTR(&handlerLCD,dataLCD);

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

		if(flagTimer){
			startSingleADC();
			flagTimer = 0;
		}

		// Se realiza un analisis de la cadena de datos obtenida.
		if (stringComplete){
			parseCommands(bufferReception);
			stringComplete = false;
		}

		if(flagAdc){
			if(XY_indicator){
				adcData = getADC();
				XY_indicator = 0;
				XY[1] = adcData;
				adcIsComplete = true;
			}else{
				XY_indicator = 1;
				adcData = getADC();
				XY[0] = adcData;
			}
			flagAdc = 0;
		}

		if (adcIsComplete == true) {
					sprintf(bufferData, "x = %d y = %d \n\r", (int) XY[0], (int) XY[1]);
					writeMsg(&handlerUSART6, bufferData);
					if (XY[0] > 1300 && XY[0] <= 2300) {
						if (XY[1] > 1300 && XY[1] <= 2300) {
							updateDuttyCycle(&handlerPwm, 8);
						} else if (XY[1] > 0 && XY[1] <= 1300) {
							updateDuttyCycle(&handlerPwm, 14);
						} else if (XY[1] > 2300 && XY[1] < 4100) {
							updateDuttyCycle(&handlerPwm, 20);
						}
					} else if (XY[0] > 2300 && XY[0] <= 4100) {
						if (XY[1] > 1300 && XY[1] <= 2300) {
							updateDuttyCycle(&handlerPwm, 10);
						} else if (XY[1] > 0 && XY[1] <= 1300) {
							updateDuttyCycle(&handlerPwm, 12);
						}
					} else if (XY[0] >= 0 && XY[0] <= 1300) {
						if (XY[1] > 0 && XY[1] <= 1300) {
							updateDuttyCycle(&handlerPwm, 16);
						} else if (XY[1] > 1300 && XY[1] <= 2300) {
							updateDuttyCycle(&handlerPwm, 18);
						}
					} else {
						__NOP();
					}

					adcIsComplete = false;
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

	handlerTxPin.pGPIOx = GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber 					= PIN_11;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode				= AF8;
	GPIO_Config(&handlerTxPin);

	handlerRxPin.pGPIOx = GPIOA;
	handlerRxPin.GPIO_PinConfig.GPIO_PinNumber 					= PIN_12;
	handlerRxPin.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
	handlerRxPin.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerRxPin.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerRxPin.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerRxPin.GPIO_PinConfig.GPIO_PinAltFunMode				= AF8;
	GPIO_Config(&handlerRxPin);

//	handlerTxPin.pGPIOx = GPIOA;
//	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber 					= PIN_2;
//	handlerTxPin.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
//	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
//	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
//	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
//	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode				= AF7;
//	GPIO_Config(&handlerTxPin);
//
//	handlerRxPin.pGPIOx = GPIOA;
//	handlerRxPin.GPIO_PinConfig.GPIO_PinNumber 					= PIN_3;
//	handlerRxPin.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
//	handlerRxPin.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
//	handlerRxPin.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
//	handlerRxPin.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
//	handlerRxPin.GPIO_PinConfig.GPIO_PinAltFunMode				= AF7;
//	GPIO_Config(&handlerRxPin);

	handlerPinPWM.pGPIOx = GPIOB;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinNumber 		   		= PIN_4;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinAltFunMode				= AF2;
	GPIO_Config(&handlerPinPWM);

	handlerSCL.pGPIOx 											= GPIOB;
	handlerSCL.GPIO_PinConfig.GPIO_PinNumber 					= PIN_8;
	handlerSCL.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_ALTFN;
	handlerSCL.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_OPENDRAIN;
	handlerSCL.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerSCL.GPIO_PinConfig.GPIO_PinPuPdControl 				= GPIO_PUPDR_PULLUP;
	handlerSCL.GPIO_PinConfig.GPIO_PinAltFunMode				= AF4;
	GPIO_Config(&handlerSCL);

	handlerSDA.pGPIOx 											= GPIOB;
	handlerSDA.GPIO_PinConfig.GPIO_PinNumber 					= PIN_9;
	handlerSDA.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_ALTFN;
	handlerSDA.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_OPENDRAIN;
	handlerSDA.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerSDA.GPIO_PinConfig.GPIO_PinPuPdControl 				= GPIO_PUPDR_PULLUP;
	handlerSDA.GPIO_PinConfig.GPIO_PinAltFunMode				= AF4;
	GPIO_Config(&handlerSDA);

	handlerTimer2.ptrTIMx 										= TIM2;
	handlerTimer2.TIMx_Config.TIMx_mode 						= BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed						= BTIMER_SPEED_100us;
	handlerTimer2.TIMx_Config.TIMx_period						= 2500;
	handlerTimer2.TIMx_Config.TIMx_interruptEnable 				= 1;
	BasicTimer_Config(&handlerTimer2);
	startTimer(&handlerTimer2);

	handlerTimer4.ptrTIMx 										= TIM4;
	handlerTimer4.TIMx_Config.TIMx_mode 						= BTIMER_MODE_UP;
	handlerTimer4.TIMx_Config.TIMx_speed						= BTIMER_SPEED_100us;
	handlerTimer4.TIMx_Config.TIMx_period						= 2500;
	handlerTimer4.TIMx_Config.TIMx_interruptEnable 				= 1;
	BasicTimer_Config(&handlerTimer4);
	stopTimer(&handlerTimer4);

//	handlerUSART6.ptrUSARTx 									= USART2;
//	handlerUSART6.USART_Config.USART_mode 						= USART_MODE_RXTX;
//	handlerUSART6.USART_Config.USART_baudrate 					= USART_BAUDRATE_57600;
//	handlerUSART6.USART_Config.USART_datasize 					= USART_DATASIZE_9BIT;
//	handlerUSART6.USART_Config.USART_parity 					= USART_PARITY_ODD;
//	handlerUSART6.USART_Config.USART_stopbits             		= USART_STOPBIT_1;
//	handlerUSART6.USART_Config.USART_enableIntRX          		= USART_RX_INTERRUP_ENABLE;
//	USART_Config(&handlerUSART6);

	handlerUSART6.ptrUSARTx 									= USART6;
	handlerUSART6.USART_Config.USART_mode 						= USART_MODE_RXTX;
	handlerUSART6.USART_Config.USART_baudrate 					= USART_BAUDRATE_57600;
	handlerUSART6.USART_Config.USART_datasize 					= USART_DATASIZE_9BIT;
	handlerUSART6.USART_Config.USART_parity 					= USART_PARITY_ODD;
	handlerUSART6.USART_Config.USART_stopbits             		= USART_STOPBIT_1;
	handlerUSART6.USART_Config.USART_enableIntRX          		= USART_RX_INTERRUP_ENABLE;
	USART_Config(&handlerUSART6);

	adcAXIS_XY.channels[0] 		 								= ADC_CHANNEL_0;
	adcAXIS_XY.channels[1] 		 								= ADC_CHANNEL_1;
	adcAXIS_XY.dataAlignment     								= ADC_ALIGNMENT_RIGHT;
	adcAXIS_XY.resolution        								= ADC_RESOLUTION_12_BIT;
	adcAXIS_XY.samplingPeriod    								= ADC_SAMPLING_PERIOD_28_CYCLES;
	ADC_ConfigMultichannel(&adcAXIS_XY, 2);

	handlerPwm.config.channel		= PWM_CHANNEL_1;
	handlerPwm.config.prescaler 	= 1600;
	handlerPwm.config.periodo 		= 200;
	handlerPwm.config.duttyCicle 	= 5;
	handlerPwm.ptrTIMx = TIM3;
	pwm_Config(&handlerPwm);
	startPwmSignal(&handlerPwm);

	handlerRTC.RTC_Config.RTC_Hours = 23;
	handlerRTC.RTC_Config.RTC_Minutes = 59;
	handlerRTC.RTC_Config.RTC_Seconds = 30;
	handlerRTC.RTC_Config.RTC_Year = 22;
	handlerRTC.RTC_Config.RTC_Month = 11;
	handlerRTC.RTC_Config.RTC_TimeFormat = TIME_FORMAT_24_HOUR;
	handlerRTC.RTC_Config.RTC_TimeNotation = TIME_NOTATION_AM_OR_24;
	handlerRTC.RTC_Config.RTC_ValueDay = 5;
	RTC_Config(&handlerRTC);

	handlerLCD.ptrI2Cx				= I2C1;
	handlerLCD.modeI2C				= I2C_MODE_SM;
	handlerLCD.slaveAddress			= LCD_ADDRESS;
	i2c_config(&handlerLCD);
}

void BasicTimer2_Callback(void){
	flagStatus = 1;
}

void BasicTimer4_Callback(void){
	flagTimer = 1;
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
		writeMsg(&handlerUSART6, "CMD: dummy\n\r");
	}else if(strcmp(cmd, "startSingleADC") == 0){
		startSingleADC();
		writeMsg(&handlerUSART6, "CMD: ADC working\n\r");
		sprintf(bufferData, "x = %d, y = %d \n\r", (int) XY[0], (int) XY[1]);
		writeMsg(&handlerUSART6, bufferData);
	}else if(strcmp(cmd, "duty") == 0){
		updateDuttyCycle(&handlerPwm, (int) firstParameter);
		sprintf(bufferData, "PWM updated: %u\n\r", firstParameter);
		writeMsg(&handlerUSART6, bufferData);
	}else if (strcmp(cmd, "readDate") == 0){
		date = read_date();
		hours = date[2];
		mins = date[1];
		segs = date[0];
		day = date[4];
		month = date[5];
		year = date[6];
		sprintf(bufferData, "La fecha actual es: \n\rlas %u:%u:%u del %u/%u/%u\n\r", (unsigned int) hours, mins, segs, day, month, year );
		writeMsg(&handlerUSART6, bufferData);
	}else if(strcmp(cmd, "startTimer") == 0){
		startTimer(&handlerTimer4);
		writeMsg(&handlerUSART6, "CMD: Timer on\n\r");
	}else if(strcmp(cmd, "stopTimer") == 0){
		stopTimer(&handlerTimer4);
		writeMsg(&handlerUSART6, "CMD: Timer off\n\r");
	}else if(strcmp(cmd, "printlastADC") == 0){
		sprintf(bufferData, "x = %d, y = %d \n\r", (int) XY[0], (int) XY[1]);
		writeMsg(&handlerUSART6, bufferData);
	}else if(strcmp(cmd, "name") == 0){
		writeMsg(&handlerUSART6, "Juan Felipe Guerrero Cataño\n\r");
	}else if(strcmp(cmd, "printLCD") == 0){
		sprintf(dataLCD, "%s\n\r", firstParameter);
		LCD_setCursor(&handlerLCD,4,0);
		LCD_sendSTR(&handlerLCD,dataLCD);
	}

}

void statusLED(void){
	GPIOxTogglePin(&handlerBlinkyLed);
}
