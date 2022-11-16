#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <unistd.h>

#include "ex1.h"

#define ERROR_ON_SBRK (void *)-1

info_block *find_free_block(unsigned int size)
{
    info_block *cur = head;

    while (cur)
    {
        if (cur->is_free && cur->size >= size)
            return cur; // free block meet size requirement
        cur = cur->next;
    }

    return NULL;
}

void *allocate_aligned_block(unsigned int size, unsigned int align)
{
    void *p_cur, *p_block, *p_info_block;
    int cur, offset;

    // allocate block for info block
    if (sbrk(sizeof(info_block)) == ERROR_ON_SBRK)
        return NULL;

    p_cur = sbrk(0);
    cur = (uintptr_t)p_cur;

    // Get offset
    offset = cur % align;
    if (offset > 0)
        offset = align - offset;

    // increase offset to align
    if (sbrk(offset) == ERROR_ON_SBRK)
        return NULL;

    // get pointer to block
    p_block = sbrk(0);

    // increase block size
    if (sbrk(size) == ERROR_ON_SBRK)
        return NULL;

    // info block is behind block
    p_info_block = p_block - sizeof(info_block);

    return p_info_block;
}

void *aligned_malloc(unsigned int size, unsigned int align)
{
    void *return_block;
    info_block *info;
    void *new_block;

    // find in current list have free block meet size requirement.
    info = find_free_block(size);
    if (info)
    {
        // Found here
        info->is_free = 0; // not free now
        return_block = (void *)(info + 1);
        printf("Old block: Info at %lu - Block at %lu\n", (uintptr_t)info, (uintptr_t)return_block);
        return return_block;
    }

    // Not found any good free block

    // new block allocated with size must have info_block
    new_block = allocate_aligned_block(size, align);

    if (new_block == NULL)
        return NULL; // cannot allocate new block

    // store info block
    info = new_block;
    info->size = size;
    info->is_free = 0;
    info->next = NULL;

    // Update state of list via head and tail
    if (!head)
        head = info;
    if (tail)
        tail->next = info;
    tail = info;

    // Return new block
    return_block = (void *)(info + 1);
    printf("New block: Info at %lu - Block at %lu\n", (uintptr_t)info, (uintptr_t)return_block);
    return return_block;
}

void * aligned_free(void *block)
{

    info_block *info;
    info_block *tmp;
    void *program_break;

    if (!block) return NULL;

    // retrive block info
    info = (info_block *)block - 1;
    // get current program break;
    program_break = sbrk(0);

    // block to free is the last block in list, should update *tail and program break
    if (block + info->size == program_break)
    {
        if (head == tail)
            // no any left blocks
            head = tail = NULL;
        else
        {
            tmp = head;
            // find the block point to end block
            while (tmp)
            {
                if (tmp->next == tail)
                {
                    tmp->next = NULL;
                    tail = tmp;
                }
                tmp = tmp->next;
            }
        }
        // release memory of last block to OS
        sbrk(0 - sizeof(info_block) - info->size);
        printf("Release memory to OS\n");
        return sbrk(0);
    }

    // block to free is internal list, only update status free of info_block
    info->is_free = 1;
    return sbrk(0);
}