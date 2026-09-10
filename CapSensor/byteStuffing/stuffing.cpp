/*
 * byteStuffing.c
 *
 *  Created on: 07/01/2020
 *      Author: RafaelPires
 */

#include <byteStuffing/stuffing.h>

uint16_t Stuffing::performPartialStuffing(uint8_t *crc, uint8_t * serialized, uint16_t serializedSize, uint8_t * stuffed) {

    uint16_t i;
    uint16_t offset = 0;
    uint16_t stuffedSize = serializedSize;

    /*Perform Byte Stuffing*/
    for (i = 0; i < serializedSize; i++) {
        *crc = (*crc) ^ (serialized[i]);
        if(serialized[i] == 0x7D || serialized[i] == 0x7E){
            stuffedSize++;
            stuffed[i+offset] = 0x7D;
            offset++;
            stuffed[i+offset] = (0x20) ^ (serialized[i]);
        }else{
            stuffed[i+offset] = serialized[i];
        }

    }

    return stuffedSize;

}

uint16_t Stuffing::performStuffing(uint8_t * serialized, uint16_t serializedSize, uint8_t * stuffed) {
    uint16_t offset = 0;
    uint16_t stuffedSize = serializedSize;
    uint8_t crc = 0;
    uint16_t i;

    /*Perform Byte Stuffing*/
    for(i = 0; i < serializedSize; i++) {

        if(i < (serializedSize-1)){
            crc = (crc) ^ (serialized[i]);
        }else{
            serialized[i] = crc;
        }
        stuffed[i+offset] = serialized[i];
        /*if(serialized[i] == 0x7D || serialized[i] == 0x7E){
            stuffedSize++;
            stuffed[i+offset] = 0x7D;
            offset++;
            stuffed[i+offset] = (0x20)^(serialized[i]);
        }else{
            stuffed[i+offset] = serialized[i];
        }*/

    }

    return stuffedSize;

}

void Stuffing::performUnstuffing(uint8_t * stuffed, int * stuffedSize,uint8_t * serializazed, int * serializedSize, uint8_t * crc){

   int i =0;
   int offset =0;
   *crc = 0;

   /*perform the unstuffing*/
   while(i< *stuffedSize) {

       if(stuffed[i] == 0x7D){
           offset++;
           i++;
           serializazed[i-offset] = stuffed[i]^0x20;
       }else{
           serializazed[i-offset] = stuffed[i];
       }

       if(i<(*stuffedSize-2)){
           *crc = (*crc ^ serializazed[i-offset]);
       }

       i++;
   }

   *serializedSize=i-offset;

}
