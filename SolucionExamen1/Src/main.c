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
BasicTimer_Handler_t 	handlerTimer5 			= {0};

GPIO_Handler_t 			handlerBlinkyLed 		= {0};
GPIO_Handler_t			handlerTxPin			= {0};
GPIO_Handler_t			handlerRxPin			= {0};
GPIO_Handler_t			handlerPinPWM			= {0};
GPIO_Handler_t			handlerSCL				= {0};
GPIO_Handler_t			handlerSDA				= {0};

ADC_Config_t  			adcAXIS_XY     			= {0};

PWM_Handler_t 			handlerPwm				= {0};

RTC_Handler_t handlerRTC 						= {0};

I2C_Handler_t handlerLCD 						= {0};

// Definimos las variables que utilizaremos

bool adcIsComplete 				= false;

int16_t adcData				 	= 0;
int16_t XY[2]					= {0};

uint8_t rxData 					= 0;
uint8_t XY_indicator			= 0;
uint8_t flagStatus 				= 0;
uint8_t flagAdc 				= 0;
uint8_t flagTimer 				= 0;
uint8_t flagDate 				= 0;
uint8_t counterReception 		= 0;

bool stringComplete 			= false;

char bufferReception[64] 		= {0};
char cmd[64];
char bufferData[64] 			= {0};
char userMsg[64] 				= {0};
char dataLCD[64] 				= {0};

unsigned int firstParameter 	= 0;
unsigned int secondParameter 	= 0;

uint8_t hours 					= 0;
uint8_t mins 					= 0;
uint8_t segs 					= 0;
uint8_t day 					= 0;
uint8_t month 					= 0;
uint8_t year 					= 0;
uint16_t *date 					= 0;

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
	 * 		- 2 GPIO para la transmision I2C (SDA y SCL)
	 * 		- 1 GPIO para el control del Blinky
	 * 		- 1 GPIO para la salida PWM (con el timer 3)
	 * 		- 4 timer, el TIM2 encargado del Blinky, el TIM3 encargado del PWM (este no se configura
	 * 		  como timer, ya que el pwm lo hace), el TIM4 encargado del lanzamiento del
	 * 		  ADC y el TIM5 encargado de la tasa de refresco de la fecha en la LCD.
	 * 		- 1 Usart encargado de la comunicacion serial (USART6)
	 * 		- 1 ADC, de manera multi canal, para la lectura de los ejes X y Y del joystick
	 * 		- 1 PWM, para entregar los ciclos de trabajo al servomotor
	 * 		- 1 RTC, para desplegar la fecha y hora
	 * 		- 1 I2C, para controlar la pantalla LCD
	 *
	 */

	InitSystem();

	writeChar(&handlerUSART6, ' ');

	//Inicializamos la LCD, poniendo en las dos primeras filas mensajes

	LCD_Clear(&handlerLCD);
	delay_10();
	LCD_Init(&handlerLCD);
	delay_10();
	sprintf(dataLCD, "Examen Taller V");
	LCD_setCursor(&handlerLCD,0,0);
	LCD_sendSTR(&handlerLCD,dataLCD);
	sprintf(dataLCD, "Hecho por: JFGC");
	LCD_setCursor(&handlerLCD,0,1);
	LCD_sendSTR(&handlerLCD,dataLCD);

	/* Ciclo infinito del main */
	while(1){


		// Condicional para la interrupcion del blinky
		if(flagStatus){
			flagStatus = 0;
			statusLED();
		}

		/*
		 * Condicional para la interrupcion de la tasa de refresco de la fecha en la LCD, donde
		 * imprimimos en la tercera fila de la LCD, la fecha actual del RTC.		 *
		 */
		if(flagDate){
			flagDate = 0;
			date = read_date();
			hours = date[2];
			mins = date[1];
			segs = date[0];
			day = date[4];
			month = date[5];
			year = date[6];
			LCD_ClearScreen(&handlerLCD, 2);
			sprintf(dataLCD, "%u:%u:%u %u/%u/%u", (unsigned int) hours, mins, segs, day, month, year );
			LCD_setCursor(&handlerLCD,0,2);
			LCD_sendSTR(&handlerLCD,dataLCD);
		}

		// Con esto configuramos la recepcion de comandos
		if (rxData != '\0'){
			bufferReception[counterReception] = rxData;
			counterReception++;
			if (rxData == '@'){
				stringComplete = true;
				bufferReception[counterReception] = '\0';
				counterReception = 0;
				writeMsg(&handlerUSART6, bufferReception);
			}
			rxData = '\0';
		}


		/* Condicional para la interrupcion del timer 4, para lanzar la conversion ADC, y asi comenzar el control del servo
		   con el joystick */
		if(flagTimer){
			startSingleADC();
			flagTimer = 0;
		}

		// Se realiza un analisis de la cadena de datos obtenida
		if (stringComplete){
			parseCommands(bufferReception);
			stringComplete = false;
		}

		/*
		 * Con esta bandera, configuramos la adquisicion de datos para el joystick.
		 * Sabemos que dentro de la secuencia hay dos conversiones, cada una con su
		 * respectiva interrupcion, por lo que usamos un contador para adquirir primero
		 * el dato en X, y luego el dato en Y, almacenando estos en un arreglo y levantando
		 * la bandera adcIsComplete
		 */
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


		/*
		 * Cuando esta bandera es levantada, significa que los dos valores (X y Y) fueron
		 * adquiridos satisfactoriamente.
		 * Lo que hacemos con estos es imprimirlos por comunicacion serial, y definimos
		 * unos rangos de movimiento del joystick, para los cuales se les asignaron
		 * valores de duty arbitrarios, de manera que se pueda controlar el servo con
		 * el movimiento del joystick.
		 * El movimiento es continuo, siempre y cuando el timer 4 esté activo.
		 */
		if (adcIsComplete == true) {
					sprintf(bufferData, "x = %d y = %d \n\r", (int) XY[0], (int) XY[1]);
					writeMsg(&handlerUSART6, bufferData);
					if(XY[0] > 0 && XY[0] <= 2300) {
						if (XY[1] > 1300 && XY[1] <= 2300) {
							updateDuttyCycle(&handlerPwm, 8);
						} else if (XY[1] > 0 && XY[1] <= 1300) {
							updateDuttyCycle(&handlerPwm, 14);
						} else if (XY[1] > 2300 && XY[1] < 4100) {
							updateDuttyCycle(&handlerPwm, 20);
						}
					}else if (XY[0] > 2300 && XY[0] <= 4100) {
						if (XY[1] > 1300 && XY[1] <= 2300) {
							updateDuttyCycle(&handlerPwm, 10);
						} else if (XY[1] > 0 && XY[1] <= 1300) {
							updateDuttyCycle(&handlerPwm, 12);
						}
					}else {
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

	handlerTimer5.ptrTIMx 										= TIM5;
	handlerTimer5.TIMx_Config.TIMx_mode 						= BTIMER_MODE_UP;
	handlerTimer5.TIMx_Config.TIMx_speed						= BTIMER_SPEED_100us;
	handlerTimer5.TIMx_Config.TIMx_period						= 500;
	handlerTimer5.TIMx_Config.TIMx_interruptEnable 				= 1;
	BasicTimer_Config(&handlerTimer5);
	startTimer(&handlerTimer5);

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

	handlerPwm.config.channel									= PWM_CHANNEL_1;
	handlerPwm.config.prescaler 								= 1600;
	handlerPwm.config.periodo 									= 200;
	handlerPwm.config.duttyCicle 								= 5;
	handlerPwm.ptrTIMx 											= TIM3;
	pwm_Config(&handlerPwm);
	startPwmSignal(&handlerPwm);

	handlerRTC.RTC_Config.RTC_Hours 							= 23;
	handlerRTC.RTC_Config.RTC_Minutes							= 59;
	handlerRTC.RTC_Config.RTC_Seconds 							= 30;
	handlerRTC.RTC_Config.RTC_Year 								= 22;
	handlerRTC.RTC_Config.RTC_Month 							= 11;
	handlerRTC.RTC_Config.RTC_TimeFormat 						= TIME_FORMAT_24_HOUR;
	handlerRTC.RTC_Config.RTC_TimeNotation 						= TIME_NOTATION_AM_OR_24;
	handlerRTC.RTC_Config.RTC_ValueDay 							= 5;
	RTC_Config(&handlerRTC);

	handlerLCD.ptrI2Cx											= I2C1;
	handlerLCD.modeI2C											= I2C_MODE_SM;
	handlerLCD.slaveAddress										= LCD_ADDRESS;
	i2c_config(&handlerLCD);
}

void BasicTimer2_Callback(void){
	flagStatus = 1;
}

void BasicTimer4_Callback(void){
	flagTimer = 1;
}

void BasicTimer5_Callback(void){
	flagDate = 1;
}

void usart6Rx_Callback(void){
	rxData = getRxData();
}

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
		writeMsg(&handlerUSART6, "2) dummy				----	prints a test message\n");
		writeMsg(&handlerUSART6, "3) startSingleADC		----	start a single ADC (timer 4 off)\n");
		writeMsg(&handlerUSART6, "4) duty #d			----	updates PWM duty cycle\n");
		writeMsg(&handlerUSART6, "5) readDate			----	prints actual date and time\n");
		writeMsg(&handlerUSART6, "6) startTimer			----	start timer 4 (ADC conversion every 250ms)\n");
		writeMsg(&handlerUSART6, "7) stopTimer			----	stop timer 4 (ADC conversion every 250ms)\n");
		writeMsg(&handlerUSART6, "8) printlastADC		----	prints the last ADC value\n");
		writeMsg(&handlerUSART6, "9) name				----	shows the name of the GOAT who made this\n");
		writeMsg(&handlerUSART6, "10) printLCD			----	prints a surprise message on the LCD screen (4th row)\n");

	}else if(strcmp(cmd, "dummy") == 0 || strcmp(cmd, "2") == 0){
		writeMsg(&handlerUSART6, "\n\rCMD: dummy working\n\r");
	}else if(strcmp(cmd, "startSingleADC") == 0 || strcmp(cmd, "3") == 0){
		startSingleADC();
		writeMsg(&handlerUSART6, "\n\rCMD: ADC working\n\r");
		sprintf(bufferData, "\n\r x = %d, y = %d \n\r", (int) XY[0], (int) XY[1]);
		writeMsg(&handlerUSART6, bufferData);
	}else if(strcmp(cmd, "duty") == 0 || strcmp(cmd, "4") == 0){
		updateDuttyCycle(&handlerPwm, (int) firstParameter);
		sprintf(bufferData, "\n\rPWM updated: %u\n\r", firstParameter);
		writeMsg(&handlerUSART6, bufferData);
	}else if (strcmp(cmd, "readDate") == 0 || strcmp(cmd, "5") == 0){
		date = read_date();
		hours = date[2];
		mins = date[1];
		segs = date[0];
		day = date[4];
		month = date[5];
		year = date[6];
		sprintf(bufferData, "\n\rThe time is: \n\r %u:%u:%u - %u/%u/%u\n\r", (unsigned int) hours, mins, segs, day, month, year );
		writeMsg(&handlerUSART6, bufferData);
	}else if(strcmp(cmd, "startTimer") == 0 || strcmp(cmd, "6") == 0){
		startTimer(&handlerTimer4);
		writeMsg(&handlerUSART6, "\n\rCMD: Timer on\n\r");
	}else if(strcmp(cmd, "stopTimer") == 0 || strcmp(cmd, "7") == 0){
		stopTimer(&handlerTimer4);
		writeMsg(&handlerUSART6, "\n\rCMD: Timer off\n\r");
	}else if(strcmp(cmd, "printlastADC") == 0 || strcmp(cmd, "8") == 0){
		sprintf(bufferData, "\n\rx = %d, y = %d \n\r", (int) XY[0], (int) XY[1]);
		writeMsg(&handlerUSART6, bufferData);
	}else if(strcmp(cmd, "name") == 0 || strcmp(cmd, "9") == 0){
		writeMsg(&handlerUSART6, "\n\rJuan Felipe Guerrero Catano\n\r");
	}else if(strcmp(cmd, "printLCD") == 0 || strcmp(cmd, "10") == 0){
		sprintf(dataLCD, "Nerio, pongame 5");
		LCD_setCursor(&handlerLCD,0,3);
		LCD_sendSTR(&handlerLCD,dataLCD);
	}
}

void statusLED(void){
	GPIOxTogglePin(&handlerBlinkyLed);
}
