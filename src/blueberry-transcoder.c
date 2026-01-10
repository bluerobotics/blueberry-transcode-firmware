/*
Copyright (c) 2024 Blue Robotics Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


//*******************************************************************************************
//Includes
//*******************************************************************************************
#include <blueberry-transcoder.h>

#include <crc1021.h>
//*******************************************************************************************
//Defines
//*******************************************************************************************
#define SEQUENCE_PLACEHOLDER_BLOCK_INDEX 0
#define SEQUENCE_PLACEHOLDER_ELEMENT_LENGTH_INDEX 2
#define SEQUENCE_BLOCK_ELEMENTS_NUM_INDEX 0
#define SEQUENCE_BLOCK_DATA_START_INDEX 4

//*******************************************************************************************
//Types
//*******************************************************************************************

//*******************************************************************************************
//Variables
//*******************************************************************************************

//*******************************************************************************************
//Function Prototypes
//*******************************************************************************************
/**
 * takes the specified index and aligns it to a 4-byte boundary
 */
static BbBlock align(uint32_t i);
//*******************************************************************************************
//Code
//*******************************************************************************************

/**
 *  gets an 8-bit, unsigned integer from the specified block
 *  @param buf the buffer to read
 *  @param block the block offset in bytes
 *  @param i the index offset in bytes
 *  @return the value
 */
uint8_t getBbUint8(Bb* buf, BbBlock block, uint32_t i){
	return buf->buffer[bbWrap(buf, block + i)];
}

/**
 * sets an 8-bit, unsigned integer in the specified block
 *  @param buf the buffer to read
 *  @param block the block offset in bytes
 *  @param i the index offset in bytes
 *  @param v the value to write
 */
void setBbUint8(Bb* buf, BbBlock block, uint32_t i, uint8_t v){
	buf->buffer[bbWrap(buf, block + i)] = v;
}

/**
 * gets an 8-bit, signed integer from thespecified block
 *  @param buf the buffer to read
 *  @param block the block offset in bytes
 *  @param i the index offset in bytes
 *  @return the value
 */
int8_t getBbInt8(Bb* buf, BbBlock block, uint32_t i){
	return (int8_t)getBbUint8(buf, block, i);
}

/**
 * sets an 8-bit signed integer in the specified block
 *  @param buf the buffer to read
 *  @param block the block offset in bytes
 *  @param i the index offset in bytes
 *  @param v the value to write
 */
void setBbInt8(Bb* buf, BbBlock block, uint32_t i, int8_t v){
	setBbUint8(buf, block, i, (uint8_t)v);
}


/**
 *  gets a 16-bit, unsigned integer from the specified block
 *  @param buf the buffer to read
 *  @param block the block offset in bytes
 *  @param i the index offset in bytes
 *  @return the value
 */
uint16_t getBbUint16(Bb* buf, BbBlock block, uint32_t i){
	uint16_t result;
	uint8_t* bs = (uint8_t*)&result;
	bs[0] = getBbUint8(buf, block, i);
	bs[1] = getBbUint8(buf, block, i+1);
	return result;
}

/**
 * sets a 16-bit, unsigned integer in the specified block
 *  @param buf the buffer to read
 *  @param block the block offset in bytes
 *  @param i the index offset in bytes
 *  @param v the value to write
 */
void setBbUint16(Bb* buf, BbBlock block, uint32_t i, uint16_t v){
	uint8_t* bs = (uint8_t*)&v;
	setBbUint8(buf, block, i,   bs[0]);
	setBbUint8(buf, block, i+1, bs[1]);
}

/**
 * gets a 16-bit, signed integer from thespecified block
 *  @param buf the buffer to read
 *  @param block the block offset in bytes
 *  @param i the index offset in bytes
 *  @return the value
 */
int16_t getBbInt16(Bb* buf, BbBlock block, uint32_t i){
	return (int16_t)getBbUint16(buf, block, i);
}

/**
 * sets a 16-bit signed integer in the specified block
 *  @param buf the buffer to read
 *  @param block the block offset in bytes
 *  @param i the index offset in bytes
 *  @param v the value to write
 */
void setBbInt16(Bb* buf, BbBlock block, uint32_t i, int16_t v){
	setBbUint16(buf, block, i, (uint16_t)v);
}

/**
 *  gets a 32-bit, unsigned integer from the specified block
 *  @param buf the buffer to read
 *  @param block the block offset in bytes
 *  @param i the index offset in bytes
 *  @return the value
 */
uint32_t getBbUint32(Bb* buf, BbBlock block, uint32_t i){
	uint32_t result;
	uint8_t* bs = (uint8_t*)&result;
	bs[0] = getBbUint8(buf, block, i);
	bs[1] = getBbUint8(buf, block, i+1);
	bs[2] = getBbUint8(buf, block, i+2);
	bs[3] = getBbUint8(buf, block, i+3);
	return result;
}

/**
 * sets a 32-bit, unsigned integer in the specified block
 *  @param buf the buffer to read
 *  @param block the block offset in bytes
 *  @param i the index offset in bytes
 *  @param v the value to write
 */
void setBbUint32(Bb* buf, BbBlock block, uint32_t i, uint32_t v){
	uint8_t* bs = (uint8_t*)&v;
	setBbUint8(buf, block, i,   bs[0]);
	setBbUint8(buf, block, i+1, bs[1]);
	setBbUint8(buf, block, i+2, bs[2]);
	setBbUint8(buf, block, i+3, bs[3]);

}

/**
 * gets a 32-bit, signed integer from thespecified block
 *  @param buf the buffer to read
 *  @param block the block offset in bytes
 *  @param i the index offset in bytes
 *  @return the value
 */
int32_t getBbInt32(Bb* buf, BbBlock block, uint32_t i){
	return (int16_t)getBbUint16(buf, block, i);

}

/**
 * sets a 32-bit signed integer in the specified block
 *  @param buf the buffer to read
 *  @param block the block offset in bytes
 *  @param i the index offset in bytes
 *  @param v the value to write
 */
void setBbInt32(Bb* buf, BbBlock block, uint32_t i, int32_t v){
	setBbUint32(buf, block, i, (uint32_t)v);

}

/**
 *  gets a 32-bit, floating point value from the specified block
 *  @param buf the buffer to read
 *  @param block the block offset in bytes
 *  @param i the index offset in bytes
 *  @return the value
 *
 */
float getBbFloat32(Bb* buf, BbBlock block, uint32_t i){
	uint32_t ip = getBbUint32(buf, block, i);
	float* fp = (float*)(&ip);
	return *fp;
}

/**
 * sets a a 32-bit, floating point value in the specified block
 *  @param buf the buffer to read
 *  @param block the block offset in bytes
 *  @param i the index offset in bytes
 *  @param v the value to write
 */
void setBbFloat32(Bb* buf, BbBlock block, uint32_t i, float v){
	float* fp = &v;
	uint32_t* ip = (uint32_t*)fp;
	setBbUint32(buf, block, i, *ip);
}

/**
 * extracts a boolean from the specified block
 *  @param buf the buffer to read
 *  @param block the block offset in bytes
 *  @param i the index offset in bytes
 *  @return the value
 */
bool getBbBool(Bb* buf, BbBlock block, uint32_t i, uint32_t bitNum){
	uint8_t bf = getBbUint8(buf, block, i);
	return (bf & (1<<bitNum)) != 0;
}

/**
 * sets a boolean in a specified block
 *  @param buf the buffer to read
 *  @param block the block offset in bytes
 *  @param i the index offset in bytes
 *  @param v the value to write
 */
void setBbBool(Bb* buf, BbBlock block, uint32_t i, uint32_t bitNum, bool v){
	uint8_t* b = &(buf->buffer[bbWrap(buf, block + i)]);
	uint8_t m = 1 << bitNum;
	if(v){
		*b |= m;
	} else {
		*b &= ~m;
	}
}

/**
 * converts a linear index to a circular one
 * essentially mods the index with the buffer size
 * @return the wrapped index
 */
uint32_t bbWrap(Bb* buf, int i){
	uint32_t j = i + buf->start;
	uint32_t n = buf->bufferLength;
	return j % n;
}
/**
 * computes the crc of the buffer
 * @param buf the buffer
 * @param block the first element
 * @param one past the last element
 */
uint16_t computeCrc(Bb* buf, BbBlock block, BbBlock end){

	uint16_t crc;
	resetCrc1021P(&crc);
	for(uint32_t i = block; i < end; i += 4){
		uint32_t w = getBbUint32(buf, i, 0);

		crc1021P32(&crc, w);
//		getCrc1021P(&crc);
	}

	//do from the start to either the buffer end or the block end
	getCrc1021P(&crc);
	return crc;
}


/**
 * Gets the index for the specified sequence element. this can be used to read or write from the specified sequence element
 * @param buf - the buffer containing the data packet, message, etc.
 * @param block - the beginning of the packet, message, sequence, etc. that we are currently interrogating
 * @param i - the index (in bytes) of the sequence placeholder (which consists of a an index to the sequence length field (uint16) and the element byte count (uint16))
 * @param sequenceElement - the index of the sequence element. This must be smaller than the sequence length - but this is not checked here.
 */
BbBlock getBbSequenceElementIndex(Bb* buf, BbBlock block, uint32_t i, uint32_t sequenceElement){
	//get byte number per element
	uint32_t bn = (uint32_t)getBbUint16(buf, block, i + SEQUENCE_PLACEHOLDER_ELEMENT_LENGTH_INDEX);
	//get the index of the block containing the sequence data
	BbBlock result = (BbBlock)getBbUint16(buf, block, i + SEQUENCE_PLACEHOLDER_BLOCK_INDEX);//this is the index of the sequence block
	//now add on the displacement into the sequence data of the desired element
	result += SEQUENCE_BLOCK_DATA_START_INDEX + sequenceElement*bn;

	return result;
}
/**
 * Initializes the sequence placeholder and sequence length with the required information
 * @param buf - the buffer containing the data packet, message, etc.
 * @param block - the beginning of the packet, message, sequence, etc. that we are currently interrogating
 * @param i - the index (in bytes) of the sequence message placeholder (which consists of a an index to the sequence length field (uint16) and the element byte count (uint16))
 * @param elementByteNum - the number of bytes used by each sequence element
 * @param elementNum - the number of elements of the sequence
 * @return the block in the buffer containing the sequence data

 */
BbBlock initBbSequence(Bb* buf, BbBlock block, uint32_t i, uint32_t elementByteNum, uint32_t elementNum){
	BbBlock result = align(buf->length);//the sequence data will be added to the current end of the buffer
	//record the block index
	setBbUint16(buf, block, i + SEQUENCE_PLACEHOLDER_BLOCK_INDEX, (uint16_t)sequenceBlock);
	setBbUint16(buf, block, i + SEQUENCE_PLACEHOLDER_ELEMENT_LENGTH_INDEX, (uint16_t)sequenceBlock);
	setBbUint32(buf, result, elementNum);//record the number of elements of this sequence
	//advance buffer by the size of the sequence block
	buf->length += 4 + (sequenceElement * elementByteNum);
	return result;
}

/**
 * Gets the specified element of the array, as a block index
 * @param buf - the buffer containing the data packet, message, etc.
 * @param block - the beginning of the packet, message, sequence, etc. that we are currently interrogating
 * @param i - the index (in bytes) of the start of the array (which corresponds to the first element)
 * @param arrayElement - the item of the array that we want
 * @param arrayElementLength - the length in bytes of each array element
 */
BbBlock getBbArrayElementIndex(Bb* buf, BbBlock block, uint32_t i, uint32_t arrayElement, uint32_t arrayElementLength){
	return block + i + arrayElement*arrayElementLength;
}

/**
 * takes the specified index and aligns it to a 4-byte boundary
 */
static BbBlock align(uint32_t i){
	BbBlock result = i;

	if(i & 0b11){
		result |= 0b11;
		result += 1;
	}
	return result;
}




