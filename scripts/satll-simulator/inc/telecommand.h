#ifndef _TELECOMMAND_H
#define _TELECOMMAND_H
#include <stdint.h>
#include <stdio.h>

#include "pus.h"
#include "space_packet.h"
#include "ecss_crc.h"
#include "config.h"

#include "udp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    uint16_t N;
    uint16_t Address;
    uint16_t Value;
} TC_2_1;

extern TC_2_1 tc_2_1;

typedef struct{
    uint16_t N;
    uint8_t DeviceID;
    uint8_t protocol_specific_data;
    uint16_t commandData;
} TC_2_7;

extern TC_2_7 tc_2_7;

typedef struct{
    uint16_t N;
    uint16_t housekeeping_SID;
} TC_3_5;

extern TC_3_5 tc_3_5;

typedef struct{
    uint16_t N;
    uint16_t housekeeping_SID;
} TC_3_6;

extern TC_3_6 tc_3_6;

typedef struct{
    uint16_t N;
    uint16_t event_definition_ID;
} TC_5_5;

extern TC_5_5 tc_5_5;

typedef struct{
    uint16_t N;
    uint16_t event_definition_ID;
} TC_5_6;

extern TC_5_6 tc_5_6;




uint8_t handle_pus_tc(uint8_t *in, pusTCPacket *out);
uint8_t handle_sp(uint8_t *in, spPacket *out);


#ifdef __cplusplus
}
#endif


#endif