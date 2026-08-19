/*

    This example code is used to simulate a spacecraft sending TM[3,25] pus packets
    The expected format of the data is:
    | Space Packet Primary Header | PUS-C Header | PUS-C Data | CRC-16?

*/
#include <stdio.h>
#include <stdint.h>
#include <time.h>   // System Time

/*  Handles UDP communication
    Contains the Port definitions/values
*/
#include "inc/udp.h" 

/*
    defined Space Packet and PUS structures
*/
#include "inc/pus.h"
#include "inc/space_packet.h"
#include "inc/ecss_crc.h"

/*
    Custom handling of Telemetry and Telecommands
*/
#include "inc/telemetry.h"  
#include "inc/telecommand.h"

// Declaring Functions
void simulator_init();
uint64_t millis(void);
void send_telemetry(void);
void check_for_tc(void);

//Declaring global variables
uint64_t timeSinceBoot = 0;
uint64_t timeStart = 0; //Used for initial time offset form linux system
uint64_t previousTMSendMillis = 0;
uint16_t TelemetrySend_Interval = 2000;

/*
    UDP interface to YAMCS
*/
UDP_Socket udp;
#define UDP_BUFFER_SIZE 2048
uint8_t udp_data_in[UDP_BUFFER_SIZE];
int udp_data_length;


int main(){
    setbuf(stdout, NULL);
    timeStart = millis();

    simulator_init(); // Function call to set default values and Initialise Interfaces

    // Infinite Loop
    while(1){
        timeSinceBoot = millis() - timeStart;

        /*
            These functions are defined in telemetry.c
            Time Intervals are checked inside the functions
        */
        update_time();
        update_telemetry();
        send_telemetry();
        check_for_tc();
    }
    return 0;
}

void simulator_init(){
    //Fill in variable timeSinceBoot
    timeSinceBoot = millis() - timeStart;

    /*
        Starting UDP interface to communicate to YAMCS
        Ports and IP defined in YAMCS.yaml
    */
    if (init_udp(&udp,NULL, RX_PORT ,"127.0.0.1",TX_PORT) != 0){
        printf("Failed to initialize UDP\n");
    }
   set_udp_nonblocking(&udp);

   /*
        PUS time not being used, p-Field set to 0 with 0 bytes of time
   */
    pus_c.pus_pfield = buildpField(&pus_c, 0, 0, NUM_COARSE_BYTES, NUM_FINE_BYTES);
    
   /*
        Setting default values for TM[3,25] SID_1 packet
   */
    tm_3_25_1.SID = 1;
    tm_3_25_1.FSW_DB_VER = 0x01;
    tm_3_25_1.DAY = 0x001A;
    tm_3_25_1.HOUR = 0x000B;
    tm_3_25_1.MINUTE = 0x0012;
    tm_3_25_1.SECOND = 0x0022;
    tm_3_25_1.SC_ID = 0x69;
    tm_3_25_1.CMD_RCV_COUNT = 0x0000;
    tm_3_25_1.CMD_ACC_CNT = 0x0000;

    /*
        Setting default values for Space Packet
        These values are not expected to change in operation.    
    */
    space_packet.PacketVersionNumber = 0;
    space_packet.PacketType = TELEMETRY;
    space_packet.SecondaryHeaderFlag = 1;
    space_packet.APID = 1;   // Double Check
    space_packet.sequenceFlags = 3;
    space_packet.PacketSequenceCount = 0;
    sp_packet_count = 1;

    tm_5_1.event_enabled = 1; //Enabling the event notification for ADCS Mode Change
    tm_3_25_1.event_enabled = 1; //Enabling Periodic Telemetry


}


void send_telemetry(void){
        //TM send interval
        if((timeSinceBoot - previousTMSendMillis) >= TelemetrySend_Interval){
    
            generate_tm_3_25(1); // Populates PusFrameOut[] and SPPacketOut[]
            send_udp(&udp, SPPacketOut, SPPacketLength); // Only send Space Packet
        
            printf("Sending: ");
            for(int i = 0; i < (SPPacketLength); i++){
                printf("%02X", SPPacketOut[i]);
            }
            printf("\n");

            // Increase Sequence Counters
            sp_packet_count++;
            COUNTER_3_25++;
            
            previousTMSendMillis = timeSinceBoot;
        }

}

void check_for_tc(){
    //Check for command
    int received;
    received = receive_udp_nonblocking(&udp, udp_data_in, UDP_BUFFER_SIZE);

    if (received > 0){ // Assuming Fully intact packet (As is UDP)
        udp_data_length = received;

        printf("Received %d bytes\n", received);

        int i;
        for(i = 0; i < (received); i++){
                printf("%02X", udp_data_in[i]);
        }
        printf("\n");

        handle_sp(udp_data_in, &space_packet_tc);
        handle_pus_tc(space_packet_tc.data, &pus_c_tc);
        received = 0;
    }
}


/*
    Function to get time from a linux system
    Is used for Satellite Onboard time
*/
uint64_t millis(void){
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ((uint64_t)ts.tv_sec * 1000) +
           (ts.tv_nsec / 1000000);
}