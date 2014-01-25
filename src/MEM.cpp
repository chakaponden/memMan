/*
 * MEM.cpp
 *
 *  Created on: Jan 20, 2014
 *      Author: lenovo
 */

#include "MEM.h"

namespace mainSpace {

MEM::MEM()
{
	//fprintf(stdout, "MEM()\n");
}

MEM::~MEM()
{
	//fprintf(stdout, "~MEM()\n");
}

 // initial by how byte is needed (effective data bytes in needed)
bool MEM:: init(__int64 bytes)
{
	BYTE input[128];	// for input
	PAGE_COUNT_TYPE ramPageCount = 0, swapPageCount = 0;
	__int64 page_data = PAGE_DATA_SIZE;
	__int64 blocks = (bytes/page_data);
	if(bytes % PAGE_DATA_SIZE)
		blocks++;
	if(blocks > SWAP_MAX_PAGE)
	{
		page_data = (SWAP_MAX_PAGE*PAGE_DATA_SIZE);
		printf("Error the is no %lld bytes avaliable memory!\n", bytes);
		printf("MAX effective avaliable memory is %lld bytes !\n", page_data);
		scanf("%c", input);
		return false;
	}
	ramPageCount = blocks;
	if(blocks > RAM_MAX_PAGE)
	{
		swapPageCount = blocks - RAM_MAX_PAGE;
		ramPageCount = RAM_MAX_PAGE;
	}
	__int64 a = blocks*PAGE_DATA_SIZE;
	__int64 serv = blocks*(PAGE_MAP_SIZE+1)+2*sizeof(PAGE_COUNT_TYPE);
	if(this->ram.init(ramPageCount,swapPageCount))
	{
		printf("There are %lld effective allocated bytes\n", a);
		printf("There are %lld service allocated bytes\n", serv);
		printf("Page Data size is %lld bytes\n", (__int64)PAGE_DATA_SIZE);
		printf("Page Map size is %lld bytes\n", (__int64)PAGE_MAP_SIZE);
		printf("Page size is %lld bytes\n", (__int64)PAGE_SIZE);
		printf("One bitmap's block locate to %lld data bytes\n", (__int64)PAGE_DATA_BLOCK_SIZE);
		printf("Total allocated Pages: %lld \n", blocks);
		printf("Total allocated bytes: %lld \n", serv+a);
		printf("RAM allocated Pages: %lld \n", (__int64)(ramPageCount));
		printf("RAM allocated bytes: %lld \n", (__int64)(serv+a-(swapPageCount*PAGE_DATA_SIZE)));
		printf("SWAP allocated Pages: %lld \n", (__int64)(swapPageCount));
		printf("SWAP allocated bytes: %lld \n", (__int64)(swapPageCount*PAGE_DATA_SIZE));
		printf("Percents allocated service memory: %.5f %% \n", (float)(((float)((float)serv/(float)(serv+a)))*100));
		printf("Press 'ENTER' to draw memory map! ");
		scanf("%c", input);
		this->drawMemoryMap();
		return true;
	}
	return false;
}

// initial by how pages is needed
bool MEM:: init(PAGE_COUNT_TYPE ramPageCount = RAM_MAX_PAGE, PAGE_COUNT_TYPE swapPageCount = (SWAP_MAX_PAGE-RAM_MAX_PAGE))
{
	BYTE input[128];	// for input
	__int64 page_data = PAGE_DATA_SIZE;
	if(ramPageCount > RAM_MAX_PAGE)
	{
		printf("Error the is no %lld pages (%lld bytes) avaliable RAM memory!\n", ramPageCount, ramPageCount*page_data);
		printf("MAX effective avaliable RAM memory is %lld pages (%lld bytes) !\n", RAM_MAX_PAGE, (RAM_MAX_PAGE)*page_data);
		scanf("%c", input);
		return false;
	}
	if(swapPageCount > (SWAP_MAX_PAGE-RAM_MAX_PAGE))
	{
		printf("Error the is no %lld pages (%lld bytes) avaliable SWAP memory!\n", swapPageCount, swapPageCount*page_data);
		printf("MAX effective avaliable SWAP memory is %lld pages (%lld bytes) !\n", SWAP_MAX_PAGE, (SWAP_MAX_PAGE)*page_data);
		scanf("%c", input);
		return false;
	}
	__int64 totalPages = ramPageCount+ swapPageCount;
	__int64 a = totalPages*PAGE_DATA_SIZE;
	__int64 serv = totalPages*(PAGE_MAP_SIZE+1)+2*sizeof(PAGE_COUNT_TYPE);
	if(this->ram.init(ramPageCount,swapPageCount))
		{
			printf("There are %lld effective allocated bytes\n", a);
			printf("There are %lld service allocated bytes\n", serv);
			printf("Page Data size is %lld bytes\n", (__int64)PAGE_DATA_SIZE);
			printf("Page Map size is %lld bytes\n", (__int64)PAGE_MAP_SIZE);
			printf("Page size is %lld bytes\n", (__int64)PAGE_SIZE);
			printf("One bitmap's block locate to %lld data bytes\n", (__int64)PAGE_DATA_BLOCK_SIZE);
			printf("Total allocated Pages: %lld \n", totalPages);
			printf("Total allocated bytes: %lld \n", serv+a);
			printf("RAM allocated Pages: %lld \n", (__int64)(ramPageCount));
			printf("RAM allocated bytes: %lld \n", (__int64)(serv+a-(swapPageCount*PAGE_DATA_SIZE)));
			printf("SWAP allocated Pages: %lld \n", (__int64)(swapPageCount));
			printf("SWAP allocated bytes: %lld \n", (__int64)(swapPageCount*PAGE_DATA_SIZE));
			printf("Percents allocated service memory: %.5f %% \n", (float)(((float)((float)serv/(float)(serv+a)))*100));
			printf("Press 'ENTER' to draw memory map! ");
			scanf("%c", input);
			this->drawMemoryMap();
			return true;
		}
		return false;
}

// location = ram/swap/all = 0/1/2
// if location > 2 then locate set to 2 (force set to 2)
// return count Pages according location
PAGE_COUNT_TYPE MEM:: getPagesCount(BYTE location = 2)
{
	switch(location)
	{
		case 0:
			return this->ram.readCountPageRam();
		case 1:
			return this->ram.readCountPageSwap();
		default:
			return (this->ram.readCountPageRam() + this->ram.readCountPageSwap());
	}
}

// location = ram/swap/all = 0/1/2
// if location > 2 then location set to 2 (force set to 2)
// return free data bytes available
// use if need data <= PAGE_DATA_BLOCK_SIZE
__int64 MEM:: getFreeBytesAval(BYTE location = 2)
{
	return this->ram.readFreeBytesAval(location);
}
// location = ram/swap/all = 0/1/2
// if location > 2 then location set to 2 (force set to 2)
// return free effective data bytes available
// use if need data > PAGE_DATA_BLOCK_SIZE
__int64 MEM:: getFreeEffectiveBytesAval(BYTE location = 2)
{
	return this->ram.readFreeEffectiveBytesAval(location);
}

// malloc function
// if return Pointer with Pointer.idPage == maxValueTypeof(PAGE_COUNT_TYPE) then error
// (for PAGE_COUNT_TYPE == BYTE this value is 255)
// location = ram/swap/all = 0/1/2
// if location > 2 then location set to 2 (force set to 2)
Pointer MEM:: malloc(__int64 bytes, BYTE location = 2)
{
	return this->ram.mallocObj(bytes, location);
}
// calloc function
// if return Pointer with Pointer.idPage == maxValueTypeof(PAGE_COUNT_TYPE) then error
// (for PAGE_COUNT_TYPE == BYTE this value is 255)
// location = ram/swap/all = 0/1/2
// if location > 2 then location set to 2 (force set to 2)
Pointer MEM:: calloc(__int64 bytes, BYTE location = 2)
{
	return this->ram.callocObj(bytes, location);
}
// free function
// mark all Pointer link bitMap blocks as free (00) and update Page indicators
void MEM:: free(Pointer link)
{
	this->ram.freeObj(link);
}
// read Data
// return false if error, true if ok
bool MEM:: read(Pointer link, BYTE &buffer)
{
	return this->ram.readObj(link, buffer);
}
// write BYTE *Data to Pointer link
// write only full size data, according data size that Pointer link locate
// data size that const BYTE *Data locate must be equal data size that Pointer link locate
// return true if ok, false if error
bool MEM:: write(Pointer link, BYTE &Data)
{
	return this->ram.writeObj(link, Data);
}

// draw memory map throw ncurces
void MEM:: drawMemoryMap()
{
	__int64 ramPages = this->ram.readCountPageRam();
	__int64 swapPages = this->ram.readCountPageSwap();
	if(!ramPages && !swapPages)												// mem not initial
	{
		printf(RESETCOLOR "                              MEMORY DON'T INITIAL\n");
		printf(RESETCOLOR "\n                        YOU NEED TO INITIAL MEMORY FIRST\n");
		return;
	}
	printf(RESETCOLOR "\n                            ");printf(MAGENTA BOLD UNDERLINE "SERVICE INFORMATION COLORS:");
	printf(RESETCOLOR "\n             " PAGE_NUMS);			printf(RESETCOLOR " - page numbers");
	printf(RESETCOLOR "               sizeof('█') == byte\n");
	printf(RESETCOLOR "\n                            ");printf(MAGENTA BOLD UNDERLINE "RAM COLORS:");
	printf(RESETCOLOR "\n             " RAM_IND); 			printf(RESETCOLOR " - page indicator");
	printf("             " RAM_DATA_FREE);					printf(RESETCOLOR " - data block free\n");
	printf("             " RAM_BITS);							printf(RESETCOLOR " - page bitMap   ");
	printf("             " RAM_DATA_BUSY);					printf(RESETCOLOR " - data block busy\n");
	if(swapPages)
	{
		printf(RESETCOLOR "\n                            "); printf(MAGENTA BOLD UNDERLINE "SWAP COLORS:");
		printf(RESETCOLOR "\n             " SWAP_IND); 	printf(RESETCOLOR " - page indicator");
		printf("             " SWAP_DATA_FREE);	printf(RESETCOLOR " - data block free\n");
		printf("             " SWAP_BITS);	printf(RESETCOLOR " - page bitMap   ");
		printf("             " SWAP_DATA_BUSY);	printf(RESETCOLOR " - data block busy\n");
	}
	printf(RESETCOLOR "\n                            "); printf(MAGENTA BOLD UNDERLINE "RAM MEMORY MAP:"); printf(RESETCOLOR "\n\n");
	__int64 tmp0, tmp1, tmp2, tmp3;
	BYTE sizePageInd = sizeof(PAGE_MAP_TYPE);
	PAGE_MAP_TYPE indPage;
	TwoBits bits;
	for(tmp0 = 0; tmp0 < (2*sizeof(PAGE_COUNT_TYPE)); tmp0++)		// printf page numbers
		printf(PAGE_NUMS);
	for(tmp0 = 0; tmp0 < ramPages; tmp0++)							// printf ram pages id && swap bitMaps
	{
		for(tmp1 = 0; tmp1 < sizePageInd; tmp1++)
		{
			printf(RAM_IND);
			for(tmp2 = 0; tmp2 < PAGE_MAP_SIZE; tmp2++)
				printf(RAM_BITS);
		}
	}
	if(swapPages)
	{
		for(tmp0 = 0; tmp0 < swapPages; tmp0++)						// printf swap pages id && swap bitMaps
		{
			for(tmp1 = 0; tmp1 < sizePageInd; tmp1++)
			{
				printf(SWAP_IND);
				for(tmp2 = 0; tmp2 < PAGE_MAP_SIZE; tmp2++)
					printf(SWAP_BITS);
			}
		}
	}
	tmp0 = PAGE_MAP_COUNT;
	tmp1 = PAGE_DATA_BLOCK_SIZE;
	tmp2 = tmp0*tmp1;
	for(tmp0 = 0; tmp0 < ramPages; tmp0++)							// printf ram data blocks
	{
		this->ram.readPageInd(tmp0, indPage);
		switch(indPage)
		{
			case 0:													// all page data blocks are free
			{
				for(tmp1 = 0; tmp1 < tmp2; tmp1++)
					printf(RAM_DATA_FREE);
				break;
			}
			case PAGE_MAP_COUNT:									// all page data blocks are busy
			{
				for(tmp1 = 0; tmp1 < tmp2; tmp1++)
					printf(RAM_DATA_BUSY);
				break;
			}
			default:												// some page data blocks are free
			{														// some page data blocks are busy
																	// check details from bitMap blocks
				for(tmp1 = 0; tmp1 < PAGE_MAP_COUNT; tmp1++)
				{
					if(this->ram.read2BitsMap(tmp0, tmp1))
					{
						for(tmp3 = 0; tmp3 < PAGE_DATA_BLOCK_SIZE; tmp3++)
							printf(RAM_DATA_BUSY);
					}
					else
					{
						for(tmp3 = 0; tmp3 < PAGE_DATA_BLOCK_SIZE; tmp3++)
							printf(RAM_DATA_FREE);
					}
				}
				break;
			}
		}
	}
	if(swapPages)
	{
		printf(RESETCOLOR "\n                            "); printf(MAGENTA BOLD UNDERLINE "SWAP COLORS:");
		printf(RESETCOLOR "\n             " SWAP_IND); 	printf(RESETCOLOR " - page indicator");
		printf("             " SWAP_DATA_FREE);	printf(RESETCOLOR " - data block free\n");
		printf("             " SWAP_BITS);	printf(RESETCOLOR " - page bitMap   ");
		printf("             " SWAP_DATA_BUSY);	printf(RESETCOLOR " - data block busy\n");
		printf(RESETCOLOR "\n                            "); printf(MAGENTA BOLD UNDERLINE  "SWAP FILE MAP:"); printf(RESETCOLOR "\n\n");
		for(tmp0 = ramPages; tmp0 < swapPages+ramPages; tmp0++)				// printf swap data blocks
		{
			this->ram.readPageInd(tmp0, indPage);
			switch(indPage)
			{
				case 0:													// all page data blocks are free
				{
					for(tmp1 = 0; tmp1 < tmp2; tmp1++)
						printf(SWAP_DATA_FREE);
					break;
				}
				case PAGE_MAP_COUNT:									// all page data blocks are busy
				{
					for(tmp1 = 0; tmp1 < tmp2; tmp1++)
						printf(SWAP_DATA_BUSY);
					break;
				}
				default:												// some page data blocks are free
				{														// some page data blocks are busy
																		// check details from bitMap blocks
					for(tmp1 = 0; tmp1 < PAGE_MAP_COUNT; tmp1++)
					{
						if(this->ram.read2BitsMap(tmp0, tmp1))
						{
							for(tmp3 = 0; tmp3 < PAGE_DATA_BLOCK_SIZE; tmp3++)
								printf(SWAP_DATA_BUSY);
						}
						else
						{
							for(tmp3 = 0; tmp3 < PAGE_DATA_BLOCK_SIZE; tmp3++)
								printf(SWAP_DATA_FREE);
						}
					}
					break;
				}
			}
		}
	}
	printf(RESETCOLOR " ");
}

// clear && close all
void MEM:: end(void)
{
	this->ram.end();
}

} /* namespace mainSpace */
