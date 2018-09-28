#define MEM_IMPL
#include "mem.h"

#ifndef __cplusplus
const void* nullptr = 0;
#endif

//void test(void* ptr)
//{
//	char buff[100];
//	_itoa((int*)ptr, buff);
//}

typedef char byte;

static char _pool[POOL_SIZE];
static const char* _poolEnd = _pool + POOL_SIZE;

static short is_init = 0;
static memStatsInfo _currStats;
static freeBlock* _firstFreeBlock;

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)
#define ALIGN(n, step) (n + (step - ((n - 1) % step) - 1))

#define MIN_BLOCK_CONTENT_SIZE 16
#define DIRTY_BLOCK_HEADER_SIZE ALIGN(sizeof(dirtyBlock), 4)
#define MIN_BLOCK_SIZE ALIGN((DIRTY_BLOCK_HEADER_SIZE + MIN_BLOCK_CONTENT_SIZE), 16)

freeBlock* getList()
{
	return _firstFreeBlock;
}

void updateCurrMaxFreeBlockSize()
{
	int currMaxSize = 0;
	freeBlock* t = _firstFreeBlock;

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
	_currStats.currFreeBlocksCount = n;
}

void memInit()
{
	_currStats.currAllBlocksCount = 1;
	_currStats.currBlocksAllocatedCount = 0;
	_currStats.currBytesAllocated = 0;
	_currStats.currBytesAvailable = POOL_SIZE;
	_currStats.totalBlocksAllocated = 0;
	_currStats.totalBlocksFreed = 0;
	_currStats.currMaxFreeBlockSize = POOL_SIZE;

	freeBlock* p = (freeBlock*)_pool;
	p->next = nullptr;
	p->size = POOL_SIZE;

	_firstFreeBlock = (freeBlock*)_pool;
	is_init = 1;

	updateCurrMaxFreeBlockSize();
}
void* memAlloc(int size)
{
	if (!is_init)
		memInit();

	int desiredSize = MAX(ALIGN(size, 16), MIN_BLOCK_SIZE);

	freeBlock* currBlock = _firstFreeBlock;
	freeBlock* prev = nullptr;
	while (currBlock != nullptr && currBlock->size < desiredSize)
	{
		prev = currBlock;
		currBlock = currBlock->next;
	}

	if (currBlock == nullptr)
		return nullptr;

	if (currBlock->size > desiredSize + MIN_BLOCK_SIZE)
	{
		int oldSize = currBlock->size;
		int restSize = oldSize - desiredSize;

		freeBlock* newBlock = (byte*)currBlock + desiredSize;
		newBlock->next = currBlock->next;
		newBlock->size = restSize;

		_currStats.currAllBlocksCount++;

		if (prev == nullptr)
		{
			_firstFreeBlock = newBlock;
		}
		else
		{
			prev->next = newBlock;
		}
	}
	else
	{
		if (prev == nullptr)
		{
			_firstFreeBlock = currBlock->next;
		}
		else
		{
			prev->next = currBlock->next;
		}
	}

	dirtyBlock* result = (dirtyBlock*)currBlock;
	result->size = desiredSize;

	_currStats.currBlocksAllocatedCount++;
	_currStats.currBytesAllocated += desiredSize;
	_currStats.currBytesAvailable -= desiredSize;
	_currStats.totalBlocksAllocated++;

	updateCurrMaxFreeBlockSize();
	return ((byte*)result) + DIRTY_BLOCK_HEADER_SIZE;
}

bool tryMerge(freeBlock* a, freeBlock* b)
{
	if ((byte*)a < (byte*)b && ((byte*)a + a->size) == (byte*)b)
	{
		a->size += b->size;
		a->next = b->next;
		_currStats.totalBlocksMerged++;
		_currStats.currAllBlocksCount--;
		return true;
	}
	else
	{
		return false;
	}

	updateCurrMaxFreeBlockSize();
}

void memFree(void* ptr)
{
	dirtyBlock* dblock = (dirtyBlock*)(((char*)ptr) - DIRTY_BLOCK_HEADER_SIZE);
	int dblockSize = dblock->size;
	freeBlock* block = (freeBlock*)dblock;
	block->size = dblockSize;

	freeBlock* nextBlock = _firstFreeBlock;
	freeBlock* prevBlock = nullptr;
	while (nextBlock != nullptr && nextBlock < ptr)
	{
		prevBlock = nextBlock;
		nextBlock = nextBlock->next;
	}

	if (prevBlock == nullptr)
	{
		block->next = _firstFreeBlock;
		_firstFreeBlock = block;
	}
	else if (tryMerge(prevBlock, block))
	{
		if (nextBlock != nullptr)
		{
			if (!tryMerge(prevBlock, nextBlock))
			{
				prevBlock->next = nextBlock;
			}
		}
	}
	else
	{
		block->next = nextBlock;
		prevBlock->next = block;
		
		if (nextBlock != nullptr)
		{
			tryMerge(block, nextBlock);
		}
	}

	_currStats.currBlocksAllocatedCount--;
	_currStats.currBytesAllocated -= dblockSize;
	_currStats.currBytesAvailable += dblockSize;
	_currStats.totalBlocksFreed++;
	updateCurrMaxFreeBlockSize();
}

int memGetBlockSize(void* ptr)
{
	dirtyBlock* block = (dirtyBlock*)((int)ptr - DIRTY_BLOCK_HEADER_SIZE);
	return block->size;
}

memStatsInfo memGetStats()
{
	return _currStats;
}
