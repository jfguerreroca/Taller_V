/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include <stdint.h>

unsigned char resultado = 0, sumando_1 = 10, sumando_2 = 15, negacion = 0, l;
unsigned short aviones = 0;
unsigned int gatos = 0;

/**
 * Función principal
 */
int main(void){

	// Sumando variables

	resultado = sumando_1 + sumando_2;

	// Cambiando el valor de la variable sumando_1, aviones y gatos
	aviones = 19203;
	gatos = 0x23AD89;
	sumando_1 = 45;

	resultado = 0b00001000;

	resultado = resultado >> 1;

	// Probando los shifts
	sumando_1 = sumando_1 << 3;
	sumando_1 = sumando_1 >> 3;

	// Negación de un número
	negacion = ~resultado;

    /* Loop forever */
	while(l){

	}
}
