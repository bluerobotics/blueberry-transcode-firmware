/*
Copyright (c) 2025 Blue Robotics North Inc.

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

#include <blueberry-receiver.h>

#include <byteQ.h>
#include <stdbool.h>

#include "blueberry-transcoder.h"

//*******************************************************************************************
//Defines
//*******************************************************************************************
#define BB_UDP_PORT 0x4242
//*******************************************************************************************
//Types
//*******************************************************************************************



//*******************************************************************************************
//Variables
//*******************************************************************************************

//*******************************************************************************************
//Function Prototypes
//*******************************************************************************************
//



/**
 * A function to process a UDP packet as a blueberry packet
 * This matches the function signature of @see UdpPacketProcessor
 * @param sourceMac - the mac address of the source of the received packet
 * @param sourceIp - the IP address of the source device
 * @param sourcePort - the port that this packet was sent from
 * @param destIp - the IP address that this packet was sent to - this will either be this device's IP address or a broadcast address
 * @param destPort - the port that this packet was sent to. This will be the blueberry port no doubt
 * @param data - the data payload of the packet
 * @param dataLength - the number of bytes of the packet
 */

bool processBlueberryPacket(uint8_t sourceMac[6], uint32_t sourceIp, uint16_t sourcePort, uint32_t destIp, uint16_t destPort, uint8_t* data, uint32_t dataLength);//EthernetPacket* ep,  Ipv4Packet* ip, UdpPacket* inUp);

/**
 * scans the input queue for packets, if found will respond with a response packet on the output queue.
 * this funcion will only process at most n bytes at a time
 * It uses the input buffer to store the recieving state between calls.
 *
 * @param inP - a packet used for receiving. This should be static
 * @param inQ - the queue that the bytes
 * @param outQ - the queue that a response packet will be sent on
 * @param n - the maximum number of bytes to process in one run of this function - if 0 then assumes the whole packet is in the inQ
 */
bool transceiveBrPacketN(Bb* inP, ByteQ* inQ, ByteQ* outQ, uint32_t n);

//*******************************************************************************************
//Code
//*******************************************************************************************

#endif /* BLUEBERRY_RECEIVER_H_ */
