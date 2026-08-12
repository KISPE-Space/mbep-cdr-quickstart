#ifndef ECSS_CRC_H
#define ECSS_CRC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint16_t ecss_crc16(const uint8_t *data, uint16_t length);

#ifdef __cplusplus
}
#endif



#endif