/*
 *
 ******************************************************************************
 * @file           : main.c
 * @author         : jfguerreroca
 * @name           : Juan Felipe Guerrero Cataño
 * @brief          : Proyecto CNC Láser
 ******************************************************************************
 *
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
#include "CaptureFrecDriver.h"
#include "SysTickDriver.h"

// Definimos los handler para los perifericos necesarios en el proyecto

USART_Handler_t 				handlerUSART2			= {0};

BasicTimer_Handler_t 	        handlerTimer2 			= {0};

GPIO_Handler_t 					handlerBlinkyLed 		= {0};

GPIO_Handler_t					handlerTxPin			= {0};
GPIO_Handler_t					handlerRxPin			= {0};

GPIO_Handler_t 					handlerStepperA_X 		= {0};
GPIO_Handler_t 					handlerStepperB_X 		= {0};
GPIO_Handler_t 					handlerStepperC_X 		= {0};
GPIO_Handler_t 					handlerStepperD_X 		= {0};

GPIO_Handler_t 					handlerStepperA_Y1 		= {0};
GPIO_Handler_t 					handlerStepperB_Y1 		= {0};
GPIO_Handler_t 					handlerStepperC_Y1 		= {0};
GPIO_Handler_t 					handlerStepperD_Y1 		= {0};

GPIO_Handler_t 					handlerStepperA_Y2 		= {0};
GPIO_Handler_t 					handlerStepperB_Y2 		= {0};
GPIO_Handler_t 					handlerStepperC_Y2 		= {0};
GPIO_Handler_t 					handlerStepperD_Y2 		= {0};

// Definimos las variables que utilizaremos

uint8_t flagStatus 										= 0;
double delayTime										= 0;
uint16_t rpm											= 0;
uint16_t steps											= 0;
signed int firstParameter 								= 0;
signed int secondParameter 								= 0;
uint8_t rxData 											= 0;
uint8_t counterReception 								= 0;
char bufferReception[64] 								= {0};
char bufferData[64] 									= {0};
char userMsg[64] 										= {0};
char cmd[64];
bool stringComplete 									= false;


// Definimos las funciones que vamos a utilizar:

//Funcion para inicializar las configuraciones del sistema

void InitSystem(void);

void parseCommands (char *ptrBufferReception);

void SMlowX(void);

void SMlowY1(void);

void SMlowY2(void);

void StepperMotorX(uint8_t);

void StepperMotorY1(uint8_t);

void StepperMotorY2(uint8_t);

double RPMtoDelay (uint16_t);

void stepsY_CW (uint16_t);

void stepsY_CCW (uint16_t);

void stepsX_CW (uint16_t);

void stepsX_CCW (uint16_t);

// Funcion principal del programa

int main(void)
{
	SCB -> CPACR |= 0xF << 20;

	InitSystem();

	rpm = 50;
	delayTime = RPMtoDelay(rpm);



	/* Ciclo infinito del main */
	while(1){
		// Condicional para la interrupcion del blinky
		if(flagStatus){
			flagStatus = 0;
			GPIOxTogglePin(&handlerBlinkyLed);
		}

		if (rxData != '\0'){
			bufferReception[counterReception] = rxData;
			counterReception++;
			if (rxData == '@'){
				stringComplete = true;
				bufferReception[counterReception] = '\0';
				counterReception = 0;
				writeMsg(&handlerUSART2, bufferReception);
			}
			rxData = '\0';
		}

		if (stringComplete){
			parseCommands(bufferReception);
			stringComplete = false;
		}
}

	return 0;
}

void InitSystem(void){

	config_SysTickMs();

	handlerBlinkyLed.pGPIOx 										= GPIOA;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinNumber 					= PIN_5;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyLed);

	handlerStepperA_X.pGPIOx 										= GPIOC;
	handlerStepperA_X.GPIO_PinConfig.GPIO_PinNumber 				= PIN_0;
	handlerStepperA_X.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperA_X.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperA_X.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_MEDIUM;
	handlerStepperA_X.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperA_X);
	GPIO_WritePin(&handlerStepperA_X, RESET);

	handlerStepperB_X.pGPIOx 										= GPIOC;
	handlerStepperB_X.GPIO_PinConfig.GPIO_PinNumber 				= PIN_1;
	handlerStepperB_X.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperB_X.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperB_X.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_MEDIUM;
	handlerStepperB_X.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperB_X);
	GPIO_WritePin(&handlerStepperB_X, RESET);

	handlerStepperC_X.pGPIOx 										= GPIOC;
	handlerStepperC_X.GPIO_PinConfig.GPIO_PinNumber 				= PIN_2;
	handlerStepperC_X.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperC_X.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperC_X.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_MEDIUM;
	handlerStepperC_X.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperC_X);
	GPIO_WritePin(&handlerStepperC_X, RESET);

	handlerStepperD_X.pGPIOx 										= GPIOC;
	handlerStepperD_X.GPIO_PinConfig.GPIO_PinNumber 				= PIN_3;
	handlerStepperD_X.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperD_X.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperD_X.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_MEDIUM;
	handlerStepperD_X.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperD_X);
	GPIO_WritePin(&handlerStepperD_X, RESET);

	handlerStepperA_Y1.pGPIOx 										= GPIOA;
	handlerStepperA_Y1.GPIO_PinConfig.GPIO_PinNumber 				= PIN_6;
	handlerStepperA_Y1.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperA_Y1.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperA_Y1.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperA_Y1.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperA_Y1);
	GPIO_WritePin(&handlerStepperA_Y1, RESET);

	handlerStepperB_Y1.pGPIOx 										= GPIOA;
	handlerStepperB_Y1.GPIO_PinConfig.GPIO_PinNumber 				= PIN_7;
	handlerStepperB_Y1.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperB_Y1.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperB_Y1.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperB_Y1.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperB_Y1);
	GPIO_WritePin(&handlerStepperB_Y1, RESET);

	handlerStepperC_Y1.pGPIOx 										= GPIOA;
	handlerStepperC_Y1.GPIO_PinConfig.GPIO_PinNumber 				= PIN_8;
	handlerStepperC_Y1.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperC_Y1.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperC_Y1.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperC_Y1.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperC_Y1);
	GPIO_WritePin(&handlerStepperC_Y1, RESET);

	handlerStepperD_Y1.pGPIOx 										= GPIOA;
	handlerStepperD_Y1.GPIO_PinConfig.GPIO_PinNumber 				= PIN_9;
	handlerStepperD_Y1.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperD_Y1.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperD_Y1.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperD_Y1.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperD_Y1);
	GPIO_WritePin(&handlerStepperD_Y1, RESET);

	handlerStepperA_Y2.pGPIOx 										= GPIOB;
	handlerStepperA_Y2.GPIO_PinConfig.GPIO_PinNumber 				= PIN_1;
	handlerStepperA_Y2.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperA_Y2.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperA_Y2.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperA_Y2.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperA_Y2);
	GPIO_WritePin(&handlerStepperA_Y2, RESET);

	handlerStepperB_Y2.pGPIOx 										= GPIOB;
	handlerStepperB_Y2.GPIO_PinConfig.GPIO_PinNumber 				= PIN_2;
	handlerStepperB_Y2.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperB_Y2.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperB_Y2.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperB_Y2.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperB_Y2);
	GPIO_WritePin(&handlerStepperB_Y2, RESET);

	handlerStepperC_Y2.pGPIOx 										= GPIOB;
	handlerStepperC_Y2.GPIO_PinConfig.GPIO_PinNumber 				= PIN_14;
	handlerStepperC_Y2.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperC_Y2.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperC_Y2.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperC_Y2.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperC_Y2);
	GPIO_WritePin(&handlerStepperC_Y2, RESET);

	handlerStepperD_Y2.pGPIOx 										= GPIOB;
	handlerStepperD_Y2.GPIO_PinConfig.GPIO_PinNumber 				= PIN_15;
	handlerStepperD_Y2.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerStepperD_Y2.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerStepperD_Y2.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_MEDIUM;
	handlerStepperD_Y2.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStepperD_Y2);
	GPIO_WritePin(&handlerStepperD_Y2, RESET);

	handlerTxPin.pGPIOx = GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber 						= PIN_2;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_ALTFN;
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType 						= GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_FAST;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl 				= GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode					= AF7;
	GPIO_Config(&handlerTxPin);

	handlerRxPin.pGPIOx = GPIOA;
	handlerRxPin.GPIO_PinConfig.GPIO_PinNumber 						= PIN_3;
	handlerRxPin.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_ALTFN;
	handlerRxPin.GPIO_PinConfig.GPIO_PinOPType 						= GPIO_OTYPE_PUSHPULL;
	handlerRxPin.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_FAST;
	handlerRxPin.GPIO_PinConfig.GPIO_PinPuPdControl 				= GPIO_PUPDR_NOTHING;
	handlerRxPin.GPIO_PinConfig.GPIO_PinAltFunMode					= AF7;
	GPIO_Config(&handlerRxPin);

	handlerTimer2.ptrTIMx 											= TIM2;
	handlerTimer2.TIMx_Config.TIMx_mode 							= BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed							= BTIMER_SPEED_100us;
	handlerTimer2.TIMx_Config.TIMx_period							= 2500;
	handlerTimer2.TIMx_Config.TIMx_interruptEnable 					= 1;
	BasicTimer_Config(&handlerTimer2);

	handlerUSART2.ptrUSARTx 										= USART2;
	handlerUSART2.USART_Config.USART_mode 							= USART_MODE_RXTX;
	handlerUSART2.USART_Config.USART_baudrate 						= USART_BAUDRATE_115200;
	handlerUSART2.USART_Config.USART_datasize 						= USART_DATASIZE_8BIT;
	handlerUSART2.USART_Config.USART_parity 						= USART_PARITY_NONE;
	handlerUSART2.USART_Config.USART_stopbits             			= USART_STOPBIT_1;
	handlerUSART2.USART_Config.USART_enableIntRX          			= USART_RX_INTERRUP_ENABLE;
	USART_Config(&handlerUSART2);
}

void StepperMotorX(uint8_t motor){
	if(motor == 1){
		GPIO_WritePin(&handlerStepperA_X, SET);
		GPIO_WritePin(&handlerStepperB_X, RESET);
		GPIO_WritePin(&handlerStepperC_X, RESET);
		GPIO_WritePin(&handlerStepperD_X, RESET);
	}else if(motor == 2){
		GPIO_WritePin(&handlerStepperA_X, RESET);
		GPIO_WritePin(&handlerStepperB_X, SET);
		GPIO_WritePin(&handlerStepperC_X, RESET);
		GPIO_WritePin(&handlerStepperD_X, RESET);
	}else if(motor == 3){
		GPIO_WritePin(&handlerStepperA_X, RESET);
		GPIO_WritePin(&handlerStepperB_X, RESET);
		GPIO_WritePin(&handlerStepperC_X, SET);
		GPIO_WritePin(&handlerStepperD_X, RESET);
	}else if(motor == 4){
		GPIO_WritePin(&handlerStepperA_X, RESET);
		GPIO_WritePin(&handlerStepperB_X, RESET);
		GPIO_WritePin(&handlerStepperC_X, RESET);
		GPIO_WritePin(&handlerStepperD_X, SET);
	}
}

void StepperMotorY1(uint8_t motor){
	if(motor == 1){
		GPIO_WritePin(&handlerStepperA_Y1, SET);
		GPIO_WritePin(&handlerStepperB_Y1, RESET);
		GPIO_WritePin(&handlerStepperC_Y1, RESET);
		GPIO_WritePin(&handlerStepperD_Y1, RESET);
	}else if(motor == 2){
		GPIO_WritePin(&handlerStepperA_Y1, RESET);
		GPIO_WritePin(&handlerStepperB_Y1, SET);
		GPIO_WritePin(&handlerStepperC_Y1, RESET);
		GPIO_WritePin(&handlerStepperD_Y1, RESET);
	}else if(motor == 3){
		GPIO_WritePin(&handlerStepperA_Y1, RESET);
		GPIO_WritePin(&handlerStepperB_Y1, RESET);
		GPIO_WritePin(&handlerStepperC_Y1, SET);
		GPIO_WritePin(&handlerStepperD_Y1, RESET);
	}else if(motor == 4){
		GPIO_WritePin(&handlerStepperA_Y1, RESET);
		GPIO_WritePin(&handlerStepperB_Y1, RESET);
		GPIO_WritePin(&handlerStepperC_Y1, RESET);
		GPIO_WritePin(&handlerStepperD_Y1, SET);
	}
}

void StepperMotorY2(uint8_t motor){
	if(motor == 1){
		GPIO_WritePin(&handlerStepperA_Y2, SET);
		GPIO_WritePin(&handlerStepperB_Y2, RESET);
		GPIO_WritePin(&handlerStepperC_Y2, RESET);
		GPIO_WritePin(&handlerStepperD_Y2, RESET);
	}else if(motor == 2){
		GPIO_WritePin(&handlerStepperA_Y2, RESET);
		GPIO_WritePin(&handlerStepperB_Y2, SET);
		GPIO_WritePin(&handlerStepperC_Y2, RESET);
		GPIO_WritePin(&handlerStepperD_Y2, RESET);
	}else if(motor == 3){
		GPIO_WritePin(&handlerStepperA_Y2, RESET);
		GPIO_WritePin(&handlerStepperB_Y2, RESET);
		GPIO_WritePin(&handlerStepperC_Y2, SET);
		GPIO_WritePin(&handlerStepperD_Y2, RESET);
	}else if(motor == 4){
		GPIO_WritePin(&handlerStepperA_Y2, RESET);
		GPIO_WritePin(&handlerStepperB_Y2, RESET);
		GPIO_WritePin(&handlerStepperC_Y2, RESET);
		GPIO_WritePin(&handlerStepperD_Y2, SET);
	}
}

void SMlowX(void){
	GPIO_WritePin(&handlerStepperA_X, RESET);
	GPIO_WritePin(&handlerStepperB_X, RESET);
	GPIO_WritePin(&handlerStepperC_X, RESET);
	GPIO_WritePin(&handlerStepperD_X, RESET);
}

void SMlowY1(void){
	GPIO_WritePin(&handlerStepperA_Y1, RESET);
	GPIO_WritePin(&handlerStepperB_Y1, RESET);
	GPIO_WritePin(&handlerStepperC_Y1, RESET);
	GPIO_WritePin(&handlerStepperD_Y1, RESET);
}

void SMlowY2(void){
	GPIO_WritePin(&handlerStepperA_Y2, RESET);
	GPIO_WritePin(&handlerStepperB_Y2, RESET);
	GPIO_WritePin(&handlerStepperC_Y2, RESET);
	GPIO_WritePin(&handlerStepperD_Y2, RESET);
}

double RPMtoDelay (uint16_t rpm){
	double delay = 60000 / (rpm*48);

	return delay;
}

void stepsY_CW(uint16_t steps){
	uint8_t j = 1;
	for(uint16_t i = 1; i <= steps; i++){
		SMlowY1();
		SMlowY2();
		StepperMotorY1(j);
		StepperMotorY2(j);
		delay((uint32_t) delayTime);
		if(j == 4){
			j = 0;
		}
		j++;
	}

	SMlowY1();
	SMlowY2();
}

void stepsY_CCW(uint16_t steps){
	uint8_t h = 5;
	for(uint16_t k = steps+1; k >= 1; k--){
		SMlowY1();
		SMlowY2();
		StepperMotorY1(h);
		StepperMotorY2(h);
		delay((uint32_t) delayTime);
		if(h == 1){
			h = 5;
		}
		h--;
	}

	SMlowY1();
	SMlowY2();
}

void stepsX_CW(uint16_t steps){
	uint8_t l = 1;
	for(uint16_t m = 1; m <= steps; m++){
		SMlowX();
		StepperMotorX(l);
		delay((uint32_t) delayTime);
		if(l == 4){
			l = 0;
		}
		l++;
	}
	SMlowX();
}

void stepsX_CCW(uint16_t steps){
	uint8_t n = 5;
	for(uint16_t o = steps+1; o >= 1; o--){
		SMlowX();
		StepperMotorX(n);
		delay((uint32_t) delayTime);
		if(n == 1){
			n = 5;
		}
		n--;
	}
	SMlowX();
}

void BasicTimer2_Callback(void){
	flagStatus = 1;
}

void usart2Rx_Callback(void){
	rxData = getRxData();
}

void parseCommands (char *ptrBufferReception){
	// Esta función lee la cadena de caracteres a lo que apunta el 'ptr' y la divide
	// y almacena en tres elementos diferentes: Un string llamado 'cmd', y dos números
	// integer llamados 'firstParameter' y 'secondparameter', de esta forma, es posible
	// introducir información al micro desde el puerto serial.
	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter, &secondParameter, userMsg);
	// Este primer comando imprime una lista con los otros comandos que tiene el equipo.
	if(strcmp(cmd, "help") == 0 || strcmp(cmd, "1") == 0){
		writeMsg(&handlerUSART2, "\n");
		writeMsg(&handlerUSART2, "Help menus CMDs:\n");
		writeMsg(&handlerUSART2, "1) help				----	print this menu\n");
		writeMsg(&handlerUSART2, "2) dummy				----	prints a test message\n");
		writeMsg(&handlerUSART2, "3) moveY #steps		----	moves the # steps - Y axis (CW -> + or CCW -> -)\n");
		writeMsg(&handlerUSART2, "4) moveX #steps		----	moves the # steps - X axis (CW -> + or CCW -> -)\n");
		writeMsg(&handlerUSART2, "5) rpm #n				----	update rpm (max 200 rpm)\n");
	}else if(strcmp(cmd, "dummy") == 0 || strcmp(cmd, "2") == 0){
		writeMsg(&handlerUSART2, "\n\rCMD: dummy working\n\r");
	}else if(strcmp(cmd, "moveY") == 0 || strcmp(cmd, "3") == 0){
		if(firstParameter>0){
			stepsY_CW((int) firstParameter);
			sprintf(bufferData, "\n\rSteps Y axis CW: %u\n\r", firstParameter);
			writeMsg(&handlerUSART2, bufferData);
		}else{
			firstParameter = firstParameter * -1;
			stepsY_CCW((int) firstParameter);
			sprintf(bufferData, "\n\rSteps Y axis CCW: -%u\n\r", firstParameter);
			writeMsg(&handlerUSART2, bufferData);
		}
	}else if(strcmp(cmd, "moveX") == 0 || strcmp(cmd, "4") == 0){
		if(firstParameter>0){
			stepsX_CW((int) firstParameter);
			sprintf(bufferData, "\n\rSteps X axis CW: %u\n\r", firstParameter);
			writeMsg(&handlerUSART2, bufferData);
		}else{
			firstParameter = firstParameter * -1;
			stepsX_CCW((int) firstParameter);
			sprintf(bufferData, "\n\rSteps X axis CCW: -%u\n\r", firstParameter);
			writeMsg(&handlerUSART2, bufferData);
		}
	}else if(strcmp(cmd, "rpm") == 0 || strcmp(cmd, "5") == 0){
		if(firstParameter > 0){
			if(firstParameter >= 200){
				delayTime = RPMtoDelay(200);
				writeMsg(&handlerUSART2, "\n\rRPM updated: 200\n\r");
			}else{
				delayTime = RPMtoDelay(firstParameter);
				sprintf(bufferData, "\n\rRPM updated: %u\n\r", firstParameter);
				writeMsg(&handlerUSART2, bufferData);
			}
		}else{
			sprintf(bufferData, "\n\rIngrese un valor mayor a 0. Valor incorrecto -> %u\n\r", firstParameter);
			writeMsg(&handlerUSART2, bufferData);
		}
	}
}
