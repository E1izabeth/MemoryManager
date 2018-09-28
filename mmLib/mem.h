#pragma once

#define POOL_SIZE (1024 * 1024 * 10)

#define true (1 == 1)
#define false (1 == 0)
#define bool int

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct memStatsInfo {
		int currAllBlocksCount;
		int currBlocksAllocatedCount;
		int currBytesAllocated;
		int currBytesAvailable;

		int totalBlocksAllocated;
		int totalBlocksFreed;
		int totalBlocksMerged;

		int currMaxFreeBlockSize;
		int currFreeBlocksCount;
	} memStatsInfo;

	void* memAlloc(int size);
	void memFree(void* ptr);

	int memGetBlockSize(void* ptr);

	memStatsInfo memGetStats();


	typedef struct freeBlock {
		int size;
		struct freeBlock* next;
	} freeBlock;

	typedef struct dirtyBlock {
		int size;
	} dirtyBlock;

	freeBlock* getList();

#ifdef __cplusplus
}
#endif
