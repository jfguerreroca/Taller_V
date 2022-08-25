/*
 * stm32f411xx_hal.h
 *
 *  Created on: Aug 25, 2022
 *      Author: felipe
 */

#ifndef STM32F411XX_HAL_H_
#define STM32F411XX_HAL_H_

#include <stdint.h>
#include <stddef.h>

#define HSI_CLOCK_SPEED		16000000	// Value for the main clock signal (HSI -> High Speed Internal)
#define HSE_CLOCK_SPEED		4000000		// Value for the main clock signal (HSE -> High Speed External)

#define NOP()	asm("NOP")
#define __weak	__attribute__((weak))

/*
 * Base addresses of Flash and SRAM memories
 * Dataaheet, Memory Map, figure 14
 * (Remember, 1KByte = 1024 bytes
 */
#define FLASH_BASE_ADDR		0x08000000U		// Esta es la memoria del programa, 512KB
#define SRAM_BASE_ADDR		0x20000000U		// Esta es la memoria RAM, 128KB

/* NOTA: Observar que existen unos registros específicos del Cortex M4 en la región 0xE0000000U
 * Los
 *
 */

#endif /* STM32F411XX_HAL_H_ */
