/*
 * types.h
 *
 *  Created on: Jan 20, 2014
 *      Author: lenovo
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>



#define BYTE						uint8_t
#define __int64						long long
#define SWAP_FILE_NAME				"SWAP.dat"												// имя файла подкачки
#define PAGE_COUNT_TYPE				BYTE													// тип кол-ва страниц (определяет макс ко-во страниц)
#define PAGE_MAP_TYPE				BYTE													// тип кол-ва блоков битовой карты (определяет макс кол-во блоков данных,
																							// т.е. значение PAGE_MAP_COUNT)
#define PAGE_MAP_COUNT				32														// кол-во блоков битовой карты (один блок всегда = 2 бита)
																							// всегда должно быть в диапазоне значений типа PAGE_MAP_TYPE
/*																							возможные значения блока битовой карты:
																								00 свободен
																								01 занят
																								10 занят и это последний блок
																								11 занят но это параметры, где находится продолжение

																							   диапазон значений кол-ва блоков битовой карты = [4,1020]
																							   определяется типом PAGE_MAP_TYPE. если PAGE_MAP_TYPE == BYTE, то
																							   максимум = 1020 т.к. maxValueType(PAGE_MAP_TYPE == BYTE) == 255; 255*4=1020
																							   значение вседа должно быть кратно 4-ём!!!
																							   чтобы размер битовой карты измерялся в байтах (2bits*4=1byte)
*/
#define PAGE_DATA_BLOCK_OPTIONAL	0														// размер дополнительных данных в каждом блоке
																							// размер опционален и может быть любым (см. ниже что за он и зачем)
#define PAGE_DATA_BLOCK_SIZE		(sizeof(PAGE_COUNT_TYPE)+sizeof(PAGE_MAP_TYPE))+PAGE_DATA_BLOCK_OPTIONAL
																							// размер блока данных, адресуемый одним блоком битовой карты
/*																							   == минимальный размер блока + размер доп. данных
 * 																							   минимальный размер блока обязателен!
 * 																							   размер доп. данных опционален и может быть любым
																						   	   (sizeof(PAGE_COUNT_TYPE)+sizeof(PAGE_MAP_TYPE)) == минимальный размер блока
																							   нужен чтобы в нём при значении 11 блока битовой карты, соответствующего этому
																							   блоку данных, смогли поместиться параметры, где находится продолжение данных
																							   эти параметры: номер страницы и номер блока данных
*/
#define PAGE_MAP_SIZE				(PAGE_MAP_COUNT/4)										// размер битовой карты в байтах
#define PAGE_DATA_SIZE				PAGE_MAP_COUNT*PAGE_DATA_BLOCK_SIZE						// эффективный размер данных = размер всех блоков данных одной страницы
#define PAGE_SIZE					(PAGE_MAP_SIZE+PAGE_DATA_SIZE+1)						// +1 byte for allocated indicator =
																							// free/ram/swap  = 0/[0,RAM_MAX_PAGE]/[RAM_MAX_PAGE+1,SWAP_MAX_PAGE]
																							// размер страницы с учётом служебных данных
																							// (размер индикатора занятости страницы и размер битовой карты страницы)
#define SWAP_MAX_PAGE				((pow((long double)2,(long double)(8*sizeof(PAGE_COUNT_TYPE))))-2)
																							// максимальное кол-во страниц-1 в SWAP = верхний диапазон занятости SWAP
																							// (определяется типом PAGE_COUNT_TYPE) = 254 для PAGE_COUNT_TYPE == BYTE
#define RAM_MAX_PAGE				(((pow((long double)2,(long double)(8*sizeof(PAGE_COUNT_TYPE))))-2)/2)
																							// максимальное кол-во страниц-1 в RAM = верхний диапазон занятости RAM
																							// (определяется типом PAGE_COUNT_TYPE) = 127 для PAGE_COUNT_TYPE == BYTE
#define PAGE_MAP_MAX_COUNT			((pow((long double)2,(long double)(8*sizeof(PAGE_MAP_TYPE))))-2)
																							// максимальное кол-во блоков битовой карты-1
																							//(определяется типом PAGE_MAP_TYPE) = 254 для PAGE_COUNT_TYPE == BYTE
struct DataLocation
{
	void *ramUkData;			// not NULL if data blocks are in RAM
	FILE *swapUkData;			// not NULL if data blocks are in swap
								// both are NULL if error
	operator bool() { if(ramUkData) return ramUkData; else return swapUkData;}
	DataLocation(void) { ramUkData = NULL;  swapUkData = NULL;}
};

// type, that return when memory allocate
// this type point to first start allocated block
struct Pointer
{
	PAGE_COUNT_TYPE idPage;
	PAGE_MAP_TYPE idBlock;
	Pointer(void) {idPage = idBlock = 0;}
};

struct TwoBits
{
	BYTE first;
	BYTE second;
	operator bool() { if(first || second) return true; else return false;}
	TwoBits(void) { first = second = false; }
};

/* reset terminal color for initial colours */
#define RESETCOLOR 	"\033[0m"

/* OUTPUT COLORS */
#define BLACK   	"\e[30m"
#define RED     	"\e[31m"
#define GREEN   	"\e[32m"
#define YELLOW  	"\e[33m"
#define BLUE    	"\e[34m"
#define MAGENTA 	"\e[35m"
#define CYAN		"\e[36m"
#define WHITE		"\e[37m"
#define WHITE_BACK	"\e[47m"

/* OUTPUT STYLE */
#define BOLD 		"\e[1m"
#define UNDERLINE	"\e[4m"
#define NOBOLD 		"\e[22m"
#define NOBLINK 	"\e[25m"
#define BLINK		"\e[5m"					// for correct blinking work
											// use xTerm as terminal
											// blinking does not work with terminals
											// that use vte. it's vte bug
/* OUTPUT SYMBOLS */
#define PAGE_NUMS_S			"█"				// pages_numbers (ram&&swap)
#define RAM_IND_S			"◙"				// ram page indicator
#define RAM_BITS_S			"█"				// ram bitMap blocks
#define RAM_DATA_FREE_S		"░"				// ram data block free
#define RAM_DATA_BUSY_S		"█"				// ram data block busy
#define SWAP_IND_S			"◙"				// swap page indicator
#define SWAP_BITS_S			"█"				// swap bitMap blocks
#define SWAP_DATA_FREE_S	"░"				// swap data block free
#define SWAP_DATA_BUSY_S	"█"				// swap data block busy

/* INITIAL COLORS */
#define BACKGROUND		BLACK
#define PAGE_NUMS		CYAN 	NOBLINK PAGE_NUMS_S 		BACKGROUND

#define RAM_IND			WHITE 	BLINK	RAM_IND_S 			BACKGROUND
#define RAM_BITS		GREEN 	NOBLINK RAM_BITS_S 			BACKGROUND
#define RAM_DATA_FREE	RED 	NOBLINK RAM_DATA_FREE_S		BACKGROUND
#define RAM_DATA_BUSY	RED 	NOBLINK	RAM_DATA_BUSY_S 	BACKGROUND

#define SWAP_IND		MAGENTA BLINK 	SWAP_IND_S 			BACKGROUND
#define SWAP_BITS		YELLOW 	NOBLINK SWAP_BITS_S 		BACKGROUND
#define SWAP_DATA_FREE  BLUE 	NOBLINK SWAP_DATA_FREE_S 	BACKGROUND
#define SWAP_DATA_BUSY	BLUE 	NOBLINK SWAP_DATA_BUSY_S 	BACKGROUND

/* BLINK - означает "мигание" */
#endif /* TYPES_H_ */
