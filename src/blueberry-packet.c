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

//*************************************************
//includes
//*************************************************
#include <blueberry-packet.h>
#include <crc1021.h>

//*************************************************
//defines
//*************************************************

#define START_WORD_INDEX 0
#define LENGTH_INDEX 4
#define CRC_INDEX 6
#define DATA_INDEX 8

#define MSG_KEY 0
#define MSG_LEN 4
#define MSG_MAX_ORD 6


//*************************************************
//Types
//*************************************************


//*************************************************
//Variables
//*************************************************
static const int START_WORD_VAL = 0x65756c42; //little endian means the lowest index byte (first received byte) is the least significant byte


//*************************************************
//Application Specific Variables
//*************************************************


//*************************************************
//function prototypes
//*************************************************

static bool checkStartWord(Bb* buf);
static bool checkLength(Bb* buf);
static bool checkCrc(Bb* buf);
static uint16_t computeCrc(Bb* buf);



//*************************************************
//code
//*************************************************

/**
 * Start a packet in the specified buffer
 * This will reset the buffer, write the header and prepare for messages to be added
 * @return the index of the first message
 */
void startBbPacket(Bb* buf){
	setBbUnit32(buf, START_WORD_INDEX, START_WORD_VAL);
	setBbUint16(buf, LENGTH_INDEX, 2);
	setBbUint16(buf, CRC_INDEX, 0xffff);
	buf->length = 8;
}



/**
 * returns the index of the first message of the specified packet buffer
 */
BbBlock getBbPacketFirstMessage(Bb* buf){
	return DATA_INDEX;
}

/**
 * advances to the next message in this packet
 */
BbBlock getBbPacketNextMessage(Bb* buf, BbBlock msg){
	uint32_t n = getUint16(buf, msg);
	return msg + (n * 4);
}

/**
 * Finishes the packet that is currently being assembled
 * This populates the length field and computes a CRC
 * This assumes that the buffer does actually contain a newly build packet and that its length field will be correct
 * @param buf - the buffer containing the newly constructed packet
 * @param computeCrc - this funciton will compute and populate the CRC field if true, otherwise it will not
 */
void finishBbPacket(Bb* buf, bool computeCrc){
//	setBbUnit32(buf, START_WORD_INDEX, START_WORD_VAL);//this has been done
	uint32_t len = (buf->length) / 4;
	if((buf->length & 0b11) > 0){//if any of the low bits are set then increment length
		++len;
	}

	setBbUint16(buf, LENGTH_INDEX, len);
	if(computeCrc){
		setBbUint16(buf, CRC_INDEX, computeCrc(buf));
	} else {

	}
}





/**
 * computes the CRC of this packet and compares it to its published CRC
 * @return - true if computed CRC matches published one
 */
static bool checkCrc(Bb* buf) {
	uint16_t acrc = computeCrc(buf);
	uint16_t pcrc = getBbUint16(buf, 0, CRC_INDEX);
	return acrc == pcrc;
}
/**
 * compares the actual length of this packet with its published length
 * @return
 */
static bool checkLength(Bb* buf) {
	uint16_t al = buf->length / 2;
	return al >= DATA_INDEX && al >= getBbUint16(buf, 0, LENGTH_INDEX);
}

/**
 * checks the recevied bytes against the start word
 * this only checks up to the number received so far, so will pass if only one byte receieved and it matches the first expected start byte
 * @return
 */
static bool checkStartWord(Bb* buf) {
	uint32_t i = buf->length;
	//i represents the number of bytes received so far
	uint32_t sw = getBbUint32(buf, 0, START_WORD_INDEX);
	sw ^= START_WORD_VAL;
	//mask off bits we have not received yet. Note that this could be written shorter but a switch-case seemed more understandable
	switch(i){
	case 0://we've received no bytes yet so don't check any
		sw &= 0x0;
		break;
	case 1://we've only received one byte so only check the lowest order (first received) byte
		sw &= 0xff;
		break;
	case 2:
		sw &= 0xffff;
		break;
	case 3:
		sw &= 0xffffff;
		break;
	default://we've received 4 or more bytes so we'll check all four start bytes
		break;
	}
	//now if there are any non-zero bits in sw then we've failed the match
	return sw == 0;


}
static int computeCrc(Bb* buf) {
	uint16_t result = -1;

	int n = getBbUint16(buf, 0, LENGTH_INDEX)*4;

	for(uint32_t i = 0; i < n; ++i){
		result = crc1021(result, getBbUint8(buf, 0, DATA_INDEX));
	}

	return result;
}

bool bBPacketReceiveAll(Bb* inPacket){
	bool result = false;

	uint32_t n = inPacket->length;
	inPacket->length = n; //this field will eventually be set by the length check
	if(checkStartWord(inPacket)){
		if(checkLength(inPacket)){
			if(checkCrc(inPacket)){
				result = true;
			}
		}
	}

	if(!result){
		//throw out the queue contents - the packet was no good

		inPacket->length = 0;
	}

	return result;


}
