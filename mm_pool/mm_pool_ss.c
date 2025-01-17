#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MEM_PAGE_SIZE 4096
typedef struct mempool_s
{
    int blocksize;
    int freecount;
    char *free_ptr;
    char *mem;
}mempool_t;

int mempool_create(mempool_t *pool, int blocksize)
{
    if (!pool || blocksize <= 0)
    {
        return -1;
    }
    pool->blocksize = blocksize;
    pool->freecount = MEM_PAGE_SIZE / blocksize;
    pool->mem = malloc(MEM_PAGE_SIZE);
    pool->free_ptr = pool->mem;
    //初始化链表
    char *ptr = pool->free_ptr;
    int i = 0;
    for (; i < pool->freecount; i++)
    {
        *(char **)ptr = ptr + blocksize;    //头部存储下一个节点的地址
        ptr = ptr + blocksize;              //指向下一个节点
    }
    *(char **)ptr = NULL;
    return 0;
}

void mempool_destroy(mempool_t *pool)
{
    if (!pool)
    {
        return;
    }
    free(pool->mem);
    pool->mem = NULL;
    pool->free_ptr = NULL;
    pool->freecount = 0;
    
}

void *mempool_alloc(mempool_t *pool)
{
    //头删法
    if (!pool || pool->freecount == 0)
    {
        return NULL;
    }
    char *ptr = pool->free_ptr;
    pool->free_ptr = *(char **)ptr;
    pool->freecount--;
    return ptr;
}

void mempool_free(mempool_t *pool, void *ptr)
{
    //头插法
    *(char **)ptr = pool->free_ptr;
    pool->free_ptr = ptr;
    pool->freecount++;

}

int main(int argc, char const *argv[])
{
    	mempool_t m;

	mempool_create(&m, 32);

	void *p1 = mempool_alloc(&m);
	printf("memp_alloc : %p\n", p1);

	void *p2 = mempool_alloc(&m);
	printf("memp_alloc : %p\n", p2);

	void *p3 = mempool_alloc(&m);
	printf("memp_alloc : %p\n", p3);

	mempool_free(&m, p2);
    //printf("memp_alloc : %p\n", p2);
    return 0;
}