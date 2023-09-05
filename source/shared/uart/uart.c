//
// Created by Dananjaya RAMANAYAKE on 01/09/2023.
//

#include "uart.h"
#include "ring_buffer.h"

extern UART_HandleTypeDef huart2;
extern uint8_t uart_ready;
extern ring_buffer_t rb;

void uart_write(uint8_t* data, const uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        uart_write_byte(data[i]);
    }
}

void uart_write_byte(uint8_t data) {
    HAL_UART_Transmit_IT(&huart2,&data,sizeof(uint8_t));
    while (HAL_UART_GetState(&huart2) ==HAL_UART_STATE_BUSY_TX ||
           HAL_UART_GetState(&huart2) == HAL_UART_STATE_BUSY_TX_RX);
}

uint32_t uart_read(uint8_t* data, const uint32_t length) {
    if(length == 0) {
        return 0;
    }

    for (uint32_t bytes_read = 0; bytes_read < length; bytes_read++) {
        if(!ring_buffer_read(&rb,&data[bytes_read])) {
            return bytes_read;
        }
    }

    return length;
}

uint8_t uart_read_byte(void) {
    uint8_t byte = 0;
    uart_read(&byte,1);
    return byte;
}

bool uart_data_available(void) {
    return !ring_buffer_empty(&rb);
}
