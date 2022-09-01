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

/**
 * Función principal del programa
 * Esta función es el corazón del programa
 * */

uint8_t dummy = 0;
uint8_t *ptrEjemplo = 0;
uint8_t valorPuntero = 0;

int main(void)
{

	dummy = 30;
	ptrEjemplo = &dummy
	*ptrEjemplo += 45;
	valorPuntero = (int)ptrEjemplo;
    /* Loop forever */
	while(1){

	}

	return 0;
}
