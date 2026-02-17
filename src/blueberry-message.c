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
	setBbUint16(bb, msg, MESSAGE_LENGTH_INDEX, bbAlign((uint16_t)(bb->length - (uint32_t)msg))/4);
}


/**
 * Gets the index for the specified sequence element. , relative to the specified message.
 * This can be used to read or write from the specified sequence element
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 * @param i - the index (in bytes) of the sequence placeholder (which consists of a an index to the sequence length field (uint16) and the element byte count (uint16))
 * @param sequenceElement - the index of the sequence element. This must be smaller than the sequence length - but this is not checked here.
 */
BbBlock getBbSequenceElementIndex(Bb* buf, BbBlock msg, uint16_t i, uint32_t sequenceElement){
	//if index is invalid then return
	if(isBbBlockInvalid(i)){
		return i;
	}
	//get byte number per element
	uint32_t bn = (uint32_t)getBbUint16(buf, msg, i + SEQUENCE_PLACEHOLDER_ELEMENT_LENGTH_INDEX);
	//get the index of the block containing the sequence data
	BbBlock result = (BbBlock)getBbUint16(buf, msg, i + SEQUENCE_PLACEHOLDER_BLOCK_INDEX);//this is the index of the sequence block
	//now add on the displacement into the sequence data of the desired element
	result += SEQUENCE_BLOCK_DATA_START_INDEX + sequenceElement*bn;
	return result;
}
/**
 * Gets the length of the specified sequence.
 * This can be used to read or write from the specified sequence element
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 * @param i - the index (in bytes) of the sequence placeholder (which consists of a an index to the sequence length field (uint16) and the element byte count (uint16))
 */
uint32_t getBbSequenceLength(Bb*buf, BbBlock msg, uint16_t i){
	//if index is invalid then return
	if(isBbBlockInvalid(i)){
		return 0;
	}
	//get the index of the block containing the sequence data
	BbBlock si = (BbBlock)getBbUint16(buf, msg, i + SEQUENCE_PLACEHOLDER_BLOCK_INDEX);//this is the index of the sequence block
	uint32_t result = 0;
	//now add on the displacement into the sequence data of the desired element
	if(si != BB_INVALID_BLOCK){
		result = getBbUint32(buf, msg + si, SEQUENCE_BLOCK_ELEMENTS_NUM_INDEX);
	}
	return result;
}
/**
 * Gets the length of the specified sequence.
 * This can be used to read or write from the specified sequence element
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 * @param i - the index (in bytes) of the string placeholder (which consists of a an index to the string length field (uint16))
 */
uint32_t getBbStringLength(Bb*buf, BbBlock msg, uint16_t i){
	//if index is invalid then return
	if(isBbBlockInvalid(i)){
		return 0;
	}
	//get the index of the block containing the sequence data
	BbBlock si = (BbBlock)getBbUint16(buf, msg, i + SEQUENCE_PLACEHOLDER_BLOCK_INDEX);//this is the index of the sequence block
	uint32_t result = 0;
	//now add on the displacement into the sequence data of the desired element
	if(si != BB_INVALID_BLOCK){
		result = getBbUint32(buf, si + msg, STRING_BLOCK_LENGTH_INDEX);//index is relative to the message start
	}
	return result;
}
/**
 * copies a string from the specified message to the specified memory location
 * the string in memory will be null terminated
 * @param buf - the buffer containing the message
 * @param msg - the index of the message in the buffer
 * @param i - the index of the string placeholder within the message
 * @param dest - the location in memory to copy the string to
 * @param n - the maximum number of characters to copy.
 * @return the number of characters copied
 */
uint32_t copyBbStringFromMessage(Bb* buf, BbBlock msg, uint16_t i, char* dest, uint32_t n){
	//get the index of the block containing the string data
	BbBlock si = (BbBlock)getBbUint16(buf, msg, i + STRING_PLACEHOLDER_BLOCK_INDEX);//this is the index of the sequence block
	si += msg;//make index absolute
	//now add on the displacement into the sequence data of the desired element
	uint32_t m = getBbUint32(buf, si, STRING_BLOCK_LENGTH_INDEX);
	if(m > n){
		m = n;
	}
	uint32_t j = 0;
	uint32_t k = STRING_BLOCK_DATA_START_INDEX;
	for(; j < m; ++j){
		dest[j] = getBbUint8(buf, si, k);
		++k;
	}
	dest[j] = 0;//add null termination
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
uint32_t copyBbStringToMessage(Bb* buf, BbBlock msg, uint16_t i, char* src, uint32_t n){
	BbBlock si;
	uint32_t slen = 0;
	if(n == 0){
		//if the string should be zero length then simply set the block index to zero
		si = BB_INVALID_BLOCK;
	} else {
		//first choose a spot to place the string and record it
		si = buf->length;

		for(; slen < n; ++slen){
			if(src[slen] == '\0'){
				break;
			}
		}
		//now slen contains the length of the string



		buf->length += bbAlign(slen + STRING_BLOCK_DATA_START_INDEX);//advance buffer length in preparation


		//now copy the data
		for(uint32_t j = 0; j < slen; ++j){
			char c = src[j];
			setBbUint8(buf, si, STRING_BLOCK_DATA_START_INDEX + j, c);

		}
		//now record the length
		setBbUint32(buf, si, STRING_BLOCK_LENGTH_INDEX, slen);


		//and update the message length to the end of the buffer
		updateBbMessageLength(buf, msg);
	}
	//record the string block in the placeholder
	setBbUint16(buf, msg, i + STRING_PLACEHOLDER_BLOCK_INDEX, si - msg);//make relative to message start
	return slen;
}



/**
 * Gets the index for the specified sequence element. , relative to the specified message.
 * This can be used to read or write from the specified sequence element
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 * @param i - the index (in bytes) of the sequence placeholder (which consists of a an index to the sequence length field (uint16) and the element byte count (uint16))
 */
uint32_t getBbSequenceElementNum(Bb* buf, BbBlock msg, uint16_t i){
	//get the index of the block containing the sequence data
	BbBlock seq = (BbBlock)getBbUint16(buf, msg, i + SEQUENCE_PLACEHOLDER_BLOCK_INDEX);//this is the index of the sequence block
	seq += msg;//sequence index is relative to message start so make absolute
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
BbBlock initBbSequence(Bb* buf, BbBlock msg, uint16_t i, uint32_t elementByteNum, uint32_t elementNum){
	BbBlock result;
	if(elementNum == 0){
		//if there are zero elements then only set the block index to zero
		result = BB_INVALID_BLOCK;
	} else {
		//determine location to place the sequence block
		 result = bbAlign(buf->length);//the sequence data will be added to the current end of the buffer
		 //expand the buffer in preparation for writing the sequence block
		 buf->length += bbAlign(4 + (elementNum * elementByteNum));
		//record the block index
		setBbUint16(buf, msg, i + SEQUENCE_PLACEHOLDER_ELEMENT_LENGTH_INDEX, (uint16_t)elementByteNum);
		setBbUint32(buf, result, SEQUENCE_BLOCK_ELEMENTS_NUM_INDEX, elementNum);//record the number of elements of this sequence
		//advance buffer by the size of the sequence block

		updateBbMessageLength(buf, msg);
	}
	setBbUint16(buf, msg, i + SEQUENCE_PLACEHOLDER_BLOCK_INDEX, (uint16_t)(result - msg));//block index is relative to the message start

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
BbBlock getBbArrayElementIndex(Bb* buf, BbBlock msg, uint16_t i, uint32_t arrayElement, uint32_t arrayElementLength){
	//if index is invalid then return
	if(isBbBlockInvalid(i)){
		return i;
	}
	return i + arrayElement*arrayElementLength;
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
/**
 * copies the source string to the target, completing on a '\0' or reaching n
 * @param target - the destination array
 * @param source - the source array
 * @param n - the maximum length of target
 * @return the number of characters copied, not including the terminating '\0' of source
 */
static uint32_t stringCopy(char * target, char * source, uint32_t n){
	uint32_t result = n;
	for(uint32_t i = 0; i < n; ++i){
		char c = source[i];
		if(c == 0x0){
			result = i;
			break;
		}
		target[i] = c;
	}
	return result;
}
/**
 * makes a topic string given a source - assumed to contain placeholders for network ID and device type
 * substitutes in the specified nid and deviceType and puts the result in the target
 * placeholder char for nid is 0x80
 * placeholder char for deviceType is 0x81
 * @param target - a pointer to the result string
 * @param targetMaxLen - the size of target - which must not be exceeded
 * @param source - a pointer to the template topic string
 * @param nid - the text that the nid placeholder will be replaced with
 * @param deviceType - the text that the device type placeholder will be replaced with
 * @return the length of the result string, not including the terminating '\0'
 */
uint32_t makeBbTopicString(char * target, uint32_t targetMaxLen, char * source, char * nid, char * deviceType){
	uint32_t i = 0;//the source index
	uint32_t j = 0;//the target index
	uint32_t n = targetMaxLen;//this is the max length of target

	bool notDone = true;

	while(j < n){
		char c = source[i];
		if(c == 0x80){
			j += stringCopy(&target[j], nid, n - j);
		} else if(c == 0x81){
			j += stringCopy(&target[j], deviceType, n - j);
		} else if(c == 0x0){
			target[j] = 0;
			break;
		} else {
			target[j] = c;
			++j;
		}
		++i;
	}
	return j;
}
