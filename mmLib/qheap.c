#include <Windows.h>
#include "mem.h"
#include "qheap.h"

typedef struct QuickHeapBlock {
	struct QuickHeapBlock* next;
}QuickHeapBlock;

typedef struct {
	QuickHeapBlock* firstFreeBlock;
	void* lastExtentBoundary;
	void* lastExtentEnd;
	int lastExtentSize;
}PoolInfo;

static bool is_init = false;
static const int QHInitPoolSize = 1024 * 16 * 10;
static PoolInfo qh_array[QH_ARRAY_COUNT];
// static PoolInfo* qh_array;

/*void memset(void* ptr, char value, int count)
{
	char* p = ptr;
	char* end = p + count;

	while (p < end)
		*(p++) = value;
}*/

void* qh_alloc(int size)
{
	void* ret;
	if (!is_init)
	{
		// qh_array = memAlloc(sizeof(PoolInfo) * QH_ARRAY_COUNT);
		memset(qh_array, 0, sizeof(qh_array));
		is_init = true;

		if (qh_array == 0)
			return 0;
	}

	int desiredSize = MAX(ALIGN(size, 16), MIN_BLOCK_SIZE);
	int qh_array_index = desiredSize / QH_STEP - 1;
	
	if (!(qh_array[qh_array_index].firstFreeBlock == 0 && qh_array[qh_array_index].lastExtentBoundary == 0
		&& qh_array[qh_array_index].lastExtentEnd == 0 && qh_array[qh_array_index].firstFreeBlock == 0))
	{
		if (qh_array[qh_array_index].firstFreeBlock != 0)
		{
			ret =  qh_array[qh_array_index].firstFreeBlock;
			qh_array[qh_array_index].firstFreeBlock = qh_array[qh_array_index].firstFreeBlock->next;
		}
		else
		{
			if (qh_array[qh_array_index].lastExtentBoundary == qh_array[qh_array_index].lastExtentEnd)
			{
				qh_array[qh_array_index].lastExtentSize *= 2;
				// void* extentPtr = memAlloc(qh_array[qh_array_index].lastExtentSize);
				void* extentPtr = HeapAlloc(GetProcessHeap(), 0, qh_array[qh_array_index].lastExtentSize);
				if (extentPtr == 0)
					return 0;

				qh_array[qh_array_index].lastExtentEnd = (int)extentPtr + qh_array[qh_array_index].lastExtentSize;
				qh_array[qh_array_index].lastExtentBoundary = (int)extentPtr + qh_array_index * QH_STEP;
				ret = extentPtr;
			}
			else
			{
				ret = qh_array[qh_array_index].lastExtentBoundary;
				qh_array[qh_array_index].lastExtentBoundary = (void*)((int)qh_array[qh_array_index].lastExtentBoundary + qh_array_index * QH_STEP);
			}
		}
	}
	else
	{
		int initPoolSize = qh_array_index * QH_STEP * 10;
		// void* extentPtr = memAlloc(qh_array_index * QH_STEP * 10);
		void* extentPtr = HeapAlloc(GetProcessHeap(), 0, initPoolSize);
		if (extentPtr == 0)
			return 0;

		qh_array[qh_array_index].lastExtentEnd = (int)extentPtr + initPoolSize;
		qh_array[qh_array_index].lastExtentSize = initPoolSize;
		qh_array[qh_array_index].lastExtentBoundary = (int)extentPtr + qh_array_index * QH_STEP;
		ret = extentPtr;
	}

	((dirtyBlock*)ret)->size = size;
	return ((byte*)ret) + DIRTY_BLOCK_HEADER_SIZE;
}


void qh_free(void* ptr)
{
	int size = memGetBlockSize(ptr);
	int qh_arr_index = size / QH_STEP - 1;
	QuickHeapBlock* freeBlock = (QuickHeapBlock*)ptr;
	freeBlock->next = qh_array[qh_arr_index].firstFreeBlock;
	qh_array[qh_arr_index].firstFreeBlock = freeBlock;
}
 