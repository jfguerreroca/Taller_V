/*
 * CaptureFrecDriver.c
 *
 *  Created on: Apr 18, 2022
 *      Author: namontoy
 */

#include "CaptureFrecDriver.h"

void capture_Config(Capture_Handler_t *ptrCaptureHandler){

	__disable_irq();

	/* 1. Activar la señal de reloj del periférico requerido */
	if(ptrCaptureHandler->ptrTIMx == TIM2){
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM3){
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM4){
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM5){
		RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	}
	else{
		__NOP();
	}

	/* 2. Estamos en UP Mode, el límite se carga en ARR y se comienza en 0 */
	ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_DIR;

	/* 3. Configuramos los bits CCxS del registro TIMy_CCMR1, de forma que sea modo salida
	 * (para cada canal hay un conjunto CCxS)
	 *
	 * 4. Además, en el mismo "case" podemos configurar el modo PWM, su polaridad.
	 *
	 * 5. Y además activamos el preload bit, para que cada vez que exista un update-event
	 * el valor cargado en el CCRx será recargado en el registro "shadow" del PWM */
	switch(ptrCaptureHandler->config.channel){
	case CAPTURE_CHANNEL_1:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(TIM_CCMR1_CC1S);
		// Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR1 = (1 << TIM_CCMR1_CC1S_Pos);

		// Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0xF << TIM_CCMR1_IC1F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (0x3 << TIM_CCMR1_IC1F_Pos);

		// Configuramos el preescaler
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0x3 <<TIM_CCMR1_IC1PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR1_IC1PSC_Pos);

		// Configuramos el flanco que deseamos capturar
		if(ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE){
			// Configuración 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1NP);
		}
		else{
			// Configuración 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC1P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1NP);
		}

		// Activamos el modulo captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC1E;

		// Activamos las interrupciones para el modo Capture/Compare del TIMx
		ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC1IE;

		break;
	}

	case CAPTURE_CHANNEL_2:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(TIM_CCMR1_CC2S);
		// Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR1 = (1 << TIM_CCMR1_CC2S_Pos);

		// Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0xF << TIM_CCMR1_IC2F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (0x3 << TIM_CCMR1_IC2F_Pos);

		// Configuramos el preescaler
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0x3 <<TIM_CCMR1_IC2PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR1_IC2PSC_Pos);

		// Configuramos el flanco que deseamos capturar
		if(ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE){
			// Configuración 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2NP);
		}else{
			// Configuración 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC2P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2NP);
		}

		// Activamos el modulo captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC2E;

		// Activamos las interrupciones para el modo Capture/Compare del TIMx
		ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC2IE;

		break;
	}

	case CAPTURE_CHANNEL_3:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(TIM_CCMR2_CC3S);
		// Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR2 = (1 << TIM_CCMR2_CC3S_Pos);

		// Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0xF << TIM_CCMR2_IC3F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (0x3 << TIM_CCMR2_IC3F_Pos);

		// Configuramos el preescaler
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0x3 <<TIM_CCMR2_IC3PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR2_IC3PSC_Pos);

		// Configuramos el flanco que deseamos capturar
		if(ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE){
			// Configuración 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3NP);
		}else{
			// Configuración 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC3P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3NP);
		}

		// Activamos el modulo captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC3E;

		// Activamos las interrupciones para el modo Capture/Compare del TIMx
		ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC3IE;

		break;
	}

	case CAPTURE_CHANNEL_4:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(TIM_CCMR2_CC4S);
		// Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR2 = (1 << TIM_CCMR2_CC4S_Pos);

		// Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0xF << TIM_CCMR2_IC4F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (0x3 << TIM_CCMR2_IC4F_Pos);

		// Configuramos el preescaler
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0x3 <<TIM_CCMR2_IC4PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR2_IC4PSC_Pos);

		// Configuramos el flanco que deseamos capturar
		if(ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE){
			// Configuración 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4NP);
		}else{
			// Configuración 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC4P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4NP);
		}

		// Activamos el modulo captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC4E;

		// Activamos las interrupciones para el modo Capture/Compare del TIMx
		ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC4IE;

		break;
	}


	default: {
		__NOP();
		break;
	}

	}// fin del switch-case

	// Configuramos el preescaler del timer, el cual define a que velocidad se incrementa nuestro timer
	ptrCaptureHandler->ptrTIMx->PSC = ptrCaptureHandler->config.timerSpeed;

	/* Activamos el canal del sistema NVIC para que lea la interrupción*/
	if(ptrCaptureHandler->ptrTIMx == TIM2){
		// Activando en NVIC para la interrupción del TIM2
		NVIC_EnableIRQ(TIM2_IRQn);
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM3){
		// Activando en NVIC para la interrupción del TIM3
		NVIC_EnableIRQ(TIM3_IRQn);
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM4){
		// Activando en NVIC para la interrupción del TIM4
		NVIC_EnableIRQ(TIM4_IRQn);
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM5){
		// Activando en NVIC para la interrupción del TIM5
		NVIC_EnableIRQ(TIM5_IRQn);
	}
	else{
		__NOP();
	}

	// Activamos el timer
	ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

	__enable_irq();
}

// Definimos los callback

__attribute__((weak)) void CaptureFrec1_Callback(void){

	__NOP();
}
__attribute__((weak)) void CaptureFrec2_Callback(void){

	__NOP();
}
__attribute__((weak)) void CaptureFrec3_Callback(void){

	__NOP();
}
__attribute__((weak)) void CaptureFrec4_Callback(void){

	__NOP();
}


/*
 * Esta función se encarga de lanzar la captura de la frecuencia (un solo valor)
 */
uint32_t getTimeStamp(Capture_Handler_t *ptrCaptureHandler){

	uint32_t timestamp = 0;

	/*
	 * Para el canal seleccionado, capturamos el valor del counter y bajamos
	 * la bandera del overcapture, de manera que pueda continuar sin problemas la
	 * captura de frecuencia.
	 */
	switch(ptrCaptureHandler->config.channel){
		case CAPTURE_CHANNEL_1:{

			// Capturamos el valor del tiempo almacenado en el CCRx
			timestamp = ptrCaptureHandler->ptrTIMx->CCR1;

			// Bajamos la bandera del overcapture
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC1OF;

			break;
		}
		case CAPTURE_CHANNEL_2:{

			// Capturamos el valor del tiempo almacenado en el CCRx
			timestamp = ptrCaptureHandler->ptrTIMx->CCR2;

			// Bajamos la bandera del overcapture
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC2OF;

			break;
		}
		case CAPTURE_CHANNEL_3:{

			// Capturamos el valor del tiempo almacenado en el CCRx
			timestamp = ptrCaptureHandler->ptrTIMx->CCR3;

			// Bajamos la bandera del overcapture
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC3OF;
			break;
		}
		case CAPTURE_CHANNEL_4:{

			// Capturamos el valor del tiempo almacenado en el CCRx
			timestamp = ptrCaptureHandler->ptrTIMx->CCR4;

			// Bajamos la bandera del overcapture
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC4OF;
			break;
		}
		default: {
			break;
		}
	}

	return timestamp;

}

/*
 * Esta funcion se encarga de recibir los dos valores tomados para encontrar el periodo, y
 * hacer las respectivas operaciones algebraicas para devolver el valor del periodo en milisegundos.
 */

uint32_t getPeriodFrec(Capture_Handler_t *ptrCaptureHandler, uint32_t ts1, uint32_t ts2){


	int rawPeriod = ts2 - ts1;
	rawPeriod = abs(rawPeriod);

	switch(ptrCaptureHandler->config.prescalerCapture){
		case CAPTURE_PREESCALER_1_1:{

			rawPeriod = rawPeriod / (1);
			break;
		}
		case CAPTURE_PREESCALER_2_1:{

			rawPeriod = rawPeriod / (2);
			break;
		}
		case CAPTURE_PREESCALER_4_1:{

			rawPeriod = rawPeriod / (4);
			break;
		}
		case CAPTURE_PREESCALER_8_1:{

			rawPeriod = rawPeriod / (8);
			break;
		}
		default: {
			break;
		}

	}

	switch(ptrCaptureHandler->config.timerSpeed){
		case CAPTURE_TIMER_SPEED_1us:{

			rawPeriod = rawPeriod * 1000;
			break;
		}
		case CAPTURE_TIMER_SPEED_10us:{

			rawPeriod = rawPeriod * 100;
			break;
		}
		case CAPTURE_TIMER_SPEED_100us:{

			rawPeriod = rawPeriod * 10;
			break;
		}
		case CAPTURE_TIMER_SPEED_1ms:{

			rawPeriod = rawPeriod;
			break;
		}
		default: {
			break;
		}

	}

	return rawPeriod;


}

/*
 * Esta funcion limpia los registros del counter para el timer y el canal seleccionado
 */
void cleanData(Capture_Handler_t *ptrCaptureHandler){

	ptrCaptureHandler->ptrTIMx->CNT = 0;

	switch(ptrCaptureHandler->config.channel){
		case CAPTURE_CHANNEL_1:{
			ptrCaptureHandler->ptrTIMx->CCR1 = 0;
			break;
		}
		case CAPTURE_CHANNEL_2:{
			ptrCaptureHandler->ptrTIMx->CCR2 = 0;
			break;
		}
		case CAPTURE_CHANNEL_3:{
			ptrCaptureHandler->ptrTIMx->CCR3 = 0;
			break;
		}
		case CAPTURE_CHANNEL_4:{
			ptrCaptureHandler->ptrTIMx->CCR4 = 0;
			break;
		}
		default: {
			break;
		}
	}
}
