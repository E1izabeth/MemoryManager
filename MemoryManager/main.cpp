#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mem.h"

void* blocks[POOL_SIZE/16];

FILE* log;

void printStats(memStatsInfo* stats)
{
    // long freeBlocks = stats->currentAllBlocksCount - stats->currentAllocatedBlocksCount;
	
	printf("currentAllocatedBlocksCount\t = %i\n", stats->currBlocksAllocatedCount);
    printf("currentAllocatedBytes\t = %i\n", stats->currBytesAllocated);
    printf("currentAvailableBytes\t = %i\n", stats->currBytesAvailable);
    // printf("currentAllBlocksCount\t = %i\n", stats->currentAllBlocksCount);

    // printf("totalBlocksCreated\t = %u\n", stats->totalBlocksCreated);
    // printf("totalBlocksMerged\t = %u\n", stats->totalBlocksMerged);
    printf("totalBlocksAllocated\t = %u\n", stats->totalBlocksAllocated);
    printf("totalBlocksFreed\t = %u\n", stats->totalBlocksFreed);

	printf("currentMaxFreeBlockSize = %i\n", stats->currMaxFreeBlockSize);
	printf("totalBlocksMerged = %i\n", stats->totalBlocksMerged);

    // printf("free space fragmentation\t = %i free blocks ~%i bytes in each\n", freeBlocks, stats->currentAvailableBytes / freeBlocks);

    //printf("Time measurements : \tmin\t\tmid\t\tmax\n");
	// printf("\t\talloc\t%f\t%f\t%f\n", allocTimes.min, allocTimes.total / allocTimes.count, allocTimes.max);
    // printf("\t\tfree \t%f\t%f\t%f\n", freeTimes.min, freeTimes.total / freeTimes.count, freeTimes.max);
}

static char _pool[POOL_SIZE];


void testAlloc1_1()
{
	auto p1 = memAlloc(2048);
	auto p2 = memAlloc(4096);
	memFree(p1);
	p1 = memAlloc(1000);
}

void testAlloc1_2()
{
	auto p1 = memAlloc(2048);
	auto p2 = memAlloc(2048);
	auto p3 = memAlloc(2048);
	auto p4 = memAlloc(2048);
	auto p5 = memAlloc(2048);
	memFree(p1);
	memFree(p3);
	memFree(p4);
	p3 = memAlloc(3000);
}

void testAlloc1_3()
{
	auto p1 = memAlloc(2048);
	auto p2 = memAlloc(2048);
	memFree(p1);
	auto p3 = memAlloc(3000);
}

void testAlloc2_1()
{
	auto p1 = memAlloc(2048);
	auto p2 = memAlloc(4096);
	auto p3 = memAlloc(2048);
	auto p4 = memAlloc(2048);
	memFree(p1);
	memFree(p3);
	p1 = memAlloc(2040);
}

void testAlloc2_2()
{
	auto p1 = memAlloc(2048);
	auto p2 = memAlloc(4096);
	auto p3 = memAlloc(3000);
	auto p4 = memAlloc(2048);
	memFree(p1);
	memFree(p3);
	p1 = memAlloc(2990);
}

void testAlloc2_3()
{
	auto p1 = memAlloc(2048);
	auto p2 = memAlloc(4096);
	auto p3 = memAlloc(3000);
	auto p4 = memAlloc(2048);
	memFree(p1);
	memFree(p3);
	p1 = memAlloc(POOL_SIZE - 2048 - 4096 - 3000 - 2040);
}

void testFree1_1()
{
	auto p1 = memAlloc(2048);
	auto p2 = memAlloc(2048);
	memFree(p1);
}

void testFree1_2()
{
	auto p1 = memAlloc(2048);
	auto p2 = memAlloc(2048);
	auto p3 = memAlloc(2048);
	auto p4 = memAlloc(2048);
	auto p5 = memAlloc(2048);
	memFree(p1);
	memFree(p3);
}

void testFree1_3()
{
	auto p1 = memAlloc(2048);
	auto p2 = memAlloc(2048);
	auto p3 = memAlloc(2048);
	auto p4 = memAlloc(POOL_SIZE - 2048 - 2048 - 2048 - 2060);
	auto p5 = memAlloc(2048);
	memFree(p1);
	memFree(p3);
	memFree(p5);
}

void testFree2_1()
{
	auto p1 = memAlloc(2048);
	auto p2 = memAlloc(2048);
	auto p3 = memAlloc(2048);
	memFree(p1);
	memFree(p2);
}

void testFree2_2()
{
	auto p1 = memAlloc(2048);
	auto p2 = memAlloc(2048);
	auto p3 = memAlloc(2048);
	auto p4 = memAlloc(2048);
	auto p5 = memAlloc(2048);
	memFree(p1);
	memFree(p2);
	memFree(p3);
}

void testFree2_3()
{
	auto p1 = memAlloc(2048);
	auto p2 = memAlloc(2048);
	auto p3 = memAlloc(2048);
	auto p4 = memAlloc(2048);
	auto p5 = memAlloc(POOL_SIZE - 2048*4 - 10);
	memFree(p1);
	memFree(p3);
	memFree(p4);
	memFree(p5);
}

void testFree3_1()
{
	auto p1 = memAlloc(2048);
	auto p2 = memAlloc(2048);
	auto p3 = memAlloc(2048);
	memFree(p2);
	memFree(p1);
}

void testFree3_2()
{
	auto p1 = memAlloc(2048);
	auto p2 = memAlloc(2048);
	auto p3 = memAlloc(2048);
	auto p4 = memAlloc(2048);
	auto p5 = memAlloc(2048);
	auto p6 = memAlloc(2048);
	memFree(p2);
	memFree(p5);
	memFree(p4);
}

void testFree3_3()
{
	auto p1 = memAlloc(2048);
	auto p2 = memAlloc(2048);
	auto p3 = memAlloc(2048);
	auto p4 = memAlloc(2048);
	auto p5 = memAlloc(2048);
	memFree(p1);
	memFree(p3);
	memFree(p5);
}

void testFree4_1()
{
	auto p1 = memAlloc(2048);
	auto p2 = memAlloc(2048);
	auto p3 = memAlloc(2048);
	auto p4 = memAlloc(2048);
	memFree(p1);
	memFree(p3);
	memFree(p2);
}

void testFree4_2()
{
	auto p1 = memAlloc(2048);
	auto p2 = memAlloc(2048);
	auto p3 = memAlloc(2048);
	auto p4 = memAlloc(2048);
	auto p5 = memAlloc(2048);
	auto p6 = memAlloc(2048);
	memFree(p2);
	memFree(p5);
	memFree(p3);
	memFree(p4);
}

void testFree4_3()
{
	auto p1 = memAlloc(4096);
	auto p2 = memAlloc(2048);
	auto p3 = memAlloc(2048);
	auto p4 = memAlloc(2048);
	memFree(p1);
	memFree(p3);
	memFree(p4);
}

void testFullFreeThenAlloc()
{
	auto p1 = memAlloc(4096);
	auto p2 = memAlloc(2048);
	auto p3 = memAlloc(2048);
	auto p4 = memAlloc(2048);
	memFree(p1);
	memFree(p2);
	memFree(p3);
	memFree(p4);
	p1 = memAlloc(2048);
	p2 = memAlloc(2048);
}

int main(int argc, char** argv)
{
	testAlloc1_1();

	int allocationSteps[] = { 16, 64, 256, 1024, 1024 * 4, 1024 * 16 };
    void* p;
    int i = 0,
        j = 0,
        k = 0;
	int tt;

	int sz;
    bool freeing = false; 
	memStatsInfo stats;

	printf("press any key to start\n\n");
	_getch();

	srand(time(NULL));

	if (argc > 1) printf("fixed allocation steps");
	else printf("random allocation steps");

	log = fopen("log.txt", "w");


	//testFullFreeThenAlloc();

	//stats = memGetStats();
	//printStats(&stats);

	do
	{
		if (argc > 1)
		{
			tt = sz = rand() % 6;
			sz = allocationSteps[sz];
		}
		else
		{
			sz = rand() * 1024 * 16 / RAND_MAX;
		}

		p = memAlloc(sz);
		blocks[k] = p; 

		/*if (p == nullptr)
		{
			for (int t = 0; t < k; t++)
			{
				memFree(blocks[t]);
			}

			break;
		}*/

		stats = memGetStats();
       
		if (!freeing && stats.currBytesAvailable <= POOL_SIZE * 2 / 10)
        {
            printf("\nStart freeing!\n");
            printStats(&stats);

            freeing = true;
        }

        if (freeing)
        {
            int n = rand() * k / RAND_MAX; 

            memFree(blocks[n]);
            blocks[n] = blocks[k];
            k--;
        }
        
        i++;
        k++; 
		
        if (i % 1000 == 0) 
        {
            i = 0;
            j++;
            printf("\nStats #%i :\n", j);
            printStats(&stats);
        }

		//writeAddresses(log);
    } while (p != 0);

	if (log > 0)
		fclose(log);

    printf("\nAllocation of %i bytes failed!\n", sz);
    printf("\nFinal stats :\n");

	stats = memGetStats();
	printStats(&stats);

    _getch();
	return 0;
}