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
//*******************************************************************************************
//Defines
//*******************************************************************************************
#define PARSER_NUM (100)
#define MSG_RX_MAX (20)
//*******************************************************************************************
//Types;
//*******************************************************************************************
typedef struct {
	uint16_t key;
	BbParser parser;
} ParserKeyValue;

//*******************************************************************************************
//Variables
//*******************************************************************************************
static ParserKeyValue m_parsers[PARSER_NUM];//the list of parsers. Will always be sorted by key
static uint32_t m_totalNum = 0;
static uint32_t m_rxKeys[MSG_RX_MAX];
static uint32_t m_rxKeyCount = 0;
//*******************************************************************************************
//Function Prototypes
//*******************************************************************************************
/**
 * find the parser that is assigned to the specified key
 * @param key - the specified key
 * @param index - a pointer to the resulting index value - this will indicate the index of the equal or first greater key value
 * @return - -1 if not found, else the index of the function pointer of the parser
 */
static BbParser lookup(uint16_t key, uint32_t * index);
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
	BbBlock msg =
}
/**
 * registers a parser for a given message
 */
void registerBbParser(uint16_t messageKey, BbParser parser){
	uint32_t i;
	BbParser p = lookup(messageKey, &i);
	if(p == NULL){
		//we didn't find a match so make room
		//move everything up from the ith location
		for(uint32_t j = m_totalNum; j > i; --j){
			ParserKeyValue * kvNew = &(m_parsers[j]);
			ParserKeyValue * kvOld = &(m_parsers[j - 1]);
			kvNew->key = kvOld->key;
			kvNew->parser = kvOld->parser;
		}
		++m_totalNum;//we now have one more parser
	}
	m_parsers[i].key = messageKey;
	m_parsers[i].parser = parser;
}

/**
 * find the parser that is assigned to the specified key
 * @param key - the specified key
 * @param index - a pointer to the resulting index value - this will indicate the index of the equal or first greater key value
 * @return - -1 if not found, else the index of the function pointer of the parser
 */
static BbParser lookup(uint16_t key, uint32_t * index){
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
		} else {//if(kv.key > key){
			max = i;//could discard the ith term but then we'd need to check for i being either zero or min, so don't bother
		}
	}
	*index = i;
	return result;




}
