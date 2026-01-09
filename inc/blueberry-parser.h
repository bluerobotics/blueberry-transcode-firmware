/*
Copyright (c) 2025 kenneth

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



#ifndef INC_BLUEBERRY_PARSER_H_
#define INC_BLUEBERRY_PARSER_H_



/**
 * A module to scan through the messages in a packet and call the appropriate message parser
 */

//*******************************************************************************************
//Includes
//*******************************************************************************************
#include <blueberry-transcoder.h>
//*******************************************************************************************
//Defines
//*******************************************************************************************

//*******************************************************************************************
//Types
//*******************************************************************************************
/**
 * A function pointer prototype for parsing a message
 * @param bb - the buffer containing the message
 * @param msg - an index to the message to be parsed
 */
typedef void (*BbParser)(Bb* bb, BbBlock msg);

/**
 * A function pointer prototype for creating a message
 * @param bb - the buffer that will contain the message
 * @param msg - the block where the message shall be written
 * @return - the next block after the new message is written
 */
typedef BbBlock (*BbBuilder)(Bb* bb, BbBlock msg);
//*******************************************************************************************
//Variables
//*******************************************************************************************

//*******************************************************************************************
//Function Prototypes
//*******************************************************************************************

/**
 * processes a blueberry packet and parses each message
 */
void parseBbPacket(Bb* buf);
/**
 * registers a parser for a given message
 */
void registerBbParser(uint32_t moduleMessageKey, BbParser parser);

/**
 * Must be called at init
 */
void initBbParser(void);

/**
 * a function to test the start word of the packet. It will check only up to the Bb.length. It should return true so long as the start word is good
 */
bool checkBbPreamble(Bb* bb);
/**
 * a function to test the length of the received packet so far. It should return true when enough bytes have been received
 */
bool checkBbLength(Bb* bb);
/**
 * a function to check the CRC of the received bytes. It will return true with a correct match
 */
bool checkBbCrc(Bb* bb);

/**
 * does any preliminary header setup and computes the locationo for the starting message
 */
BbBlock startBbPacket(Bb* bb);

/**
 * Finalize the packet in preparation for sending
 * This relies on the buffer having the final length set correctly
 * And all messages construted correctly
 */
void finishBbPacket(Bb* bb);

/**
 * checks if a potential packet has at least enough bytes received to contain a packet header
 */
bool minBbLengthCheck(Bb* bb);

/**
 * gets the module/message key from the specified message
 */
uint32_t getBbMessageKey(Bb* bb, BbBlock msg);
/**
 * gets the max ordinal field from the specified message
 * This is the ordinal of the last field of the specified message.
 * This is useful to tell if the message was populated as expected or if it lacks a few fields, as might happen if it was constructed by an older version of the protocol
 */
uint8_t getBbMessageMaxOrdinal(Bb* bb, BbBlock msg);

//*******************************************************************************************
//Code
//*******************************************************************************************

#endif /* INC_BLUEBERRY_PARSER_H_ */
