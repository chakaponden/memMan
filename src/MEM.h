/*
 * MEM.h
 *
 *  Created on: Jan 20, 2014
 *      Author: lenovo
 */

#ifndef MEM_H_
#define MEM_H_

#include "types.h"
#include "RAM.h"
#include "SWAP.h"
namespace mainSpace {

class MEM					// MEMORY
{
public:
		// initial by how bytes are needed (effective data bytes are needed)
		// return true if ok, false if error
	bool init(__int64 bytes);
		// initial by how pages are needed
		// return true if ok, false if error
	bool init(PAGE_COUNT_TYPE ramPageCount, PAGE_COUNT_TYPE swapPageCount);
		// location = ram/swap/all = 0/1/2
		// if location > 2 then locate set to 2 (force set to 2)
		// return count Pages according location
	PAGE_COUNT_TYPE getPagesCount(BYTE locate);
		// location = ram/swap/all = 0/1/2
		// return free data bytes available
		// use if need data <= PAGE_DATA_BLOCK_SIZE
	__int64 getFreeBytesAval(BYTE location);
		// location = ram/swap/all = 0/1/2
		// return free effective data bytes available
		// use if need data > PAGE_DATA_BLOCK_SIZE
	__int64 getFreeEffectiveBytesAval(BYTE location);
		// malloc function
		// if return Pointer with Pointer.idPage == maxValueTypeof(PAGE_COUNT_TYPE) then error
		// (for PAGE_COUNT_TYPE == BYTE this value is 255)
		// location = ram/swap/all = 0/1/2
		// if location > 2 then location set to 2 (force set to 2)
	Pointer malloc(__int64 bytes, BYTE location);
		// calloc function
		// if return Pointer with Pointer.idPage == maxValueTypeof(PAGE_COUNT_TYPE) then error
		// (for PAGE_COUNT_TYPE == BYTE this value is 255)
		// location = ram/swap/all = 0/1/2
		// if location > 2 then location set to 2 (force set to 2)
	Pointer calloc(__int64 bytes, BYTE location);
		// free function
		// mark all Pointer link bitMap blocks as free (00) and update Page indicators
		// + set link.idPage to max type value (for BYTE == 255)
		// idPage set to max type value in constructor (for type 'BYTE', value is 255)
		// it means, that there is no allocated memory for this pointer
	void free(Pointer &link);
		// read Data
		// return false if error, true if ok
	bool read(Pointer link, BYTE &buffer);
		// write BYTE *Data to Pointer link
		// write only full size data, according data size that Pointer link locate
		// data size that const BYTE *Data locate must be equal data size that Pointer link locate
		// return true if ok, false if error
	bool write(Pointer link, BYTE &Data);
		// draw memory map throw ncurces
	void drawMemoryMap();
		// end work = clear && close all
	void end(void);
	MEM();
	virtual ~MEM();
private:
	RAM ram;
};

} /* namespace mainSpace */
#endif /* MEM_H_ */
