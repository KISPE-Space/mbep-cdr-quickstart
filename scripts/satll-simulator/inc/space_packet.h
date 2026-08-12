#ifndef _SPACEPACKET_H
#define _SPACEPACKET_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SP_POLYNOMIAL 0x1021
#define SP_CRC_INIT 0xFFFF

#define SP_HEADER_LENGTH 6

#define TELEMETRY 0
#define TELECOMMAND 1

typedef struct {
    uint8_t PacketVersionNumber;
    uint8_t PacketType;
    uint8_t SecondaryHeaderFlag;
    uint16_t APID;
    uint8_t sequenceFlags;
    uint16_t PacketSequenceCount;
    uint16_t PacketDataLength;
    uint8_t data[256]; //Will decide in future
    uint16_t SpacePacketCRC16;
}spPacket;

extern spPacket space_packet;
extern spPacket space_packet_tc;

uint16_t buildSPPacket(const spPacket *in, uint8_t *out);
uint16_t buildSPHeader(spPacket *in, uint8_t PacketVersionNumber, uint8_t PacketType, uint8_t SecondaryHeaderFlag, uint16_t APID, uint8_t sequenceFlags, uint16_t PacketSequenceCount, uint16_t PacketDataLength, uint8_t *PusFrameOutput);
   
// uint8_t handle_sp(uint8_t *in, spPacket *out);


#ifdef __cplusplus
}
#endif


#endif