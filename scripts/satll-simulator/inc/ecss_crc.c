#include "ecss_crc.h"

uint16_t ecss_crc16(const uint8_t *data, uint16_t length){

    uint16_t crc_polynomial = 0x1021;
    uint16_t crc_shift = 0xFFFF; //ECSS Spec states initial value of 0xFFFF instead of 0

    uint32_t data_byte_counter = 0;
    for(data_byte_counter = 0; data_byte_counter < length; data_byte_counter++){
        crc_shift ^= (uint16_t)(data[data_byte_counter] << 8);  
        //set the crc shift register to the byte starting
        // Already XORs the incoming byte ontop of the crc register as progressing through the bytes - cool
        
        for (int i = 0; i < 8; i++){    
            if ((crc_shift & 0x8000) != 0){  // left most bit checks if = 1 for alignment
            /* most significant bit set, shift crc register and perform XOR operation */
                crc_shift = (uint16_t)((crc_shift << 1) ^ crc_polynomial);
            } else { /* most significant bit not set, go to next bit */
                crc_shift <<= 1;
            }
        }
    }
    return crc_shift;
}