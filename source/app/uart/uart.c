//
// Created by Dananjaya RAMANAYAKE on 01/09/2023.
//

#include "uart.h"

extern UART_HandleTypeDef huart2;
extern uint8_t data_buffer;
extern uint8_t uart_ready;
extern uint8_t data_available;

void uart_write(uint8_t* data, const uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        uart_write_byte(data[i]);
    }
}
void uart_write_byte(uint8_t data) {
    HAL_UART_Transmit_IT(&huart2,&data,sizeof(uint8_t));
    while(uart_ready != SET);
    uart_ready = RESET;
}
uint32_t uart_read(uint8_t* data, const uint32_t length) {
    data_available = 0;
    if (length > 0) {
        *data = data_buffer;
        return 1;
    }
    return 0;
}
uint8_t uart_read_byte(void) {
    HAL_UART_Receive_IT(&huart2,&data_buffer, 1);
    return data_buffer;
}
