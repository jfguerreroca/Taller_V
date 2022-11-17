/*
 * RCC100xDriver.c
 *
 *  Created on: 12 nov 2022
 *      Author: Felipe
 */

#include "RCC100xDriver.h"

void set100MHzRCC (void){

	// Desactivamos las interrupciones globales

	__disable_irq();

	// Se cambian los Wait States para que el micro pueda trabajar a 100MHz

	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |= FLASH_ACR_LATENCY_3WS;

	// Se configuran las constantes para el PLL, de manera que la salida de este sea de 100MHz

	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
	RCC->PLLCFGR |=  RCC_PLLCFGR_PLLM_3;

	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
	RCC->PLLCFGR |=  RCC_PLLCFGR_PLLN_2;
	RCC->PLLCFGR |=  RCC_PLLCFGR_PLLN_5;
	RCC->PLLCFGR |=  RCC_PLLCFGR_PLLN_6;

	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;

	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLQ;
	RCC->PLLCFGR |=  RCC_PLLCFGR_PLLQ_2;

	// Escogemos que el PLL sea el system clock

	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |=  RCC_CFGR_SW_PLL;

	// Configuramos los prescaler de los buses

	RCC->CFGR &= ~RCC_CFGR_HPRE;
	RCC->CFGR |=  RCC_CFGR_HPRE_DIV1;

	RCC->CFGR &= ~RCC_CFGR_PPRE1;
	RCC->CFGR |=  RCC_CFGR_PPRE1_DIV2;

	RCC->CFGR &= ~RCC_CFGR_PPRE2;
	RCC->CFGR |=  RCC_CFGR_PPRE2_DIV1;

	// Activamos y configuramos el pin MCO1 para medir que si esté trabajando a la frecuencia que deberia

	RCC->CFGR |=  RCC_CFGR_MCO1;

	RCC->CFGR &= ~RCC_CFGR_MCO1PRE;

	// Activamos el PLL

	RCC->CR |= RCC_CR_PLLON;

	// Activamos las interrupciones globales

	__enable_irq();

}

void configTimers (void){

	// Configuramos el factor de multiplicacion para los timer del APB1 (x2)

	TIM2->CR1 &= ~TIM_CR1_CKD;
	TIM2->CR1 |=  TIM_CR1_CKD_0;

	TIM3->CR1 &= ~TIM_CR1_CKD;
	TIM3->CR1 |=  TIM_CR1_CKD_0;

	TIM4->CR1 &= ~TIM_CR1_CKD;
	TIM4->CR1 |=  TIM_CR1_CKD_0;

	TIM5->CR1 &= ~TIM_CR1_CKD;
	TIM5->CR1 |=  TIM_CR1_CKD_0;

}
