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

//*******************************************************************************************
//Defines
//*******************************************************************************************

//*******************************************************************************************
//Types
//*******************************************************************************************
/**
 * A function pointer prototype for parsing a message
 * @param buf - the buffer containing the message
 * @param msg - an index to the message to be parsed
 */
typedef void (*BbParser)(Bb* buf, BbBlock msg);
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
void registerBbParser(uint16_t messageKey, BbParser parser);

/**
 * Must be called at init
 */
void initBbParser(void);



//*******************************************************************************************
//Code
//*******************************************************************************************

#endif /* INC_BLUEBERRY_PARSER_H_ */
