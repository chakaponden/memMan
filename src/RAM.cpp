/*
 * RAM.cpp
 *
 *  Created on: Jan 20, 2014
 *      Author: lenovo
 */

#include "RAM.h"

namespace mainSpace {


RAM::RAM(void)
{
	//fprintf(stdout, "RAM()\n");
	RAM::ramUk = NULL;
}


RAM::~RAM(void)
{
	//fprintf(stdout, "~RAM()\n");
}

// initial RAM::ramUk:
bool RAM:: init(PAGE_COUNT_TYPE ramPageCount = RAM_MAX_PAGE, PAGE_COUNT_TYPE swapPageCount = (SWAP_MAX_PAGE-RAM_MAX_PAGE))
	// create RAM::ramUk && initial first byte RAM::ramUk = count of RAM::ramUk Pages = 0
{
	if(ramPageCount <= 0 && swapPageCount <= 0)
	{
		printf("Error initial ram RAM::ramUk\n");
		return false;
	}
	RAM::ramUk = NULL;
	void *tmp = NULL;
	if(!(RAM::ramUk = calloc(1,2*sizeof(PAGE_COUNT_TYPE)+ramPageCount*PAGE_SIZE+swapPageCount*(1+PAGE_MAP_SIZE))))
	{
		printf("Error initial ram RAM::ramUk\n");
		return false;
	}
	tmp = RAM::ramUk;
	memcpy(tmp,&ramPageCount,sizeof(PAGE_COUNT_TYPE));
	tmp = ((BYTE*)tmp  + sizeof(PAGE_COUNT_TYPE));
	memcpy(tmp,&swapPageCount,sizeof(PAGE_COUNT_TYPE));
	if(swapPageCount)
		this->swap.init(SWAP_FILE_NAME,swapPageCount);
	return true;
}

// return count ram Pages
PAGE_COUNT_TYPE RAM:: readCountPageRam(void)
{
	PAGE_COUNT_TYPE tmp = 0;
	if(this->ramUk)
		memcpy(&tmp,RAM::ramUk,sizeof(PAGE_COUNT_TYPE));
	return tmp;
}

// return count swap Pages
PAGE_COUNT_TYPE RAM:: readCountPageSwap(void)
{

	PAGE_COUNT_TYPE tmp = 0;
	if(this->ramUk)
	{
		void *tmpUk = RAM::ramUk;
		tmpUk = ((BYTE*)tmpUk  + sizeof(PAGE_COUNT_TYPE));
		memcpy(&tmp,tmpUk,sizeof(PAGE_COUNT_TYPE));
	}
	return tmp;
}

// search free page from RAM::ramUk;  !!! return freePageNum + 1 !!!
// if location > 2 then location set to 2 (force set to 2)
PAGE_COUNT_TYPE RAM::  searchFreePage(BYTE location = 2)		// location = ram/swap/all = 0/1/2
{
	PAGE_COUNT_TYPE startPage = 0,  endPage = 0;
	switch(location)
	{
		case 0:
		{
			startPage = 0;
			endPage = readCountPageRam();
			break;
		}
		case 1:
		{
			startPage = readCountPageRam();
			endPage = readCountPageRam() + readCountPageSwap();
			break;
		}
		default:
		{
			startPage = 0;
			endPage = readCountPageRam() + readCountPageSwap();
			break;
		}
	}
	BYTE allocate = 1;
	void *tmpUk = ((BYTE*)RAM::ramUk  + 2*sizeof(PAGE_COUNT_TYPE));
	for(;startPage < endPage; startPage++)
	{
		memcpy(&allocate,tmpUk,sizeof(BYTE));
		if(!allocate)
			return startPage+1;												// return freePageNum + 1
		tmpUk = ((BYTE*)tmpUk  + PAGE_MAP_SIZE+sizeof(PAGE_MAP_TYPE));
	}
	return 0;
}

// search data page RAM::ramUk by idPage
DataLocation  RAM:: readPageData(PAGE_COUNT_TYPE idPage)
{
	DataLocation tmp;
	PAGE_COUNT_TYPE pageCount = readCountPageRam() + readCountPageSwap();
	if(idPage >= pageCount)
	{
		printf("Error bad argument - out of range searchDataPagebyId RAM::ramUk\n");
		return tmp;
	}
	if(idPage < readCountPageRam())
		tmp.ramUkData = ((BYTE*)RAM::ramUk  + ((2*sizeof(PAGE_COUNT_TYPE))+(pageCount*(sizeof(PAGE_MAP_TYPE)+PAGE_MAP_SIZE))
											+ idPage*PAGE_DATA_SIZE));
	else
		tmp.swapUkData = this->swap.searchDataPagebyId(idPage-this->readCountPageRam());
	return tmp;
}

// search block page RAM::ramUk by idPage && idBlock
DataLocation  RAM:: readPageBlock(PAGE_COUNT_TYPE idPage, PAGE_MAP_TYPE idBlock)
{
	DataLocation tmp;
	PAGE_COUNT_TYPE pageCount = readCountPageRam() + readCountPageSwap();
	if(idPage >= pageCount)
	{
		printf("Error bad argument - out of range searchBlockPagebyId RAM::ramUk\n");
		return tmp;
	}
	if(idPage < readCountPageRam())
		tmp.ramUkData = ((BYTE*)RAM::ramUk  + ((2*sizeof(PAGE_COUNT_TYPE))+(pageCount*(sizeof(PAGE_MAP_TYPE)+PAGE_MAP_SIZE))
											+ idPage*PAGE_DATA_SIZE+idBlock*PAGE_DATA_BLOCK_SIZE));
	else
		tmp.swapUkData = this->swap.searchBlockPagebyId((idPage-this->readCountPageRam()),idBlock);
	return tmp;
}

// search map Page RAM::ramUk by idPage
void* RAM:: readPageMap(PAGE_COUNT_TYPE idPage)
{
	PAGE_COUNT_TYPE pageCount = readCountPageRam() + readCountPageSwap();
	if(idPage >= pageCount)
	{
		printf("Error bad argument - out of range searchMapPagebyId RAM::ramUk\n");
		return NULL;
	}
	return ((BYTE*)RAM::ramUk  + 2*sizeof(PAGE_COUNT_TYPE)+idPage*(1+PAGE_MAP_SIZE)+1);
}

// search allocated indicator BYTE Page RAM::ramUk by idPage
void* RAM:: readPageInd(PAGE_COUNT_TYPE idPage)
{
	PAGE_COUNT_TYPE pageCount = readCountPageRam() + readCountPageSwap();
	if(idPage >= pageCount)
	{
		printf("Error bad argument - out of range searchIndPagebyId RAM::ramUk\n");
		return NULL;
	}
	return ((BYTE*)RAM::ramUk  + 2*sizeof(PAGE_COUNT_TYPE)+idPage*(1+PAGE_MAP_SIZE));
}


// write allocated indicator PAGE_COUNT_TYPE Page RAM::ramUk by idPage
bool RAM:: writePageInd(PAGE_COUNT_TYPE idPage, PAGE_MAP_TYPE ind = 0)
{
	PAGE_COUNT_TYPE pageCount = readCountPageRam() + readCountPageSwap();
	if(idPage >= pageCount)
	{
		printf("Error bad argument - out of range writePageInd RAM::ramUk\n");
		return false;
	}
	void *ramUkData = ((BYTE*)RAM::ramUk  + 2*sizeof(PAGE_COUNT_TYPE)+idPage*(1+PAGE_MAP_SIZE));
	memcpy(ramUkData,&ind,sizeof(PAGE_MAP_TYPE));
	return true;
}

// write map Page by Id Page RAM::ramUk		(FUNCTON DOES NOT CHANGE BYTE *Map)
bool RAM:: writePageMap(PAGE_COUNT_TYPE idPage, const BYTE *Map)
{
	PAGE_COUNT_TYPE pageCount = readCountPageRam() + readCountPageSwap();
	if(idPage >= pageCount)
	{
		printf("Error bad argument - out of range writePageMap RAM::ramUk\n");
		return false;
	}
	memcpy(RAM::readPageMap(idPage),Map,PAGE_MAP_SIZE);
	return true;
}


// write data Page by Id Page RAM::ramUk		(FUNCTON DOES NOT CHANGE BYTE *Map)
bool  RAM:: writePageData(PAGE_COUNT_TYPE idPage, const BYTE *Data)
{
	DataLocation tmp;
	PAGE_COUNT_TYPE pageCount = readCountPageRam() + readCountPageSwap();
	if(idPage >= pageCount)
	{
		printf("Error bad argument - out of range writePageData RAM::ramUk\n");
		return false;
	}
	tmp = RAM::readPageData(idPage);
	if(tmp.ramUkData)
	{
		memcpy(tmp.ramUkData,Data,PAGE_DATA_SIZE);
		return true;
	}
	else
		return this->swap.writePageData(Data,idPage-readCountPageRam());
}

// add Page to RAM::ramUk		(FUNCTON DOES NOT CHANGE BYTE *Page)
// if location > 2 then location set to 2 (force set to 2) in function searchFreePage()
bool  RAM:: addPage(PAGE_MAP_TYPE ind, const BYTE *Map, const BYTE *Data, BYTE location = 2)		// location = where search free Page =
																								// ram/swap/all = 0/1/2
{
	PAGE_COUNT_TYPE freePageId = searchFreePage(location);
	if(!freePageId)
		return false;
	if(writePage(ind,Map,Data,freePageId-1))
		return true;
	return false;
}

// write Page to RAM::ramUk by Id
bool  RAM:: writePage(PAGE_MAP_TYPE ind, const BYTE *Map, const BYTE *Data, PAGE_COUNT_TYPE idPage)
{
	PAGE_COUNT_TYPE pageCount = readCountPageRam() + readCountPageSwap();
	if(idPage >= pageCount)
	{
		printf("Error bad argument - out of range writePagebyId RAM::ramUk\n");
		return false;
	}
	if(RAM::writePageInd(idPage,ind) && RAM::writePageMap(idPage,Map) && RAM::writePageData(idPage,Data))
		return true;
	return false;
}

// remove Page from RAM::ramUk by idPage
bool  RAM:: removePage(PAGE_COUNT_TYPE idPage)					// remove Page from RAM::ramUk by numPage
{
	if(RAM::writePageInd(idPage,0))
		return true;
	return false;
}

// write block RAM::ramUk by IdPage && idBlock
bool RAM:: writePageBlock(PAGE_COUNT_TYPE idPage, PAGE_MAP_TYPE idBlock, const BYTE *Block)
{
	PAGE_COUNT_TYPE pageCount = readCountPageRam() + readCountPageSwap();
	if(idPage >= pageCount)
	{
		printf("Error bad argument - out of range writePageBlockbyId RAM::ramUk\n");
		return false;
	}
	DataLocation tmp;
	tmp = RAM::readPageBlock(idPage,idBlock);
	if(tmp.ramUkData)
	{
		memcpy(tmp.ramUkData,Block,PAGE_DATA_BLOCK_SIZE);
		return true;
	}
	else
		return this->swap.writeBlockData(Block,idPage-readCountPageRam(),idBlock);
}

// free memory
void RAM:: end(void)
{
	if(RAM::ramUk != NULL)
	{
		free(RAM::ramUk);
		RAM::ramUk = NULL;
	}
	this->swap.end();
}

// set 2 bits by by idPage && idBlock in map
bool RAM:: write2BitsMap(PAGE_COUNT_TYPE idPage,PAGE_MAP_TYPE idBlock, TwoBits bits)
{
	void *tmpUk = NULL;
	PAGE_MAP_TYPE tmp = 0;
	if(NULL == (tmpUk = (readPageMap(idPage))))
		return false;
	__int64 a = PAGE_MAP_COUNT;
	if(idBlock >= a)
	{
		printf("Error bad argument - out of range block write2BitsMap RAM::ramUk\n");
		return false;
	}
	tmpUk = ((BYTE*)tmpUk  + idBlock/4);
	memcpy(&tmp,tmpUk,sizeof(PAGE_MAP_TYPE));
	PAGE_MAP_TYPE bitInByte = 2*(idBlock%4);
	BYTE number = 0b10000000;	// 10000000 == byte in bits; 0bXXXXXXXX bits format. 8 bits == 1 byte
	if(bits.first)
		tmp |= (number>>bitInByte);
	else
		tmp &= ~(number>>bitInByte);
	if(bits.second)
		tmp |= (number>>(bitInByte+1));
	else
		tmp &= ~(number>>(bitInByte+1));
	memcpy(tmpUk,&tmp,sizeof(PAGE_MAP_TYPE));
	return true;
}

// get 2 bits by by idPage && idBlock in map
TwoBits RAM:: read2BitsMap(PAGE_COUNT_TYPE idPage,PAGE_MAP_TYPE idBlock)
{
	void *tmpUk = NULL;
	TwoBits tmpBits; tmpBits.first = true; tmpBits.second = true;
	if(NULL == (tmpUk = (readPageMap(idPage))))
		return tmpBits;
	if(idBlock >= PAGE_MAP_COUNT)
	{
		printf("Error bad argument - out of range block read2BitsMap RAM::ramUk\n");
		return  tmpBits;
	}
	tmpUk = ((BYTE*)tmpUk  + idBlock/4);
	BYTE tmp = 0;
	BYTE number = 0b10000000;	// 10000000 == byte in bits; 0bXXXXXXXX bits format. 8 bits == 1 byte
	memcpy(&tmp,tmpUk,sizeof(PAGE_MAP_TYPE));
	PAGE_MAP_TYPE bitInByte = 2*(idBlock%4);
	tmpBits.first = (bool)(tmp&(number>>bitInByte));
	tmpBits.second= (bool)(tmp&(number>>(bitInByte+1)));
	return tmpBits;
}


// returns free bytes +1 from Page
// read all Page map by idPage and update allocated indicator first BYTE Page
// indicates how many blocks are busy (in use)
// 0 = all block are free, PAGE_MAP_COUNT = all blocks are allocated. may take value = [0,PAGE_MAP_COUNT]
__int64 RAM::  updatePageInd(PAGE_COUNT_TYPE idPage)
{
	//void *tmpUk = NULL;
	TwoBits tmpBits;
	tmpBits.first = tmpBits.second = 1;
	if(idPage >= readCountPageRam() + readCountPageSwap())
	{
		printf("Error bad argument - out of range Page updatePageInd RAM::ramUk\n");
		return  0;
	}
	PAGE_MAP_TYPE mapBlockId = 0;
	PAGE_MAP_TYPE mapfreeBlocks = PAGE_MAP_COUNT;
	while(mapBlockId < PAGE_MAP_COUNT)
	{
		 tmpBits = read2BitsMap(idPage, mapBlockId);
		 if(tmpBits)
			 mapfreeBlocks--;
		 mapBlockId++;
	}
	PAGE_MAP_TYPE ind = PAGE_MAP_COUNT - mapfreeBlocks;
	if(!writePageInd(idPage,ind))
		return 0;
	return (mapfreeBlocks*PAGE_DATA_BLOCK_SIZE)+1;
}

// return free effective data bytes available in page
__int64 RAM:: readPageFreeEffectiveBytes(PAGE_COUNT_TYPE idPage)
{
	if(idPage >= readCountPageRam() + readCountPageSwap())
	{
		printf("Error bad argument - out of range Page  readPageFreeEffectiveBytes RAM::ramUk\n");
		return  0;
	}
	BYTE firstFreeBlock = 1;
	__int64 effectiveBytesAval = 0;
	for(PAGE_MAP_TYPE blockNumber = 0; blockNumber < PAGE_MAP_COUNT; blockNumber++)
	{
		if(!(this->read2BitsMap(idPage, blockNumber)))
		{
			if(firstFreeBlock)
				firstFreeBlock = 0;
			else
				effectiveBytesAval += PAGE_DATA_BLOCK_SIZE;
		}
		else
			firstFreeBlock = 1;
	}
	return effectiveBytesAval;
}


// location = ram/swap/all = 0/1/2
// if location > 2 then location set to 2 (force set to 2)
// return free effective data bytes available
// use if need data > PAGE_DATA_BLOCK_SIZE
__int64 RAM:: readFreeEffectiveBytesAval(BYTE location = 2)
{
	PAGE_COUNT_TYPE startPage = 0,  endPage = 0;
		switch(location)
		{
			case 0:
			{
				startPage = 0;
				endPage = this->readCountPageRam();
				break;
			}
			case 1:
			{
				startPage = this->readCountPageRam();
				endPage = readCountPageRam() + readCountPageSwap();
				break;
			}
			default:
			{
				startPage = 0;
				endPage = readCountPageRam() + readCountPageSwap();
				break;
			}
		}
		BYTE firstFreeBlock = 1;
		__int64 effectiveBytesAval = 0;
		void *tmpUk = ((BYTE*)RAM::ramUk  + 2*sizeof(PAGE_COUNT_TYPE));
		for(;startPage < endPage; startPage++)
		{
			tmpUk = (void*)((BYTE*)tmpUk + sizeof(PAGE_MAP_TYPE));
			for(PAGE_MAP_TYPE blockNumber = 0; blockNumber < PAGE_MAP_COUNT; blockNumber++)
			{
				if(!(this->read2BitsMap(startPage, blockNumber)))
				{
					if(firstFreeBlock)
						firstFreeBlock = 0;
					else
						effectiveBytesAval += PAGE_DATA_BLOCK_SIZE;
				}
				else
					firstFreeBlock = 1;
			}
			firstFreeBlock = 1;
		}
	return effectiveBytesAval;
}

// location = ram/swap/all = 0/1/2
// if location > 2 then location set to 2 (force set to 2)
// return free data bytes available
// use if need data <= PAGE_DATA_BLOCK_SIZE
__int64 RAM:: readFreeBytesAval(BYTE location = 2)
{
	PAGE_COUNT_TYPE startPage = 0,  endPage = 0;
		switch(location)
		{
			case 0:
			{
				startPage = 0;
				endPage = this->readCountPageRam();
				break;
			}
			case 1:
			{
				startPage = this->readCountPageRam();
				endPage = readCountPageRam() + readCountPageSwap();
				break;
			}
			default:
			{
				startPage = 0;
				endPage = readCountPageRam() + readCountPageSwap();
				break;
			}
		}
		PAGE_MAP_TYPE ind = 0;
		__int64 bytesAval = 0;
		void *tmpUk = ((BYTE*)RAM::ramUk  + 2*sizeof(PAGE_COUNT_TYPE));
		for(;startPage < endPage; startPage++)
		{
			memcpy(&ind,tmpUk,sizeof(BYTE));
			bytesAval += (PAGE_MAP_COUNT-ind)*PAGE_DATA_BLOCK_SIZE;
			tmpUk = ((BYTE*)tmpUk  + PAGE_MAP_SIZE+sizeof(PAGE_MAP_TYPE));
		}
	return bytesAval;
}

// read allocated indicator BYTE Page MEM::RAM::ramUk by idPage
// return true if ok, false if error
bool RAM:: readPageInd(PAGE_COUNT_TYPE idPage, PAGE_MAP_TYPE &ind)
{
	void *tmp = this->readPageInd(idPage);
	if(tmp == NULL)
		return false;
	memcpy(&ind,tmp,sizeof(PAGE_MAP_TYPE));
	return true;
}

// read map Page MEM::RAM::ramUk by idPage
// return true if ok, false if error
bool RAM:: readPageMap(PAGE_COUNT_TYPE idPage, BYTE &map)
{
	void *tmp = this->readPageMap(idPage);
	if(tmp == NULL)
		return false;
	memcpy(&map,tmp,PAGE_MAP_SIZE);
	return true;
}

// read page Data by idPage
// return true if ok, false if error
bool RAM:: readPageData(PAGE_COUNT_TYPE idPage, BYTE &data)
{
	DataLocation tmp = this->readPageData(idPage);
	if(tmp.ramUkData == NULL && tmp.swapUkData == NULL)
		return false;
	if(tmp.ramUkData)
	{
		memcpy(&data,tmp.ramUkData,PAGE_DATA_SIZE);
		return true;
	}
	else
		return this->swap.readData(idPage-readCountPageRam(), data);
}

// search block page MEM::RAM::ramUk by idPage && idBlock
// return true if ok, false if error
bool RAM:: readPageBlock(PAGE_COUNT_TYPE idPage, PAGE_MAP_TYPE idBlock, BYTE &block)
{
	DataLocation tmp = this->readPageBlock(idPage, idBlock);
	if(tmp.ramUkData == NULL && tmp.swapUkData == NULL)
		return false;
	if(tmp.ramUkData)
	{
		memcpy(&block,tmp.ramUkData,PAGE_DATA_BLOCK_SIZE);
		return true;
	}
	else
		return this->swap.readBlock(idPage-readCountPageRam(),idBlock,block);
}

// malloc function
// if return Pointer with Pointer.idPage == maxValueTypeof(PAGE_COUNT_TYPE) then error
// (for PAGE_COUNT_TYPE == BYTE this value is 255)
Pointer RAM:: mallocObj(__int64 bytes, BYTE location = 2)
{
	Pointer obj;
	TwoBits busyBits; busyBits.first = false; busyBits.second = true;		// '01' == block is busy bits
	TwoBits endBits; endBits.first = true; endBits.second = false;			// '10' == last block bits
	TwoBits paramBits; paramBits.first = true; paramBits.second = true;		// '11' == continue parameters (idPage && idBlock) block bits
	BYTE continueBlock[PAGE_DATA_BLOCK_SIZE] = {0};
	BYTE busyMap[PAGE_MAP_SIZE];											// 01010101 in bits. all BitMap blocks set to '01' == busy
	for(PAGE_MAP_TYPE z = 0; z < PAGE_MAP_SIZE; z++)
		busyMap[z] = 0b01010101;PAGE_COUNT_TYPE startPage = 0,  endPage = 0;
	switch(location)
	{
		case 0:
		{
			startPage = 0;
			endPage = this->readCountPageRam();
			break;
		}
		case 1:
		{
			startPage = this->readCountPageRam();
			endPage = readCountPageRam() + readCountPageSwap();
			break;
		}
		default:
		{
			startPage = 0;
			endPage = readCountPageRam() + readCountPageSwap();
			break;
		}
	}

	void *tmpUk = ((BYTE*)RAM::ramUk  + 2*sizeof(PAGE_COUNT_TYPE));

	if(bytes <= PAGE_DATA_BLOCK_SIZE)
	{
		PAGE_MAP_TYPE ind = 0;
		for(;startPage < endPage; startPage++)
		{
			memcpy(&ind,tmpUk,sizeof(BYTE));
			if(ind < PAGE_MAP_COUNT)											// if free data block exist in page
			{
				for(PAGE_MAP_TYPE blockNumber = 0; blockNumber < PAGE_MAP_COUNT; blockNumber++)
				{
					if(!(this->read2BitsMap(startPage, blockNumber)))			// if free block found (bitMap block value == '00')
					{
						obj.idPage = startPage;
						obj.idBlock = blockNumber;
						this->write2BitsMap(startPage,blockNumber,endBits);		// write bits as 'busy and it's last data block' == '10'
						break;
					}
				}
				this->updatePageInd(startPage);
				return obj;														// return obj
			}
			tmpUk = ((BYTE*)tmpUk  + PAGE_MAP_SIZE+sizeof(PAGE_MAP_TYPE));		// tmpUk to next page indicator
		}
		obj.idPage = SWAP_MAX_PAGE;												// if no free block found
		obj.idPage++;															// set idPage to maxTypeValue (it indicates error)
		printf("Error the is no %lld bytes avaliable memory!\n", bytes);
		printf("Total free memory is %lld bytes !\n", this->readFreeBytesAval(location));
		printf("MAX effective avaliable memory is %lld bytes !\n", this->readFreeEffectiveBytesAval(location));
		return obj;																// for type 'BYTE' idPage value == 255 if error
	}
	else
	{
		if(readFreeEffectiveBytesAval(location) < bytes)
		{
			printf("Error the is no %lld bytes avaliable memory!\n", bytes);
			printf("Total free memory is %lld bytes !\n", this->readFreeBytesAval(location));
			printf("MAX effective avaliable memory is %lld bytes !\n", this->readFreeEffectiveBytesAval(location));
			obj.idPage = SWAP_MAX_PAGE;											// if no free requested bytes
			obj.idPage++;														// set idPage to maxTypeValue (it indicates error)
			return obj;															// for type 'BYTE' idPage value == 255 if error
		}
		PAGE_COUNT_TYPE idPage;
		if(bytes >= PAGE_DATA_SIZE)												// if requested data size >= page data size
		{
			if((idPage = this->searchFreePage(location)))						// free page found
			{
				this->writePageMap(idPage-1,busyMap);							// set all BitMap blocks to '01' == busy
				this->updatePageInd(idPage-1);
				if(bytes == PAGE_DATA_SIZE)
					this->write2BitsMap(idPage-1,PAGE_MAP_COUNT-1,endBits);		// mark last bitMap block as last data block (set to '10')
				else															// if requested bytes > page data size
				{
					Pointer continuePoint;
					this->write2BitsMap(idPage-1,PAGE_MAP_COUNT-1,paramBits);	// mark last bitMap block contains parameters (set to '11')
																				// parameters == idPage && idBlock
					continuePoint = this->mallocObj(bytes-(PAGE_DATA_SIZE-1), location);
					memcpy((void*)&continueBlock,&(continuePoint.idPage),sizeof(PAGE_COUNT_TYPE));
					memcpy((void*)((BYTE*)&continueBlock+sizeof(PAGE_COUNT_TYPE)),&(continuePoint.idBlock),sizeof(PAGE_MAP_TYPE));
					this->writePageBlock(idPage-1,PAGE_MAP_COUNT-1,continueBlock);
				}
				obj.idPage = idPage-1;
				obj.idBlock = 0;
				return obj;
			}
			else																// no free page found
			{
				PAGE_COUNT_TYPE idPageLargeFreeBytes = 0;
				__int64 largeFreeBytes = 0;
				for(;startPage < endPage; startPage++)
				{
					if(this->readPageFreeEffectiveBytes(startPage) > largeFreeBytes)
					{
						idPageLargeFreeBytes = startPage;
						largeFreeBytes = this->readPageFreeEffectiveBytes(startPage);
					}
				}
				BYTE firstFreeBlock = 1, blockAllocate = 1, firstBlockAllocate = 1;
				PAGE_MAP_TYPE endBlock = 0, startBlock = 0, firstStartBlock;
				Pointer continuePoint;
				for(PAGE_MAP_TYPE blockNumber = 0; blockNumber < PAGE_MAP_COUNT; blockNumber++)
				{
					if(!(this->read2BitsMap(idPageLargeFreeBytes, blockNumber)))
					{
						if(firstFreeBlock)															// if it's first free block
							firstFreeBlock = 0;
						else
						{
							if(blockAllocate)														// if this block is second free solid block
							{
								blockAllocate = 0;
								startBlock = blockNumber-1;
								if(firstBlockAllocate)												// if first free block in whole page
								{																	// then remember it's block number
									firstBlockAllocate = 0;
									firstStartBlock = blockNumber-1;
								}
								else	// if it's no first free second block
								{		// in whole page
										// then save it's start block in previous free endBlock this page
										// (write idPage && idBlock to previous free endBlock this page)
									this->write2BitsMap(idPageLargeFreeBytes, endBlock, paramBits);
										// set bitMap block to '11' == parameters block
									continuePoint.idPage = idPageLargeFreeBytes; continuePoint.idBlock = startBlock;
									memcpy((void*)&continueBlock,&(continuePoint.idPage),sizeof(PAGE_COUNT_TYPE));
									memcpy((void*)((BYTE*)&continueBlock+sizeof(PAGE_COUNT_TYPE)),&(continuePoint.idBlock),sizeof(PAGE_MAP_TYPE));
									this->writePageBlock(idPageLargeFreeBytes,endBlock,continueBlock);

								}
										// set first free bitMap block to '01' == busy block
								this->write2BitsMap(idPageLargeFreeBytes, blockNumber-1, busyBits);
							}
										// set second free bitMap block to '01' == busy block
							this->write2BitsMap(idPageLargeFreeBytes, blockNumber, busyBits);
							endBlock = blockNumber;													// set endBlock for save parameters in it
						}																			// for future
					}
					else
					{
						blockAllocate = 1;
						firstFreeBlock = 1;
					}
				}
				this->write2BitsMap(idPageLargeFreeBytes,endBlock,paramBits);	// mark last bitMap block contains parameters (set to '11')
																				// parameters == idPage && idBlock
				this->updatePageInd(idPageLargeFreeBytes);
				continuePoint = this->mallocObj(bytes-largeFreeBytes, location);
				memcpy((void*)&continueBlock,&(continuePoint.idPage),sizeof(PAGE_COUNT_TYPE));
				memcpy((void*)((BYTE*)&continueBlock+sizeof(PAGE_COUNT_TYPE)),&(continuePoint.idBlock),sizeof(PAGE_MAP_TYPE));
				this->writePageBlock(idPageLargeFreeBytes,endBlock,continueBlock);
				obj.idPage = idPageLargeFreeBytes;
				obj.idBlock = firstStartBlock;
				return obj;
			}
		}
		else																					// if requested data size < page data size
		{
			PAGE_COUNT_TYPE idPageLargeFreeBytes = 0;
			__int64 largeFreeBytes = 0;
			for(;startPage < endPage; startPage++)
			{
				if(this->readPageFreeEffectiveBytes(startPage) > largeFreeBytes)
				{
					idPageLargeFreeBytes = startPage;
					largeFreeBytes = this->readPageFreeEffectiveBytes(startPage);
				}
			}
			BYTE firstFreeBlock = 1, blockAllocate = 1, firstBlockAllocate = 1;
			PAGE_MAP_TYPE endBlock = 0, startBlock = 0, firstStartBlock;
			Pointer continuePoint;
			__int64 effectiveDataBytesWritten = bytes;											// if effectiveDataBytesWritten <= 0, then page data is full
			for(PAGE_MAP_TYPE blockNumber = 0; effectiveDataBytesWritten > 0 && blockNumber < PAGE_MAP_COUNT; blockNumber++)	// and there are no free data blocks in this page
			{
				if(!(this->read2BitsMap(idPageLargeFreeBytes, blockNumber)))
				{
					if(firstFreeBlock)															// if it's first free block
						firstFreeBlock = 0;
					else
					{
						if(blockAllocate)														// if this block is second free solid block
						{
							blockAllocate = 0;
							startBlock = blockNumber-1;
							if(firstBlockAllocate)												// if first free block in whole page
							{																	// then remember it's block number
								firstBlockAllocate = 0;
								firstStartBlock = blockNumber-1;
							}
							else	// if it's no first free second block
							{		// in whole page
									// then save it's start block in previous free endBlock this page
									// (write idPage && idBlock to previous free endBlock this page)
								this->write2BitsMap(idPageLargeFreeBytes, endBlock, paramBits);
								effectiveDataBytesWritten += PAGE_DATA_BLOCK_SIZE;
									// set bitMap block to '11' == parameters block
								continuePoint.idPage = idPageLargeFreeBytes; continuePoint.idBlock = startBlock;
								memcpy((void*)&continueBlock,&(continuePoint.idPage),sizeof(PAGE_COUNT_TYPE));
								memcpy((void*)((BYTE*)&continueBlock+sizeof(PAGE_COUNT_TYPE)),&(continuePoint.idBlock),sizeof(PAGE_MAP_TYPE));
								this->writePageBlock(idPageLargeFreeBytes,endBlock,continueBlock);
							}
									// set first free bitMap block to '01' == busy block
							this->write2BitsMap(idPageLargeFreeBytes, blockNumber-1, busyBits);
							effectiveDataBytesWritten -= PAGE_DATA_BLOCK_SIZE;
						}
									// set second free bitMap block to '01' == busy block
						this->write2BitsMap(idPageLargeFreeBytes, blockNumber, busyBits);
						effectiveDataBytesWritten -= PAGE_DATA_BLOCK_SIZE;
						endBlock = blockNumber;													// set endBlock for save parameters in it
					}																			// for future
				}
				else
				{
					blockAllocate = 1;
					firstFreeBlock = 1;
				}
			}
			if(largeFreeBytes < bytes)											// if all free page data blocks are already mark as busy ('01')
			{																	// but it's not enough (need more data), call recursive
				this->write2BitsMap(idPageLargeFreeBytes,endBlock,paramBits);	// mark last bitMap block contains parameters (set to '11')
																				// parameters == idPage && idBlock
				effectiveDataBytesWritten += PAGE_DATA_BLOCK_SIZE;
				continuePoint = this->mallocObj(bytes-largeFreeBytes, location);
				memcpy((void*)&continueBlock,&(continuePoint.idPage),sizeof(PAGE_COUNT_TYPE));
				memcpy((void*)((BYTE*)&continueBlock+sizeof(PAGE_COUNT_TYPE)),&(continuePoint.idBlock),sizeof(PAGE_MAP_TYPE));
				this->writePageBlock(idPageLargeFreeBytes,endBlock,continueBlock);
			}
			else
				this->write2BitsMap(idPageLargeFreeBytes,endBlock,endBits);		// mark last bitMap block as last data block (set to '10')
			this->updatePageInd(idPageLargeFreeBytes);
			obj.idPage = idPageLargeFreeBytes;
			obj.idBlock = firstStartBlock;
			return obj;
		}
	}
	return obj;
}
// calloc function
// if return Pointer with Pointer.idPage == maxValueTypeof(PAGE_COUNT_TYPE) then error
// (for PAGE_COUNT_TYPE == BYTE this value is 255)
Pointer RAM:: callocObj(__int64 bytes, BYTE location = 2)
{
	Pointer obj;
	TwoBits busyBits; busyBits.first = false; busyBits.second = true;		// '01' == block is busy bits
	TwoBits endBits; endBits.first = true; endBits.second = false;			// '10' == last block bits
	TwoBits paramBits; paramBits.first = true; paramBits.second = true;		// '11' == continue parameters (idPage && idBlock) block bits
	BYTE freeBlock[PAGE_DATA_BLOCK_SIZE] = {0};
	BYTE freePage[PAGE_DATA_SIZE] = {0};
	BYTE continueBlock[PAGE_DATA_BLOCK_SIZE] = {0};
	BYTE busyMap[PAGE_MAP_SIZE];											// 01010101 in bits. all BitMap blocks set to '01' == busy
	for(PAGE_MAP_TYPE z = 0; z < PAGE_MAP_SIZE; z++)
		busyMap[z] = 0b01010101;
	PAGE_COUNT_TYPE startPage = 0,  endPage = 0;
	switch(location)
	{
		case 0:
		{
			startPage = 0;
			endPage = this->readCountPageRam();
			break;
		}
		case 1:
		{
			startPage = this->readCountPageRam();
			endPage = readCountPageRam() + readCountPageSwap();
			break;
		}
		default:
		{
			startPage = 0;
			endPage = readCountPageRam() + readCountPageSwap();
			break;
		}
	}

	void *tmpUk = ((BYTE*)RAM::ramUk  + 2*sizeof(PAGE_COUNT_TYPE));

	if(bytes <= PAGE_DATA_BLOCK_SIZE)
	{
		PAGE_MAP_TYPE ind = 0;
		for(;startPage < endPage; startPage++)
		{
			memcpy(&ind,tmpUk,sizeof(BYTE));
			if(ind < PAGE_MAP_COUNT)											// if free data block exist in page
			{
				for(PAGE_MAP_TYPE blockNumber = 0; blockNumber < PAGE_MAP_COUNT; blockNumber++)
				{
					if(!(this->read2BitsMap(startPage, blockNumber)))			// if free block found (bitMap block value == '00')
					{
						obj.idPage = startPage;
						obj.idBlock = blockNumber;
						this->write2BitsMap(startPage,blockNumber,endBits);		// write bits as 'busy and it's last data block' == '10'
						this->writePageBlock(startPage,blockNumber,freeBlock);	// free data block (set all block bytes to '0')
						break;
					}
				}
				this->updatePageInd(startPage);
				return obj;														// return obj
			}
			tmpUk = ((BYTE*)tmpUk  + PAGE_MAP_SIZE+sizeof(PAGE_MAP_TYPE));		// tmpUk to next page indicator
		}
		printf("Error the is no %lld bytes avaliable memory!\n", bytes);
		printf("Total free memory is %lld bytes !\n", this->readFreeBytesAval(location));
		printf("MAX effective avaliable memory is %lld bytes !\n", this->readFreeEffectiveBytesAval(location));
		obj.idPage = SWAP_MAX_PAGE;												// if no free block found
		obj.idPage++;															// set idPage to maxTypeValue (it indicates error)
		return obj;																// for type 'BYTE' idPage value == 255 if error
	}
	else
	{
		if(readFreeEffectiveBytesAval(location) < bytes)
		{
			printf("Error the is no %lld bytes avaliable memory!\n", bytes);
			printf("Total free memory is %lld bytes !\n", this->readFreeBytesAval(location));
			printf("MAX effective avaliable memory is %lld bytes !\n", this->readFreeEffectiveBytesAval(location));
			obj.idPage = SWAP_MAX_PAGE;											// if no free requested bytes
			obj.idPage++;														// set idPage to maxTypeValue (it indicates error)
			return obj;															// for type 'BYTE' idPage value == 255 if error
		}
		PAGE_COUNT_TYPE idPage;
		if(bytes >= PAGE_DATA_SIZE)												// if requested data size >= page data size
		{
			if((idPage = this->searchFreePage(location)))						// free page found
			{
				this->writePageMap(idPage-1,busyMap);							// set all BitMap blocks to '01' == busy
				this->updatePageInd(idPage-1);
				this->writePageData(idPage-1,freePage);						// free page data (set all page bytes to '0')
				if(bytes == PAGE_DATA_SIZE)
					this->write2BitsMap(idPage-1,PAGE_MAP_COUNT-1,endBits);		// mark last bitMap block as last data block (set to '10')
				else															// if requested bytes > page data size
				{
					Pointer continuePoint;
					this->write2BitsMap(idPage-1,PAGE_MAP_COUNT-1,paramBits);	// mark last bitMap block contains parameters (set to '11')
																				// parameters == idPage && idBlock
					continuePoint = this->callocObj(bytes-(PAGE_DATA_SIZE-1), location);
					memcpy((void*)&continueBlock,&(continuePoint.idPage),sizeof(PAGE_COUNT_TYPE));
					memcpy((void*)((BYTE*)&continueBlock+sizeof(PAGE_COUNT_TYPE)),&(continuePoint.idBlock),sizeof(PAGE_MAP_TYPE));
					this->writePageBlock(idPage-1,PAGE_MAP_COUNT-1,continueBlock);
				}
				obj.idPage = idPage-1;
				obj.idBlock = 0;
				return obj;
			}
			else																// no free page found
			{
				PAGE_COUNT_TYPE idPageLargeFreeBytes = 0;
				__int64 largeFreeBytes = 0;
				for(;startPage < endPage; startPage++)
				{
					if(this->readPageFreeEffectiveBytes(startPage) > largeFreeBytes)
					{
						idPageLargeFreeBytes = startPage;
						largeFreeBytes = this->readPageFreeEffectiveBytes(startPage);
					}
				}
				BYTE firstFreeBlock = 1, blockAllocate = 1, firstBlockAllocate = 1;
				PAGE_MAP_TYPE endBlock = 0, startBlock = 0, firstStartBlock;
				Pointer continuePoint;
				for(PAGE_MAP_TYPE blockNumber = 0; blockNumber < PAGE_MAP_COUNT; blockNumber++)
				{
					if(!(this->read2BitsMap(idPageLargeFreeBytes, blockNumber)))
					{
						if(firstFreeBlock)															// if it's first free block
							firstFreeBlock = 0;
						else
						{
							if(blockAllocate)														// if this block is second free solid block
							{
								blockAllocate = 0;
								startBlock = blockNumber-1;
								if(firstBlockAllocate)												// if first free block in whole page
								{																	// then remember it's block number
									firstBlockAllocate = 0;
									firstStartBlock = blockNumber-1;
								}
								else	// if it's no first free second block
								{		// in whole page
										// then save it's start block in previous free endBlock this page
										// (write idPage && idBlock to previous free endBlock this page)
									this->write2BitsMap(idPageLargeFreeBytes, endBlock, paramBits);
										// set bitMap block to '11' == parameters block
									continuePoint.idPage = idPageLargeFreeBytes; continuePoint.idBlock = startBlock;
									memcpy((void*)&continueBlock,&(continuePoint.idPage),sizeof(PAGE_COUNT_TYPE));
									memcpy((void*)((BYTE*)&continueBlock+sizeof(PAGE_COUNT_TYPE)),&(continuePoint.idBlock),sizeof(PAGE_MAP_TYPE));
									this->writePageBlock(idPageLargeFreeBytes,endBlock,continueBlock);

								}
										// set first free bitMap block to '01' == busy block
								this->write2BitsMap(idPageLargeFreeBytes, blockNumber-1, busyBits);
								this->writePageBlock(idPageLargeFreeBytes,blockNumber-1,freeBlock);	// free data block (set all block bytes to '0')
							}
										// set second free bitMap block to '01' == busy block
							this->write2BitsMap(idPageLargeFreeBytes, blockNumber, busyBits);
							this->writePageBlock(idPageLargeFreeBytes,blockNumber,freeBlock);		// free data block (set all block bytes to '0')
							endBlock = blockNumber;													// set endBlock for save parameters in it
						}																			// for future
					}
					else
					{
						blockAllocate = 1;
						firstFreeBlock = 1;
					}
				}
				this->write2BitsMap(idPageLargeFreeBytes,endBlock,paramBits);	// mark last bitMap block contains parameters (set to '11')
																				// parameters == idPage && idBlock
				this->updatePageInd(idPageLargeFreeBytes);
				continuePoint = this->mallocObj(bytes-largeFreeBytes, location);
				memcpy((void*)&continueBlock,&(continuePoint.idPage),sizeof(PAGE_COUNT_TYPE));
				memcpy((void*)((BYTE*)&continueBlock+sizeof(PAGE_COUNT_TYPE)),&(continuePoint.idBlock),sizeof(PAGE_MAP_TYPE));
				this->writePageBlock(idPageLargeFreeBytes,endBlock,continueBlock);
				obj.idPage = idPageLargeFreeBytes;
				obj.idBlock = firstStartBlock;
				return obj;
			}
		}
		else																					// if requested data size < page data size
		{
			PAGE_COUNT_TYPE idPageLargeFreeBytes = 0;
			__int64 largeFreeBytes = 0;
			for(;startPage < endPage; startPage++)
			{
				if(this->readPageFreeEffectiveBytes(startPage) > largeFreeBytes)
				{
					idPageLargeFreeBytes = startPage;
					largeFreeBytes = this->readPageFreeEffectiveBytes(startPage);
				}
			}
			BYTE firstFreeBlock = 1, blockAllocate = 1, firstBlockAllocate = 1;
			PAGE_MAP_TYPE endBlock = 0, startBlock = 0, firstStartBlock;
			Pointer continuePoint;
			__int64 effectiveDataBytesWritten = bytes;									// if effectiveDataBytesWritten <= 0, then page data is full
			for(PAGE_MAP_TYPE blockNumber = 0; effectiveDataBytesWritten > 0 && blockNumber < PAGE_MAP_COUNT; blockNumber++)	// and there are no free data blocks in this page
			{
				if(!(this->read2BitsMap(idPageLargeFreeBytes, blockNumber)))
				{
					if(firstFreeBlock)															// if it's first free block
						firstFreeBlock = 0;
					else
					{
						if(blockAllocate)														// if this block is second free solid block
						{
							blockAllocate = 0;
							startBlock = blockNumber-1;
							if(firstBlockAllocate)												// if first free block in whole page
							{																	// then remember it's block number
								firstBlockAllocate = 0;
								firstStartBlock = blockNumber-1;
							}
							else	// if it's no first free second block
							{		// in whole page
									// then save it's start block in previous free endBlock this page
									// (write idPage && idBlock to previous free endBlock this page)
								this->write2BitsMap(idPageLargeFreeBytes, endBlock, paramBits);
								effectiveDataBytesWritten += PAGE_DATA_BLOCK_SIZE;
									// set bitMap block to '11' == parameters block
								continuePoint.idPage = idPageLargeFreeBytes; continuePoint.idBlock = startBlock;
								memcpy((void*)&continueBlock,&(continuePoint.idPage),sizeof(PAGE_COUNT_TYPE));
								memcpy((void*)((BYTE*)&continueBlock+sizeof(PAGE_COUNT_TYPE)),&(continuePoint.idBlock),sizeof(PAGE_MAP_TYPE));
								this->writePageBlock(idPageLargeFreeBytes,endBlock,continueBlock);
							}
									// set first free bitMap block to '01' == busy block
							this->write2BitsMap(idPageLargeFreeBytes, blockNumber-1, busyBits);
							this->writePageBlock(idPageLargeFreeBytes,blockNumber-1,freeBlock);	// free data block (set all block bytes to '0')
							effectiveDataBytesWritten -= PAGE_DATA_BLOCK_SIZE;
						}
									// set second free bitMap block to '01' == busy block
						this->write2BitsMap(idPageLargeFreeBytes, blockNumber, busyBits);
						this->writePageBlock(idPageLargeFreeBytes,blockNumber,freeBlock);		// free data block (set all block bytes to '0')
						effectiveDataBytesWritten -= PAGE_DATA_BLOCK_SIZE;
						endBlock = blockNumber;													// set endBlock for save parameters in it
					}																			// for future
				}
				else
				{
					blockAllocate = 1;
					firstFreeBlock = 1;
				}
			}
			if(largeFreeBytes < bytes)											// if all free page data blocks are already mark as busy ('01')
			{																	// but it's not enough (need more data), call recursive
				this->write2BitsMap(idPageLargeFreeBytes,endBlock,paramBits);	// mark last bitMap block contains parameters (set to '11')
																				// parameters == idPage && idBlock
				effectiveDataBytesWritten += PAGE_DATA_BLOCK_SIZE;
				continuePoint = this->mallocObj(bytes-largeFreeBytes, location);
				memcpy((void*)&continueBlock,&(continuePoint.idPage),sizeof(PAGE_COUNT_TYPE));
				memcpy((void*)((BYTE*)&continueBlock+sizeof(PAGE_COUNT_TYPE)),&(continuePoint.idBlock),sizeof(PAGE_MAP_TYPE));
				this->writePageBlock(idPageLargeFreeBytes,endBlock,continueBlock);
			}
			else
			{
				this->write2BitsMap(idPageLargeFreeBytes,endBlock,endBits);		// mark last bitMap block as last data block (set to '10')
				this->writePageBlock(idPageLargeFreeBytes,endBlock,freeBlock);	// free data block (set all block bytes to '0')
			}
			this->updatePageInd(idPageLargeFreeBytes);
			obj.idPage = idPageLargeFreeBytes;
			obj.idBlock = firstStartBlock;
			return obj;
		}
	}
	return obj;
}
// free function
// mark all Pointer link bitMap blocks as free (00) and update Page indicators
// + set link.idPage to max type value (for BYTE == 255)
// idPage set to max type value in constructor (for type 'BYTE', value is 255)
// it means, that there is no allocated memory for this pointer
void RAM:: freeObj(Pointer &link)
{
	if(link.idPage > SWAP_MAX_PAGE)											// if Pointer link is Null pointer (see Pointer struct)
		return;
	TwoBits readBits, eraseBits;
	DataLocation tmpLocation;
	Pointer tmpPointer;
	BYTE readBlock[PAGE_DATA_BLOCK_SIZE];
	while(!(readBits.first == true))										// if bitMap block == '10' || '11' =>
	{																		// data block contains data and it is the last block
		readBits = this->read2BitsMap(link.idPage, link.idBlock);			// or data block contains parameters of continue (idPage && idBlock)
																			// and it's processing in recursive function call

		this->write2BitsMap(link.idPage, link.idBlock, eraseBits);			// erase 2bits (set to '00' == free)
		if(readBits.first == true && readBits.second == true)				// if bitMap block == '11' =>
		{																	// data block contains parameters of continue (idPage && idBlock)
			this->readPageBlock(link.idPage, link.idBlock, *readBlock);
			memcpy(&(tmpPointer.idPage),(void*)&readBlock,sizeof(PAGE_COUNT_TYPE));
			memcpy(&(tmpPointer.idBlock),(void*)((BYTE*)&readBlock+sizeof(PAGE_COUNT_TYPE)),sizeof(PAGE_MAP_TYPE));
			this->freeObj(tmpPointer);										// recursive function call
		}
		link.idBlock++;
	}
	this->updatePageInd(link.idPage);										// update page indicator
	link.idPage = SWAP_MAX_PAGE; link.idPage++;
}
// read Data
// return false if error, true if ok
bool RAM:: readObj(Pointer link, BYTE &buffer)
{
	TwoBits readBits;
	Pointer tmpPointer;
	BYTE *tmpUk = &buffer;
	BYTE readBlock[PAGE_DATA_BLOCK_SIZE];
	while(!(readBits.first == true && readBits.second == false))			// if bitMap block == '10' =>
	{																		// data block contains data and it is the last block (then exit)
		readBits = this->read2BitsMap(link.idPage, link.idBlock);
		if(!(this->readPageBlock(link.idPage, link.idBlock, *readBlock)))
			return false;
		if(readBits.first == true && readBits.second == true)				// if bitMap block == '11' =>
		{																	// data block contains parameters of continue (idPage && idBlock)
																			// and it's processing in recursive function call
			memcpy(&(tmpPointer.idPage),(void*)&readBlock,sizeof(PAGE_COUNT_TYPE));
			memcpy(&(tmpPointer.idBlock),(void*)((BYTE*)&readBlock+sizeof(PAGE_COUNT_TYPE)),sizeof(PAGE_MAP_TYPE));
			return this->readObj(tmpPointer, *tmpUk);						// recursive function call
		}
		memcpy((void*)tmpUk,(void*)&readBlock,PAGE_DATA_BLOCK_SIZE);
		link.idBlock++;
		tmpUk += PAGE_DATA_BLOCK_SIZE;
	}
	return true;
}
// write BYTE *Data to Pointer link
// write only full size data, according data size that Pointer link locate
// data size that const BYTE *Data locate must be equal data size that Pointer link locate
// return true if ok, false if error
bool RAM:: writeObj(Pointer link, BYTE &Data)
{
	TwoBits readBits;
	Pointer tmpPointer;
	BYTE *tmpUk = &Data;
	BYTE readBlock[PAGE_DATA_BLOCK_SIZE];
	while(!(readBits.first == true && readBits.second == false))			// if bitMap block == '10' =>
	{																		// data block contains data and it is the last block (then exit)
		readBits = this->read2BitsMap(link.idPage, link.idBlock);
		if(readBits.first == true && readBits.second == true)				// if bitMap block == '11' =>
		{																	// data block contains parameters of continue (idPage && idBlock)
																			// and it's processing in recursive function call
			if(!(this->readPageBlock(link.idPage, link.idBlock, *readBlock)))
				return false;
			memcpy(&(tmpPointer.idPage),(void*)&readBlock,sizeof(PAGE_COUNT_TYPE));
			memcpy(&(tmpPointer.idBlock),(void*)((BYTE*)&readBlock+sizeof(PAGE_COUNT_TYPE)),sizeof(PAGE_MAP_TYPE));
			return this->writeObj(tmpPointer, *tmpUk);						// recursive function call
		}
		if(!(this->writePageBlock(link.idPage, link.idBlock, tmpUk)))
			return false;
		tmpUk += PAGE_DATA_BLOCK_SIZE;
		link.idBlock++;
	}
	return true;
}

} /* namespace mainSpace */
