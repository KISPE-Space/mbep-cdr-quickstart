#include "space_packet.h"
#include "ecss_crc.h"
#include <stdio.h>
#include <stdint.h>

//Version 1.1

spPacket space_packet ={0};
spPacket space_packet_tc = {0};

uint16_t buildSPHeader(spPacket *in, uint8_t PacketVersionNumber, uint8_t PacketType, uint8_t SecondaryHeaderFlag, uint16_t APID, uint8_t sequenceFlags, uint16_t PacketSequenceCount, uint16_t PacketDataLength, uint8_t *PusFrameOutput){
    in->PacketVersionNumber = PacketVersionNumber;
    in->PacketType = PacketType;
    in->SecondaryHeaderFlag = SecondaryHeaderFlag;
    in->APID = APID;
    in->sequenceFlags = sequenceFlags;
    in->PacketSequenceCount = PacketSequenceCount;

    return 0;
}

uint16_t buildSPPacket(const spPacket *in, uint8_t *out){

    uint16_t i = 0; // Index counter

    uint16_t SPPacketID = 0;
    //Need to cast uint8s to uint16s as shiffting out of the size of 8 bits
    SPPacketID = (((uint16_t)in->PacketVersionNumber & 0x07) << 13) | (((uint16_t)in->PacketType & 0x01) << 12) | (((uint16_t)in->SecondaryHeaderFlag & 0x01) << 11) | ((uint16_t)in->APID & 0x07FFu); 

    // Found from https://subethasoftware.com/2014/12/16/splitting-a-16-bit-value-to-two-8-bit-values-in-c/
    out[i++] = *((uint8_t*)&(SPPacketID)+1);
    out[i++] = *((uint8_t*)&(SPPacketID)+0);

    uint16_t PacketSequenceControl = 0;
    PacketSequenceControl = (((uint16_t)in->sequenceFlags & 0x03) << 14) | (((uint16_t)in->PacketSequenceCount & 0x3FFF));     


    out[i++] = *((uint8_t*)&(PacketSequenceControl) + 1);
    out[i++] = *((uint8_t*)&(PacketSequenceControl) + 0);

    //Packet length - 1
    out[i++] = *((uint8_t*)&(in->PacketDataLength) + 1);
    out[i++] = *((uint8_t*)&(in->PacketDataLength) + 0);

    uint8_t data_byte_counter = 0;
    for(data_byte_counter = 0; data_byte_counter < (in->PacketDataLength - 1); data_byte_counter++){ //in->PacketDataLength - 1 as (packetlength + crc) + 1 fills the sp packet length field but not th elnegth ver which the crc is applied
        out[i++] = in->data[data_byte_counter];
    }
    // printf("SP Issue: Length %02X \n" , (in->PacketDataLength - 1));

    space_packet.SpacePacketCRC16 = ecss_crc16(out, i);
    out[i++] = (uint8_t)(space_packet.SpacePacketCRC16 >> 8);
    out[i++] = (uint8_t)(space_packet.SpacePacketCRC16 & 0xFF);

    return i;
}


// uint8_t handle_sp(uint8_t *in, spPacket *out){
//     uint16_t i = 0; // Index counter

//     out->PacketVersionNumber = (in[i] >> 5) & 0x07;
//     out->PacketType = (in[i] >> 4) & 0x01;
//     out->SecondaryHeaderFlag = (in[i] >> 3) & 0x01;
//     out->APID = ((in[i++] & 0x07) << 8 ) | in[i++];
//     out->sequenceFlags = (in[i] >> 6) & 0x03;
//     out->PacketSequenceCount = ((in[i++] & 0x3F) << 8 ) | in[i++];
//     out->PacketDataLength = (in[i++] << 8) | in[i++];

//     int j = 0;
//     for(j = 0; j < (out->PacketDataLength - 1); j++){ // (- 1 is removing the header and crc length)
//         out->data[j] = in[i++];
//     }

//     out->SpacePacketCRC16 = (in[i++] << 8) | in[i++];
//     if(ecss_crc16(in , out->PacketDataLength + 7) == 0 ){
//         // printf(" %04X " , ecss_crc16(in , out->PacketDataLength + 5));
//         printf("Correct SP CRC \n");
//     } else {
//         printf("Incorrect SP CRC");
//     }

// }


