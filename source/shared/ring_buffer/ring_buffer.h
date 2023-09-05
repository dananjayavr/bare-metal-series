//
// Created by Dananjaya RAMANAYAKE on 04/09/2023.
//

#ifndef _RING_BUFFER_H
#define _RING_BUFFER_H

#include <stdlib.h>
#include <stdint-gcc.h>
#include <stdbool.h>

typedef struct ring_buffer_t {
    uint8_t *buffer; // pointer to data location in memory
    uint32_t mask; // mask used to perform pointer arithmetic
    uint32_t read_index;  // read pointer
    uint32_t write_index; // writer pointer
} ring_buffer_t;

void ring_buffer_setup(ring_buffer_t *rb, uint8_t *buffer, uint32_t size);
bool ring_buffer_empty(ring_buffer_t *rb);
bool ring_buffer_write(ring_buffer_t *rb, uint8_t byte);
bool ring_buffer_read(ring_buffer_t *rb, uint8_t *byte);


#endif //_RING_BUFFER_H
