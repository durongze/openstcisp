#ifndef CRC24_H
#define CRC24_H

#include <stdint.h>

uint32_t crc24(uint32_t init_vect, uint8_t *buffer, uint32_t len);
uint16_t crc16(uint8_t* data, uint32_t len);
uint16_t sum16(uint8_t* data, uint32_t size);

#endif