/*
 * printf.c
 *
 *  Created on: Jun 15, 2021
 *      Author: bartek
 */

#include "printf.h"

int __io_putchar(int ch)
{
	// if ('\n' == ch)
	// 	__io_putchar('\r');
	HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
	return (status == HAL_OK ? ch : 0);
}
