/*
 * SWAP.h
 *
 *  Created on: Jan 20, 2014
 *      Author: lenovo
 */

#ifndef SWAP_H_
#define SWAP_H_
#include "types.h"

namespace mainSpace {

class SWAP			// SWAP
	{
	private:
		FILE *swapUk;
	public:
		SWAP(void);
		virtual ~SWAP(void);
			// initial swap file; create swap && initial
			// return 1 if ok, 0 if error
		bool init(const char *fileName, PAGE_COUNT_TYPE swapPageCount);
			// set Data to swap		(FUNCTON DOES NOT CHANGE BYTE *Data)
			// return 1 if ok, 0 if error
		bool writePageData(const BYTE *Data, PAGE_COUNT_TYPE idPage);
			// set Block to swap	(FUNCTON DOES NOT CHANGE BYTE *Block)
			// return 1 if ok, 0 if error
		bool writeBlockData(const BYTE *Block, PAGE_COUNT_TYPE idPage, PAGE_MAP_TYPE idBlock);
			// search Data from swapUk by idPage
			// return NULL if error
		FILE* searchDataPagebyId(PAGE_COUNT_TYPE idPage);
			// search Block from swapUk by idPage && idBlock
			// return NULL if error
		FILE* searchBlockPagebyId(PAGE_COUNT_TYPE idPage, PAGE_MAP_TYPE idBlock);
			// get Data from swap		(FUNCTON DOES NOT CHANGE Data)
			// return NULL if error
		bool readData(PAGE_COUNT_TYPE idPage, BYTE &Data);
			// get Block from swap	(FUNCTON DOES NOT CHANGE Block)
			// return NULL if error
		bool readBlock(PAGE_COUNT_TYPE idPage, PAGE_MAP_TYPE idBlock, BYTE &Block);
			// open old SWAP file, that is already exist
			// return true if ok, false if error
		bool open(const char *fileName);
			// close file
		void close(void);
			// end work && close file
		void end(void);
	};

} /* namespace mainSpace */
#endif /* SWAP_H_ */
