#include "telecommand.h"

TC_2_1 tc_2_1 = {0};
TC_2_7 tc_2_7 = {0};
TC_3_5 tc_3_5 = {0};
TC_3_6 tc_3_6 = {0};
TC_5_5 tc_5_5 = {0};
TC_5_6 tc_5_6 = {0};

uint8_t handle_sp(uint8_t *in, spPacket *out){
    uint16_t i = 0; // Index counter

    out->PacketVersionNumber = (in[i] >> 5) & 0x07;
    out->PacketType = (in[i] >> 4) & 0x01;

    // if (out->PacketType == 1){
    //     tm_3_25_1.CMD_RCV_COUNT++;
    // }

    out->SecondaryHeaderFlag = (in[i] >> 3) & 0x01;
    out->APID = ((in[i++] & 0x07) << 8 ) | in[i++];
    out->sequenceFlags = (in[i] >> 6) & 0x03;
    out->PacketSequenceCount = ((in[i++] & 0x3F) << 8 ) | in[i++];
    out->PacketDataLength = (in[i++] << 8) | in[i++];

    int j = 0;
    for(j = 0; j < (out->PacketDataLength - 1); j++){ // (- 1 is removing the header and crc length)
        out->data[j] = in[i++];
    }

    out->SpacePacketCRC16 = (in[i++] << 8) | in[i++];
    if(ecss_crc16(in , out->PacketDataLength + 7) == 0 ){
        // printf(" %04X " , ecss_crc16(in , out->PacketDataLength + 5));
        printf("Correct SP CRC \n");
    } else {
        printf("Incorrect SP CRC\n");
    }

}

uint8_t handle_pus_tc(uint8_t *in, pusTCPacket *out){
    uint16_t i = 0; // Index counter

    out->PUS_VersionNumber = (in[i] >> 4) & 0x0F;
    /*
        Temp counter Fix as YAMCS SP TC Header Incorrect 
    */
    if (out->PUS_VersionNumber == 1){
        tm_3_25_1.CMD_RCV_COUNT++;
    }

    //Ack Flags
    out->acknowledgement_flags = in[i++] & 0x0F;
    //Bit 1
    out->ack_flag.successful_acceptance_request = (out->acknowledgement_flags >> 3) & 0x01;
    //Bit 2
    out->ack_flag.successful_start_request = (out->acknowledgement_flags >> 2) & 0x01;
    //Bit 3
    out->ack_flag.successful_progress_request = (out->acknowledgement_flags >> 1) & 0x01;
    //Bit 4
    out->ack_flag.successful_completion_request = (out->acknowledgement_flags) & 0x01;

    out->service_type_ID = in[i++];
    out->message_subtype_ID = in[i++];
    out->source_ID = (in[i++] << 8) | in[i++];

    printf("Telecommand Type %d \n" , out->service_type_ID);
    printf("Telecommand Type %d \n" , out->message_subtype_ID);

    //The length of the data is dependant upon the type and subtype
    switch(out->service_type_ID){
        case 2: //TC[2,X]
            switch(out->message_subtype_ID){
                case 1: //TC[2,1]
                    out->tc_data_length = 4 + 6;
                    // tc_2_1.N = (in[i++] << 8) | in[i++];
                    // tc_2_1.Address = (in[i++] << 8) | in[i++];
                    // tc_2_1.Value = (in[i++] << 8) | in[i++];
                    
                    tc_2_1.N = in[i++];
                    tc_2_1.Address = (in[i++] << 24) | (in[i++] << 16) | (in[i++] << 8) | in[i++];
                    
                    switch(tc_2_1.Address){
                        case 1:         //ADCS_PWR
                                // TM[1,1] successful acceptance verification report
                                if(out->ack_flag.successful_acceptance_request){
                                    printf("TM[1,1] successful acceptance verification report\n");
                                    //generate_tm_1_1(tc_frame.data);
                                    //send_udp(&udp, TMFrameOut, TMFrameLength);
                                }
                                tm_3_25_1.CMD_ACC_CNT++;
                                //TM[1,3] needs to be sent  
                                //TM[1,5] needs to be sent 
                                    
                                printf("ADCS Power Toggle \n");

                                //TM[1,7] successful completion of execution verification report
                                if(out->ack_flag.successful_completion_request){
                                    printf("TM[1,7] successful completion of execution verification report\n");
                                    // generate_tm_1_7(tc_frame.data);
                                    // send_udp(&udp, TMFrameOut, TMFrameLength);
                                }
                            break;
                        case 2:         //EO_CAMERA
                            break;
                        case 3:         //LED TEST
                            switch(tc_2_1.Value){
                                case 0:
                                    // TM[1,1] successful acceptance verification report
                                    if(out->ack_flag.successful_acceptance_request){
                                        printf("TM[1,1] successful acceptance verification report\n");
                                        //generate_tm_1_1(tc_frame.data);
                                        //send_udp(&udp, TMFrameOut, TMFrameLength);
                                    }
                                    tm_3_25_1.CMD_ACC_CNT++;
                                    //TM[1,3] needs to be sent  
                                    //TM[1,5] needs to be sent 
                                    
                                    printf("Turning LED OFF \n");

                                    //TM[1,7] successful completion of execution verification report
                                    if(out->ack_flag.successful_completion_request){
                                        printf("TM[1,7] successful completion of execution verification report\n");
                                        // generate_tm_1_7(tc_frame.data);
                                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                                    }
                                    break;
                                case 1:
                                    // TM[1,1] successful acceptance verification report
                                    if(out->ack_flag.successful_acceptance_request){
                                        printf("TM[1,1] successful acceptance verification report\n");
                                        // generate_tm_1_1(tc_frame.data);
                                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                                    }
                                    tm_3_25_1.CMD_ACC_CNT++;
                                    //TM[1,3] needs to be sent  
                                    //TM[1,5] needs to be sent  

                                    printf("Turning LED ON \n");

                                    //TM[1,7] successful completion of execution verification report
                                    if(out->ack_flag.successful_completion_request){
                                        printf("TM[1,7] successful completion of execution verification report\n");
                                        // generate_tm_1_7(tc_frame.data);
                                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                                    }
                                    break;
                                default:
                                    printf("Incorrect LED State \n");

                                    //TM[1,2] failed acceptance verification report
                                    if(out->ack_flag.successful_acceptance_request){
                                        printf("TM[1,2] failed acceptance verification report\n");
                                        // generate_tm_1_2(tc_frame.data);
                                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                                    }
                                    //TM[1,8] failed completion of execution verification report
                                    if(out->ack_flag.successful_completion_request){
                                        printf("TM[1,8] failed completion of execution verification report\n");
                                        // generate_tm_1_8(tc_frame.data);
                                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                                    }
                                    break;
                            }
                            break;
                        default:
                            //TM[1,2] failed acceptance verification report
                            if(out->ack_flag.successful_acceptance_request){
                                printf("TM[1,2] failed acceptance verification report\n");
                                // generate_tm_1_2(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }
                            // TM[1,8] failed completion of execution verification report
                            if(out->ack_flag.successful_completion_request){
                                printf("TM[1,8] failed completion of execution verification report\n");
                                // generate_tm_1_8(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }
                            break;
                    }
                    break;
                case 7: //TC[2,7]
                    // N field
                    tc_2_7.N = (in[i++] << 8) | in[i++];
                    // Physical Device ID
                    tc_2_7.DeviceID = in[i++];
                    // protocol-specific data
                    tc_2_7.protocol_specific_data = in[i++];
                    //command data    
                    tc_2_7.commandData = (in[i++] << 8) | in[i++];
        
                    switch(tc_2_7.N){
                        case 1:
                            switch(tc_2_7.DeviceID){    //CAN-TS Address
                                case 0x02 : //PAYLOAD

                                    switch(tc_2_7.protocol_specific_data){
                                        case 0: //Take a Photo
                                            // TM[1,1] successful acceptance verification report
                                            if(out->ack_flag.successful_acceptance_request){
                                                printf("TM[1,1] successful acceptance verification report\n");
                                                // generate_tm_1_1(tc_frame.data);
                                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                                            }
                                            tm_3_25_1.CMD_ACC_CNT++;

                                            printf("Capturing Image");
                                            
                                            //TM[1,7] successful completion of execution verification report
                                            if(out->ack_flag.successful_completion_request){
                                                printf("TM[1,7] successful completion of execution verification report\n");
                                                // generate_tm_1_7(tc_frame.data);
                                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                                            }
                                            break;

                                        default:
                                            printf("Error Unsupported PAYLOAD Command");
                                            //TM[1,2] failed acceptance verification report
                                            if(out->ack_flag.successful_acceptance_request){
                                                printf("TM[1,2] failed acceptance verification report\n");
                                                // generate_tm_1_2(tc_frame.data);
                                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                                            }
                                            // TM[1,8] failed completion of execution verification report
                                            if(out->ack_flag.successful_completion_request){
                                                printf("TM[1,8] failed completion of execution verification report\n");
                                                // generate_tm_1_8(tc_frame.data);
                                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                                            }
                                            break;
                                    }

                                    break;
                                case 0x05 : //COMMS
                                    switch(tc_2_7.protocol_specific_data){
                                        case 0: // Set Frequency
                                        // TM[1,1] successful acceptance verification report
                                            if(out->ack_flag.successful_acceptance_request){
                                                printf("TM[1,1] successful acceptance verification report\n");
                                                // generate_tm_1_1(tc_frame.data);
                                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                                            }
                                            tm_3_25_1.CMD_ACC_CNT++;

                                            float frequency = 864 + ((float)tc_2_7.commandData) * ((float)0.001);
                                            printf("Setting Frequency: %.3f" , frequency);
                                            
                                            //TM[1,7] successful completion of execution verification report
                                            if(out->ack_flag.successful_completion_request){
                                                printf("TM[1,7] successful completion of execution verification report\n");
                                                // generate_tm_1_7(tc_frame.data);
                                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                                            }
                                            break;
                                        case 1: // Set Transmit Power
                                            break;
                                        case 2: // Set Modem Config
                                            break;    

                                        default:
                                            printf("Error Unsupported COMMS Command");
                                            //TM[1,2] failed acceptance verification report
                                            if(out->ack_flag.successful_acceptance_request){
                                                printf("TM[1,2] failed acceptance verification report\n");
                                                // generate_tm_1_2(tc_frame.data);
                                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                                            }
                                            // TM[1,8] failed completion of execution verification report
                                            if(out->ack_flag.successful_completion_request){
                                                printf("TM[1,8] failed completion of execution verification report\n");
                                                // generate_tm_1_8(tc_frame.data);
                                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                                            }
                                            break;
                                    }    
                                    break;
                                case 0x07 : //ADCS
                                    //CAN-TS channel is the tc_2_7.protocol_specific_data
                                    switch(tc_2_7.protocol_specific_data){
                                        case 0: //Mode Change
                                            switch(tc_2_7.commandData){
                                                case 0: // Switch ADCS Mode to IDLE
                                                    // TM[1,1] successful acceptance verification report
                                                    if(out->ack_flag.successful_acceptance_request){
                                                        printf("TM[1,1] successful acceptance verification report\n");
                                                        // generate_tm_1_1(tc_frame.data);
                                                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                                                    }
                                                    tm_3_25_1.CMD_ACC_CNT++;

                                                    printf("Changing ADCS Mode to IDLE");
                                                    
                                                    // TM[5,1] informative event report
                                                    if(tm_5_1.event_enabled){
                                                        //tm_5_1 data
                                                        //Event ID - Misison Defined
                                                        tm_5_1.data[0] = 0x00;
                                                        tm_5_1.data[1] = 0x01;

                                                        tm_5_1.data[2] = (uint8_t)(tc_2_7.commandData >> 8);
                                                        tm_5_1.data[3] = (uint8_t)(tc_2_7.commandData & 0xFF);

                                                        printf("TM[5,1] informative event report\n");
                                                        // generate_tm_5_1(tm_5_1.data);
                                                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                                                    }
                                                        

                                                    //TM[1,7] successful completion of execution verification report
                                                    if(out->ack_flag.successful_completion_request){
                                                        printf("TM[1,7] successful completion of execution verification report\n");
                                                        // generate_tm_1_7(tc_frame.data);
                                                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                                                    }
                                                    break;
                                                case 1: //Switch ADCS Mode to DETUMBLE
                                                    printf("Changing ADCS Mode to DETUMBLE");
                                                    // TM[1,1] successful acceptance verification report
                                                    if(out->ack_flag.successful_acceptance_request){
                                                        printf("TM[1,1] successful acceptance verification report\n");
                                                        // generate_tm_1_1(tc_frame.data);
                                                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                                                    }
                                                    tm_3_25_1.CMD_ACC_CNT++;

                                                    // TM[5,1] informative event report
                                                    if(tm_5_1.event_enabled){
                                                    //tm_5_1 data
                                                    //Event ID - Misison Defined
                                                    tm_5_1.data[0] = 0x00;
                                                    tm_5_1.data[1] = 0x01;

                                                    tm_5_1.data[2] = (uint8_t)(tc_2_7.commandData >> 8);
                                                    tm_5_1.data[3] = (uint8_t)(tc_2_7.commandData & 0xFF);

                                                    //Generate TM[5,1]
                                                    printf("TM[5,1] informative event report\n");
                                                    // generate_tm_5_1(tm_5_1.data);
                                                    // send_udp(&udp, TMFrameOut, TMFrameLength);
                                                    }

                                                    //TM[1,7] successful completion of execution verification report
                                                    if(out->ack_flag.successful_completion_request){
                                                        printf("TM[1,7] successful completion of execution verification report\n");
                                                        // generate_tm_1_7(tc_frame.data);
                                                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                                                    }
                                                    break;
                                                case 2: //Switch ADCS Mode to TRACK
                                                    printf("Changing ADCS Mode to TRACK");
                                                    // TM[1,1] successful acceptance verification report
                                                    if(out->ack_flag.successful_acceptance_request){
                                                        printf("TM[1,1] successful acceptance verification report\n");
                                                        // generate_tm_1_1(tc_frame.data);
                                                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                                                    }
                                                    tm_3_25_1.CMD_ACC_CNT++;

                                                    if(tm_5_1.event_enabled){
                                                        //tm_5_1 data
                                                        //Event ID - Misison Defined
                                                        tm_5_1.data[0] = 0x00;
                                                        tm_5_1.data[1] = 0x01;

                                                        tm_5_1.data[2] = (uint8_t)(tc_2_7.commandData >> 8);
                                                        tm_5_1.data[3] = (uint8_t)(tc_2_7.commandData & 0xFF);

                                                        //Generate TM[5,1]
                                                        printf("TM[5,1] informative event report\n");
                                                        // generate_tm_5_1(tm_5_1.data);
                                                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                                                    }
                                                    //TM[1,7] successful completion of execution verification report
                                                    if(out->ack_flag.successful_completion_request){
                                                        printf("TM[1,7] successful completion of execution verification report\n");
                                                        // generate_tm_1_7(tc_frame.data);
                                                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                                                    }
                                                    
                                                    break;
                                                case 3: //Switch ADCS Mode to MANUAL
                                                    printf("Changing ADCS Mode to MANUAL");
                                                    // TM[1,1] successful acceptance verification report
                                                    if(out->ack_flag.successful_acceptance_request){
                                                        printf("TM[1,1] successful acceptance verification report\n");
                                                        // generate_tm_1_1(tc_frame.data);
                                                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                                                    }
                                                    tm_3_25_1.CMD_ACC_CNT++;

                                                    // TM[5,1] informative event report
                                                    if(tm_5_1.event_enabled){
                                                        //tm_5_1 data
                                                        //Event ID - Misison Defined
                                                        tm_5_1.data[0] = 0x00;
                                                        tm_5_1.data[1] = 0x01;

                                                        tm_5_1.data[2] = (uint8_t)(tc_2_7.commandData >> 8);
                                                        tm_5_1.data[3] = (uint8_t)(tc_2_7.commandData & 0xFF);

                                                        //Generate TM[5,1]
                                                        printf("TM[5,1] informative event report\n");
                                                        // generate_tm_5_1(tm_5_1.data);
                                                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                                                    }

                                                    //TM[1,7] successful completion of execution verification report
                                                    if(out->ack_flag.successful_completion_request){
                                                        printf("TM[1,7] successful completion of execution verification report\n");
                                                        // generate_tm_1_7(tc_frame.data);
                                                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                                                    }
                                                    break;
                                                default:
                                                    printf("Error Unsupported ADCS Mode");
                                                    //TM[1,2] failed acceptance verification report
                                                    if(out->ack_flag.successful_acceptance_request){
                                                        printf("TM[1,2] failed acceptance verification report\n");
                                                        // generate_tm_1_2(tc_frame.data);
                                                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                                                    }
                                                    // TM[1,8] failed completion of execution verification report
                                                    if(out->ack_flag.successful_completion_request){
                                                        printf("TM[1,8] failed completion of execution verification report\n");
                                                        // generate_tm_1_8(tc_frame.data);
                                                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                                                    }
                                                    break;
                                            }
                                        case 2: //Sets The X axis Magnetometer ON or OFF
                                            break;
                                        case 3: //Sets The Y axis Magnetometer ON or OFF
                                            break;
                                        case 4: //Sets The Z axis Magnetometer ON or OFF
                                            break;
                                        case 6: //Sets the Kp value
                                            break;
                                        case 7: //Sets the Kd value
                                            break;
                                        case 8: //Wheel Speed
                                            break;

                                        default:
                                        printf("Error Unsupported ADCS Command");
                                            //TM[1,2] failed acceptance verification report
                                            if(out->ack_flag.successful_acceptance_request){
                                                printf("TM[1,2] failed acceptance verification report\n");
                                                // generate_tm_1_2(tc_frame.data);
                                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                                            }
                                            // TM[1,8] failed completion of execution verification report
                                            if(out->ack_flag.successful_completion_request){
                                                printf("TM[1,8] failed completion of execution verification report\n");
                                                // generate_tm_1_8(tc_frame.data);
                                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                                            }
                                            break;
                                            //break;
                                    }

                                    //CAN-TS data is the tc_2_7.commandData

                                    break;
                                default:
                                    printf("Unsupported Device ID");
                                    break;

                            }

                            break;
                        default:
                            //TM[1,2] failed acceptance verification report
                            if(out->ack_flag.successful_acceptance_request){
                                printf("TM[1,2] failed acceptance verification report\n");
                                // generate_tm_1_2(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }
                            // TM[1,8] failed completion of execution verification report
                            if(out->ack_flag.successful_completion_request){
                                printf("TM[1,8] failed completion of execution verification report\n");
                                // generate_tm_1_8(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }
                            break;
                    }       
                    
                default:
                    printf("Unsupported Telecommand Subtype  %d \n" , out->message_subtype_ID);
                    //TM[1,2] failed acceptance verification report
                    if(out->ack_flag.successful_acceptance_request){
                        printf("TM[1,2] failed acceptance verification report\n");
                        // generate_tm_1_2(tc_frame.data);
                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                    }
                    // TM[1,8] failed completion of execution verification report
                    if(out->ack_flag.successful_completion_request){
                        printf("TM[1,8] failed completion of execution verification report\n");
                        // generate_tm_1_8(tc_frame.data);
                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                    }
                    break;
            }
            break;
        case 3: //TC[3,X]
            switch(out->message_subtype_ID){
                case 5: // TC[3,5] enable the periodic generation of housekeeping parameter reports.
                    tc_3_5.N = (in[i++] << 8) | in[i++];
                    // Physical Device ID
                    tc_3_5.housekeeping_SID = (in[i++] << 8) | in[i++];

                    //The 3,25 SIDs
                    switch(tc_3_5.housekeeping_SID){
                            case 1: //SID 1
                            // TM[1,1] successful acceptance verification report
                            if(out->ack_flag.successful_acceptance_request){
                                printf("TM[1,1] successful acceptance verification report\n");
                                // generate_tm_1_1(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }
                            tm_3_25_1.CMD_ACC_CNT++;

                            tm_3_25_1.event_enabled = 1;   //ENABLE
                            
                            //TM[1,7] successful completion of execution verification report
                            if(out->ack_flag.successful_completion_request){
                                printf("TM[1,7] successful completion of execution verification report\n");
                                // generate_tm_1_7(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }

                        default:
                            printf("Unsupported SID");
                            //TM[1,2] failed acceptance verification report
                            if(out->ack_flag.successful_acceptance_request){
                                printf("TM[1,2] failed acceptance verification report\n");
                                // generate_tm_1_2(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }
                            // TM[1,8] failed completion of execution verification report
                            if(out->ack_flag.successful_completion_request){
                                printf("TM[1,8] failed completion of execution verification report\n");
                                // generate_tm_1_8(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }
                            break;

                    }
                case 6: //TC[3,6]
                    //The 3,25 SIDs
                    switch(tc_3_5.housekeeping_SID){
                        case 1:
                            // TM[1,1] successful acceptance verification report
                            if(out->ack_flag.successful_acceptance_request){
                                printf("TM[1,1] successful acceptance verification report\n");
                                // generate_tm_1_1(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }
                            tm_3_25_1.CMD_ACC_CNT++;

                            tm_3_25_1.event_enabled = 0;   //DISABLE
                            
                            // TM[1,7] successful completion of execution verification report
                            if(out->ack_flag.successful_completion_request){
                                printf("TM[1,7] successful completion of execution verification report\n");
                                // generate_tm_1_7(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }

                            break;
                        default:
                            printf("Unsupported SID");
                                                        //TM[1,2] failed acceptance verification report
                            if(out->ack_flag.successful_acceptance_request){
                                printf("TM[1,2] failed acceptance verification report\n");
                                // generate_tm_1_2(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }
                            // TM[1,8] failed completion of execution verification report
                            if(out->ack_flag.successful_completion_request){
                                printf("TM[1,8] failed completion of execution verification report\n");
                                // generate_tm_1_8(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }
                            break;
                    }
                    break;    

                default:
                    printf("Unsupported Subtype");
                    //TM[1,2] failed acceptance verification report
                    if(out->ack_flag.successful_acceptance_request){
                        printf("TM[1,2] failed acceptance verification report\n");
                        // generate_tm_1_2(tc_frame.data);
                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                    }
                    // TM[1,8] failed completion of execution verification report
                    if(out->ack_flag.successful_completion_request){
                        printf("TM[1,8] failed completion of execution verification report\n");
                        // generate_tm_1_8(tc_frame.data);
                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                    }
                    break;
            }

            break;
        case 5: //TC[5,X] Events
            switch(out->message_subtype_ID){
                case 5: //TC[5,5] enable the report generation of event definitions
                    // N field
                    tc_5_5.N = (in[i++] << 8) | in[i++];
                    // Event Definition ID
                    tc_5_5.event_definition_ID = (in[i++] << 8) | in[i++];

                    //Switch the event ID to enable or disable a flag
                    switch(tc_5_5.event_definition_ID){
                        case 1: //Mission defined ADCS Mode Change
                            // TM[1,1] successful acceptance verification report
                            if(out->ack_flag.successful_acceptance_request){
                                printf("TM[1,1] successful acceptance verification report\n");
                                // generate_tm_1_1(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }
                            tm_3_25_1.CMD_ACC_CNT++;

                            tm_5_1.event_enabled = 1;   //ENABLE

                            //TM[1,7] successful completion of execution verification report
                            if(out->ack_flag.successful_completion_request){
                                printf("TM[1,7] successful completion of execution verification report\n");
                                // generate_tm_1_7(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }

                            break;

                        default:
                            printf("Undefined Event ID\n");
                            //TM[1,2] failed acceptance verification report
                            if(out->ack_flag.successful_acceptance_request){
                                printf("TM[1,2] failed acceptance verification report\n");
                                // generate_tm_1_2(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }
                            // TM[1,8] failed completion of execution verification report
                            if(out->ack_flag.successful_completion_request){
                                printf("TM[1,8] failed completion of execution verification report\n");
                                // generate_tm_1_8(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }
                            break;

                    }



                    break;
                case 6: //TC[5,6] disable the report generation of event definitions
                    // N field
                    tc_5_6.N = (in[i++] << 8) | in[i++];
                    // Event Definition ID
                    tc_5_6.event_definition_ID = (in[i++] << 8) | in[i++];

                    //Switch the event ID to enable or disable a flag
                    switch(tc_5_5.event_definition_ID){
                        case 1: //Mission defined ADCS Mode Change
                            // TM[1,1] successful acceptance verification report
                            if(out->ack_flag.successful_acceptance_request){
                                printf("TM[1,1] successful acceptance verification report\n");
                                // generate_tm_1_1(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }
                            tm_3_25_1.CMD_ACC_CNT++;
                            
                            tm_5_1.event_enabled = 0;   //DISABLE

                            //TM[1,7] successful completion of execution verification report
                            if(out->ack_flag.successful_completion_request){
                                printf("TM[1,7] successful completion of execution verification report\n");
                                // generate_tm_1_7(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }
                            break;

                        default:
                            printf("Undefined Event ID\n");
                            //TM[1,2] failed acceptance verification report
                            if(out->ack_flag.successful_acceptance_request){
                                printf("TM[1,2] failed acceptance verification report\n");
                                // generate_tm_1_2(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }
                            // TM[1,8] failed completion of execution verification report
                            if(out->ack_flag.successful_completion_request){
                                printf("TM[1,8] failed completion of execution verification report\n");
                                // generate_tm_1_8(tc_frame.data);
                                // send_udp(&udp, TMFrameOut, TMFrameLength);
                            }
                            break;

                    }
                    
                    break;

                default:
                    printf("Unsupported Subtype");
                    //TM[1,2] failed acceptance verification report
                    if(out->ack_flag.successful_acceptance_request){
                        printf("TM[1,2] failed acceptance verification report\n");
                        // generate_tm_1_2(tc_frame.data);
                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                    }
                    // TM[1,8] failed completion of execution verification report
                    if(out->ack_flag.successful_completion_request){
                        printf("TM[1,8] failed completion of execution verification report\n");
                        // generate_tm_1_8(tc_frame.data);
                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                    }
                    break;
            }
            break;
        case 17:
            switch(out->message_subtype_ID){
                case 1: // TC[17,1]
                    // TM[1,1] successful acceptance verification report
                    if(out->ack_flag.successful_acceptance_request){
                        printf("TM[1,1] successful acceptance verification report\n");
                        // generate_tm_1_1(tc_frame.data);
                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                    }
                    tm_3_25_1.CMD_ACC_CNT++;

                    // generate_tm_17_2();
                    // send_udp(&udp, TMFrameOut, TMFrameLength);

                    //TM[1,7] successful completion of execution verification report
                    if(out->ack_flag.successful_completion_request){
                        printf("TM[1,7] successful completion of execution verification report\n");
                        // generate_tm_1_7(tc_frame.data);
                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                    }
                    break;

                default:
                    printf("Unsupported Subtype");
                    //TM[1,2] failed acceptance verification report
                    if(out->ack_flag.successful_acceptance_request){
                        printf("TM[1,2] failed acceptance verification report\n");
                        // generate_tm_1_2(tc_frame.data);
                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                    }
                    // TM[1,8] failed completion of execution verification report
                    if(out->ack_flag.successful_completion_request){
                        printf("TM[1,8] failed completion of execution verification report\n");
                        // generate_tm_1_8(tc_frame.data);
                        // send_udp(&udp, TMFrameOut, TMFrameLength);
                    }
                    break;
            }
            break;
        case 23:
            // switch(){

            //     default:
            //         break;
            // }

            break;                

        default:
            printf("Unsupported Telecommand Type %d \n" , out->service_type_ID);
            //TM[1,2] failed acceptance verification report
            if(out->ack_flag.successful_acceptance_request){
                printf("TM[1,2] failed acceptance verification report\n");
                // generate_tm_1_2(tc_frame.data);
                // send_udp(&udp, TMFrameOut, TMFrameLength);
            }
            //TM[1,8] failed completion of execution verification report
            if(out->ack_flag.successful_completion_request){
                printf("TM[1,8] failed completion of execution verification report\n");
                // generate_tm_1_8(tc_frame.data);
                // send_udp(&udp, TMFrameOut, TMFrameLength);
            }
            break;
    }

}
