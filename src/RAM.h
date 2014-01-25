/*
 * RAM.h
 *
 *  Created on: Jan 20, 2014
 *      Author: lenovo
 */

#ifndef RAM_H_
#define RAM_H_
#include "types.h"
#include "SWAP.h"
namespace mainSpace {

class RAM			// RAM
	{
	private:
		void *ramUk;
		SWAP swap;
	public:
		RAM(void);
		virtual ~RAM(void);
			// initial ram
			// return true if ok, false if error
		bool init(PAGE_COUNT_TYPE ramPageCount, PAGE_COUNT_TYPE swapPageCount);
			// return count ram Pages
		PAGE_COUNT_TYPE readCountPageRam(void);
			// return count swap Pages
		PAGE_COUNT_TYPE readCountPageSwap(void);
			// location = ram/swap/all = 0/1/2
			// if location > 2 then location set to 2 (force set to 2)
			// search free page !!! return freePageNum + 1 !!!
			// return 0 if no free pages
		PAGE_COUNT_TYPE searchFreePage(BYTE location);
			// search allocated indicator BYTE Page MEM::RAM::ramUk by idPage
			// return pointer to page indicator or return NULL if error
		void* readPageInd(PAGE_COUNT_TYPE idPage);
			// read allocated indicator BYTE Page MEM::RAM::ramUk by idPage
			// return true if ok, false if error
		bool readPageInd(PAGE_COUNT_TYPE idPage, PAGE_MAP_TYPE &ind);
			// search map Page MEM::RAM::ramUk by idPage
			// return pointer to page map begin or return NULL if error
		void* readPageMap(PAGE_COUNT_TYPE idPage);
			// read map Page MEM::RAM::ramUk by idPage
			// return true if ok, false if error
		bool readPageMap(PAGE_COUNT_TYPE idPage, BYTE &map);
			// search page Data by idPage
			// if return tmp.(ramUkData == NULL && tmp.swapUkData == NULL) then error
		DataLocation readPageData(PAGE_COUNT_TYPE idPage);
			// read page Data by idPage
			// return true if ok, false if error
		bool readPageData(PAGE_COUNT_TYPE idPage, BYTE &data);
			// search block page MEM::RAM::ramUk by idPage && idBlock
			// if return tmp.(ramUkData == NULL && tmp.swapUkData == NULL) then error
		DataLocation readPageBlock(PAGE_COUNT_TYPE idPage, PAGE_MAP_TYPE idBlock);
			// search block page MEM::RAM::ramUk by idPage && idBlock
			// return true if ok, false if error
		bool readPageBlock(PAGE_COUNT_TYPE idPage, PAGE_MAP_TYPE idBlock, BYTE &block);
			// write Page service information indicator = free/ram/swap by idPage
			// page indicator = free/ram/swap = 0/[0,RAM_MAX_PAGE]/[RAM_MAX_PAGE+1,SWAP_MAX_PAGE]
			// return true if ok, false if error
		bool writePageInd(PAGE_COUNT_TYPE idPage, PAGE_MAP_TYPE ind);
			// write map Page by Id Page MEM::RAM::ramUk		(FUNCTON DOES NOT CHANGE BYTE *Map)
			// return true if ok, false if error
		bool writePageMap(PAGE_COUNT_TYPE idPage, const BYTE *Map);
			// write data Page by Id Page MEM::RAM::ramUk		(FUNCTON DOES NOT CHANGE BYTE *Data)
			// return true if ok, false if error
		bool writePageData(PAGE_COUNT_TYPE idPage, const BYTE *Data);
			// write Block MEM::RAM::ramUk	by IdPage && idBlock
			// return true if ok, false if error
		bool writePageBlock(PAGE_COUNT_TYPE idPage, PAGE_MAP_TYPE idBlock, const BYTE *Block);
			// write Page MEM::RAM::ramUk	by IdPage
			// return true if ok, false if error
		bool writePage(PAGE_MAP_TYPE ind, const BYTE *Map, const BYTE *Data, PAGE_COUNT_TYPE idPage);
			// location = ram/swap/all = 0/1/2
			// if location > 2 then location set to 2 (force set to 2) (see it in function searchFreePage())
			// add Page to first free page (it search for free page)		(FUNCTON DOES NOT CHANGE BYTE *Page)
			// does not add page if there is no free memory = (returns false)
			// write BYTE ind, const BYTE *Map, const BYTE *Data, BYTE location to first free page founded
			// return true if ok, false if error
		bool addPage(PAGE_MAP_TYPE ind, const BYTE *Map, const BYTE *Data, BYTE location);
			// remove Page by idPage = write 0 to page indicator (free/ram/swap = 0/[0,RAM_MAX_PAGE]/[RAM_MAX_PAGE+1,SWAP_MAX_PAGE])
			// does not mark bit map block as 'free'! you need manually call updatePageInd() for do it
			// return true if ok, false if error
		bool removePage(PAGE_COUNT_TYPE idPage);
			// set 2 bits by by idPage && idBlock in map
			// return true if ok, false if error
		bool write2BitsMap(PAGE_COUNT_TYPE idPage, PAGE_MAP_TYPE idBlock, TwoBits bits);
			// get 2 bits by idPage && idBlock in map
			// return 2 bits that indicate status of bit map block (by idPage && idBlock in map)
		TwoBits read2BitsMap(PAGE_COUNT_TYPE idPage, PAGE_MAP_TYPE idBlock);
			// read all Page map by idPage and update allocated indicator first BYTE Page
			// indicates how many blocks are busy (in use)
			// 0 = all block are free, PAGE_MAP_COUNT = all blocks are allocated. may take value = [0,PAGE_MAP_COUNT]
			// returns free bytes +1 from Page or 0 if error
		__int64 updatePageInd(PAGE_COUNT_TYPE idPage);
			// location = ram/swap/all = 0/1/2
			// if location > 2 then location set to 2 (force set to 2)
			// return free data bytes available
			// use if need data <= PAGE_DATA_BLOCK_SIZE
		__int64 readFreeBytesAval(BYTE location);
			// location = ram/swap/all = 0/1/2
			// if location > 2 then location set to 2 (force set to 2)
			// return free effective data bytes available
			// use if need data > PAGE_DATA_BLOCK_SIZE
		__int64 readFreeEffectiveBytesAval(BYTE location);
			// return free effective data bytes available in page
		__int64 readPageFreeEffectiveBytes(PAGE_COUNT_TYPE idPage);
			// malloc function
			// if return Pointer with Pointer.idPage == maxValueTypeof(PAGE_COUNT_TYPE) then error
			// (for PAGE_COUNT_TYPE == BYTE this value is 255)
/**/		Pointer mallocObj(__int64 bytes, BYTE location);
			// calloc function
			// if return Pointer with Pointer.idPage == maxValueTypeof(PAGE_COUNT_TYPE) then error
			// (for PAGE_COUNT_TYPE == BYTE this value is 255)
/**/		Pointer callocObj(__int64 bytes, BYTE location);
			// free function
			// mark all Pointer link bitMap blocks as free (00) and update Page indicators
			// + set link.idPage to max type value (for BYTE == 255)
			// idPage set to max type value in constructor (for type 'BYTE', value is 255)
			// it means, that there is no allocated memory for this pointer
/**/		void freeObj(Pointer &link);
			// read Data
			// return false if error, true if ok
/**/		bool readObj(Pointer link, BYTE &buffer);
			// write BYTE *Data to Pointer link
			// write only full size data, according data size that Pointer link locate
			// data size that const BYTE *Data locate must be equal data size that Pointer link locate
			// return true if ok, false if error
/**/		bool writeObj(Pointer link, BYTE &Data);
			// free memory
		void end(void);
	};


} /* namespace mainSpace */
#endif /* RAM_H_ */
