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


//*******************************************************************************************
//Includes
//*******************************************************************************************
#include <blueberry-parser.h>
#include <blueberry-receiver.h>
#include <blueberry-message.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <queue.h>
//*******************************************************************************************
//Defines
//*******************************************************************************************
#define PROCESSOR_NUM (100)
#define MSG_Q_SIZE (20)

#define MAKE_KEY(mod, msg) ((((uint32_t)mod) << 16) | ((uint32_t)msg))

#define PACKET_PREAMBLE (0x65756c42) //(0x45554c42)
#define PACKET_PREAMBLE_INDEX (0)
#define PACKET_LENGTH_INDEX (4)
#define PACKET_CRC_INDEX (6)
#define PACKET_FIRST_MESSAGE_INDEX (8)




//*******************************************************************************************
//Types;
//*******************************************************************************************
typedef struct {
	uint32_t key;
	BbProcessor parser;
} ProcessorKeyValue;

typedef struct {
	ProcessorKeyValue m_processors[PROCESSOR_NUM];
	uint32_t num;
} Processors;

//*******************************************************************************************
//Variables
//*******************************************************************************************
static Processors m_parsers;
static Processors m_builders;
static uint32_t m_rxQ[MSG_Q_SIZE];
static uint32_t m_rxQFront = 0;
static uint32_t m_rxQBack = 0;
//*******************************************************************************************
//Function Prototypes
//*******************************************************************************************
/**
 * find the parser that is assigned to the specified key
 * @param key - the specified key
 * @param index - a pointer to the resulting index value - this will indicate the index of the equal or first greater key value
 * @return - -1 if not found, else the index of the function pointer of the parser
 */
static BbProcessor lookup(Processors * ps, uint32_t key, uint32_t * index);
static void registerProcessor(Processors * ps, uint32_t key, BbProcessor p);
//*******************************************************************************************
//Code
//*******************************************************************************************
/**
 * Must be called at init
 */
void initBbParser(void){
	m_parsers.num = 0;
	m_builders.num = 0;
	registerUdpListener(BB_UDP_PORT, processBlueberryPacket, false);
	setEthernetPort(BB_UDP_PORT, BB_UDP_PORT);
}

/**
 * processes a blueberry packet and parses each message
 * This assumes that the buffer has been properly received and is at the start of the packet
 */
void parseBbPacket(Bb* buf){
	uint32_t packetLength = getBbUint16(buf, 0, PACKET_LENGTH_INDEX)*4;
	if(packetLength == 0){
		return;
	}
	BbBlock msg = PACKET_FIRST_MESSAGE_INDEX;

	while(msg < packetLength){

		if(isBbMessageEmpty(buf, msg)){
			//this seems malformed so quite parsing packet
			break;
		} else {
			//we have enough data for a message
			uint32_t k = getBbMessageKey(buf, msg);

			uint32_t i;
			BbProcessor p = lookup(&m_parsers, k, &i);
			if(p != NULL){
				//call the parser
				(*p)(buf, msg);

				//record in the queue that the particular type of message was received
				queueBbMessage(k);

			}
		}
		uint32_t len = getBbMessageLength(buf, msg);
		msg += len;

	}
}
/**
 * requests that the next packet should have the message with the specified key added.
 * @param key - the module/message key for the desired message
 */
void queueBbMessage(uint32_t key){
	m_rxQ[m_rxQBack] = key;
	justAddedToQueueBack(&m_rxQFront, &m_rxQBack, MSG_Q_SIZE);
}



/**
 * registers a parser for a given message
 * This will add a parser if module & key are new
 * This will overwrite a parser if module & key already exist in the list
 * Will fail silently if the list is full
 */
void registerBbParser(uint32_t moduleMessageKey, BbProcessor parser){
	registerProcessor(&m_parsers, moduleMessageKey, parser);
}
/**
 * register a message processor for adding a message to a buffer
 */
void registerBbBuilder(uint32_t moduleMessageKey, BbProcessor builder){
	registerProcessor(&m_builders, moduleMessageKey, builder);
}

static void registerProcessor(Processors * ps, uint32_t key, BbProcessor p){

	if(ps->num >= PROCESSOR_NUM){
		return;
	}
	uint32_t i;
	uint32_t k = key;
	BbProcessor pt = lookup(ps, k, &i);
	if(pt == NULL){
		//we didn't find a match so make room by
		//moving everything up one, from the ith location to the end
		for(uint32_t j = ps->num; j > i; --j){
			ProcessorKeyValue * kvNew = &(ps->m_processors[j]);
			ProcessorKeyValue * kvOld = &(ps->m_processors[j - 1]);
			kvNew->key = kvOld->key;
			kvNew->parser = kvOld->parser;
		}
		++ps->num;//we now have one more parser
	}
	ps->m_processors[i].key = k;
	ps->m_processors[i].parser = p;
}




/**
 * find the parser that is assigned to the specified key
 * or find the position where the specified key might be placed in the list if it is not there
 * @param ps - the processors struct to look in
 * @param key - the key to lookup
 * @param index - a pointer to the resulting index value - this will indicate the index of the equal or first greater key value
 * @return - -1 if not found, else the index of the function pointer of the parser
 */
static BbProcessor lookup(Processors * ps, uint32_t key, uint32_t * index){

	BbProcessor result = NULL;
	//use binary search to find parser
	uint32_t min = 0;
	uint32_t max = ps->num == 0 ? 0 : ps->num - 1;//this will be one past the last element of the list
	uint32_t i;
	uint32_t ikey = 0;
	while(true){
		i = (min + max) / 2;//i will be greater than or equal to i because of the integer math
		ProcessorKeyValue kv = ps->m_processors[i];
		ikey = kv.key;
		if(ps->num == 0){
			break;
		} else if(ikey == key){
			result = kv.parser;
			break;
		} else if(ikey < key){
			//the key in the list is less than the search key
			++i;//advance to the next spot
			if(i > max){
				//we're already at the end of the list so
				break;
			}
			min = i;//constrain the range based on the latest test
		} else {
			//the key in the list is greater than the search key
			//so the right answer probably lives below this one
			if(i <= min || min == max){
				//if we're at the minimum of the range or the range has shrunk to 0 then I guess we're done.
				break;
			}
			//the current location is too big so move to the next smaller one
			//and constrain the range accordingly
			--i;
			max = i;
		}
	}
	*index = i;
	return result;
}
/**
 * checks if a potential packet has at least enough bytes received to contain a packet header
 */
bool minBbLengthCheck(Bb* bb){
	return bb->length >= PACKET_FIRST_MESSAGE_INDEX;
}
/**
 * a function to test the start word of the packet. It will check only up to the Bb.length. It should return true so long as the start word is good
 */
bool checkBbPreamble(Bb* bb){
	uint32_t n = bb->length;
	uint32_t a = 0;
	n = n > 4 ? 4 : n;//n will now cap out at 4
	uint32_t b = PACKET_PREAMBLE & (0xffffffff >> ((4 - n)*8));
	for(uint32_t j = 0; j < n; ++j){
		a |= ((uint32_t)getBbUint8(bb, 0, PACKET_PREAMBLE_INDEX + j)) << (j*8);
	}


	return (a ^ b) == 0;
}
/**
 * a function to test the length of the received packet so far. It should return true when enough bytes have been received
 */
bool checkBbLength(Bb* bb){
	uint32_t len = getBbUint16(bb, 0, PACKET_LENGTH_INDEX)*4;
	uint32_t n = bb->length;

	return n >= PACKET_FIRST_MESSAGE_INDEX && n >= len;
}
/**
 * a function to check the CRC of the received bytes. It will return true with a correct match
 */
bool checkBbCrc(Bb* bb){
	uint32_t n = bb->length;
	uint16_t crcA = (uint16_t)getBbUint16(bb, 0, PACKET_CRC_INDEX);
	uint16_t crcB = computeCrc(bb, PACKET_FIRST_MESSAGE_INDEX, n);
	return crcA == crcB;
}

/**
 * does any preliminary header setup and computes the location for the starting message
 */
BbBlock startBbPacket(Bb* bb){
	bb->length = PACKET_FIRST_MESSAGE_INDEX;
	setBbUint32(bb, 0, 0, PACKET_PREAMBLE);
//	setBbUint16(bb, 0, PACKET_CRC_INDEX, 0xffff);
//	setBbUint16(bb, 0, PACKET_LENGTH_INDEX, 0);

	return PACKET_FIRST_MESSAGE_INDEX;
}
/**
 * Finalize the packet in preparation for sending
 * This relies on the buffer having the final length set correctly
 * And all messages construted correctly
 */
void finishBbPacket(Bb* bb){
	uint32_t n = bbAlign(bb->length);
//	setBbUint32(bb, 0, 0, PACKET_PREAMBLE);
	setBbUint16(bb, 0, PACKET_LENGTH_INDEX, (uint16_t)(n/4));
	uint16_t crc = computeCrc(bb, PACKET_FIRST_MESSAGE_INDEX, n);
	setBbUint16(bb, 0, PACKET_CRC_INDEX, crc);
}

/**
 * indicates that messages were received and should trigger a corresponding packet of messages to be sent
 */
bool isBbPacketRequested(){
	return isQueueNotEmpty(&m_rxQFront, &m_rxQBack, MSG_Q_SIZE);
}

/**
 * Make a packet in the specified buffer that contains all queued messages
 * bb - the buffer to make the packet in
 */
void makeBbPacketWithQueuedMessages(Bb* bb){
	bool started = false;
	BbBlock msg = PACKET_FIRST_MESSAGE_INDEX;
	while(isQueueNotEmpty(&m_rxQFront, &m_rxQBack, MSG_Q_SIZE)){
		uint32_t key = m_rxQ[m_rxQFront];
		doneWithQueueFront(&m_rxQFront, &m_rxQBack, MSG_Q_SIZE);
		uint32_t i = 0;
		BbProcessor p = lookup(&m_builders, key, &i);
		if(p != NULL){
			if(!started){
				startBbPacket(bb);
				started = true;
			}
			msg = bb->length;//point to the next free byte of the buffer
			(*p)(bb, msg);

		}



	}
	finishBbPacket(bb);



}

/**
 * takes the specified value and rounds it up to the nearest multiple of 4
 * this is useful to compute the next greater index that is word-aligned
 * or to round up a message length to the nearest 4-bytes
 */
BbBlock bbAlign(uint16_t i){
	BbBlock result = i;

	if(i & 0b11){
		result |= 0b11;
		result += 1;
	}
	return result;
}






