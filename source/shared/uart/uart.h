//
// Created by Dananjaya RAMANAYAKE on 01/09/2023.
//

#ifndef _UART_H
#define _UART_H

#include <stdint-gcc.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_usart.h"

#define RING_BUFFER_SIZE (128) // For maximum of ~10 ms latency

void uart_write(uint8_t* data, const uint32_t length);
void uart_write_byte(uint8_t data);
uint32_t uart_read(uint8_t* data, const uint32_t length);
uint8_t uart_read_byte(void);
bool uart_data_available(void);


#endif //_UART_H
