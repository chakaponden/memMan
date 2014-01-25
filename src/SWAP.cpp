/*
 * SWAP.cpp
 *
 *  Created on: Jan 20, 2014
 *      Author: lenovo
 */

#include "SWAP.h"
#include <unistd.h>

namespace mainSpace {


SWAP::SWAP(void)
{
	//fprintf(stdout, "SWAP()\n");
	SWAP::swapUk = NULL;
}
// close file
SWAP::~SWAP(void)
{
	//fprintf(stdout, "~SWAP()\n");
}

// initial SWAP::swapUk file
bool SWAP:: init(const char *fileName, PAGE_COUNT_TYPE swapPageCount = (SWAP_MAX_PAGE-RAM_MAX_PAGE))
	// create SWAP::swapUk && initial first byte SWAP::swapUk = count of SWAP::swapUk Pages = 0
{
	if(swapPageCount <= 0)
	{
		printf("Error initial swap\n");
		return false;
	}
	BYTE emptyPage[PAGE_DATA_SIZE] = {0};
	if(!(SWAP::swapUk = fopen(fileName,"wb+")))
	{
		printf("Error creating init SWAP::swapUk.dat\n");
		return false;
	}
	PAGE_COUNT_TYPE i = 0;
	while(i < swapPageCount)
	{
		if(!fwrite(emptyPage,PAGE_DATA_SIZE,1,SWAP::swapUk))
		{
			fclose(SWAP::swapUk);
			printf("Error initial data init SWAP::swapUk\n");
			return false;
		}
		i++;
	}
	if(fseek(SWAP::swapUk,0,SEEK_SET))
	{
		printf("Error fseek init SWAP::swapUk.dat\n");
		return false;
	}
	return true;
}

// search used page from RAM::ramUk by idPage
FILE*  SWAP::  searchDataPagebyId(PAGE_COUNT_TYPE idPage)
{
	FILE *tmp = SWAP::swapUk;
	if(fseek(tmp,idPage*PAGE_DATA_SIZE,SEEK_SET))
	{
		printf("Error fseek searchDataPagebyId RAM::ramUk\n");
		return NULL;
	}
	if(feof(tmp))
	{
		printf("Error feof searchDataPagebyId SWAP::swapUk\n");
		return NULL;
	}
	return tmp;												// search OK!
}

// set Data to SWAP::swapUk		(FUNCTON DOES NOT CHANGE BYTE *Page)
bool  SWAP:: writePageData(const BYTE *Data, PAGE_COUNT_TYPE idPage)		// add Page to SWAP::swapUk
{
	FILE *tmpUk = NULL;
	if(NULL == (tmpUk = (SWAP::searchDataPagebyId(idPage))))
		return false;
	if(!fwrite(Data,PAGE_DATA_SIZE,1,tmpUk))						// 74 bytes = sizeof(swapPage)
	{
		printf("Error write writeData to SWAP::swapUk.dat\n");
		return  false;
	}
	return true;
}

// search Block from swapUk by idPage && idBlock
FILE* SWAP:: searchBlockPagebyId(PAGE_COUNT_TYPE idPage, PAGE_MAP_TYPE idBlock)
{
	FILE *tmpUk = NULL;
	if(NULL == (tmpUk = (SWAP::searchDataPagebyId(idPage))))
		return NULL;
	if(fseek(tmpUk,idBlock*PAGE_DATA_BLOCK_SIZE,SEEK_CUR))
	{
		printf("Error fseek searchBlockPagebyId RAM::ramUk\n");
		return NULL;
	}
	if(feof(tmpUk))
	{
		printf("Error feof searchBlockPagebyId SWAP::swapUk\n");
		return NULL;
	}
	return tmpUk;												// search OK!
}

// set Block to swap	(FUNCTON DOES NOT CHANGE BYTE *Block)
bool SWAP:: writeBlockData(const BYTE *Block, PAGE_COUNT_TYPE idPage, PAGE_MAP_TYPE idBlock)
{
	FILE *tmpUk = NULL;
	if(NULL == (tmpUk = (SWAP::searchBlockPagebyId(idPage,idBlock))))
		return false;
	if(!fwrite(Block,PAGE_DATA_BLOCK_SIZE,1,tmpUk))
	{
		printf("Error write writeBlock to SWAP::swapUk\n");
		return  false;
	}
	return true;
}

// get Data from swap		(FUNCTON DOES NOT CHANGE Data)
bool SWAP:: readData(PAGE_COUNT_TYPE idPage, BYTE &Data)
{
	FILE *tmpUk = NULL;
	if(NULL == (tmpUk = (SWAP::searchDataPagebyId(idPage))))
		return false;
	if(!fread(&Data,PAGE_DATA_SIZE,1,tmpUk))
	{
		printf("Error write readData from SWAP::swapUk\n");
		return  false;
	}
	return true;
}

// get Block from swap	(FUNCTON DOES NOT CHANGE Block)
bool SWAP:: readBlock(PAGE_COUNT_TYPE idPage, PAGE_MAP_TYPE idBlock, BYTE &Block)
{
	FILE *tmpUk = NULL;
	if(NULL == (tmpUk = (SWAP::searchBlockPagebyId(idPage, idBlock))))
		return false;
	if(!fread(&Block,PAGE_DATA_BLOCK_SIZE,1,tmpUk))
	{
		printf("Error write readBlock from SWAP::swapUk\n");
		return  false;
	}
	return true;
}


// open old SWAP file, that is already exist
// return true if ok, false if error
bool SWAP:: open(const char *fileName)
{
	this->close();
	if(access(fileName, F_OK) < 0)
	{                                                        // file not exist
			printf("Error open SWAP::swapUk file. SWAP file does not exist\n");
			return false;
	}
	if(!(this->swapUk = fopen(fileName, "ab")))				// open at end
	{
		printf("Error open SWAP::swapUk file\n");
		return false;
	}
	return true;
}


// close file
void SWAP:: close(void)
{
	if(SWAP::swapUk != NULL)
	{
		if(fclose(SWAP::swapUk))
		{
			printf("Could not close SWAP::swapUk!\n");
			return;
		}
	}
	this->swapUk = NULL;
}

// close file
void SWAP:: end(void)
{
	this->close();
}


} /* namespace mainSpace */
