/*
 * main.c
 *
 *  Created on: Aug 20, 2022
 *      Author: Felipe
 */

#include <stdint.h>

unsigned char numero_anhos = 0;
unsigned short dias_anho = 0;
unsigned short segundos_hora = 0;
unsigned short dias_transcurridos = 0;
unsigned int horas_transcurridas = 0;
unsigned int segundos_transcurridos = 0;
unsigned int dias_shift = 0;
unsigned int horas_shift = 0;
unsigned int dias_invertido = 0;
unsigned char prueba = 0;

int main(void)
{
	//Punto 1
	numero_anhos = 22;
	dias_anho = 365;
	segundos_hora = 3600;

	//Punto 2
	dias_transcurridos = (numero_anhos * dias_anho) + 139;

	//Punto 3
	horas_transcurridas = dias_transcurridos * 24;

	//Punto 4
	segundos_transcurridos = horas_transcurridas * segundos_hora;

	//Punto 5
	// dias = 1111111101001   horas = 101111110111011000  segundos = 0x2a11ad80

	//Punto 6
	dias_shift = dias_transcurridos << 1;
	dias_shift = dias_transcurridos << 2;
	dias_shift = dias_transcurridos << 4;

	//Punto 7
	horas_shift = horas_transcurridas >> 1;
	horas_shift = horas_transcurridas >> 2;
	horas_shift = horas_transcurridas >> 4;

	//Punto 8
	dias_invertido = ~dias_transcurridos;
	dias_invertido = dias_invertido + 1;
	dias_invertido = dias_invertido + dias_transcurridos;
	prueba = 157;
	prueba = ~prueba;

    /* Loop forever */
	for(;;);
}


