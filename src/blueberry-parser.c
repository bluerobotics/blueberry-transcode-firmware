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


//*******************************************************************************************
//Includes
//*******************************************************************************************
#include <blueberry-parser.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <queue.h>
//*******************************************************************************************
//Defines
//*******************************************************************************************
#define PARSER_NUM (100)
#define MSG_Q_SIZE (20)

#define MAKE_KEY(mod, msg) ((((uint32_t)mod) << 16) | ((uint32_t)msg))
//*******************************************************************************************
//Types;
//*******************************************************************************************
typedef struct {
	uint32_t key;
	BbParser parser;
} ParserKeyValue;

//*******************************************************************************************
//Variables
//*******************************************************************************************
static ParserKeyValue m_parsers[PARSER_NUM];//the list of parsers. Will always be sorted by key
static uint32_t m_totalNum = 0;
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
static BbParser lookup(uint32_t key, uint32_t * index);
//*******************************************************************************************
//Code
//*******************************************************************************************
/**
 * Must be called at init
 */
void initBbParser(void){
	for(uint32_t i = 0; i < PARSER_NUM; ++i){
		m_parsers[i].key = -1;
		m_parsers[i].parser = NULL;
	}
	m_totalNum = 0;
}

/**
 * processes a blueberry packet and parses each message
 * This assumes that the buffer has been properly received and is at the start of the packet
 */
void parseBbPacket(Bb* buf){
	uint32_t packetLength = getBbUint16(buf, 0, PACKET_LENGTH_INDEX);
	if(packetLength == 0){
		return;
	}
	BbBlock msg = PACKET_FIRST_MESSAGE_INDEX;
	bool done = false;

	while(!done){
		uint16_t length = getBbUint16(buf, msg, MESSAGE_LENGTH_INDEX);
		if(length == 0){
			break;
		}
		uint16_t moduleKey = getBbUint16(buf, msg, MODULE_KEY_INDEX);
		uint16_t messageKey = getBbUint16(buf, msg, MESSAGE_KEY_INDEX);
		uint32_t k = MAKE_KEY(moduleKey, messageKey);
		uint32_t i;
		BbParser p = lookup(k, &i);
		if(p != NULL){
			(*p)(buf, msg);
			m_rxQ[m_rxQBack] = k;
			justAddedToQueueBack(&m_rxQFront, &m_rxQBack, MSG_Q_SIZE);
		}
		msg += length;
		if(msg >= packetLength){
			done = true;
		}
	}
}
/**
 * registers a parser for a given message
 * This will add a parser if module & key are new
 * This will overwrite a parser if module & key already exist in the list
 * Will fail silently if the list is full
 */
void registerBbParser(uint16_t moduleKey, uint16_t messageKey, BbParser parser){
	if(m_totalNum >= PARSER_NUM){
		return;
	}
	uint32_t i;
	uint32_t k = MAKE_KEY(moduleKey, messageKey);
	BbParser p = lookup(k, &i);
	if(p == NULL){
		//we didn't find a match so make room by
		//moving everything up one, from the ith location to the end
		for(uint32_t j = m_totalNum; j > i; --j){
			ParserKeyValue * kvNew = &(m_parsers[j]);
			ParserKeyValue * kvOld = &(m_parsers[j - 1]);
			kvNew->key = kvOld->key;
			kvNew->parser = kvOld->parser;
		}
		++m_totalNum;//we now have one more parser
	}
	m_parsers[i].key = k;
	m_parsers[i].parser = parser;
}

/**
 * find the parser that is assigned to the specified key
 * @param key - the key to lookup
 * @param index - a pointer to the resulting index value - this will indicate the index of the equal or first greater key value
 * @return - -1 if not found, else the index of the function pointer of the parser
 */
static BbParser lookup(uint32_t key, uint32_t * index){

	BbParser result = NULL;
	//use binary search to find parser
	uint32_t min = 0;
	uint32_t max = m_totalNum;//this will be one past the last element of the list
	uint32_t i;
	while(true){
		i = min + max / 2;//i will be greater than or equal to i because of the integer math
		ParserKeyValue kv = m_parsers[i];
		if(kv.key == key){
			result = kv.parser;
			break;
		} else if(min == max){
			break;//we've converged but not matched
		} else if(kv.key < key){
			min = i + 1;//get the ith term out of the new range or we'll get stuck
		} else {//if(kv.msgKey > msgKey){
			max = i;//could discard the ith term but then we'd need to check for i being either zero or min, so don't bother
		}
	}
	*index = i;
	return result;




}
