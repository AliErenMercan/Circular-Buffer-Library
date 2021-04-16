/*
 * circularBuffer.h
 *
 *  Created on: Oct 30, 2020
 *      Author: Ali Eren Mercan
 */

/*
 *
 *		  ~~~~~USAGE EXAMPLE~~~~~
 * <--------------Macros-------------->
 *	#define RX_SIZE 20
 *	#define CB_SIZE 64
 *	#define DATA_SIZE 10
 *	#define HEADER_SIZE 2
 *
 * <------------Definitions----------->
 *  CB_Prop CB;
 *	uint8_t header[2]={4,5};
 *	uint8_t arrangedcsRovData[DATA_SIZE];
 *	uint8_t csRovRx[RX_SIZE];
 *
 *	<------------Setup----------->
 *	CBInit(&CB, CB_SIZE);
 *
 *	<------------Function getting arranged data----------->
 *	GetRxData(&CB, csRovRx, arrangedcsRovData, DATA_SIZE, RX_SIZE, header, HEADER_SIZE); // return status (0 or 1);
 */

#ifndef INC_CIRCULARBUFFER_H_
#define INC_CIRCULARBUFFER_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <main.h>

typedef struct{
	size_t numOfElements;
	uint8_t *buffer;
	uint8_t indexOfThisBuffer;
}CB_Prop;
#define CIRCULAR_BUFFER_MAX 10


typedef struct {
	size_t numOfElements;
	uint8_t* CB;
	volatile size_t head;
	volatile size_t tail;
} _CB_Struct;

union crcConverter{
	uint32_t fullData;
	uint8_t bytes[4];
}crcConverter;

void CBInit(CB_Prop *CB_Prop,size_t numOfElements);
uint8_t GetRxData( CB_Prop *CB_Prop, uint8_t* src, uint8_t* dst, size_t srcSize, size_t rxSize, uint8_t* pHeader, uint8_t headerSize);

#endif /* INC_CIRCULARBUFFER_H_ */
