#pragma once


#define POOL_SIZE (1024 * 1024 * 10)

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct memStatsInfo {
		int currBlocksAllocatedCount;
		int currBytesAllocated;
		int currBytesAvailable;

		int totalBlocksAllocated;
		int totalBlocksFreed;
		int totalBlocksMerged;

		int currMaxFreeBlockSize;
	} memStatsInfo;

	void* memAlloc(int size);
	void memFree(void* ptr);

	memStatsInfo memGetStats();

#ifdef __cplusplus
}
#endif
