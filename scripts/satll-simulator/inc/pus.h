#ifndef _PUS_H
#define _PUS_H
#include <stdint.h>
#include <stdio.h>
#include "telemetry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_COARSE_SIZE 7
#define MAX_FINE_SIZE 10

#define INVALID_TIME 0
#define CUC 1


typedef struct {
    uint8_t PUS_VersionNumber;
    uint8_t spacecraft_time_reference_status;
    uint8_t service_type_ID;
    uint8_t message_subtype_ID;
    uint16_t message_type_counter;
    uint16_t destination_ID;
   
    // uint8_t pus_coarse_time[7];
    // uint8_t pus_fine_time[10];
        struct pfield{
            uint8_t pfield_extension;
            uint8_t TimeCodeIdentidification;
            uint8_t number_of_coarse_bytes;
            uint8_t number_of_fine_bytes;
        } pField;
    uint8_t pus_pfield;
    uint8_t epoch_day;
    uint8_t epoch_month;
    uint16_t epoch_year;
    uint8_t epoch_hour;
    uint8_t epoch_minute;
    uint8_t epoch_seconds;
    uint8_t pus_time[18];

    uint8_t pus_coarse_time_size;
    uint8_t pus_fine_time_size;
    uint8_t spare [1];
    uint8_t pus_data[256]; //Will decide in future
}pusPacket;

extern pusPacket pus_c;

typedef struct {
    uint8_t PUS_VersionNumber;
    struct ack{
            uint8_t successful_acceptance_request;
            uint8_t successful_start_request;
            uint8_t successful_progress_request;
            uint8_t successful_completion_request;
        } ack_flag;
    uint8_t acknowledgement_flags;
    uint8_t service_type_ID;
    uint8_t message_subtype_ID;
    uint16_t source_ID;
    uint16_t tc_data_length;
    uint8_t tc_data[256]; //Will decide in future
}pusTCPacket;

extern pusTCPacket pus_c_tc;



extern uint8_t PusFrame[256];

/**********************         FUNCTIONS       ****************************** */

uint8_t setServiceTypeID(uint8_t serviceTypeID);    //Function Call to set the Service Type ID
uint8_t validService(uint8_t serviceTypeID);        //Function Call to check if the service is valid/supported

uint8_t setMessageSubtypeID(uint8_t MessageTypeID); //Function Call to set the Message Sub-Type ID
uint8_t validMessage(uint8_t MessageTypeID);        //Function Call to check if the Sub-Type is valid/supported

uint8_t setCoarseTimeSize(uint8_t CoarseSize);
uint8_t setFineTimeSize(uint8_t FineSize);

uint8_t setEpoch(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minutes, uint8_t seconds);

uint8_t buildpField(pusPacket *in, uint8_t pfield_extension, uint8_t TimeCodeIdentidification, uint8_t number_of_coarse_bytes, uint8_t number_of_fine_bytes);
// uint16_t buildPUSFrame(const pusPacket *in, uint8_t *out);
uint16_t buildPUSFrame(const pusPacket *in, uint16_t datalength , uint8_t *out);
uint16_t buildPUSHeader(pusPacket *in, uint8_t VersionNumber, uint8_t time_reference, uint8_t serviceType, uint8_t subType, uint16_t subTypeCounter, uint16_t destinationID, uint8_t pField, uint8_t time[18], uint8_t *PusFrameOutput);
   

// uint8_t handle_pus_tc(uint8_t *in, pusTCPacket *out);


#ifdef __cplusplus
}
#endif


#endif