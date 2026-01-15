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

#define STRING_PLACEHOLDER_BLOCK_INDEX 0
#define STRING_BLOCK_LENGTH_INDEX 0
#define STRING_BLOCK_DATA_START_INDEX 4




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
/**
 * updates the message length field fromm the buffer length
 * This is only used during message creation
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 */
static void updateBbMessageLength(Bb* bb, BbBlock msg);
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
 * gets the length of this message in bytes
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 */
uint32_t getBbMessageLength(Bb* bb, BbBlock msg){
	return (uint32_t)getBbUint16(bb, msg, MESSAGE_LENGTH_INDEX)*4;
}
/**
 * updates the message length field fromm the buffer length
 * This is only used during message creation
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 */
static void updateBbMessageLength(Bb* bb, BbBlock msg){
	setBbUint16(bb, msg, MESSAGE_LENGTH_INDEX, align((uint16_t)(bb->length - (uint32_t)msg))/4);
}


/**
 * Gets the index for the specified sequence element. , relative to the specified message.
 * This can be used to read or write from the specified sequence element
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 * @param i - the index (in bytes) of the sequence placeholder (which consists of a an index to the sequence length field (uint16) and the element byte count (uint16))
 * @param sequenceElement - the index of the sequence element. This must be smaller than the sequence length - but this is not checked here.
 */
BbBlock getBbSequenceElementIndex(Bb* buf, BbBlock msg, uint32_t i, uint32_t sequenceElement){
	//if index is invalid then return
	if(isBbIndexInvalid(i)){
		return i;
	}
	//get byte number per element
	uint32_t bn = (uint32_t)getBbUint16(buf, msg, i + SEQUENCE_PLACEHOLDER_ELEMENT_LENGTH_INDEX);
	//get the index of the block containing the sequence data
	BbBlock result = (BbBlock)getBbUint16(buf, msg, i + SEQUENCE_PLACEHOLDER_BLOCK_INDEX);//this is the index of the sequence block
	//now add on the displacement into the sequence data of the desired element
	result += SEQUENCE_BLOCK_DATA_START_INDEX + sequenceElement*bn;
	result -= msg;//subtract this so the result is still relative to the specified message

	return result;
}
/**
 * Gets the length of the specified sequence.
 * This can be used to read or write from the specified sequence element
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 * @param i - the index (in bytes) of the sequence placeholder (which consists of a an index to the sequence length field (uint16) and the element byte count (uint16))
 */
uint32_t getBbSequenceLength(Bb*buf, BbBlock msg, uint32_t i){
	//if index is invalid then return
	if(isBbIndexInvalid(i)){
		return 0;
	}
	//get the index of the block containing the sequence data
	BbBlock si = (BbBlock)getBbUint16(buf, msg, i + SEQUENCE_PLACEHOLDER_BLOCK_INDEX);//this is the index of the sequence block
	//now add on the displacement into the sequence data of the desired element
	return getBbUint32(buf, si, SEQUENCE_BLOCK_ELEMENTS_NUM_INDEX);
}

/**
 * copies a string from the specified message to the specified memory location
 * @param buf - the buffer containing the message
 * @param msg - the index of the message in the buffer
 * @param i - the index of the string placeholder within the message
 * @param dest - the location in memory to copy the string to
 * @param n - the maximum number of characters to copy.
 * @return the number of characters copied
 */
uint32_t copyBbStringFromMessage(Bb* buf, BbBlock msg, uint32_t i, char* dest, uint32_t n){
	//get the index of the block containing the string data
	BbBlock si = (BbBlock)getBbUint16(buf, msg, i + STRING_PLACEHOLDER_BLOCK_INDEX);//this is the index of the sequence block
	//now add on the displacement into the sequence data of the desired element
	uint32_t m = getBbUint32(buf, si, STRING_BLOCK_LENGTH_INDEX);
	if(m > n){
		m = n;
	}
	uint32_t k = STRING_BLOCK_DATA_START_INDEX;
	for(uint32_t j = 0; j < m; ++j){
		dest[j] = getBbUint8(buf, si, k);
		++k;
	}
	return m;
}

/**
 * copies a string from the specified memory location to the message
 * This will stop when it reads a null (\0) or when it has moved n characters.
 * @param buf - the buffer containing the message
 * @param msg - the index of the message in the buffer
 * @param i - the index of the string placeholder within the message
 * @param src - the location in memory to copy the string from
 * @param n - the maximum number of characters to copy.
 * @return the number of characters copied
 */
uint32_t copyBbStringToMessage(Bb* buf, BbBlock msg, uint32_t i, char* src, uint32_t n){
	//first choose a spot to place the string and record it
	BbBlock si = buf->length;
	setBbUint16(buf, msg, i + STRING_PLACEHOLDER_BLOCK_INDEX, si);

	//now copy the data
	uint32_t j = 0;
	uint32_t k = STRING_BLOCK_DATA_START_INDEX;
	for(; j < n; ++j){
		char c = src[j];
		if(c == '\0'){
			break;
		}
		setBbUint8(buf, si, k, c);
		++k;
	}
	//now record the length
	setBbUint32(buf, si, STRING_BLOCK_LENGTH_INDEX, j);

	//update the buffer length
	buf->length += k;

	//and update the message length
	updateBbMessageLength(buf, msg);

	return k;
}



/**
 * Gets the index for the specified sequence element. , relative to the specified message.
 * This can be used to read or write from the specified sequence element
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 * @param i - the index (in bytes) of the sequence placeholder (which consists of a an index to the sequence length field (uint16) and the element byte count (uint16))
 */
uint32_t getBbSequenceElementNum(Bb* buf, BbBlock msg, uint32_t i){
	//get the index of the block containing the sequence data
	BbBlock seq = (BbBlock)getBbUint16(buf, msg, i + SEQUENCE_PLACEHOLDER_BLOCK_INDEX);//this is the index of the sequence block
	uint32_t result = getBbUint16(buf, seq, SEQUENCE_BLOCK_ELEMENTS_NUM_INDEX);
	return result;
}
/**
 * Initializes the sequence placeholder and sequence length with the required information
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 * @param i - the index (in bytes) of the sequence placeholder (which consists of a an index to the sequence length field (uint16) and the element byte count (uint16)) within the message
 * @param elementByteNum - the number of bytes used by each sequence element
 * @param elementNum - the number of elements of the sequence
 * @return the block in the buffer containing the sequence data

 */
BbBlock initBbSequence(Bb* buf, BbBlock msg, uint32_t i, uint32_t elementByteNum, uint32_t elementNum){
	//determine location to place the sequence block
	BbBlock result = align(buf->length);//the sequence data will be added to the current end of the buffer
	//record the block index
	setBbUint16(buf, msg, i + SEQUENCE_PLACEHOLDER_BLOCK_INDEX, (uint16_t)result);
	setBbUint16(buf, msg, i + SEQUENCE_PLACEHOLDER_ELEMENT_LENGTH_INDEX, (uint16_t)elementByteNum);
	setBbUint32(buf, result, SEQUENCE_BLOCK_ELEMENTS_NUM_INDEX, elementNum);//record the number of elements of this sequence
	//advance buffer by the size of the sequence block
	buf->length += 4 + (elementNum * elementByteNum);
	updateBbMessageLength(buf, msg);
	return result;
}

/**
 * Gets the specified element of the array, as a block index, relative to the specified message
 * This can be used to read or write from the specified sequence element
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 * @param i - the index (in bytes) of the start of the array (which corresponds to the first element)
 * @param arrayElement - the item of the array that we want
 * @param arrayElementLength - the length in bytes of each array element
 */
BbBlock getBbArrayElementIndex(Bb* buf, BbBlock msg, uint32_t i, uint32_t arrayElement, uint32_t arrayElementLength){
	//if index is invalid then return
	if(isBbIndexInvalid(i)){
		return i;
	}
	return i + arrayElement*arrayElementLength;
}

/**
 * takes the specified value and rounds it up to the nearest multiple of 4
 * this is useful to compute the next greater index that is word-aligned
 * or to round up a message length to the nearest 4-bytes
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
	return len < MESSAGE_FIRST_DATA;
}
