/*
 * test.cpp
 *
 *  Created on: Jan 20, 2014
 *      Author: lenovo
 */

#include "MEM.h"
using namespace mainSpace;


void showMemMap(MEM man)
{
	BYTE input[128];											// for input
	system("clear");											// clear screen
	man.drawMemoryMap();										// draw memory map
	scanf("%c", input);											// press 'ENTER'
}

void example0()
{
	/* example of allocate mem for object */
	/* read-write-free operations with allocated mem/obj */
		BYTE tmp[] = {												// temp data
				 0xAA, 0xCC, 0xDD, 0xFF, 0xFF, 0xFF, 0xFF, 0xEE,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBB };
		BYTE tmpMap[] = {											// temp data
				 0x11, 0x44, 0x22, 0x81, 0x41, 0xFF, 0xFF, 0x79,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBB };
		bool ret;													// for return code
		__int64 ramPages = 2; __int64 swapPages = 5;				// pages num for allocate
		MEM man; man.init(ramPages, swapPages);						// init memory manager
		showMemMap(man);
		Pointer uk0, uk1, uk2, uk3, uk4;
		uk0 = man.calloc(52, 2);									// allocate mem for object 0
		ret = man.read(uk0, *tmp);									// read obj data to *tmp
		showMemMap(man);
		uk1 = man.calloc(140,2);									// allocate mem object 1
		showMemMap(man);
		uk2 = man.calloc(35,2);										// allocate mem object 2
		showMemMap(man);
		man.free(uk1);												// free mem object 1
		showMemMap(man);
		uk3 = man.calloc(102,2);									// allocate mem object 3
		showMemMap(man);
		uk4 = man.calloc(80,2);										// allocate mem object 4
		showMemMap(man);
		ret = man.write(uk0, *tmpMap);								// write data from *tmpMap to obj
		ret = man.read(uk0, *tmp);									// read obj data to *tmp
		uk1 = man.calloc(37,2);										// allocate mem object 1
		showMemMap(man);
		man.free(uk2);												// free mem object 2
		showMemMap(man);
		uk2 = man.calloc(162,2);									// allocate mem object 2
		showMemMap(man);
		man.free(uk2);												// free mem object 2
		showMemMap(man);
		man.free(uk0);												// free mem object 0
		showMemMap(man);
		man.free(uk1);												// free mem object 1
		showMemMap(man);
		man.free(uk3);												// free mem object 3
		showMemMap(man);
		man.free(uk4);												// free mem object 4
		showMemMap(man);
		man.end();													// end work with memory manager
}

void example1()
{
	/* for use this low-level example */
	/* you need to change MEM:: private RAM ram to MEM:: public RAM ram */
		/*
		 __int64 ramPages = 25; __int64 swapPages = 10;
		MEM man; man.init(ramPages, swapPages);
		__int64 freea = man.getFreeBytesAval(1);
		BYTE tmp[] = {
				 0xAA, 0xCC, 0xDD, 0xFF, 0xFF, 0xFF, 0xFF, 0xEE,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBB };
		BYTE tmpMap[] = {
				 0x11, 0x44, 0x22, 0x81, 0x41, 0xFF, 0xFF, 0x79,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBB };

		TwoBits bits; bits.first = false; bits.second = true;
		PAGE_MAP_TYPE ind = 0;
		BYTE block[PAGE_DATA_BLOCK_SIZE] = {0};
		BYTE map[PAGE_MAP_SIZE] = {0};
		BYTE data[PAGE_DATA_SIZE] = {0};
		man.ram.write2BitsMap(22, 3, bits);
		man.ram.writePageInd(22, 0x4A);
		man.ram.writePageBlock(22, 3, tmp);
		man.ram.writePageData(22,tmp);
		man.ram.writePageMap(22,tmpMap);

		bits = man.ram.read2BitsMap(22, 3);
		man.ram.readPageInd(22, ind);
		man.ram.readPageBlock(22, 3, *block);
		man.ram.readPageData(22,*data);
		man.ram.readPageMap(22,*map);

		BYTE *idPage = (BYTE*)&block;
		BYTE *idBlock = (BYTE*)&block+sizeof(PAGE_COUNT_TYPE);
		Pointer tmpPointer;
		memcpy(&(tmpPointer.idPage),(void*)&block,sizeof(PAGE_COUNT_TYPE));
		memcpy(&(tmpPointer.idBlock),(void*)((BYTE*)&block+sizeof(PAGE_COUNT_TYPE)),sizeof(PAGE_MAP_TYPE));

		man.end();
		*/
}

void example2()
{
	/* allocate how bytes are needed */
	/*
		__int64 bytes = 0;
		printf("HOW many bytes allocate for you?\n");
		scanf("%lld",&bytes);
		MEM man;
		man.init(bytes);
		man.end();
	*/
}

int	main(int argc, char *argv[])
{
	example0();
	return 0;
}



