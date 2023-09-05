//
// Created by Dananjaya RAMANAYAKE on 04/09/2023.
//

/*
 * Example of mask usage to calculate next index (assuming buffer size of 8):
 *
| Index | Binary | Index + 1 | (Index + 1) & Mask | New Index |
|-------|--------|-----------|--------------------|-----------|
| 0     | 00000000 | 00000001 | 00000001 & 00000111 | 1         |
| 1     | 00000001 | 00000010 | 00000010 & 00000111 | 2         |
| 2     | 00000010 | 00000011 | 00000011 & 00000111 | 3         |
| 3     | 00000011 | 00000100 | 00000100 & 00000111 | 4         |
| 4     | 00000100 | 00000101 | 00000101 & 00000111 | 5         |
| 5     | 00000101 | 00000110 | 00000110 & 00000111 | 6         |
| 6     | 00000110 | 00000111 | 00000111 & 00000111 | 7         |
| 7     | 00000111 | 00001000 | 00001000 & 00000111 | **0**     |
 *
 */

#include "ring_buffer.h"

void ring_buffer_setup(ring_buffer_t *rb, uint8_t *buffer, uint32_t size) {
    rb->buffer = buffer;
    rb->read_index = 0;
    rb->write_index = 0;
    rb->mask = size - 1; // assuming the size is power of two
}

bool ring_buffer_empty(ring_buffer_t *rb) {
    return rb->read_index == rb->write_index;
}

bool ring_buffer_read(ring_buffer_t *rb, uint8_t *byte) {
    uint32_t local_read_index = rb->read_index; // local copies are for thread safety
    uint32_t local_write_index = rb->write_index; // local copies are for thread safety

    if(local_read_index == local_write_index) {
        return false;
    }

    *byte = rb->buffer[local_read_index];
    local_read_index = (local_read_index + 1) & rb->mask; // see the comment above for an explanation
    rb->read_index = local_read_index;

    return true;
}

bool ring_buffer_write(ring_buffer_t *rb, uint8_t byte) {
    uint32_t local_read_index = rb->read_index; // local copies are for thread safety
    uint32_t local_write_index = rb->write_index; // local copies are for thread safety

    uint32_t next_write_index = (local_write_index + 1) & rb->mask;

    if(next_write_index == local_read_index) {
        return false; // discard data and return
    }

    rb->buffer[local_write_index] = byte;
    rb->write_index = next_write_index;

    return true;
}
