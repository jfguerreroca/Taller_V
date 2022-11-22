/*
 * BasicTimer.h
 *
 *  Created on: Apr 18, 2022
 *      Author: namontoy
 */

#ifndef INC_BASICTIMER_H_
#define INC_BASICTIMER_H_

#include "stm32f4xx.h"

#define BTIMER_MODE_UP		0
#define BTIMER_MODE_DOWN	1

#define BTIMER_SPEED_10us			 159
#define BTIMER_SPEED_100us			 1599
#define BTIMER_SPEED_1ms			 15999
#define BTIMER_SPEED_RCC100_10us     999
#define BTIMER_SPEED_RCC100_100us    9999
#define BTIMER_SPEED_RCC100_1ms      99999

#define BTIMER_INTERRUP_ENABLE 		1
#define BTIMER_INTERRUP_DISABLE 	0



/* Estructura que contiene la configuración mínima necesaria para el manejo del Timer.*/
typedef struct
{
	uint8_t		TIMx_mode; 		// Up or dowm
	uint32_t	TIMx_speed;		// A qué velocidad se incrementa el Timer
	uint32_t	TIMx_period;	// Valor en ms del periodo del Timer
	uint8_t		TIMx_interruptEnable;	// Activa o desactiva el modo interrupción del timer.
}BasicTimer_Config_t;

/* Handler para el Timer*/
typedef struct
{
	TIM_TypeDef			*ptrTIMx;
	BasicTimer_Config_t	TIMx_Config;
}BasicTimer_Handler_t;

void BasicTimer_Config(BasicTimer_Handler_t *ptrBTimerHandler);
void BasicTimer2_Callback(void); /* Esta función debe ser sobre-escrita en el main para que el sistema funcione*/
void BasicTimer3_Callback(void); /* Esta función debe ser sobre-escrita en el main para que el sistema funcione*/
void BasicTimer4_Callback(void); /* Esta función debe ser sobre-escrita en el main para que el sistema funcione*/
void BasicTimer5_Callback(void); /* Esta función debe ser sobre-escrita en el main para que el sistema funcione*/
void startTimer(BasicTimer_Handler_t *ptrBTimerHandler);
void stopTimer(BasicTimer_Handler_t *ptrBTimerHandler);
#endif /* INC_BASICTIMER_H_ */
