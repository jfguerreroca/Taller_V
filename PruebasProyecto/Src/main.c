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
double delayTimeX										= 0;
double delayTimeY										= 0;
uint16_t rpmX											= 0;
uint16_t rpmY											= 0;
uint16_t steps											= 0;
uint16_t cond1											= 0;
uint16_t cond2											= 0;
uint16_t cond3											= 0;
uint16_t cond4											= 0;
int counterX											= 0;
int	counterY											= 0;
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

double RPMtoDelayX (uint16_t);

double RPMtoDelayY (uint16_t);

void stepsX_CW (uint16_t);

void stepsX_CCW (uint16_t);

void stepsY_CW (uint16_t);

void stepsY_CCW (uint16_t);

void stepsXYDiag_CW (uint16_t);

void stepsXYDiag_CCW (uint16_t);

void stepsXYDiagInv_CW(uint16_t);

void stepsXYDiagInv_CCW(uint16_t);

// Funcion principal del programa

int main(void)
{
	SCB -> CPACR |= 0xF << 20;

	InitSystem();

	rpmX = 200;
	rpmY = 100;

	delayTimeX = RPMtoDelayX(rpmX);
	delayTimeY = RPMtoDelayY(rpmY);

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

double RPMtoDelayX (uint16_t rpm){
	double delay = 60000 / (rpm*48);

	return delay;
}

double RPMtoDelayY (uint16_t rpm){
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
		delay((uint32_t) delayTimeY);
		if(j == 4){
			j = 0;
		}
		j++;
		counterY += 1;
		if(counterY < 0 || counter > 540){
			break;
		}
	}

	SMlowY1();
	SMlowY2();
}

void stepsY_CCW(uint16_t steps){
	uint8_t h = 5;
	for(uint16_t k = steps; k >= 1; k--){
		SMlowY1();
		SMlowY2();
		StepperMotorY1(h);
		StepperMotorY2(h);
		delay((uint32_t) delayTimeY);
		if(h == 1){
			h = 5;
		}
		h--;
		counterY -= 1;
		if(counterY < 0 || counter > 540){
			break;
		}
	}

	SMlowY1();
	SMlowY2();
}

void stepsX_CW(uint16_t steps){
	uint8_t l = 1;
	for(uint16_t m = 1; m <= steps; m++){
		SMlowX();
		StepperMotorX(l);
		delay((uint32_t) delayTimeX);
		if(l == 4){
			l = 0;
		}
		l++;
		counterX += 1;
		if(counterX < 0 || counter > 576){
			break;
		}
	}

	SMlowX();
}

void stepsX_CCW(uint16_t steps){
	uint8_t n = 5;
	for(uint16_t o = steps; o >= 1; o--){
		SMlowX();
		StepperMotorX(n);
		delay((uint32_t) delayTimeX);
		if(n == 1){
			n = 5;
		}
		n--;
		counterX -= 1;
		if(counterX < 0 || counter > 576){
			break;
		}
	}

	SMlowX();
}

void stepsXYDiag_CW(uint16_t steps){
	uint8_t j = 1;
	for(uint16_t i = 1; i <= steps; i++){
		SMlowX();
		SMlowY1();
		SMlowY2();
		StepperMotorX(j);
		StepperMotorY1(j);
		StepperMotorY2(j);
		delay((uint32_t) delayTimeY);
		if(j == 4){
			j = 0;
		}
		j++;
		counterX += 1;
		counterY += 1;
		if(counterX < 0 || counter > 576){
			break;
		}
		if(counterY < 0 || counter > 540){
			break;
		}
	}
	SMlowX();
	SMlowY1();
	SMlowY2();
}

void stepsXYDiag_CCW(uint16_t steps){
	uint8_t n = 5;
	for(uint16_t o = steps; o >= 1; o--){
		SMlowX();
		SMlowY1();
		SMlowY2();
		StepperMotorX(n);
		StepperMotorY1(n);
		StepperMotorY2(n);
		delay((uint32_t) delayTimeY);
		if(n == 1){
			n = 5;
		}
		n--;
		counterX -= 1;
		counterY -= 1;
		if(counterX < 0 || counter > 576){
			break;
		}
		if(counterY < 0 || counter > 540){
			break;
		}
	}
	SMlowX();
	SMlowY1();
	SMlowY2();
}

void stepsXYDiagInv_CW(uint16_t steps){
	uint8_t n = 5;
	uint8_t j = 1;
	for(uint16_t o = steps; o >= 1; o--){
		SMlowX();
		StepperMotorX(n);
		if(n == 1){
			n = 5;
		}
		n--;
		counterX -= 1;
		if(counterX < 0 || counter > 576){
			break;
		}
		for(uint16_t i = 1; i <= steps; i++){
			SMlowY1();
			SMlowY2();
			StepperMotorY1(j);
			StepperMotorY2(j);
			delay((uint32_t) delayTimeY);
			if(j == 4){
				j = 0;
			}
			j++;
			counterY += 1;
			if(counterY < 0 || counter > 540){
				break;
			}
			break;
		}
	}

	SMlowX();
	SMlowY1();
	SMlowY2();
}

void stepsXYDiagInv_CCW(uint16_t steps){
	uint8_t l = 1;
	uint8_t h = 5;
	for(uint16_t m = 1; m <= steps; m++){
		SMlowX();
		StepperMotorX(l);
		if(l == 4){
			l = 0;
		}
		l++;
		counterX += 1;
		if(counterX < 0 || counter > 576){
			break;
		}
		for(uint16_t k = steps; k >= 1; k--){
			SMlowY1();
			SMlowY2();
			delay((uint32_t) delayTimeY);
			StepperMotorY1(h);
			StepperMotorY2(h);
			delay((uint32_t) delayTimeY);
			if(h == 1){
				h = 5;
			}
			h--;
			counterY -= 1;
			if(counterY < 0 || counter > 540){
				break;
			}
			break;
		}
	}

	SMlowY1();
	SMlowY2();
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
		writeMsg(&handlerUSART2, "1) help					----	print this menu\n");
		writeMsg(&handlerUSART2, "2) dummy					----	prints a test message\n");
		writeMsg(&handlerUSART2, "3) moveY #steps			----	moves the # steps - Y axis (CW -> + or CCW -> -)\n");
		writeMsg(&handlerUSART2, "4) moveX #steps			----	moves the # steps - X axis (CW -> + or CCW -> -)\n");
		writeMsg(&handlerUSART2, "5) moveDiag #steps		----	moves the # steps - Diagonal (CW -> + or CCW -> -)\n");
		writeMsg(&handlerUSART2, "6) moveDiagInv #steps		----	moves the # steps - Diagonal (CW -> + or CCW -> -)\n");
		writeMsg(&handlerUSART2, "7) rpmX #n				----	update rpm (max 200 rpm)\n");
		writeMsg(&handlerUSART2, "8) rpmY #n				----	update rpm (max 100 rpm)\n");
		writeMsg(&handlerUSART2, "9) reset					----	set default values\n");
	}else if(strcmp(cmd, "dummy") == 0 || strcmp(cmd, "2") == 0){
		writeMsg(&handlerUSART2, "\n\rCMD: dummy working\n\r");
	}else if(strcmp(cmd, "moveY") == 0 || strcmp(cmd, "3") == 0){
		cond1 = counterY+firstParameter;
		cond2 = counterY-firstParameter;
		if(cond1 > 540 || cond2 < 0){
			writeMsg(&handlerUSART2, "\n\rWrong coordinates, off limits\n\r");
		}else{
			if(firstParameter>0){
				stepsY_CW((int) firstParameter);
				sprintf(bufferData, "\n\rSteps Y axis CW: %u\n\r", firstParameter);
				writeMsg(&handlerUSART2, bufferData);
				sprintf(bufferData, "\n\rCounter Y axis: %u\n\r", counterY);
				writeMsg(&handlerUSART2, bufferData);
			}else{
				firstParameter = firstParameter * -1;
				stepsY_CCW((int) firstParameter);
				sprintf(bufferData, "\n\rSteps Y axis CCW: -%u\n\r", firstParameter);
				writeMsg(&handlerUSART2, bufferData);
				sprintf(bufferData, "\n\rCounter Y axis: %u\n\r", counterY);
				writeMsg(&handlerUSART2, bufferData);
			}
		}
	}else if(strcmp(cmd, "moveX") == 0 || strcmp(cmd, "4") == 0){
		cond1 = counterX+firstParameter;
		cond2 = counterX-firstParameter;
		if(cond1 > 576 || cond2 < 0){
			writeMsg(&handlerUSART2, "\n\rWrong coordinates, off limits\n\r");
		}else{
			if(firstParameter>0){
				stepsX_CW((int) firstParameter);
				sprintf(bufferData, "\n\rSteps X axis CW: %u\n\r", firstParameter);
				writeMsg(&handlerUSART2, bufferData);
				sprintf(bufferData, "\n\rCounter X axis: %u\n\r", counterX);
				writeMsg(&handlerUSART2, bufferData);
			}else{
				firstParameter = firstParameter * -1;
				stepsX_CCW((int) firstParameter);
				sprintf(bufferData, "\n\rSteps X axis CCW: -%u\n\r", firstParameter);
				writeMsg(&handlerUSART2, bufferData);
				sprintf(bufferData, "\n\rCounter X axis: %u\n\r", counterX);
				writeMsg(&handlerUSART2, bufferData);
			}
		}
	}else if(strcmp(cmd, "moveDiag") == 0 || strcmp(cmd, "5") == 0){
		cond1 = counterX+firstParameter;
		cond2 = counterX-firstParameter;
		cond3 = counterY+firstParameter;
		cond4 = counterY-firstParameter;
		if(cond1 > 576 || cond2 < 0 || cond3 > 540 || cond4 < 0){
			writeMsg(&handlerUSART2, "\n\rWrong coordinates, off limits\n\r");
		}else{
			if(firstParameter>0){
				stepsXYDiag_CW((int) firstParameter);
				sprintf(bufferData, "\n\rDiagonal Steps CW: %u\n\r", firstParameter);
				writeMsg(&handlerUSART2, bufferData);
				sprintf(bufferData, "\n\rCounter X axis: %u\n\r", counterX);
				writeMsg(&handlerUSART2, bufferData);
				sprintf(bufferData, "\n\rCounter Y axis: %u\n\r", counterY);
				writeMsg(&handlerUSART2, bufferData);
			}else{
				firstParameter = firstParameter * -1;
				stepsXYDiag_CCW((int) firstParameter);
				sprintf(bufferData, "\n\rDiagonal Steps CCW: %u\n\r", firstParameter);
				writeMsg(&handlerUSART2, bufferData);
				sprintf(bufferData, "\n\rCounter X axis: %u\n\r", counterX);
				writeMsg(&handlerUSART2, bufferData);
				sprintf(bufferData, "\n\rCounter Y axis: %u\n\r", counterY);
				writeMsg(&handlerUSART2, bufferData);
			}
		}
	}else if(strcmp(cmd, "moveDiagInv") == 0 || strcmp(cmd, "6") == 0){
		cond1 = counterX+firstParameter;
		cond2 = counterX-firstParameter;
		cond3 = counterY+firstParameter;
		cond4 = counterY-firstParameter;
		if(cond1 > 576 || cond2 < 0 || cond3 > 540 || cond4 < 0){
			writeMsg(&handlerUSART2, "\n\rWrong coordinates, off limits\n\r");
		}else{
			if(firstParameter>0){
				stepsXYDiagInv_CW((int) firstParameter);
				sprintf(bufferData, "\n\rInverted Diagonal Steps CW: %u\n\r", firstParameter);
				writeMsg(&handlerUSART2, bufferData);
				sprintf(bufferData, "\n\rCounter X axis: %u\n\r", counterX);
				writeMsg(&handlerUSART2, bufferData);
				sprintf(bufferData, "\n\rCounter Y axis: %u\n\r", counterY);
				writeMsg(&handlerUSART2, bufferData);
			}else{
				firstParameter = firstParameter * -1;
				stepsXYDiagInv_CCW((int) firstParameter);
				sprintf(bufferData, "\n\rInverted Diagonal Steps CCW: %u\n\r", firstParameter);
				writeMsg(&handlerUSART2, bufferData);
				sprintf(bufferData, "\n\rCounter X axis: %u\n\r", counterX);
				writeMsg(&handlerUSART2, bufferData);
				sprintf(bufferData, "\n\rCounter Y axis: %u\n\r", counterY);
				writeMsg(&handlerUSART2, bufferData);
			}
		}
	}else if(strcmp(cmd, "rpmX") == 0 || strcmp(cmd, "7") == 0){
		if(firstParameter > 0){
			if(firstParameter >= 200){
				delayTimeX = RPMtoDelayX(200);
				writeMsg(&handlerUSART2, "\n\rRPM X updated: 200\n\r");
			}else{
				delayTimeX = RPMtoDelayX(firstParameter);
				sprintf(bufferData, "\n\rRPM X updated: %u\n\r", firstParameter);
				writeMsg(&handlerUSART2, bufferData);
			}
		}else{
			sprintf(bufferData, "\n\rIngrese un valor mayor a 0. Valor incorrecto -> %u\n\r", firstParameter);
			writeMsg(&handlerUSART2, bufferData);
		}
	}else if(strcmp(cmd, "rpmY") == 0 || strcmp(cmd, "8") == 0){
		if(firstParameter > 0){
			if(firstParameter >= 100){
				delayTimeY = RPMtoDelayY(100);
				writeMsg(&handlerUSART2, "\n\rRPM Y updated: 100\n\r");
			}else{
				delayTimeY = RPMtoDelayY(firstParameter);
				sprintf(bufferData, "\n\rRPM Y updated: %u\n\r", firstParameter);
				writeMsg(&handlerUSART2, bufferData);
			}
		}else{
			sprintf(bufferData, "\n\rIngrese un valor mayor a 0. Valor incorrecto -> %u\n\r", firstParameter);
			writeMsg(&handlerUSART2, bufferData);
		}
	}else if(strcmp(cmd, "reset") == 0 || strcmp(cmd, "9") == 0){
		counterX = 0;
		counterY = 0;
		rpmX = 200;
		rpmY = 100;
		writeMsg(&handlerUSART2, "\n\rSystem default values OK\n\r");
		sprintf(bufferData, "\n\rRPM X updated: %u\n\r", rpmX);
		writeMsg(&handlerUSART2, bufferData);
		sprintf(bufferData, "\n\rRPM Y updated: %u\n\r", rpmY);
		writeMsg(&handlerUSART2, bufferData);
		sprintf(bufferData, "\n\rCoordinates X: %u Y: %u\n\r", counterX, counterY);
		writeMsg(&handlerUSART2, bufferData);
	}
}
