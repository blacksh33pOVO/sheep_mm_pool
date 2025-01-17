#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct mp_node_s
{
	unsigned char *last;
	unsigned char *end;
	struct mp_node_s *next;
}mp_node_t;

typedef struct mp_large_s {
	struct mp_large_s *next;
	void *alloc;
} mp_large_t;

typedef struct mp_pool_s
{
	size_t max;
	struct mp_node_s *head;
	struct mp_large_s *large;
}mp_pool_t;
 
int mp_create(mp_pool_t *pool, size_t size);
void mp_destory(mp_pool_t *pool);
void *mp_alloc(mp_pool_t *pool, size_t size);
void mp_free(mp_pool_t *pool, void *ptr);

int mp_create(mp_pool_t *pool, size_t size)
{
    if (pool == NULL || size <= 0)	return -1;

    //分配第一个节点
    void *mem = malloc(size);
    if (mem == NULL)	return -1;
    struct mp_node_s* node = (mp_node_t*)mem;
    node->last = (char *)mem + sizeof(mp_node_t);
    node->end = (char *) mem + size;
    node->next = NULL;

    //初始化
    pool->head = node;
    pool->large = NULL;
    pool->max = size;
    return 0;

}

void mp_destory(mp_pool_t *pool)
{
    
    if(pool == NULL) return;
    //大内存块 头删法
    mp_large_t* l = pool->large;
    for (; l; l = l->next)
    {
        if (l->alloc)
        {
            free(l->alloc);
        }
    }
    pool->large = NULL;

    //小内存块 头删法
    mp_node_t* node = pool->head;
    for (; node; node = node->next)
    {
        if (node)
        {
            free(node);
        }
    }


}

static void *mp_alloc_block(mp_pool_t *pool, size_t size)
{
    void *ptr = NULL;
    mp_node_t *p = pool->head;
    //遍历所有节点，如果有节点内存足够，直接用
    while (p)
    {
        if (p->end - p->last >= size)
        {
            ptr = p->last;
            p->last += size;
            return ptr;
        }
        p = p->next;    
    }

    //如果没有节点内存足够，分配一个新节点
    void *mem = malloc(size);
    if (mem == NULL)	return NULL;
    struct mp_node_s* node = (mp_node_t*)mem;
    node->last = (char *)mem + sizeof(mp_node_t);
    node->end = (char *) mem + size;

    ptr = node->last;
    node->last += size;

    //tail insert
    mp_node_t *iter = pool->head;
    while (iter->next!= NULL) {
        iter = iter->next;
    }
    iter->next = node;
    return ptr;
    
}

static void *mp_alloc_large(mp_pool_t *pool, size_t size)
{
    if (!pool)
        return NULL;

    void *ptr = malloc(size);
    if (!ptr)
        return NULL;
    //头插法,遍历链表，如果有空的直接用
    mp_large_t* l = pool->large;
    for (; l; l = l->next)
    {
        if (!l->alloc)
        {
            l->alloc = ptr;
            return ptr;
        }
    }
    
    //如果没有，分配一个新节点,头插法
    l = mp_alloc_block(pool, sizeof(mp_large_t));
    l->alloc = ptr;
    l->next = pool->large;
    pool->large = l;
    return ptr;
    
}

void *mp_alloc(mp_pool_t *pool, size_t size)
{
    if (size > pool->max)
    {
        return mp_alloc_large(pool, size);
    }


    return mp_alloc_block(pool, size);
}

void mp_free(mp_pool_t *pool, void *ptr)
{
    mp_large_t* l = pool->large;
    while (ptr != l->alloc)
    {
        l = l->next;
    }
    free(l->alloc);

    //
}


int main() {
	mp_pool_t *pool = malloc(sizeof(mp_pool_t));
	
	mp_create(pool, 4096);
	

	int *p = mp_alloc(pool, sizeof(int));
	*p = 100;
	printf("%d\n", *p);
	printf("%p\n", p);
	//mp_free(pool, p);

	int *p1 = mp_alloc(pool, 100);
	*p1 = 200;
	printf("%d\n", *p1);
	printf("%p\n", p1);

	int *p2 = mp_alloc(pool, 200);
	*p2 = 300;
	printf("%d\n", *p2);
	printf("%p\n", p2);
    int *p3 = mp_alloc(pool, 6000);
    *p3 = 5000;
	printf("%d\n", *p3);
	printf("%p\n", p3);
	mp_destory(pool);
	free(pool);
	return 0;
}
