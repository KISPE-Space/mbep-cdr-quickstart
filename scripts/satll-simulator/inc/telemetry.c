#include "telemetry.h"

// Global Telemetry Variables
/*
  PUS-C data packet output
*/
uint8_t PusFrameOut[256] = {0};
uint16_t PusFrameLength = 0;
/*
  Space Packet output
*/
uint8_t SPPacketOut[256] = {0};
uint16_t SPPacketLength = 0;

/*
  Message Type Counters
*/
uint16_t COUNTER_1_1 = 0;
uint16_t COUNTER_1_2 = 0;
uint16_t COUNTER_1_7 = 0;
uint16_t COUNTER_1_8 = 0;
uint16_t COUNTER_3_25 = 0;
uint16_t COUNTER_5_1 = 0;
uint16_t COUNTER_9_2 = 0;
uint16_t COUNTER_17_2 = 0;
uint16_t sp_packet_count = 0;

// uint16_t SID_1 = 1;
// const uint16_t FSW_DB_VER = 0x0010;
// const uint16_t SC_ID = 105;
// uint16_t DAY = 6;
// uint16_t HOUR = 10;
// uint16_t MINUTE = 40;
// uint16_t SECOND = 0;
// uint16_t CMD_RCV_COUNT = 0;
// uint16_t CMD_ACC_CNT = 0;
// uint32_t coarse_time = 0x6A3DC100;      //Cheating a bit here to set the epoch

/*
  Defining Time intervals
*/
uint64_t previousMillis = 0;
uint64_t previousSecondUpdate = 0;
uint64_t previousTelemetryUpdate = 0;

uint16_t SecondUpdate_Interval = 1000;
uint16_t TelemetryUpdate_Interval = 2000;
uint32_t coarse_time = 0x6A3DC100;      //Cheating a bit here to set the epoch

/*
  Initialising TM packets to 0
*/


TM_1_1 tm_1_1 = {0};
TM_1_2 tm_1_2 = {0};
TM_1_7 tm_1_7 = {0};
TM_1_8 tm_1_8 = {0};
TM_3_25_1 tm_3_25_1 = {0};
TM_5_1 tm_5_1 = {0};
TM_9_2 tm_9_2 = {0};
TM_17_2 tm_17_2 = {0};



void update_time(void){
        /*
         Checking if 1s passed since last call
         Not accurate time but appropriate for current implementation
        */
        if((timeSinceBoot - previousSecondUpdate) >= SecondUpdate_Interval ){    //Updates every second
        /*
                Updating PUS header Time Field
        */
        coarse_time = coarse_time + 1;
        pus_c.pus_time[0] = (uint8_t)(coarse_time >> 24);
        pus_c.pus_time[1] = (uint8_t)(coarse_time >> 16);
        pus_c.pus_time[2] = (uint8_t)(coarse_time >> 8);
        pus_c.pus_time[3] = (uint8_t)(coarse_time & 0xFF);

        /*
                Updating SATLL Onboard Time, which is send as TM[3,25] SID1 data
        */
        tm_3_25_1.SECOND = tm_3_25_1.SECOND + 1;
        if(tm_3_25_1.SECOND == 60){
                tm_3_25_1.SECOND = 0;
                tm_3_25_1.MINUTE = tm_3_25_1.MINUTE + 1;
                if(tm_3_25_1.MINUTE == 60){
                        tm_3_25_1.MINUTE = 0;
                        tm_3_25_1.HOUR = tm_3_25_1.HOUR + 1;
                        if(tm_3_25_1.HOUR == 24){
                                tm_3_25_1.HOUR = 0;
                                tm_3_25_1.DAY = tm_3_25_1.DAY + 1;
                        }
                }
        }
        previousSecondUpdate = timeSinceBoot;
        } else {

        }
  
}


void update_telemetry(void){
        if((timeSinceBoot - previousTelemetryUpdate) >= TelemetryUpdate_Interval ){  
                /*
                  Only updating TM[3,25] SID_1 Telemetry Data
                */    
                update_time();
                //Update CMD counter

                previousTelemetryUpdate = timeSinceBoot;
        } 
}


// TM[1,1] successful acceptance verification report
uint8_t generate_tm_1_1(uint8_t *in){
        uint8_t Service_Type = 1;
        uint8_t Sub_Type = 1;

        uint16_t i = 0; // Index counter
        tm_1_1.Packet_ID = (uint16_t)(in[i++] << 8) | in[i++];
        tm_1_1.Sequence_Control = (uint16_t)(in[i++] << 8) | in[i++];

        uint16_t position_counter = 0;
        //Bulid PUS header/Frame
        buildPUSHeader(&pus_c, 2, 1, Service_Type, Sub_Type, COUNTER_1_1, 0, pus_c.pus_pfield, pus_c.pus_time, PusFrameOut);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_1.Packet_ID >> 8);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_1.Packet_ID & 0xFF);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_1.Sequence_Control >> 8);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_1.Sequence_Control & 0xFF);
        
        // Build the PUS frame
        PusFrameLength = buildPUSFrame(&pus_c, position_counter ,PusFrameOut);

        //Update Space Packet Header
        buildSPHeader(&space_packet, 0, TELEMETRY, 1, space_packet.APID, 3, sp_packet_count, 0, SPPacketOut);
        //Put the pus packet into the sp data field
        for(position_counter = 0; position_counter < PusFrameLength; position_counter++){
                space_packet.data[position_counter] = PusFrameOut[position_counter];
        }
        space_packet.PacketDataLength = (position_counter + 2) - 1; // (Data length + CRC-16) - 1
        SPPacketLength = buildSPPacket(&space_packet, SPPacketOut);

        sp_packet_count++;
        COUNTER_1_1++;
        return 1;
}

// TM[1,2] failed acceptance verification report
uint8_t generate_tm_1_2(uint8_t *in){
        uint8_t Service_Type = 1;
        uint8_t Sub_Type = 2;

        uint16_t i = 0; // Index counter
        tm_1_2.Packet_ID = (uint16_t)(in[i++] << 8) | in[i++];
        tm_1_2.Sequence_Control = (uint16_t)(in[i++] << 8) | in[i++];
        tm_1_2.failure_code = 1; //In accordance to the MBEP ICD
        tm_1_2.failure_data = 0x0000;

        uint16_t position_counter = 0;
        //Manually Adding Subtype here - need to change
        //Bulid PUS header/Frame
        buildPUSHeader(&pus_c, 2, 1, Service_Type, Sub_Type, COUNTER_1_2, 0, pus_c.pus_pfield, pus_c.pus_time, PusFrameOut);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.Packet_ID >> 8);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.Packet_ID & 0xFF);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.Sequence_Control >> 8);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.Sequence_Control & 0xFF);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.failure_code >> 8);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.failure_code & 0xFF);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.failure_data >> 8);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.failure_data & 0xFF);
        // Build the PUS frame
        PusFrameLength = buildPUSFrame(&pus_c, position_counter ,PusFrameOut);
        
        //Update Space Packet Header
        buildSPHeader(&space_packet, 0, TELEMETRY, 1, space_packet.APID, 3, sp_packet_count, 0, SPPacketOut);
        //Put the pus packet into the sp data field
        for(position_counter = 0; position_counter < PusFrameLength; position_counter++){
                space_packet.data[position_counter] = PusFrameOut[position_counter];
        }
        space_packet.PacketDataLength = (position_counter + 2) - 1; // (Data length + CRC-16) - 1
        SPPacketLength = buildSPPacket(&space_packet, SPPacketOut);

        sp_packet_count++;
        COUNTER_1_2++;
}


// TM[1,7] successful completion of execution verification report
uint8_t generate_tm_1_7(uint8_t *in){
        uint8_t Service_Type = 1;
        uint8_t Sub_Type = 7;

        uint16_t i = 0; // Index counter
        tm_1_2.Packet_ID = (uint16_t)(in[i++] << 8) | in[i++];
        tm_1_2.Sequence_Control = (uint16_t)(in[i++] << 8) | in[i++];
        tm_1_2.failure_code = 1; //In accordance to the MBEP ICD
        tm_1_2.failure_data = 0x0000;

        uint16_t position_counter = 0;
        //Manually Adding Subtype here - need to change
        //Bulid PUS header/Frame
        buildPUSHeader(&pus_c, 2, 1, Service_Type, Sub_Type, COUNTER_1_2, 0, pus_c.pus_pfield, pus_c.pus_time, PusFrameOut);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.Packet_ID >> 8);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.Packet_ID & 0xFF);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.Sequence_Control >> 8);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.Sequence_Control & 0xFF);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.failure_code >> 8);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.failure_code & 0xFF);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.failure_data >> 8);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.failure_data & 0xFF);
        // Build the PUS frame
        PusFrameLength = buildPUSFrame(&pus_c, position_counter ,PusFrameOut);
        
        //Update Space Packet Header
        buildSPHeader(&space_packet, 0, TELEMETRY, 1, space_packet.APID, 3, sp_packet_count, 0, SPPacketOut);
        //Put the pus packet into the sp data field
        for(position_counter = 0; position_counter < PusFrameLength; position_counter++){
                space_packet.data[position_counter] = PusFrameOut[position_counter];
        }
        space_packet.PacketDataLength = (position_counter + 2) - 1; // (Data length + CRC-16) - 1
        SPPacketLength = buildSPPacket(&space_packet, SPPacketOut);
        
        sp_packet_count++;
        COUNTER_1_7++;

}

// TM[1,8] failed completion of execution verification report
uint8_t generate_tm_1_8(uint8_t *in){
        uint8_t Service_Type = 1;
        uint8_t Sub_Type = 8;

        uint16_t i = 0; // Index counter
        tm_1_2.Packet_ID = (uint16_t)(in[i++] << 8) | in[i++];
        tm_1_2.Sequence_Control = (uint16_t)(in[i++] << 8) | in[i++];
        tm_1_2.failure_code = 1; //In accordance to the MBEP ICD
        tm_1_2.failure_data = 0x0000;

        uint16_t position_counter = 0;
        //Manually Adding Subtype here - need to change
        //Bulid PUS header/Frame
        buildPUSHeader(&pus_c, 2, 1, Service_Type, Sub_Type, COUNTER_1_2, 0, pus_c.pus_pfield, pus_c.pus_time, PusFrameOut);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.Packet_ID >> 8);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.Packet_ID & 0xFF);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.Sequence_Control >> 8);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.Sequence_Control & 0xFF);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.failure_code >> 8);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.failure_code & 0xFF);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.failure_data >> 8);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_1_2.failure_data & 0xFF);
        // Build the PUS frame
        PusFrameLength = buildPUSFrame(&pus_c, position_counter ,PusFrameOut);
        
        //Update Space Packet Header
        buildSPHeader(&space_packet, 0, TELEMETRY, 1, space_packet.APID, 3, sp_packet_count, 0, SPPacketOut);
        //Put the pus packet into the sp data field
        for(position_counter = 0; position_counter < PusFrameLength; position_counter++){
                space_packet.data[position_counter] = PusFrameOut[position_counter];
        }
        space_packet.PacketDataLength = (position_counter + 2) - 1; // (Data length + CRC-16) - 1
        SPPacketLength = buildSPPacket(&space_packet, SPPacketOut);
        
        sp_packet_count++;
        COUNTER_1_8++;
}




// TM[3,25] Housekeeping parameter report
uint16_t generate_tm_3_25(uint8_t SID){
        uint8_t Service_Type = 3;
        uint8_t Sub_Type = 25;
    switch(SID){
        case 1:
                uint16_t position_counter = 0;
                buildPUSHeader(&pus_c, 2, 1, Service_Type, Sub_Type, COUNTER_3_25, 0, pus_c.pus_pfield, pus_c.pus_time, PusFrameOut);

                // pus_c.pus_data[position_counter++] = (uint8_t)(tm_3_25_1.SID >> 8);
                pus_c.pus_data[position_counter++] = (uint8_t)(tm_3_25_1.SID & 0xFF);

                pus_c.pus_data[position_counter++] = (uint8_t)(tm_3_25_1.FSW_DB_VER >> 8);
                pus_c.pus_data[position_counter++] = (uint8_t)(tm_3_25_1.FSW_DB_VER & 0xFF);
                
                pus_c.pus_data[position_counter++] = (uint8_t)(tm_3_25_1.DAY >> 8);
                pus_c.pus_data[position_counter++] = (uint8_t)(tm_3_25_1.DAY & 0xFF);

                pus_c.pus_data[position_counter++] = (uint8_t)(tm_3_25_1.HOUR >> 8);
                pus_c.pus_data[position_counter++] = (uint8_t)(tm_3_25_1.HOUR & 0xFF);

                pus_c.pus_data[position_counter++] = (uint8_t)(tm_3_25_1.MINUTE >> 8);
                pus_c.pus_data[position_counter++] = (uint8_t)(tm_3_25_1.MINUTE & 0xFF);

                pus_c.pus_data[position_counter++] = (uint8_t)(tm_3_25_1.SECOND >> 8);
                pus_c.pus_data[position_counter++] = (uint8_t)(tm_3_25_1.SECOND & 0xFF);

                pus_c.pus_data[position_counter++] = (uint8_t)(tm_3_25_1.SC_ID >> 8);
                pus_c.pus_data[position_counter++] = (uint8_t)(tm_3_25_1.SC_ID & 0xFF);

                pus_c.pus_data[position_counter++] = (uint8_t)(tm_3_25_1.CMD_RCV_COUNT >> 8);
                pus_c.pus_data[position_counter++] = (uint8_t)(tm_3_25_1.CMD_RCV_COUNT & 0xFF);

                pus_c.pus_data[position_counter++] = (uint8_t)(tm_3_25_1.CMD_ACC_CNT >> 8);
                pus_c.pus_data[position_counter++] = (uint8_t)(tm_3_25_1.CMD_ACC_CNT & 0xFF);

                // Build the PUS frame
                PusFrameLength = buildPUSFrame(&pus_c, position_counter ,PusFrameOut);
                
                //Update Space Packet Header
                buildSPHeader(&space_packet, 0, TELEMETRY, 1, space_packet.APID, 3, sp_packet_count, 0, SPPacketOut);
                //Put the pus packet into the sp data field
                for(position_counter = 0; position_counter < PusFrameLength; position_counter++){
                        space_packet.data[position_counter] = PusFrameOut[position_counter];
                }

                //here add option for crc enabled/disabled check to decide to add +2 OR NOT
                space_packet.PacketDataLength = (position_counter + 2) - 1; // (Data length + CRC-16) - 1
                SPPacketLength = buildSPPacket(&space_packet, SPPacketOut);
                
                //Once Sent
                sp_packet_count++;
                COUNTER_3_25++;
                break;
        case 2:
                break;
        case 3:
                break;
        case 4:
                break;
        case 5: 
                break;
        case 6:
                break;
        default:
            break;
    }

}

// TM[5,1] informative event report
uint8_t generate_tm_5_1(uint8_t *in){
        uint8_t Service_Type = 5;
        uint8_t Sub_Type = 1;

        uint16_t i = 0; // Index counter

        tm_5_1.Event_ID = (uint16_t)(in[i++] << 8) | in[i++];
       // tm_5_1.Event_ID = 1; // As per ICD
        tm_5_1.auxilary_data = (uint16_t)(in[i++] << 8) | in[i++]; // The system turned on

        //PUS
        uint16_t position_counter = 0;
        buildPUSHeader(&pus_c, 2, 1, Service_Type, Sub_Type, COUNTER_5_1, 0, pus_c.pus_pfield, pus_c.pus_time, PusFrameOut);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_5_1.Event_ID >> 8);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_5_1.Event_ID & 0xFF);

        pus_c.pus_data[position_counter++] = (uint8_t)(tm_5_1.auxilary_data >> 8);
        pus_c.pus_data[position_counter++] = (uint8_t)(tm_5_1.auxilary_data & 0xFF);
        // Build the PUS frame
        PusFrameLength = buildPUSFrame(&pus_c, position_counter ,PusFrameOut);

        //Update Space Packet Header
        buildSPHeader(&space_packet, 0, TELEMETRY, 1, space_packet.APID, 3, sp_packet_count, 0, SPPacketOut);
        //Put the pus packet into the sp data field
        for(position_counter = 0; position_counter < PusFrameLength; position_counter++){
                space_packet.data[position_counter] = PusFrameOut[position_counter];
        }
        space_packet.PacketDataLength = (position_counter + 2) - 1; // (Data length + CRC-16) - 1
        SPPacketLength = buildSPPacket(&space_packet, SPPacketOut);


        sp_packet_count++;
        COUNTER_5_1++;
        return 1;
}

// TM[5,2] low severity anomaly report
// TM[5,3] medium severity anomaly report
// TM[5,4] high severity anomaly report

// TM[9,2] CUC time report
uint8_t generate_tm_9_2(uint8_t *pfield_in , uint8_t *in){
        uint8_t Service_Type = 9;
        uint8_t Sub_Type = 2;

        uint16_t i = 0; // Index counter
        tm_9_2.pField = pfield_in[i];
        tm_9_2.spacecraft_coarse_time_size = ((tm_9_2.pField >> 2) & 0x03) + 1;
        tm_9_2.spacecraft_fine_time_size = (tm_9_2.pField & 0x03);
                //PUS
        uint16_t position_counter = 0;
        buildPUSHeader(&pus_c, 2, 1, Service_Type, Sub_Type, COUNTER_9_2, 0, pus_c.pus_pfield, pus_c.pus_time, PusFrameOut);
        
        for (position_counter = 0; i< tm_9_2.spacecraft_coarse_time_size; position_counter++){
                pus_c.pus_data[position_counter] = (uint8_t)(in[position_counter]);
        }
        for (position_counter; i< tm_9_2.spacecraft_fine_time_size; position_counter++){
                pus_c.pus_data[position_counter] = (uint8_t)(in[position_counter]);
        }
        // Build the PUS frame
        PusFrameLength = buildPUSFrame(&pus_c, position_counter ,PusFrameOut);
        //Update Space Packet Header
        buildSPHeader(&space_packet, 0, TELEMETRY, 1, space_packet.APID, 3, sp_packet_count, 0, SPPacketOut);
        //Put the pus packet into the sp data field
        for(position_counter = 0; position_counter < PusFrameLength; position_counter++){
                space_packet.data[position_counter] = PusFrameOut[position_counter];
        }
        space_packet.PacketDataLength = (position_counter + 2) - 1; // (Data length + CRC-16) - 1
        SPPacketLength = buildSPPacket(&space_packet, SPPacketOut);

        sp_packet_count++;
        COUNTER_9_2++;
}

//TM[17,2] are-you-alive connection test report
uint8_t generate_tm_17_2(void){
        uint8_t Service_Type = 17;
        uint8_t Sub_Type = 2;
        //PUS
        uint16_t position_counter = 0;
        buildPUSHeader(&pus_c, 2, 1, Service_Type, Sub_Type, COUNTER_17_2, 0, pus_c.pus_pfield, pus_c.pus_time, PusFrameOut);
        
        // Build the PUS frame
        position_counter = 0;
        PusFrameLength = buildPUSFrame(&pus_c, position_counter ,PusFrameOut);
        //Update Space Packet Header
        buildSPHeader(&space_packet, 0, TELEMETRY, 1, space_packet.APID, 3, sp_packet_count, 0, SPPacketOut);
        //Put the pus packet into the sp data field
        for(position_counter = 0; position_counter < PusFrameLength; position_counter++){
                space_packet.data[position_counter] = PusFrameOut[position_counter];
        }
        space_packet.PacketDataLength = (position_counter + 2) - 1; // (Data length + CRC-16) - 1
        SPPacketLength = buildSPPacket(&space_packet, SPPacketOut);


        sp_packet_count++;
        COUNTER_17_2++;
}
