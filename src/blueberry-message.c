/*
Copyright (c) 2026 Blue Robotics

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


//********************************************************************************
//includes
//********************************************************************************
#include <blueberry-message.h>

//********************************************************************************
//defines
//********************************************************************************
#define MODULE_MESSAGE_KEY_INDEX (0)
#define MESSAGE_LENGTH_INDEX (4)
#define MESSAGE_MAX_ORDINAL_INDEX (6)

#define MESSAGE_FIRST_DATA (8)

#define SEQUENCE_PLACEHOLDER_BLOCK_INDEX 0
#define SEQUENCE_PLACEHOLDER_ELEMENT_LENGTH_INDEX 2
#define SEQUENCE_BLOCK_ELEMENTS_NUM_INDEX 0
#define SEQUENCE_BLOCK_DATA_START_INDEX 4

//********************************************************************************
//types
//********************************************************************************

//********************************************************************************
//variables
//********************************************************************************

//********************************************************************************
//function prototypes
//********************************************************************************
/**
 * takes the specified index and aligns it to a 4-byte boundary
 */
static BbBlock align(uint32_t i);
//********************************************************************************
//code
//********************************************************************************

/**
 * gets the module/message key from the specified message
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 */
uint32_t getBbMessageKey(Bb* bb, BbBlock msg){
	return getBbUint32(bb, msg, MODULE_MESSAGE_KEY_INDEX);
}
/**
 * gets the max ordinal field from the specified message
 * This is the ordinal of the last field of the specified message.
 * This is useful to tell if the message was populated as expected or if it lacks a few fields, as might happen if it was constructed by an older version of the protocol
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 */
uint8_t getBbMessageMaxOrdinal(Bb* bb, BbBlock msg){
	return getBbUint8(bb, msg, MESSAGE_MAX_ORDINAL_INDEX);
}
/**
  * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 * gets the length of this message in bytes
 */
uint32_t getBbMessageLength(Bb* bb, BbBlock msg){
	return (uint32_t)getBbUint16(bb, msg, MESSAGE_LENGTH_INDEX);
}


/**
 * Gets the index for the specified sequence element. this can be used to read or write from the specified sequence element
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message * @param i - the index (in bytes) of the sequence placeholder (which consists of a an index to the sequence length field (uint16) and the element byte count (uint16))
 * @param sequenceElement - the index of the sequence element. This must be smaller than the sequence length - but this is not checked here.
 */
BbBlock getBbSequenceElementIndex(Bb* buf, BbBlock msg, uint32_t i, uint32_t sequenceElement){
	//get byte number per element
	uint32_t bn = (uint32_t)getBbUint16(buf, msg, i + SEQUENCE_PLACEHOLDER_ELEMENT_LENGTH_INDEX);
	//get the index of the block containing the sequence data
	BbBlock result = (BbBlock)getBbUint16(buf, msg, i + SEQUENCE_PLACEHOLDER_BLOCK_INDEX);//this is the index of the sequence block
	//now add on the displacement into the sequence data of the desired element
	result += SEQUENCE_BLOCK_DATA_START_INDEX + sequenceElement*bn;

	return result;
}
/**
 * Initializes the sequence placeholder and sequence length with the required information
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message * @param i - the index (in bytes) of the sequence message placeholder (which consists of a an index to the sequence length field (uint16) and the element byte count (uint16))
 * @param elementByteNum - the number of bytes used by each sequence element
 * @param elementNum - the number of elements of the sequence
 * @return the block in the buffer containing the sequence data

 */
BbBlock initBbSequence(Bb* buf, BbBlock msg, uint32_t i, uint32_t elementByteNum, uint32_t elementNum){
	BbBlock result = align(buf->length);//the sequence data will be added to the current end of the buffer
	//record the block index
	setBbUint16(buf, msg, i + SEQUENCE_PLACEHOLDER_BLOCK_INDEX, (uint16_t)result);
	setBbUint16(buf, msg, i + SEQUENCE_PLACEHOLDER_ELEMENT_LENGTH_INDEX, (uint16_t)elementByteNum);
	setBbUint32(buf, result, SEQUENCE_BLOCK_ELEMENTS_NUM_INDEX, elementNum);//record the number of elements of this sequence
	//advance buffer by the size of the sequence block
	buf->length += 4 + (elementNum * elementByteNum);
	return result;
}

/**
 * Gets the specified element of the array, as a block index
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 * @param i - the index (in bytes) of the start of the array (which corresponds to the first element)
 * @param arrayElement - the item of the array that we want
 * @param arrayElementLength - the length in bytes of each array element
 */
BbBlock getBbArrayElementIndex(Bb* buf, BbBlock msg, uint32_t i, uint32_t arrayElement, uint32_t arrayElementLength){
	return msg + i + arrayElement*arrayElementLength;
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
/**
 * Checks to see if this message contains data
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 *
 */
bool isBbMessageEmpty(Bb* buf, BbBlock msg){
	uint32_t len = getBbMessageLength(buf, msg);
	return len > MESSAGE_FIRST_DATA;
}
