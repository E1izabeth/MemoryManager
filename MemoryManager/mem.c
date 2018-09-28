#include <stdio.h>
#include "mem.h"
#include "stdafx.h"

#ifndef __cplusplus
const void* nullptr = 0;
#endif

//void test(void* ptr)
//{
//	char buff[100];
//	_itoa((int*)ptr, buff);
//}

typedef char byte;

typedef struct freeBlock {
	int size;
	struct freeBlock* next;
} freeBlock;

typedef struct dirtyBlock {
	int size;
} dirtyBlock;

static char _pool[POOL_SIZE];
const char* _poolEnd = _pool + POOL_SIZE;

short is_init;
memStatsInfo _currStats;
freeBlock* firstFreeBlock;
const int minBlockSize = 16;

void writeAddresses(FILE* file)
{
	freeBlock* temp = firstFreeBlock;
	while(temp != nullptr)
	{
		fputs(itoa((int)(void*)temp), file);
		temp = temp->next;
	}
}


void memInit()
{
	_currStats.currBlocksAllocatedCount = 0;
	_currStats.currBytesAllocated = 0;
	_currStats.currBytesAvailable = POOL_SIZE;
	_currStats.totalBlocksAllocated = 0;
	_currStats.totalBlocksFreed = 0;
	_currStats.currMaxFreeBlockSize = POOL_SIZE;

	freeBlock* p = (freeBlock*)_pool;
	p->next = nullptr;
	p->size = POOL_SIZE;
		
	firstFreeBlock = (freeBlock*)_pool;
	is_init = 1;
}

//void* giveFree(int size)
//{
//	void* address = 0;
//	char buff[4];
//	
//	freeBlock* t = firstFreeBlock;	
//		
//	char isFound = 0;
//	freeBlock* pred = t;
//	while (t->size < size + sizeof(dirtyBlock) && t->next != nullptr && !isFound)
//	{
//		pred = t;
//		t = (freeBlock*)t->next;
//
//		if (t->size >= size + sizeof(dirtyBlock))
//		{
//			isFound = 1;
//		}
//	}
//	freeBlock* next = t->next;
//
//
//	if (isFound || t->size >= size + sizeof(dirtyBlock))
//	{
//		address = t;
//		if (t->size - size > sizeof(dirtyBlock) + sizeof(freeBlock) + minBlockSize)
//		{
//			int oldSize = t->size;
//			t = (freeBlock*)((byte*)t + size);
//			pred->next = t;
//			t->next = next;
//			t->size = oldSize - size;
//		}
//		else
//		{
//			pred->next = next;
//		}
//	}
//	return address;
//}


void updateCurrMaxFreeBlockSize()
{
	int currMaxSize = 0;
	freeBlock* t = firstFreeBlock;

	int n = 0;
	freeBlock* prev;

	while (t != nullptr)
	{
		if (t->size > currMaxSize)
			currMaxSize = t->size;

		prev = t;
		t = t->next;
		n++;
	}

	_currStats.currMaxFreeBlockSize = currMaxSize;
}

void* giveFree(int size)
{
	freeBlock* temp = firstFreeBlock;
	freeBlock* prev = nullptr;
	while (temp->size < size + sizeof(dirtyBlock))
	{
		prev = temp;
		temp = temp->next;
		if (temp == nullptr)
			break;
	}
	void* ptr = temp;

	if (temp != nullptr)
	{
		if (temp->size > size + sizeof(dirtyBlock) + sizeof(freeBlock) + 16)
		{
			if ((void*)temp == (void*)firstFreeBlock)
			{
				int oldSize = firstFreeBlock->size;
				freeBlock* oldNext = firstFreeBlock->next;
				firstFreeBlock = (freeBlock*)((char*)firstFreeBlock + size);
				// test(firstFreeBlock);

				// test(oldNext);
				firstFreeBlock->size = oldSize - size;
				firstFreeBlock->next = oldNext;

				// updateCurrMaxFreeBlockSize();
			}
			else
			{
				// updateCurrMaxFreeBlockSize();
				
				int oldSize = temp->size;
				freeBlock* oldNext = temp->next;
				
				temp = (freeBlock*)((char*)temp + size);

				//test(temp);
				//if (temp->next != nullptr)
				//	test(temp->next);
				//test(oldNext);

				//updateCurrMaxFreeBlockSize();

				temp->size = oldSize - size;
				//updateCurrMaxFreeBlockSize();
				temp->next = oldNext;
				//updateCurrMaxFreeBlockSize();
				prev->next = temp;
			
				//updateCurrMaxFreeBlockSize();
			}
		}
		else
		{
			if ((void*)temp == (void*)firstFreeBlock)
			{
				firstFreeBlock = firstFreeBlock->next;
			
				// updateCurrMaxFreeBlockSize();
			}
			else
			{
				// test(temp->next);
				prev->next = temp->next;
			
				// updateCurrMaxFreeBlockSize();
			}
		}
	}
	return ptr;
}


void* memAlloc(int size)
{
	if (!is_init)
	{
		memInit();
	}
	
	dirtyBlock* ptr = (dirtyBlock*)giveFree(size);
	if (ptr != nullptr)
	{
		ptr->size = size;

		_currStats.currBlocksAllocatedCount += 1;
		_currStats.currBytesAllocated += size;
		_currStats.currBytesAvailable -= size;
		_currStats.totalBlocksAllocated += 1;

		updateCurrMaxFreeBlockSize();
	}
	return ptr;
}

void memFree(void* ptr)
{
	dirtyBlock* temp = (dirtyBlock*)ptr;
	int size = temp->size;
	freeBlock* tempFree = (freeBlock*)ptr;
	if ((char*)temp < (char*)firstFreeBlock)
	{
		if ((char*)(temp) + size == (char*)firstFreeBlock)
		{
			tempFree->size = firstFreeBlock->size + size;
			// test(firstFreeBlock->next);
			tempFree->next = firstFreeBlock->next;
			firstFreeBlock = tempFree;
			_currStats.totalBlocksMerged += 1;
		}
		else
		{
			tempFree->size = size;
			// test(firstFreeBlock);
			tempFree->next = firstFreeBlock;
			firstFreeBlock = tempFree;
		}
	}
	else
	{
		freeBlock* left = firstFreeBlock;
		if (left == nullptr)
		{
			tempFree->next = nullptr;
			tempFree->size = size;
			firstFreeBlock = tempFree;
		}
		else
		{
			while ((void*)left->next < (void*)temp && left->next != nullptr)
			{
				left = left->next;
			}
			freeBlock* right = left->next;

			if ((char*)left + left->size == (char*)temp && (char*)temp + size == (char*)right)
			{
				left->size += size + right->size;
				// test(right->next);
				left->next = right->next;
				_currStats.totalBlocksMerged += 1;
			}
			else if ((char*)left + left->size == (char*)temp)
			{
				left->size += size;
				_currStats.totalBlocksMerged += 1;
			}
			else if ((char*)temp + size == (char*)right)
			{
				tempFree->size = size + right->size;
				// test(right->next);
				tempFree->next = right->next;
				left->next = tempFree;
				// test(tempFree);
				_currStats.totalBlocksMerged += 1;
			}
			else
			{
				tempFree->size = size;
				// test(left->next);
				tempFree->next = left->next;
				// test(tempFree);
				left->next = tempFree;
			}
		}
	}
	 
	_currStats.currBlocksAllocatedCount -= 1;
	_currStats.currBytesAllocated -= size;
	_currStats.currBytesAvailable += size;
	_currStats.totalBlocksFreed += 1;
	updateCurrMaxFreeBlockSize();
}

#ifdef XXXXX
//void memFree(void* ptr)
//{
//	int sizeToFree = ((dirtyBlock*)ptr)->size;
//
//	if (ptr < firstFreeBlockAddress)
//	{
//		void* oldFirst = firstFreeBlockAddress;
//		int size = ((dirtyBlock*)ptr)->size;
//
//		((freeBlock*)ptr)->size = size;
//		((freeBlock*)ptr)->next = oldFirst;
//
//		firstFreeBlockAddress = ptr;
//	}
//	else 
//	{
//		freeBlock* left = firstFreeBlockAddress;
//
//		while (left->next < ptr && left->next != nullptr)
//		{
//			left = (freeBlock*)left->next;
//		}
//
//		if (left->next == nullptr)
//		{
//			int s = ((dirtyBlock*)ptr)->size;
//			((freeBlock*)ptr)->next = nullptr;
//			((freeBlock*)ptr)->size = s;
//			left->next = ptr;
//		}
//		else
//		{
//			int size = ((dirtyBlock*)ptr)->size;
//			void* next = ((freeBlock*)left)->next;
//
//			((freeBlock*)ptr)->size = size;
//			((freeBlock*)ptr)->next = next;
//			((freeBlock*)left)->next = ptr;
//		}
//	}
//
//	freeBlock* t = firstFreeBlockAddress;
//	if (t->next != nullptr)
//	{
//		if (t == ptr)
//		{
//			if ((int)ptr + ((freeBlock*)ptr)->size == (int)((freeBlock*)ptr)->next)
//			{
//				int size = ((freeBlock*)ptr)->size + ((freeBlock*)((freeBlock*)ptr)->next)->size;
//				((freeBlock*)ptr)->next = ((freeBlock*)((freeBlock*)ptr)->next)->next;
//				((freeBlock*)ptr)->size = size;
//				_currStats.totalBlocksMerged += 1;
//			}
//		}
//		else
//		{
//			while (t->next != ptr)
//			{
//				t = (freeBlock*)t->next;
//			}
//
//			if (t + t->size == ptr && (int)ptr + ((freeBlock*)ptr)->size == (int)((freeBlock*)ptr)->next)
//			{
//				int size = t->size + ((freeBlock*)ptr)->size + ((freeBlock*)((freeBlock*)ptr)->next)->size;
//				t->next = ((freeBlock*)((freeBlock*)ptr)->next)->next;
//				t->size = size;
//				_currStats.totalBlocksMerged += 2;
//			}
//			else if (t + t->size == ptr)
//			{
//				int size = t->size + ((freeBlock*)ptr)->size;
//				t->next = ((freeBlock*)ptr)->next;
//				t->size = size;
//				_currStats.totalBlocksMerged += 1;
//			}
//			else if ((int)ptr + ((freeBlock*)ptr)->size == (int)((freeBlock*)ptr)->next)
//			{ 
//				int size = ((freeBlock*)ptr)->size + ((freeBlock*)((freeBlock*)ptr)->next)->size;
//				((freeBlock*)ptr)->next = ((freeBlock*)((freeBlock*)ptr)->next)->next;
//				((freeBlock*)ptr)->size = size;
//				_currStats.totalBlocksMerged += 1;
//			}
//			else 
//			{
//				ptr = ptr;
//				// do nothing
//			}
//		}
//
//	}
//	_currStats.currBlocksAllocatedCount -= 1;
//	_currStats.currBytesAllocated -= sizeToFree;
//	_currStats.currBytesAvailable += sizeToFree;
//	_currStats.totalBlocksFreed += 1;
//	updateCurrMaxFreeBlockSize();
//}
//void memFree(void* ptr)
//{
//	if ((int)ptr > (int)_pool && (int)ptr < (int)_pool + POOL_SIZE)
//	{
//		freeBlock* t = firstFreeBlockAddress;
//
//		if ((int)ptr < (int)firstFreeBlockAddress)
//		{
//			if ((int)ptr + ((dirtyBlock*)ptr)->size == (int)firstFreeBlockAddress)
//			{
//				void* oldFirst = firstFreeBlockAddress;
//				firstFreeBlockAddress = ptr;
//				((freeBlock*)ptr)->next = ((freeBlock*)oldFirst)->next;
//				((freeBlock*)ptr)->size = ((dirtyBlock*)ptr)->size + ((freeBlock*)oldFirst)->size;
//			}
//			else
//			{
//				void* oldFirst = firstFreeBlockAddress;
//				firstFreeBlockAddress = ptr;
//				int s = ((dirtyBlock*)ptr)->size;
//				(firstFreeBlockAddress)->next = oldFirst;
//				(firstFreeBlockAddress)->size = s;
//			}
//
//		}
//		else
//		{
//
//			while (t < ptr && t != nullptr)
//			{
//				t = (freeBlock*)t->next;
//			}
//
//			if (t != nullptr)
//			{
//
//				short leftMerge = 0;
//				short rightMerge = 0;
//
//				if (t + t->size == ptr)
//				{
//					_currStats.totalBlocksMerged += 1;
//					t->size += ((dirtyBlock*)ptr)->size;
//					leftMerge = 1;
//				}
//
//				freeBlock* n = (freeBlock*)t->next;
//
//				if (t == (void*)((int)ptr + ((dirtyBlock*)ptr)->size))
//				{
//					if (leftMerge)
//					{
//						t->size += n->size;
//						t->next = n->next;
//
//						_currStats.totalBlocksMerged += 1;
//					}
//					else
//					{
//
//						_currStats.totalBlocksMerged += 1;
//						t->next = ptr;
//						freeBlock* newBlock = (freeBlock*)ptr;
//						newBlock->size = ((dirtyBlock*)ptr)->size + n->size;
//						newBlock->next = n->next;
//						t->next = newBlock;
//					}
//					rightMerge = 1;
//				}
//
//				if (!leftMerge && !rightMerge)
//				{
//					freeBlock* newBlock = (freeBlock*)ptr;
//
//					if (t < ptr)
//					{
//						t->next = newBlock;
//						newBlock->next = n;
//					}
//					else
//					{
//						newBlock->next = t;
//						t->next = n;
//					}
//					newBlock->size = ((dirtyBlock*)ptr)->size;
//				}
//			}
//			_currStats.currBlocksAllocatedCount -= 1;
//			_currStats.currBytesAllocated -= ((dirtyBlock*)ptr)->size;
//			_currStats.currBytesAvailable += ((dirtyBlock*)ptr)->size;
//			_currStats.totalBlocksFreed += 1;
//			updateCurrMaxFreeBlockSize();
//		}
//	}
//}

//void memFree(void* ptr)
//{
//	if (ptr >= _pool && ptr < _poolEnd)
//	{
//		freeBlock* t = firstFreeBlock;
//		freeBlock* temp = (freeBlock*)ptr;
//		int s = ((dirtyBlock*)ptr)->size;
//
//		if (ptr < firstFreeBlock)
//		{
//			if ((int)ptr + ((dirtyBlock*)ptr)->size == (int)firstFreeBlock)
//			{
//				freeBlock* oldFirst = firstFreeBlock;
//				firstFreeBlock = ptr;
//				temp->next = oldFirst->next;
//				temp->size = s + oldFirst->size;
//			}
//			else
//			{
//				freeBlock* oldFirst = firstFreeBlock;
//				firstFreeBlock = ptr;
//				
//				(firstFreeBlock)->next = oldFirst;
//				(firstFreeBlock)->size = s;
//			}
//
//		}
//		else
//		{
//			freeBlock* left = firstFreeBlock;
//
//			while (left->next < ptr && left->next != nullptr)
//			{
//				left = left->next;
//			}
//
//			if (left->next == nullptr)
//			{
//				left->next = ptr;
//				temp->next = nullptr;
//				temp->size = s;
//			}
//			else
//			{
//				freeBlock* right = left->next;
//
//				//left merge
//				if ((int)(void*)left + left->size == (int)ptr)
//				{
//					((dirtyBlock*)ptr)->size = s;
//					temp = left;
//					temp->next = right;
//					temp->size += s;
//					_currStats.totalBlocksMerged += 1;
//				}
//
//				//right merge
//				if ((int)(void*)temp + temp->size == (int)right)
//				{
//					temp->next = right->next;
//					temp->size += right->size;
//					_currStats.totalBlocksMerged += 1;
//				}
//			}
//		}
//		_currStats.currBlocksAllocatedCount -= 1;
//		_currStats.currBytesAllocated -= s;
//		_currStats.currBytesAvailable += s;
//		_currStats.totalBlocksFreed += 1;
//		updateCurrMaxFreeBlockSize();
//	}
//}

#endif 

memStatsInfo memGetStats()
{
	return _currStats;
}
