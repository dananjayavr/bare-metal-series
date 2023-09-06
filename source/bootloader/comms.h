//
// Created by Dananjaya RAMANAYAKE on 06/09/2023.
//

#ifndef _COMMS_H
#define _COMMS_H

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define PACKET_DATA_LENGTH      16
#define PACKET_LENGTH_BYTES     1
#define PACKET_CRC_BYTES        1
#define PACKET_LENGTH           (PACKET_LENGTH_BYTES + PACKET_DATA_LENGTH + PACKET_CRC_BYTES)

#define PACKET_RETX_DATA0       0x19
#define PACKET_ACK_DATA0        0x15

typedef struct comms_packet_t {
    uint8_t length;
    uint8_t data[PACKET_DATA_LENGTH];
    uint8_t crc;
} comms_packet_t;

void comms_setup(void);
void comms_update(void);

bool comms_packets_available(void);
void comms_write(comms_packet_t *packet);
void comms_read(comms_packet_t *packet);
uint8_t comms_compute_crc(comms_packet_t *packet);
bool comms_is_single_byte_packet(const comms_packet_t  *packet, uint8_t byte);

void comms_packet_copy(const comms_packet_t *source, comms_packet_t *dest);


#endif //_COMMS_H
