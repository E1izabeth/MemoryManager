#pragma once

#define QH_ARRAY_COUNT 1024
#define QH_STEP 16

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)
#define ALIGN(n, step) (n + (step - ((n - 1) % step) - 1))
#define MIN_BLOCK_CONTENT_SIZE 16
#define DIRTY_BLOCK_HEADER_SIZE ALIGN(sizeof(dirtyBlock), 4)
#define MIN_BLOCK_SIZE ALIGN((DIRTY_BLOCK_HEADER_SIZE + MIN_BLOCK_CONTENT_SIZE), 16)

#ifdef __cplusplus
extern "C"
{
#endif

void* qh_alloc(int size);
void qh_free(void* ptr);

#ifdef __cplusplus
}
#endif