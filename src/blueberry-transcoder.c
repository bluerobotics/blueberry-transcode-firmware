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

//*******************************************************************************************
//Types
//*******************************************************************************************

//*******************************************************************************************
//Variables
//*******************************************************************************************

//*******************************************************************************************
//Function Prototypes
//*******************************************************************************************

//*******************************************************************************************
//Code
//*******************************************************************************************

/**
 *  gets an 8-bit, unsigned integer from the specified block
 *  @param buf the buffer to read
 *  @param p the block offset
 *  @param i the index offset
 */
uint8_t getBbUint8(Bb* buf, BbBlock p, uint32_t i){
	return buf->buffer[bbWrap(buf, p + i)];
}

/**
 * sets an 8-bit, unsigned integer in the specified block
 */
void setBbUint8(Bb* buf, BbBlock p, uint32_t i, uint8_t v){
	buf->buffer[bbWrap(buf, p + i)] = v;
}

/**
 * gets an 8-bit, signed integer from thespecified block
 */
int8_t getBbInt8(Bb* buf, BbBlock p, uint32_t i){
	return (int8_t)getBbUint8(buf, p, i);
}

/**
 * sets an 8-bit signed integer in the specified block
 */
void setBbInt8(Bb* buf, BbBlock p, uint32_t i, int8_t v){
	setBbUint8(buf, p, i, (uint8_t)v);
}


/**
 *  gets a 16-bit, unsigned integer from the specified block
 */
uint16_t getBbUint16(Bb* buf, BbBlock p, uint32_t i){
	uint16_t result;
	uint8_t* bs = (uint8_t*)&result;
	bs[0] = getBbUint8(buf, p, i);
	bs[1] = getBbUint8(buf, p, i+1);
	return result;
}

/**
 * sets a 16-bit, unsigned integer in the specified block
 */
void setBbUint16(Bb* buf, BbBlock p, uint32_t i, uint16_t v){
	uint8_t* bs = (uint8_t*)&v;
	setBbUint8(buf, p, i,   bs[0]);
	setBbUint8(buf, p, i+1, bs[1]);
}

/**
 * gets a 16-bit, signed integer from thespecified block
 */
int16_t getBbInt16(Bb* buf, BbBlock p, uint32_t i){
	return (int16_t)getBbUint16(buf, p, i);
}

/**
 * sets a 16-bit signed integer in the specified block
 */
void setBbInt16(Bb* buf, BbBlock p, uint32_t i, int16_t v){
	setBbUint16(buf, p, i, (uint16_t)v);
}

/**
 *  gets a 32-bit, unsigned integer from the specified block
 */
uint32_t getBbUint32(Bb* buf, BbBlock p, uint32_t i){
	uint32_t result;
	uint8_t* bs = (uint8_t*)&result;
	bs[0] = getBbUint8(buf, p, i);
	bs[1] = getBbUint8(buf, p, i+1);
	bs[2] = getBbUint8(buf, p, i+2);
	bs[3] = getBbUint8(buf, p, i+3);
	return result;
}

/**
 * sets a 32-bit, unsigned integer in the specified block
 */
void setBbUint32(Bb* buf, BbBlock p, uint32_t i, uint32_t v){
	uint8_t* bs = (uint8_t*)&v;
	setBbUint8(buf, p, i,   bs[0]);
	setBbUint8(buf, p, i+1, bs[1]);
	setBbUint8(buf, p, i+2, bs[2]);
	setBbUint8(buf, p, i+3, bs[3]);

}

/**
 * gets a 32-bit, signed integer from thespecified block
 */
int32_t getBbInt32(Bb* buf, BbBlock p, uint32_t i){
	return (int16_t)getBbUint16(buf, p, i);

}

/**
 * sets a 32-bit signed integer in the specified block
 */
void setBbInt32(Bb* buf, BbBlock p, uint32_t i, int32_t v){
	setBbUint32(buf, p, i, (uint32_t)v);

}

/**
 *  gets a 32-bit, floating point value from the specified block
 */
float getBbFloat32(Bb* buf, BbBlock p, uint32_t i){
	uint32_t ip = getBbUint32(buf, p, i);
	float* fp = (float*)(&ip);
	return *fp;
}

/**
 * sets a a 32-bit, floating point value in the specified block
 */
void setBbFloat32(Bb* buf, BbBlock p, uint32_t i, float v){
	float* fp = &v;
	uint32_t* ip = (uint32_t*)fp;
	setBbUint32(buf, p, i, *ip);
}

/**
 * extracts a boolean from the specified block
 */
bool getBbBool(Bb* buf, BbBlock p, uint32_t i, uint32_t bitNum){
	uint8_t bf = getBbUint8(buf, p, i);
	return (bf & (1<<bitNum)) != 0;
}

/**
 * sets a boolean in a specified block
 */
void setBbBool(Bb* buf, BbBlock p, uint32_t i, uint32_t bitNum, bool v){
	uint8_t* b = &(buf->buffer[bbWrap(buf, p + i)]);
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
 * @param start the first element
 * @param one past the last element
 */
uint16_t computeCrc(Bb* buf, BbBlock start, BbBlock end){

	uint32_t i0, i1, i2, i3;
	i0 = buf->start + start;
	uint32_t n = buf->bufferLength;
	if(end < buf->start + start){//check if this buffer wraps
		i1 = n;
		i2 = 0;
		i3 = end;
	} else {
		i1 = end;
		i2 = 0;
		i3 = 0;
	}

	uint32_t* is = (uint32_t*)buf;
	uint16_t crc = 0xffff;
	//do from the start to either the buffer end or the block end
	for(uint32_t i = i0; i < i1; i += 4){
		crc1021P32(&crc, is[i]);
		getCrc1021P(&crc);
	}
	//now do from the buffer start to the block end, but only if the buffer has wrapped
	for(uint32_t i = i2; i < i3; i += 4){
		crc1021P32(&crc, is[i]);
		getCrc1021P(&crc);

	}
	return crc;
}

