#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define NUM_COARSE_BYTES 2
#define NUM_FINE_BYTES 1


extern uint8_t PusFrameOut[256];
extern uint16_t PusFrameLength;

extern uint8_t SPPacketOut[256];
extern uint16_t SPPacketLength;

extern uint8_t TMFrameOut[256];
extern uint16_t TMFrameLength;

extern uint8_t MasterFrameCount;
extern uint8_t VirtualChannelCount;
extern uint16_t COUNTER_3_25;
extern uint16_t sp_packet_count;

// extern uint16_t SID_1;
// extern const uint16_t FSW_DB_VER;
// extern const uint16_t SC_ID;
// extern uint16_t DAY;
// extern uint16_t HOUR;
// extern uint16_t MINUTE;
// extern uint16_t SECOND;
// extern uint16_t CMD_RCV_COUNT;
// extern uint16_t CMD_ACC_CNT;

extern uint32_t coarse_time;
extern uint16_t SecondUpdate;
extern uint16_t TMInternal;

extern unsigned long prevMillis;
extern uint64_t timeStart;
extern uint64_t timeSinceBoot;
extern uint64_t previousSecondUpdate;


#endif