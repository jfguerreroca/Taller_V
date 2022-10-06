/**
 ******************************************************************************
 * @file           : main.c
 * @author         : jfguerreroca
 * @brief          : Configuración básica de un proyecto
 ******************************************************************************
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stdbool.h>
#include "BasicTimer.h"
#include "GPIOxDriver.h"

/**
 * Función principal del programa
 * Esta función es el corazón del programa
 * */

BasicTimer_Handler_t 	handlerTimer2 		= {0};
GPIO_Handler_t 			handlerBlinkyLed 	= {0};

int main(void)
{

	handlerBlinkyLed.pGPIOx = GPIOA;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyLed);
	GPIO_WritePin(&handlerBlinkyLed, SET);

	handlerTimer2.ptrTIMx = TIM2;
	handlerTimer2.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed = BTIMER_SPEED_100us;
	handlerTimer2.TIMx_Config.TIMx_period = 2500;
	handlerTimer2.TIMx_Config.TIMx_interruptEnable = 1;

    /* Loop forever */
	while(1){

	}

	return 0;
}
