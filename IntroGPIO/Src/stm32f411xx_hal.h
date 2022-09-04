/*
 * stm32f4xx.h
 *
 *  Created on: Aug 29, 2022
 *      Author: felipe
 *
 *  Este archivo contiene la información básica del MCU:
 *  1. Valores del reloj principal
 *  2. Distribución básica de la memoria
 *  3. Posiciones de memoria de los periféricos disponibles en el micro
 *  4. REgistros de periféricos
 *  5. Definiciones de las constantes más básicas
 *
 *  Nota: la definición del NVIC será realizada al momento de describir el uso de las interrupciones
 */

#ifndef INC_STM32F4XX_H_
#define INC_STM32F4XX_H_

#include <stdint.h>
#include <stddef.h>

#define HSI_CLOCK_SPEED      	16000000      // Value for the main clock signal (HSI - High Speed Internal)
#define HSE_CLOCK_SPEED      	4000000       // Value for the main clock signal (HSE - High Speed External)

#define NOP()      asm("NOP")
#define __weak     __attribute_((weak))

/*
 * Base addresses of Flash and SRAM memories
 * Datasheet, Memory Map, Figure 14
 * 1KByte = 1024 bytes
 */

#define FLASH_BASE_ADDR      	0x08000000U     /* Esta es la memoria del programa, 512 KB. Defino
												dónde inician*/
#define SRAM_BASE_ADDR       	0x20000000U     // Esta es la memoria RAM, 128KB

/*NOTA: existen unos registros específicos del Cortex M4 en la región 0xE0000000U
 * Los controladores de las interrupciones se encuentran allí.
 */

/*Ahora agregamos la dirección de memoria base para cada uno de los periféricos que posee el micro.
 * Buses:
 * -APB1 (Advance Peripheral Bus)
 * -APB2
 * -AHB1 (Advanced High-performance Bus)
 * -AHB2
 */

/*
 * AHBx and APBx Bus Peripherals base addresses
 */
#define APB1_BASE_ADDR       	 0x40000000U //Doy la riección del bus
#define APB2_BASE_ADDR        	 0x40010000U
#define AHB1_BASE_ADDR        	 0x40020000U
#define AHB2_BASE_ADDR         	 0x50000000U

/**
 * Repito el procedimiento de establecimiento de direcciones (dónde se albergan los registros) para cada
 * una de las posiciones de memoria de los periféricos que se describen en la Tabla 1,
 * correspondientes a los buses  a APB1, APB2, AHB1, AHB2
 */
/* Comienzo con las posiciones de memoria para el periférico de AHB2*/

#define USB_OTG_FS_BASE_ADDR     (AHB2_BASE_ADDR + 0x0000U)

/*Realizo la definición de las posiciones de memoria para los periféricos de AHB1*/
#define RCC_BASE_ADDR            (AHB1_BASE_ADDR + 0x3800U) // Dirección específica del periférico dentro de mi bus
#define GPIOH_BASE_ADDR          (AHB1_BASE_ADDR + 0x1C00U) //Es como desplazarme ciertos kilometros en la línea o en el bus
#define GPIOE_BASE_ADDR          (AHB1_BASE_ADDR + 0x1000U)
#define GPIOD_BASE_ADDR          (AHB1_BASE_ADDR + 0x0C00U)
#define GPIOC_BASE_ADDR          (AHB1_BASE_ADDR + 0x0800U)
#define GPIOB_BASE_ADDR			 (AHB1_BASE_ADDR + 0x0400U)
#define GPIOA_BASE_ADDR          (AHB1_BASE_ADDR + 0x0000U) //El bus más a través del cual se llega
// Los buses llegan a peripericos diferentes, los conectan


/**
 * Macros genéricos
 */
//Les asigno un valor a las palabras

#define ENABLE                    1
#define DISABLE                   0
#define SET                       ENABLE
#define CLEAR                     DISABLE
#define RESET                     DISABLE
#define GPIO_PIN_SET              SET
#define GPIO_PIN_RESET            RESET
#define FLAG_SET                  SET
#define FLAG_RESET                RESET
#define I2C_WRITE                 0
#define I2C_READ                  1


/* ====== INICIO DE LA DESCRIPCIÓN DE LOS ELEMENTOS QUE COMPONEN EL PERIFÉRICO ====== */
/** Definición de la estructura de datos que representa a cada uno de los registros que componen al
 * periférico RCC. Se enuncian cada uno de los registros.
 * Solo se definen los bits de registros: 6.3.1 (RCC_CR) hasta 6.3.12 (RCC_APB2ENR), 6.3.17 (RCC_BDCR)
 * y 6.3.18 (RCC_CSR)*/
//RCC es el periférico corazón, se debe configurar
//Buses como líneas del metro
//Periféricos como estaciones
//RCC como oficina principal
//RCC proporciona la sincronización para que se den los procesos
//Activo la sincronización del reloj
//Activo el bus que tiene la agrupación de memoria de todos los periféricos
//Activo el periférico ESPECÍFICO

/*RCC como tablero de control. CR es una linea de 32 botones del tablero del 0 al 32*/

typedef struct
{
	volatile uint32_t CR;              //Clock Control Register                    ADDR_OFFSET : 0x00
	volatile uint32_t PLLCFGR;         //PLL Configuration Register                ADDR_OFFSET : 0x04
	volatile uint32_t CFGR;            //Clock Configuration register              ADDR_OFFSET : 0x08
	volatile uint32_t CIR;             //Clock Interrupt Register                  ADDR_OFFSET : 0x0C
	volatile uint32_t AHB1RSTR;        //AHB1 peripheral Reset Register            ADDR_OFFSET : 0x10
	volatile uint32_t AHB2RSTR;        //AHB2 peripheral Reset Register            ADDR_OFFSET : 0x14
	volatile uint32_t reserved0;       //reserved                                  ADDR_OFFSET : 0x18
	volatile uint32_t reserved1;       //reserved                                  ADDR_OFFSET : 0x1C
	volatile uint32_t APB1RSTR;        //APB1 peripheral Reset Register            ADDR_OFFSET : 0x20
	volatile uint32_t APB2RSTR;        //APB2 peripheral Reset Register            ADDR_OFFSET : 0x24
	volatile uint32_t reserved2;       //reserved                                  ADDR_OFFSET : 0x28
	volatile uint32_t reserved3;       //reserved                                  ADDR_OFFSET : 0x2C
	volatile uint32_t AHB1ENR;         //AHB1 peripheral clock enable register     ADDR_OFFSET : 0x30
	volatile uint32_t AHB2ENR;         //AHB2 peripheral clock enable register     ADDR_OFFSET : 0x34
	volatile uint32_t reserved4;       //reserved                                  ADDR_OFFSET : 0x38
	volatile uint32_t reserved5;       //reserved                                  ADDR_OFFSET : 0x3C
	volatile uint32_t APB1ENR;         //APB1 peripheral clock enable register     ADDR_OFFSET : 0x40
	volatile uint32_t APB2ENR;         //APB2 peripheral clock enable register     ADDR_OFFSET : 0x44
	volatile uint32_t reserved6;       //reserved                                  ADDR_OFFSET : 0x48
	volatile uint32_t reserved7;       //reserved                                  ADDR_OFFSET : 0x4C
	volatile uint32_t AHB1PENR;        //AHB1 clock enable low power register      ADDR_OFFSET : 0x50
	volatile uint32_t AHB2PENR;        //AHB2 clock enable low power register      ADDR_OFFSET : 0x54
	volatile uint32_t reserved8;       //reserved                                  ADDR_OFFSET : 0x58
	volatile uint32_t reserved9;       //reserved                                  ADDR_OFFSET : 0x5C
	volatile uint32_t APB1PENR;        //APB1 clock enable low power register      ADDR_OFFSET : 0x60
    volatile uint32_t APB2PENR;        //APB2 clock enable low power register      ADDR_OFFSET : 0x64
    volatile uint32_t reserved10;      //reserved                                  ADDR_OFFSET : 0x68
    volatile uint32_t reserved11;      //reserved                                  ADDR_OFFSET : 0x6C
    volatile uint32_t BDCR;            //Backup domain control register            ADDR_OFFSET : 0x70
    volatile uint32_t CSR;             //Clock control and status register         ADDR_OFFSET : 0x74
    volatile uint32_t reserved12;      //reserved                                  ADDR_OFFSET : 0x78
    volatile uint32_t reserved13;      //reserved                                  ADDR_OFFSET : 0x7C
    volatile uint32_t SSCGR;           //Spread spectrum clock generation reg      ADDR_OFFSET : 0x80
    volatile uint32_t PLLI2SCFGR;      //PLLI2S configuration register             ADDR_OFFSET : 0x84
    volatile uint32_t reserved14;      //reserved                                  ADDR_OFFSET : 0x88
    volatile uint32_t DCKCFGR;         //Dedicated Clock configuration Reg         ADDR_OFFSET : 0x8C

}RCC_RegDef_t;

/* Se realiza un puntero RCC_RegDef_t que apunta a la posición exacta del periférico RCC, de forma que
 * cada miembro de la estructura coincide con cada uno de los SFR en la memoria del micro. Esta acción
 * se realiza en un macro, de forma que el nuevo elemento "RCC" queda disponible para cada clase .c que
 * incluya este archivo
 */

#define RCC               ((RCC_RegDef_t *) RCC_BASE_ADDR) //Con el puntero, designo una variable en una posición necesaria


/* A continuación se realiza la descripción bit a bit de cada uno de los GPIOx que componen a RCC*/

#define RCC_AHB1ENR_GPIOA_EN          0
#define RCC_AHB1ENR_GPIOB_EN          1
#define RCC_AHB1ENR_GPIOC_EN          2
#define RCC_AHB1ENR_GPIOD_EN          3
#define RCC_AHB1ENR_GPIOE_EN          4
#define RCC_AHB1ENR_GPIOH_EN          7
#define RCC_AHB1ENR_CRCEN             12
#define RCC_AHB1ENR_DMA1_EN           21
#define RCC_AHB1ENR_DMA2_EN           22

/* ====== FIN de la descripción de kis ekementos que hacen parte del periférico RCC ====== */

/* +++==== INICIO de la descripción de los elementos que hacen parte del periférico GPIOx ====+++ */
/* Definición de la estructura de datos que representa a cada uno de los registros que componen el
 * periférico GPIO
 */
typedef struct
{
	volatile uint32_t MODER;           //port mode register                        ADDR_OFFSET : 0x00
    volatile uint32_t OTYPER;          //Port output type register                 ADDR_OFFSET : 0x04
	volatile uint32_t OSPEEDR;         //Port output speed register                ADDR_OFFSET : 0x08
	volatile uint32_t PUPDR;           //Port pull-up/pull-down register           ADDR_OFFSET : 0x0C
	volatile uint32_t IDR;             //Port input data register                  ADDR_OFFSET : 0x10
	volatile uint32_t ODR;             //Port output data register                 ADDR_OFFSET : 0x14
	volatile uint32_t BSRR;            //port bit set/reset register               ADDR_OFFSET : 0x18
	volatile uint32_t LCKR;            //port configuration lock register          ADDR_OFFSET : 0x1C
	volatile uint32_t AFRL;            //alternate function low register           ADDR_OFFSET : 0x20
	volatile uint32_t AFRH;            //alternate function high register          ADDR_OFFSET : 0x24

}GPIOx_RegDef_t;

/*Creo el puntero que define a GPIOx. Debo hacer que cada GPIOx (A, B, C, ...) quede ubicado
 *exactamente sobre la posición de memoria correcta. Debido a que son varios GPIOx, debo hace la
 *definición para cada uno
 * */

#define GPIOA                      ((GPIOx_RegDef_t *) GPIOA_BASE_ADDR)
#define GPIOB                      ((GPIOx_RegDef_t *) GPIOB_BASE_ADDR)
#define GPIOC                      ((GPIOx_RegDef_t *) GPIOC_BASE_ADDR)
#define GPIOD                      ((GPIOx_RegDef_t *) GPIOD_BASE_ADDR)
#define GPIOE                      ((GPIOx_RegDef_t *) GPIOE_BASE_ADDR)
#define GPIOH                      ((GPIOx_RegDef_t *) GPIOH_BASE_ADDR)
									/*PUNTERO         - POSICIÓN DE MEMORIA */

/** Valores estándar para las configuraciones **/
/* 8.4.1 GPIOx_MODER (2 bits por cada PIN)*/
#define GPIO_MODE_IN               0
#define GPIO_MODE_OUT              1
#define GPIO_MODE_ALTFN            2
#define GPIO_MODE_ANALOG           3

/* 8.4.2 GPIOx_OTYPER (1 bit por PIN)*/
#define GPIO_OTYPE_PUSHPULL        0
#define GPIO_OTYPE_OPENDRAIN       1

/* 8.4.3 GPIOx_OSPEEDR (2 bits por cada PIN) */
#define GPIO_OSPEED_LOW            0
#define GPIO_OSPEED_MEDIUM         1
#define GPIO_OSPEED_FAST           2
#define GPIO_OSPEED_HIGH           3

/*8.4.4 GPIOx_PUPDR (dos bits por cada PIN) */
#define GPIO_PUPDR_NOTHING         0
#define GPIO_PUPDR_PULLUP          1
#define GPIO_PUPDR_PULLDOWN        2
#define GPIO_PUPDR_RESERVED        3

/* 8.4.5 GPIOX_IDR (1 bit por PIN) este es el registro para leer el estado de un pin */

/* 8.4.6 GPIOx_ODR (1 bit por PIN) este es el registro para escribir el estado de un PIN
 * (1 o 0). Se prefiere el registro BSRR*/

/*Definición de los nombres de los pines*/
#define PIN_0                      0
#define PIN_1                      1
#define PIN_2                      2
#define PIN_3                      3
#define PIN_4                      4
#define PIN_5                      5
#define PIN_6                      6
#define PIN_7                      7
#define PIN_8                      8
#define PIN_9                      9
#define PIN_10                     10
#define PIN_11                     11
#define PIN_12                     12
#define PIN_13                     13
#define PIN_14                     14
#define PIN_15                     15

/*Definición de las funciones alternativas*/
#define AF0                        0b0000
#define AF1                        0b0001
#define AF2                        0b0010
#define AF3                        0b0011
#define AF4                        0b0100
#define AF5                        0b0101
#define AF6                        0b0110
#define AF7                        0b0111
#define AF8                        0b1000
#define AF9                        0b1001
#define AF10                       0b1010
#define AF11                       0b1011
#define AF12                       0b1100
#define AF13                       0b1101
#define AF14                       0b1110
#define AF15                       0b1111

/*
 * typedef struct
 * {
 * 		volatile uint32_t dummy;   //Ejemplo para dummies          ADDR_OFFSET: 0X00
 * } DUMMY_RegDef_t;
 *
 */
#endif /*INC_STM32F4XX_H_*/





