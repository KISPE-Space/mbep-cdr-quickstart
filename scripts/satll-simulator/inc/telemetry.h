#ifndef _TELEMETRY_H
#define _TELEMETRY_H
#include <stdint.h>
#include <stdio.h>

#include "pus.h"
#include "space_packet.h"
#include "ecss_crc.h"
#include "config.h"


#ifdef __cplusplus
extern "C" {
#endif

void update_time(void);
void update_telemetry(void);

uint8_t generate_tm_1_1(uint8_t *in);
uint8_t generate_tm_1_2(uint8_t *in);
uint8_t generate_tm_1_7(uint8_t *in);
uint8_t generate_tm_1_8(uint8_t *in);
uint8_t generate_tm_5_1(uint8_t *in);
uint8_t generate_tm_9_2(uint8_t *pfield_in , uint8_t *in);
uint8_t generate_tm_17_2(void);  //TM[17,2] are-you-alive connection test report


uint16_t generate_tm_3_25(uint8_t SID);

typedef struct {
    // uint16_t SID;
    uint8_t SID;
    uint16_t FSW_DB_VER;
    uint16_t DAY;
    uint16_t HOUR;
    uint16_t MINUTE;
    uint16_t SECOND;
    uint16_t SC_ID;
    uint16_t CMD_RCV_COUNT;
    uint16_t CMD_ACC_CNT;
    uint8_t event_enabled;
}TM_3_25_1;

extern TM_3_25_1 tm_3_25_1;

typedef struct {
    uint16_t Packet_ID; // Does include packet version number 
    uint16_t Sequence_Control;
}TM_1_1;
extern TM_1_1 tm_1_1;

typedef struct {
    uint16_t Packet_ID; // Does include packet version number 
    uint16_t Sequence_Control;
    uint16_t failure_code;
    uint16_t failure_data;

}TM_1_2;
extern TM_1_2 tm_1_2;

typedef struct {
    uint16_t Packet_ID; // Does include packet version number 
    uint16_t Sequence_Control;
}TM_1_7;
extern TM_1_7 tm_1_7;

typedef struct {
    uint16_t Packet_ID; // Does include packet version number 
    uint16_t Sequence_Control;
    uint16_t failure_code;
    uint16_t failure_data;

}TM_1_8;
extern TM_1_8 tm_1_8;

typedef struct {
    uint16_t Event_ID;
    uint16_t auxilary_data;
    uint8_t data[4];
    uint8_t event_enabled;
}TM_5_1;
extern TM_5_1 tm_5_1;

typedef struct {
    uint8_t rate_exponential_value;
    uint8_t pField; //Should equal the spacecraft/pus header pField unless stated otherwise
    uint8_t spacecraft_time[18];
    uint8_t spacecraft_coarse_time_size;
    uint8_t spacecraft_fine_time_size;  

}TM_9_2;
extern TM_9_2 tm_9_2;

typedef struct {
    
}TM_17_2;
extern TM_17_2 tm_17_2;

#ifdef __cplusplus
}
#endif


#endif