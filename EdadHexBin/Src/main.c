/*
 * main.c
 *
 *  Created on: Aug 20, 2022
 *      Author: Felipe Guerrero
 *	Mail: jfguerreroca@unal.edu.co
 */

#include <stdint.h>

//Variables Punto 1
unsigned char numero_anhos = 0;
unsigned short dias_anho = 0;
unsigned short segundos_hora = 0;
//Variables Punto 2
unsigned short dias_transcurridos = 0;
//Variables Punto 3
unsigned int horas_transcurridas = 0;
//Variables Punto 4
unsigned int segundos_transcurridos = 0;
//Variables Punto 6
unsigned int dias_shift1 = 0;
unsigned int dias_shift2 = 0;
unsigned int dias_shift3 = 0;
//Variables Punto 7
unsigned int horas_shift1 = 0;
unsigned int horas_shift2 = 0;
unsigned int horas_shift3 = 0;
//Variables Punto 8
unsigned int dias_invertido = 0;
unsigned char prueba_not = 0;
//Variables Punto 9
unsigned int mascara = 0;
unsigned int segundos_transcurridos_m = 0;
//Variables Punto 10
unsigned int mascara_2 = 0;
unsigned int segundos_transcurridos_m2 = 0;
unsigned int mascara_3 = 0;
unsigned int segundos_transcurridos_m3 = 0;
//Variables Punto 11
unsigned int operador_signo_admiracion = 0;

int main(void)
{
	/**
	 * Punto 1: Inicializadas las variables para el numero de años (con char para un
	 * arreglo de 1 bytes), dias del año y segundos de una hora (con short para un
	 * arreglo de 2 bytes), se procede a asignarles su valor. Cabe destacar que los
	 * tipos de variables se escogieron con base al tamaño de cada valor que sería
	 * asignado, de manera que no se use mas espacio del necesario.
	 */
	numero_anhos = 22;
	dias_anho = 365;
	segundos_hora = 3600;

	/**
	 * Punto 2: Se realiza el cálculo de los dias transcurridos desde mi fecha de
	 * nacimiento hasta el 16 de Agosto. Para esto se hace una multiplicación del
	 * numero de años (22) y los dias por año (365), lo que da los dias transcurridos
	 * desde el 31 de Marzo del 2000 hasta el 31 de Marzo del 2022. Luego se hace el
	 * calculo manual de los dias que hay entre el 31 de Marzo y el 16 de Agosto, y se
	 * suma este valor como constante a la multiplicación anterior.
	 */
	dias_transcurridos = (numero_anhos * dias_anho) + 138;

	/**
	 * Punto 3: Con el cálculo anterior, se multiplica el numero de dias por 24 horas,
	 * y se le suman las 12 horas correspondientes para posicionarnos en el 17 de Agosto del 2022
	 * al medio día.
	 */
	horas_transcurridas = (dias_transcurridos * 24) + 12;

	/**
	 * Punto 4: Las horas transcurridas se multiplican por 3600 segundos/hora, obteniendo
	 * los segundos transcurridos desde el nacimiento hasta el 17 de Agosto al medio dia
	 */
	segundos_transcurridos = horas_transcurridas * segundos_hora;

	/**
	 * Punto 5: Las variables convertidas son:
	 * 											dias = 0b1111111101001
	 * 											horas = 0b101111110111011000
	 * 											segundos = 0x2a11ad80
	 *
	 * Las variables las encontré utilizando el apartado de live expressions, donde deja cambiar
	 * el formato de numero y copiar las variables necesarias. El tipo de variable sigue siendo
	 * el mismo en cada caso, ya que, aunque cambiemos el formato en el que observamos el numero, la maquina
	 * solo entiende el formato binario, por lo que es indiferente para esta el formato con el
	 * que visualizamos los valores.
	 */

	/**
	 * Punto 6: La operacion shift izquierda multiplica el valor de la variable por potencias de 2, y el
	 * exponente de este "2" lo define el numero de shifts realizados. Por ejemplo si hacemos shift a una
	 * variable igual a 5, hacer 3 shifts daria como resultado 5*(2**3) = 40.
	 * En nuestro caso, dias_shift1 = dias_trascurridos * 2**1, dias_shift2 = dias_trascurridos * 2**2 y
	 * dias_shift3 = dias_trascurridos * 2**4.
	 */
	dias_shift1 = dias_transcurridos << 1;
	dias_shift2 = dias_transcurridos << 2;
	dias_shift3 = dias_transcurridos << 4;

	/**
	 * Punto 7: De manera similar al punto anterior, el shift derecho tambien tiene asociado las potencias de 2,
	 * pero esta vez se hace una division, por lo que si hacemos shift a una variable igual a 5, hacer 3 shifts
	 * daria como resultado 8/(2**3) = 1. En nuestro caso, horas_shift1 = dias_trascurridos / 2**1,
	 * horas_shift2 = dias_trascurridos / 2**2 y horas_shift3 = dias_trascurridos / 2**4.
	 */
	horas_shift1 = horas_transcurridas >> 1;
	horas_shift2 = horas_transcurridas >> 2;
	horas_shift3 = horas_transcurridas >> 4;

	/**
	 * Punto 8: Cuando invertimos el valor de la variable dias_transcurridos, los valores que son 1 pasan a 0, y los que son 0 pasan a 1.
	 * Si le sumamos 1 a este valor y sumamos la variable original, da como resultado 0, ya que el resultado se desborda y termina en la
	 * posición 0.
	 * Esto sucede con cualquier otro valor. Por ejemplo si analizamos el tipo de variable char, sabemos que esta tiene un arreglo de
	 * 8 bits, por lo que tenemos 256 posibles valores decimales, [-127,126] o [0,255]. Para saber que numero obtendremos al invertir,
	 * basta con contar las posiciones desde ese valor hasta su limite mas cercano (por ejemplo prueba_not=7 está a 119 posiciones del 126),
	 * y luego contar estas mismas posiciones desde el otro limite (-127 + 119 = -8). Al sumar el numero 1, es posible obtener el mismo
	 * valor con signo contrario (-8 + 1 = -7), siempre y cuando este valor se encuentre dentro del rango con signo, en este caso [-127,126].
	 * Luego, al sumar la variable original, el arreglo de 8 bits llega hasta el valor maximo, y justamente por el 1 sumado anteriormente,
	 * se desborda, reiniciando el arreglo a 0. Esto aplica para todos los tipos de variable.
	 */
	dias_invertido = ~dias_transcurridos;
	dias_invertido = dias_invertido + 1;
	dias_invertido = dias_invertido + dias_transcurridos;
	prueba_not = 7;
	prueba_not = ~prueba_not;
	prueba_not = prueba_not + 1;
	prueba_not = prueba_not + 7;

	/**
	 * Punto 9: la variable de segundos en HEX es 0x2a1104c0, y vemos que la cuarta posicion es impar (1).
	 * Aplicamos la mascara 0x0a000400, para preservar las posiciones 0, 2 y 6 con un operador AND.
	 */
	mascara = 0x0a000400;
	segundos_transcurridos_m = segundos_transcurridos & mascara;

	/**
	 * Punto 10: Aplicamos la mascara 0x040, lo que nos da como resultado 0x040. Al ser diferente de 0,
	 * aplicamos la mascara 0x201104c0, que solo anula la sexta posicion de la variable de los segundos.
	 */
	mascara_2 = 0x040;
	segundos_transcurridos_m2 = segundos_transcurridos & mascara_2;
	mascara_3 = 0x201104c0;
	segundos_transcurridos_m3 = segundos_transcurridos & mascara_3;

	/**
	 * Punto 11: Asignamos el valor 5 a la variable del operador, aplicamos el operador ! y da como resultado 0,
	 * ya que el valor es distinto de 0. Cuando aplicamos de nuevo el operador ! (NOT), da como resultado 1,
	 * ya que el valor es igual a 0.
	 */
	operador_signo_admiracion = 5;
	operador_signo_admiracion = !segundos_transcurridos_m3;
	operador_signo_admiracion = !operador_signo_admiracion;


    /* Loop forever */
	for(;;);
}


