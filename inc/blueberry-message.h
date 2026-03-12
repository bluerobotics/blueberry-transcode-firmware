/*
Copyright (c) 2025 Blue Robotics

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
#ifndef INC_BLUEBERRY_MESSAGE_H_
#define INC_BLUEBERRY_MESSAGE_H_

//*******************************************************************************************
//Includes
//*******************************************************************************************
#include <blueberry-transcoder.h>
#include <stdint.h>
#include <stdbool.h>
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
/**
 * gets the module/message key from the specified message
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 */
uint32_t getBbMessageKey(Bb* bb, BbBlock msg);
/**
 * gets the max ordinal field from the specified message
 * This is the ordinal of the last field of the specified message.
 * This is useful to tell if the message was populated as expected or if it lacks a few fields, as might happen if it was constructed by an older version of the protocol
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 */
uint8_t getBbMessageMaxOrdinal(Bb* bb, BbBlock msg);

/**
  * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 * gets the length of this message in bytes
 */
uint32_t getBbMessageLength(Bb* bb, BbBlock msg);

/**
 * Gets the index for the specified sequence element. this can be used to read or write from the specified sequence element
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 * @param i - the index (in bytes) of the sequence placeholder (which consists of a an index to the sequence length field (uint16) and the element byte count (uint16))
 * @param sequenceElement - the index of the sequence element. This must be smaller than the sequence length - but this is not checked here.
 */
BbBlock getBbSequenceElementIndex(Bb* buf, BbBlock msg, uint16_t i, uint32_t sequenceElement);
/**
 * Gets the length of the specified sequence.
 * This can be used to read or write from the specified sequence element
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 * @param i - the index (in bytes) of the sequence placeholder (which consists of a an index to the sequence length field (uint16) and the element byte count (uint16))
 */
uint32_t getBbSequenceLength(Bb*buf, BbBlock msg, uint16_t i);
/**
 * Initializes the sequence placeholder and sequence length with the required information
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 * @param i - the index (in bytes) of the sequence message placeholder (which consists of a an index to the sequence length field (uint16) and the element byte count (uint16))
 * @param elementByteNum - the number of bytes used by each sequence element
 * @param elementNum - the number of elements of the sequence
 * @return the block in the buffer containing the sequence data

 */
BbBlock initBbSequence(Bb* buf, BbBlock msg, uint16_t i, uint32_t elementByteNum, uint32_t elementNum);


/**
 * Gets the specified element of the array, as a block index
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 * @param i - the index (in bytes) of the start of the array (which corresponds to the first element)
 * @param arrayElement - the item of the array that we want
 * @param arrayElementLength - the length in bytes of each array element
 */
BbBlock getBbArrayElementIndex(Bb* buf, BbBlock msg, uint16_t i, uint32_t arrayElement, uint32_t arrayElementLength);

/**
 * Checks to see if this message contains data
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 *
 */
bool isBbMessageEmpty(Bb* buf, BbBlock msg);

/**
 * Gets the index for the specified sequence element. , relative to the specified message.
 * This can be used to read or write from the specified sequence element
 * @param buf - the buffer containing the data packet, message, etc.
 * @param msg - the index of the beginning of the message
 * @param i - the index (in bytes) of the sequence placeholder (which consists of a an index to the sequence length field (uint16) and the element byte count (uint16))
 */
uint32_t getBbSequenceElementNum(Bb* buf, BbBlock msg, uint16_t i);


/**
 * copies a string from the specified message to the specified memory location
 * @param buf - the buffer containing the message
 * @param msg - the index of the message in the buffer
 * @param i - the index of the string placeholder within the message
 * @param dest - the location in memory to copy the string to
 * @param n - the maximum number of characters to copy.
 * @return the number of characters copied
 */
uint32_t copyBbStringFromMessage(Bb* buf, BbBlock msg, uint16_t i, char* dest, uint32_t n);

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
uint32_t copyBbStringToMessage(Bb* buf, BbBlock msg, uint16_t i, char* src, uint32_t n);

//*******************************************************************************************
//Code
//*******************************************************************************************

#endif /* INC_BLUEBERRY_MESSAGE_H_ */
