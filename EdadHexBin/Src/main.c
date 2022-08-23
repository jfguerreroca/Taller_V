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
unsigned char prueba_not = 0;
unsigned int mascara = 0;
unsigned int segundos_transcurridos_m = 0;
unsigned int mascara_2 = 0;
unsigned int segundos_transcurridos_m2 = 0;
unsigned int mascara_3 = 0;
unsigned int segundos_transcurridos_m3 = 0;
unsigned int operador_signo_admiracion = 5;

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
	prueba_not = -127;
	prueba_not = ~prueba_not;

	//Punto 9
	mascara = 0x0a000d00;
	segundos_transcurridos_m = segundos_transcurridos & mascara;

	//Punto 10
	mascara_2 = 0x040;
	segundos_transcurridos_m2 = segundos_transcurridos & mascara_2;
	mascara_3 = 0x2a110d80;
	segundos_transcurridos_m3 = segundos_transcurridos & mascara_3;

	//Punto 11
	operador_signo_admiracion = !segundos_transcurridos_m3;


    /* Loop forever */
	for(;;);
}


