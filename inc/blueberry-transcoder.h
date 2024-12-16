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
 * gets a single byte value from a block
 */
uint8_t getBbUint8(Bb* buf, BbBlock p, uint32_t i);
/**
 * sets a single byte value in a block
 */
void setBbUint8(Bb* buf, BbBlock p, uint32_t i, uint8_t v);

int8_t getBbInt8(Bb* buf, BbBlock p, uint32_t i);
void setBbInt8(Bb* buf, BbBlock p, uint32_t i, int8_t v);

uint16_t getBbUint8(Bb* buf, BbBlock p, uint32_t i);
void setBbUint8(Bb* buf, BbBlock p, uint32_t i, uint16_t v);

int16_t getBbInt8(Bb* buf, BbBlock p, uint32_t i);
void setBbInt8(Bb* buf, BbBlock p, uint32_t i, int16_t v);

uint32_t getBbUint8(Bb* buf, BbBlock p, uint32_t i);
void setBbUint8(Bb* buf, BbBlock p, uint32_t i, uint32_t v);

int32_t getBbInt8(Bb* buf, BbBlock p, uint32_t i);
void setBbInt8(Bb* buf, BbBlock p, uint32_t i, int32_t v);

float getBbUint8(Bb* buf, BbBlock p, uint32_t i);
void setBbUint8(Bb* buf, BbBlock p, uint32_t i, float v);

float getBbInt8(Bb* buf, BbBlock p, uint32_t i);
void setBbInt8(Bb* buf, BbBlock p, uint32_t i, float v);

float getBbBool(Bb* buf, BbBlock p, uint32_t i, uint32_t bitNum);
void setBbBool(Bb* buf, BbBlock p, uint32_t i, float v, uint32_t bitNum, bool v);

/**
 * converts a linear index to a circular one
 * essentially mods the index with the buffer size
 * @return the wrapped index
 */
uint32_t bbWrap(uint32_t i, uint32_t n);

/**
 * Computes the index of the next block in a packet.
 * Note that this does not wrap the index - it leaves it as if the buffer is linear.
 * This is for convenience of testing for bounds
 * @return the next block in the paycket
 */
Block getBbNextBlock(Bb* buf, BbBlock p);
/**
 * checks if there is another block in this packet given the last decoded block
 */

bool isBbNextBlock(Bb* buf, BbBlock p);






//*******************************************************************************************
//Code
//*******************************************************************************************
