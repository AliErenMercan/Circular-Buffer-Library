/*
 * circularBuffer.h
 *
 *  Created on: Oct 30, 2020
 *      Author: Ali Eren Mercan
 */

#ifndef INC_CIRCULARBUFFER_H_
#define INC_CIRCULARBUFFER_H_

#include <stdint.h>
#include <stdio.h>

typedef struct{
	uint8_t numOfElements;
	uint8_t *buffer;
	uint8_t indexOfThisBuffer;
	uint8_t *header;
	uint8_t headersize;
}CB_Prop;
#define CIRCULAR_BUFFER_MAX 10


typedef struct {
	uint8_t numOfElements;
	uint8_t* CB;
	volatile uint8_t head;
	volatile uint8_t tail;
} _CB_Struct;

union crcConverter{
	uint32_t fullData;
	uint8_t bytes[4];
}crcConverter;

static uint8_t numOfCircularBuffers = 0;

static _CB_Struct _CircularBuffers[CIRCULAR_BUFFER_MAX];
void CBInit(CB_Prop *CB_Prop,size_t numOfElements);
uint8_t crcCheck(uint8_t *message, size_t l);
uint8_t GetRxData( CB_Prop *CB_Prop, uint8_t* src, uint8_t* dst, size_t srcSize, uint8_t* pHeader, uint8_t headerSize);
static uint8_t CircularBufferInit(CB_Prop *CB_Prop);
static uint8_t _IsCBFull(_CB_Struct *CBS);
static uint8_t _IsCBEmpty(_CB_Struct *CBS);
static uint8_t CBRead(CB_Prop *CB_Prop, const uint8_t *data);
static uint8_t CBWrite(CB_Prop *CB_Prop, uint8_t *data);
static uint8_t SearchForBeginning(CB_Prop *CB_Prop, uint8_t* pHeader, uint8_t headerSize, uint8_t srcSize);

#endif /* INC_CIRCULARBUFFER_H_ */
