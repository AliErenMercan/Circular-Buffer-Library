/*
 * circularBuffer.c
 *
 *  Created on: Oct 30, 2020
 *      Author: Ali Eren Mercan
 */


#include <circularBuffer.h>

static uint8_t numOfCircularBuffers = 0;
static _CB_Struct _CircularBuffers[CIRCULAR_BUFFER_MAX];



static uint8_t CircularBufferInit(CB_Prop *CB_Prop){
	uint8_t idx = CB_Prop->indexOfThisBuffer;
    uint8_t err = 0;
    if ((idx < CIRCULAR_BUFFER_MAX) && (&(CB_Prop->indexOfThisBuffer) != NULL) && (CB_Prop != NULL)){
        if (CB_Prop->buffer != NULL){
            /* Check that the size of the ring buffer is a power of 2 */
            if (((CB_Prop->numOfElements - 1) & CB_Prop->numOfElements) == 0){
                /* Initialize the ring buffer internal variables */
                _CircularBuffers[CB_Prop->indexOfThisBuffer].head = 0;
                _CircularBuffers[CB_Prop->indexOfThisBuffer].tail = 0;
                _CircularBuffers[CB_Prop->indexOfThisBuffer].CB = CB_Prop->buffer;
                _CircularBuffers[CB_Prop->indexOfThisBuffer].numOfElements = CB_Prop->numOfElements;
                err= 1;
            }
        }
    }
    return err;
}

static uint8_t _IsCBFull(_CB_Struct *CBS){
    return ((CBS->head - CBS->tail) == CBS->numOfElements) ? 1 : 0;
}

static uint8_t _IsCBEmpty(_CB_Struct *CBS){
    return ((CBS->head - CBS->tail) == 0U) ? 1 : 0;
}

static uint8_t CBRead(CB_Prop *CB_Prop, const uint8_t *data){
    uint8_t err = 1;
    if ((CB_Prop->indexOfThisBuffer < CIRCULAR_BUFFER_MAX) && (_IsCBFull(&_CircularBuffers[CB_Prop->indexOfThisBuffer]) == 0)) {
        const size_t offset = _CircularBuffers[CB_Prop->indexOfThisBuffer].head & (_CircularBuffers[CB_Prop->indexOfThisBuffer].numOfElements - 1);
		_CircularBuffers[CB_Prop->indexOfThisBuffer].CB[offset] = data[0];
        _CircularBuffers[CB_Prop->indexOfThisBuffer].head++;
    } else {
        err = 0;
    }
    return err;
}

static uint8_t CBWrite(CB_Prop *CB_Prop, uint8_t *data){
    uint8_t err = 1;
    if ((CB_Prop->indexOfThisBuffer < CIRCULAR_BUFFER_MAX) && (_IsCBEmpty(&_CircularBuffers[CB_Prop->indexOfThisBuffer]) == 0)) {
        const size_t offset = _CircularBuffers[CB_Prop->indexOfThisBuffer].tail & (_CircularBuffers[CB_Prop->indexOfThisBuffer].numOfElements - 1);
		data[0] = _CircularBuffers[CB_Prop->indexOfThisBuffer].CB[offset];
        _CircularBuffers[CB_Prop->indexOfThisBuffer].tail++;
    } else {
        err = 0;
    }
    return err;
}

static uint8_t crcCheck(uint8_t *message, size_t l){
   uint32_t crc, msb;
   uint8_t err = 0;
   crc = 0xFFFFFFFF;

   for(uint8_t i = 0; i < 4; i++){
	   crcConverter.bytes[i] = message[l + i];
   }

   for(size_t i = 0; i < l; i++) {
      // xor next byte to upper bits of crc
      crc ^= (((uint32_t)message[i])<<24);
      for (uint8_t j = 0; j < 8; j++) {    // Do eight times.
            msb = crc>>31;
            crc <<= 1;
            crc ^= (0 - msb) & 0x04C11DB7;
      }
   }

   if(crcConverter.fullData == crc){
	   err = 1;
   }

   return err;
}

static uint8_t SearchForBeginning(CB_Prop *CB_Prop, uint8_t* pHeader, uint8_t headerSize, size_t srcSize){
	uint8_t err = 0;
	for(size_t i= 0; i<CB_Prop->numOfElements;i++){
		if(_CircularBuffers[CB_Prop->indexOfThisBuffer].CB[i] == pHeader[0]){
			uint8_t headerCounter = 1;
			while((headerCounter<headerSize) && (_CircularBuffers[CB_Prop->indexOfThisBuffer].CB[i + headerCounter] == pHeader[headerCounter])){
				headerCounter++;
			}
			if(headerCounter==headerSize && crcCheck(_CircularBuffers[CB_Prop->indexOfThisBuffer].CB +i,srcSize-4)){
				err = 1;
				_CircularBuffers[CB_Prop->indexOfThisBuffer].head = i + srcSize;
				_CircularBuffers[CB_Prop->indexOfThisBuffer].tail = i;
				return err;
			}
		}
	}
	return err;
}



void CBInit(CB_Prop *CB_Prop, size_t numOfElements){
	CB_Prop->numOfElements = numOfElements;
	CB_Prop->buffer = malloc(numOfElements * sizeof(uint8_t));
	CB_Prop->indexOfThisBuffer = numOfCircularBuffers;
	numOfCircularBuffers++;
	CircularBufferInit(CB_Prop);
}

uint8_t GetRxData(CB_Prop *CB_Prop, uint8_t* src, uint8_t* dst, size_t srcSize, size_t rxSize, uint8_t* pHeader,uint8_t headerSize){
	uint8_t err = 0;
	size_t currentIdxOfCB =0;
	while(CBRead(CB_Prop,&src[(currentIdxOfCB) % rxSize])){
		currentIdxOfCB++;
	}


	currentIdxOfCB = 0;
	if(headerSize!=0 && SearchForBeginning(CB_Prop, pHeader, headerSize, srcSize)){
		while(CBWrite(CB_Prop,&dst[currentIdxOfCB])){
			currentIdxOfCB++;
		}
		err=1;
	}
	else{
		_CircularBuffers[CB_Prop->indexOfThisBuffer].tail = _CircularBuffers[CB_Prop->indexOfThisBuffer].head;
	}

	return err;
}
