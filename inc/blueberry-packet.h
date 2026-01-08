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
#ifndef BLUEBERRY_TRANSCODE_FIRMWARE_INC_BLUEBERRY_PACKET_H_
#define BLUEBERRY_TRANSCODE_FIRMWARE_INC_BLUEBERRY_PACKET_H_
//*************************************************
//includes
//*************************************************
#include <blueberry-transcoder.h>
#include <stdBool.h>

//*************************************************
//defines
//*************************************************


//*************************************************
//Types
//*************************************************


//*************************************************
//Variables
//*************************************************


//*************************************************
//Application Specific Variables
//*************************************************


//*************************************************
//function prototypes
//*************************************************

/**
 * Start a packet in the specified buffer
 * This will reset the buffer, write the header and prepare for messages to be added
 */
void startBbPacket(Bb* buf);
/**
 * Finishes the packet that is currently being assembled
 * This populates the length field and computes a CRC
 * @param buf - the buffer containing the newly constructed packet
 * @param computeCrc - this funciton will compute and populate the CRC field if true, otherwise it will not
 */
void finishBbPacket(Bb* buf, bool computeCrc);
/**
 * returns the index of the first message of the specified packet buffer
 */
BbBlock getBbPacketFirstMessage(Bb* buf);
/**
 * advances to the next message in this packet
 */
BbBlock getBbPacketNextMessage(Bb* buf, BbBlock msg);

bool bBPacketReceiveAll(Bb* inPacket);

#endif /* BLUEBERRY_TRANSCODE_FIRMWARE_INC_BLUEBERRY_PACKET_H_ */
