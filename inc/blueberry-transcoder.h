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
#include <stdint.h>
#include <stdbool.h>
//*******************************************************************************************
//Defines
//*******************************************************************************************

//*******************************************************************************************
//Types
//*******************************************************************************************
/**
 * defines all the info needed to begin parsing a packet
 */
typedef struct {
    uint8_t* buffer;//a pointer to an array of bytes (i.e. the buffer)
    uint32_t start;//the index of the first byte of the packet
    uint32_t length;//the length of the packet in 4-byte words
    uint32_t bufferLength;//the length of the buffer, assuming it's circular
} Bb;


typedef uint32_t BbBlock; //this is really an index into the buffer, although it is assumed to be linear, even if the buffer wraps partway through the packet. If this value is i and the packet length is n, then 0 <= i < n.
typedef uint32_t BbArray;

//*******************************************************************************************
//Variables
//*******************************************************************************************

//*******************************************************************************************
//Function Prototypes
//*******************************************************************************************


/**
 *  gets an 8-bit, unsigned integer from the specified block
 */
uint8_t getBbUint8(Bb* buf, BbBlock p, uint32_t i);

/**
 * sets an 8-bit, unsigned integer in the specified block
 */
void setBbUint8(Bb* buf, BbBlock p, uint32_t i, uint8_t v);

/**
 * gets an 8-bit, signed integer from thespecified block
 */
int8_t getBbInt8(Bb* buf, BbBlock p, uint32_t i);

/**
 * sets an 8-bit signed integer in the specified block
 */
void setBbInt8(Bb* buf, BbBlock p, uint32_t i, int8_t v);


/**
 *  gets a 16-bit, unsigned integer from the specified block
 */
uint16_t getBbUint16(Bb* buf, BbBlock p, uint32_t i);

/**
 * sets a 16-bit, unsigned integer in the specified block
 */
void setBbUint16(Bb* buf, BbBlock p, uint32_t i, uint16_t v);

/**
 * gets a 16-bit, signed integer from thespecified block
 */
int16_t getBbInt16(Bb* buf, BbBlock p, uint32_t i);

/**
 * sets a 16-bit signed integer in the specified block
 */
void setBbInt16(Bb* buf, BbBlock p, uint32_t i, int16_t v);

/**
 *  gets a 32-bit, unsigned integer from the specified block
 */
uint32_t getBbUint32(Bb* buf, BbBlock p, uint32_t i);

/**
 * sets a 32-bit, unsigned integer in the specified block
 */
void setBbUint32(Bb* buf, BbBlock p, uint32_t i, uint32_t v);

/**
 * gets a 32-bit, signed integer from thespecified block
 */
int32_t getBbInt32(Bb* buf, BbBlock p, uint32_t i);

/**
 * sets a 32-bit signed integer in the specified block
 */
void setBbInt32(Bb* buf, BbBlock p, uint32_t i, int32_t v);

/**
 *  gets a 32-bit, floating point value from the specified block
 */
float getBbFloat(Bb* buf, BbBlock p, uint32_t i);

/**
 * sets a a 32-bit, floating point value in the specified block
 */
void setBbFloat(Bb* buf, BbBlock p, uint32_t i, float v);

/**
 * extracts a boolean from the specified block
 */
bool getBbBool(Bb* buf, BbBlock p, uint32_t i, uint32_t bitNum);

/**
 * sets a boolean in a specified block
 */
void setBbBool(Bb* buf, BbBlock p, uint32_t i, uint32_t bitNum, bool v);

/**
 * converts a linear index to a circular one
 * essentially mods the index with the buffer size
 * @return the wrapped index
 */
uint32_t bbWrap(uint32_t i, uint32_t n);








//*******************************************************************************************
//Code
//*******************************************************************************************
