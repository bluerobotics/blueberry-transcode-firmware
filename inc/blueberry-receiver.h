/*
Copyright (c) 2025 Kenneth MacCallum

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

#ifndef BLUEBERRY_TRANSCODE_FIRMWARE_INC_BLUEBERRY_RECEIVER_H_
#define BLUEBERRY_TRANSCODE_FIRMWARE_INC_BLUEBERRY_RECEIVER_H_

//*******************************************************************************************
//Includes
//*******************************************************************************************
#include <byteQ.h>
#include <stdint.h>
#include <stdbool.h>

//*******************************************************************************************
//Defines
//*******************************************************************************************

//*******************************************************************************************
//Types
//*******************************************************************************************
typedef struct {
	uint32_t rxTime;
	Bb packet;
} BlueberryReceiverState;

//*******************************************************************************************
//Variables
//*******************************************************************************************

//*******************************************************************************************
//Function Prototypes
//*******************************************************************************************
/**
 * Receive n bytes from the queue.
 * If the queue has less than n bytes available, then only use what is available.
 * This is intended for use with a UART
 * This functi0n will test the CRC
 */
Bb* blueberryReceive(ReceiverState* s, ByteQ* q, uint32_t n);
/**
 * Receive all bytes from the queue, or as many as is necessary to receive the packet
 * Assumes that the packet starts at the beginning of the queue
 * This is indended for use with ethernet
 * This function will not test the CRC
 */
Bb* blueberryReceiveAll(ReceiverState* s, ByteQ* q);
/**
 * Free up data from the byte queue and prep the state
 */
void blueberryReceiveDone(ReceiverState* s, ByteQ* q);

//*******************************************************************************************
//Code
//*******************************************************************************************

#endif /* BLUEBERRY_TRANSCODE_FIRMWARE_INC_BLUEBERRY_RECEIVER_H_ */
