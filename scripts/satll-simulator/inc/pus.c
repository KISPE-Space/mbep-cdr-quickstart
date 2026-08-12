#include "pus.h"

//Version 1.0

uint8_t validService(uint8_t serviceTypeID);

char services[10] = {3};
char subservices[10] = {25};
pusPacket pus_c = {0};
pusTCPacket pus_c_tc = {0};


uint8_t setServiceTypeID(uint8_t serviceTypeID){

    // pus_c.service_type_ID = serviceTypeID;

    if ( validService(serviceTypeID)){
        return 1;
        pus_c.service_type_ID = serviceTypeID;
    } else {
        return 0;
    }
}

uint8_t validService(uint8_t serviceTypeID){

    uint32_t ServiceCheckCounter = 0;
    //Change this 25 to a number of implemented services size
    for(ServiceCheckCounter = 0; ServiceCheckCounter < 25; ServiceCheckCounter++){
        //create an array to hold the number of services implemented
        if( services[ServiceCheckCounter] == serviceTypeID){
            return 1;
        }
    }
    return 0;
}


uint8_t setMessageSubtypeID(uint8_t MessageTypeID){
    pus_c.message_subtype_ID = MessageTypeID;
    
    if ( validMessage(MessageTypeID)){
        return 1;
    } else {
        return 0;
    }
}

uint8_t validMessage(uint8_t MessageTypeID){

    // uint32_t ServiceCheckCounter = 0;
    // //Change this 25 to a number of implemented services size
    // for(ServiceCheckCounter = 0; ServiceCheckCounter < 25; ServiceCheckCounter++){
    //     //create an array to hold the number of services implemented
    //     if( services[ServiceCheckCounter] == serviceTypeID){
    //         return 1;
    //     }
    // }
    return 0;
}

uint8_t setCoarseTimeSize(uint8_t CoarseSize){
    if((CoarseSize > 0) && (CoarseSize <= MAX_COARSE_SIZE)){
        pus_c.pus_coarse_time_size = CoarseSize;
        return 1;
    } else {
        return 0;
    }   
}

uint8_t setFineTimeSize(uint8_t FineSize){
    if((FineSize > 0) && (FineSize <= MAX_FINE_SIZE)){
        pus_c.pus_fine_time_size = FineSize;
        return 1;
    } else {
        return 0;
    }    
}

uint8_t setEpoch(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minutes, uint8_t seconds){
    pus_c.epoch_day = day;
    pus_c.epoch_month = month;
    pus_c.epoch_year = year;
    
    return 0;
}

uint8_t buildpField(pusPacket *in, uint8_t pfield_extension, uint8_t TimeCodeIdentidification, uint8_t number_of_coarse_bytes, uint8_t number_of_fine_bytes){
    uint8_t pfield_Byte = 0;
    in->pField.pfield_extension = pfield_extension;
    in->pField.TimeCodeIdentidification = TimeCodeIdentidification;
    in->pField.number_of_coarse_bytes = number_of_coarse_bytes;
    in->pField.number_of_fine_bytes = number_of_fine_bytes;

    pfield_Byte = ((pfield_extension & 0x01) << 7) | ((TimeCodeIdentidification & 0x07) << 4) | (((number_of_coarse_bytes-1) & 0x03) << 2) | (number_of_fine_bytes & 0x03);
    
    return pfield_Byte; 
}

uint16_t buildPUSHeader(pusPacket *in, uint8_t VersionNumber, uint8_t time_reference, uint8_t serviceType, uint8_t subType, uint16_t subTypeCounter, uint16_t destinationID, uint8_t pField, uint8_t time[18], uint8_t *PusFrameOutput){
    in->PUS_VersionNumber = VersionNumber;
    in->spacecraft_time_reference_status = time_reference;
    // if(){

    // } else {
    //     //in->spacecraft_time_reference_status = INVALID;
    // }
    in->service_type_ID = serviceType;
    in->message_subtype_ID = subType;
    in->message_type_counter = subTypeCounter;
    in->destination_ID = destinationID;
    in->pus_pfield = pField;
    
    uint8_t time_byte_counter = 0;
    uint8_t time_in_bytes = 18;
    for(time_byte_counter = 0; time_byte_counter < time_in_bytes; time_byte_counter++){
        in->pus_time[time_byte_counter] = time[time_byte_counter];
    }

    //To improve
    return 0;
}


uint16_t buildPUSFrame(const pusPacket *in, uint16_t datalength , uint8_t *out){

    uint16_t i = 0; // Index counter

    uint8_t version_sctime_byte = 0;
    version_sctime_byte = ((in->PUS_VersionNumber & 0x0F) << 4) | (in->spacecraft_time_reference_status & 0x0F); // shift the lower 4 (0x0F) left << by 4
    out[i++] = version_sctime_byte;
    out[i++] = in->service_type_ID;
    out[i++] = in->message_subtype_ID;

    // Found from https://subethasoftware.com/2014/12/16/splitting-a-16-bit-value-to-two-8-bit-values-in-c/
    out[i++] = *((uint8_t*)&(in->message_type_counter)+1);
    out[i++] = *((uint8_t*)&(in->message_type_counter)+0);
    //out[i++] = in->message_type_counter;
    out[i++] = *((uint8_t*)&(in->destination_ID)+1);
    out[i++] = *((uint8_t*)&(in->destination_ID)+0);
    //out[i++] = in->destination_ID;

    //The pField struct needs to be converted to a single byte
    uint8_t in_pField = ((in->pField.pfield_extension & 0x01) << 7) | ((in->pField.TimeCodeIdentidification & 0x07) << 4) | ((in->pField.number_of_coarse_bytes & 0x03) << 2) | (in->pField.number_of_fine_bytes & 0x03);
    out[i++] = in->pus_pfield;  
    
    //need to loop for "j" for the length/size of the coarse and fine bytes
    uint8_t time_byte_counter = 0;
    for(time_byte_counter = 0; time_byte_counter < in->pField.number_of_coarse_bytes; time_byte_counter++){
        out[i++] = in->pus_time[time_byte_counter];
    }
    for(time_byte_counter = 0; time_byte_counter < in->pField.number_of_fine_bytes; time_byte_counter++){
        out[i++] = in->pus_time[i + in->pus_fine_time_size];
    }

    //Now data
    uint8_t data_length = datalength;// 16  + 2;
    uint16_t data_byte_counter = 0;
    
    for (data_byte_counter = 0; data_byte_counter < data_length; data_byte_counter++){
        out[i++] = pus_c.pus_data[data_byte_counter];
    }
    
    return i;
}


// uint8_t handle_pus_tc(uint8_t *in, pusTCPacket *out){
//     uint16_t i = 0; // Index counter

//     out->PUS_VersionNumber = (in[i] >> 4) & 0x0F;
//     //Ack Flags
//     out->acknowledgement_flags = in[i++] & 0x0F;
//     //Bit 1
//     out->ack_flag.successful_acceptance_request = (out->acknowledgement_flags >> 3) & 0x01;
//     //printf("successful_acceptance_request %d \n" , out->ack_flag.successful_acceptance_request);
//     //Bit 2
//     out->ack_flag.successful_start_request = (out->acknowledgement_flags >> 2) & 0x01;
//     //Bit 3
//     out->ack_flag.successful_progress_request = (out->acknowledgement_flags >> 1) & 0x01;
//     //Bit 4
//     out->ack_flag.successful_completion_request = (out->acknowledgement_flags) & 0x01;

//     out->service_type_ID = in[i++];
//     out->message_subtype_ID = in[i++];
//     out->source_ID = (in[i++] << 8) | in[i++];

//     printf("Telecommand Type %d \n" , out->service_type_ID);
//     printf("Telecommand Type %d \n" , out->message_subtype_ID);

//     //The length of the data is dependant upon the type and subtype
//     switch(out->service_type_ID){
//         case 2:
//             switch(out->message_subtype_ID){
//                 case 1: //TC[2,1]

//                     //Todo Move away from specific "tc_2_1. " as user may use different.... but for CDR is ok
//                     //Need to tidy code and move away from many nesteed cases, pull out into a function such as check_subtype() , check_servicetype() etc.

//                     out->tc_data_length = 4 + 6;
//                     tc_2_1.N = (in[i++] << 8) | in[i++];
//                     tc_2_1.Address = (in[i++] << 8) | in[i++];
//                     tc_2_1.Value = (in[i++] << 8) | in[i++];
//                     switch(tc_2_1.Address){
//                         case 1:         //ADCS_PWR
//                             break;
//                         case 2:         //EO_CAMERA
//                             break;
//                         case 3:         //LED TEST
//                             switch(tc_2_1.Value){
//                                 case 0:
//                                     //TM[1,1] needs to be sent
//                                     if(out->ack_flag.successful_acceptance_request){
//                                         printf("Successfull Acceptance Request");
//                                         generate_tm_1_1(in);
//                                     }
//                                     //TM[1,3] needs to be sent  
//                                     //TM[1,5] needs to be sent 
//                                     printf("Turning LED OFF \n");
//                                     //TM[1,7] needs to be sent
//                                     break;
//                                 case 1:
//                                     //TM[1,1] needs to be sent
//                                     //TM[1,1] needs to be sent
//                                     if(out->ack_flag.successful_acceptance_request){
//                                         printf("Successfull Acceptance Request\n"); //currently doing tm inside of pus - not what its intended for
//                                         //Now how to send over udp without having to import and use loads of udp crap
//                                         //This is a tailored version for the mbep cdr of these libs
//                                         generate_tm_1_1(tc_frame.data);
//                                         send_udp(&udp, TMFrameOut, TMFrameLength);
//                                     }
//                                     //TM[1,3] needs to be sent  
//                                     //TM[1,5] needs to be sent  
//                                     printf("Turning LED ON \n");
//                                     //TM[1,7] needs to be sent
//                                     break;
//                                 default:
//                                     printf("Incorrect LED State \n");
//                                     break;
//                             }
//                             break;
//                         default:
//                             break;
//                     }
//                     break;
//                 case 7: //TC[2,7]
//                     //out->tc_data_length = ;

//                     break;
//                 default:
//                     printf("Unsupported Telecommand Subtype  %d \n" , out->message_subtype_ID);
//                     break;
//             }
//             break;
//         case 3:
//             // switch(){

//             //     default:
//             //         break;
//             // }

//             break;
//         case 5:
//             // switch(){

//             //     default:
//             //         break;
//             // }

//             break;
//         case 17:
//             // switch(){

//             //     default:
//             //         break;
//             // }
//             break;
//         case 23:
//             // switch(){

//             //     default:
//             //         break;
//             // }

//             break;                

//         default:
//             printf("Unsupported Telecommand Type %d \n" , out->service_type_ID);
//             break;
//     }



    

// }

