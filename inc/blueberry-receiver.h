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

#ifndef BLUEBERRY_RECEIVER_H_
#define BLUEBERRY_RECEIVER_H_

//*******************************************************************************************
//Includes
//*******************************************************************************************
#include <byteQ.h>
#include <stdint.h>
#include <stdbool.h>
#include <blueberry-transcoder.h>

//*******************************************************************************************
//Defines
//*******************************************************************************************

//*******************************************************************************************
//Types
//*******************************************************************************************


/**
 * a function pointer definition for checking the received packet
 * It takes one parameter: the struct for the received data.
 * It assumes the length member of the struct indicates the current number of bytes received
 */
typedef bool (*CheckFunction)(Bb*);

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
 * @param s - state for this routine to allow for multiple calls
 * @param q - the queue that the new bytes are coming from
 * @param n - the maximum number of bytes to process - this is to limit the type that this routine will take at one calling
 * @param startWordCheck - a function to test the start word of the packet. It will check only up to the Bb.length. It should return true so long as the start word is good
 * @param lengthCheck - a function to test the length of the received packet so far. It should return true when enough bytes have been received
 * @param crcCheck - a function to check the CRC of the received bytes. It will return true with a correct match
 *
 */
bool blueberryReceive(Bb* bb, ByteQ* q, uint32_t n, CheckFunction startWordCheck, CheckFunction lengthCheck, CheckFunction crcCheck);
/**
 * Receive all bytes from the queue, or as many as is necessary to receive the packet
 * Assumes that the packet starts at the beginning of the queue
 * This is indended for use with ethernet
 * This function will not test the CRC
 * @param s - state for this routine to allow for multiple calls
 * @param q - the queue that the new bytes are coming from
 * @param n - the maximum number of bytes to process - this is to limit the type that this routine will take at one calling
 * @param startWordCheck - a function to test the start word of the packet. It will check only up to the Bb.length. It should return true so long as the start word is good
 * @param lengthCheck - a function to test the length of the received packet so far. It should return true when enough bytes have been received
 */
bool blueberryReceiveAll(Bb* bb, ByteQ* q, CheckFunction startWordCheck, CheckFunction lengthCheck);
/**
 * Free up data from the byte queue and prep the state
 */
void blueberryReceiveDone(Bb* bb, ByteQ* q);

//*******************************************************************************************
//Code
//*******************************************************************************************

#endif /* BLUEBERRY_RECEIVER_H_ */
