/* mem.c A template
 *
 * Lab4: Dynamic Memory Allocation
 * Version: 1
 * ECE 2230, Fall 2018
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>

#include "mem.h"

// Global variables required in mem.c only
static chunk_t Dummy = {&Dummy, &Dummy, 0, 0};
static chunk_t * Rover = &Dummy;
int NumPages = 0;
int NumSbrkCalls = 0;

// private function prototypes
void mem_validate(void);

/* function to request 1 or more pages from the operating system.
 *
 * new_bytes must be the number of bytes that are being requested from
 *           the OS with the sbrk command.  It must be an integer 
 *           multiple of the PAGESIZE
 *
 * returns a pointer to the new memory location.  If the request for
 * new memory fails this function simply returns NULL, and assumes some
 * calling function will handle the error condition.  Since the error
 * condition is catastrophic, nothing can be done but to terminate 
 * the program.
 */
chunk_t *morecore(int new_bytes) 
{
    char *cp;
    chunk_t *new_p;
    // preconditions
    assert(new_bytes % PAGESIZE == 0 && new_bytes > 0);
    assert(PAGESIZE % sizeof(chunk_t) == 0);
    cp = sbrk(new_bytes);
    if (cp == (char *) -1)  // no space available
        return NULL;
    new_p = (chunk_t *) cp;
    NumSbrkCalls++;
    NumPages += new_bytes/PAGESIZE;
    return new_p;
}

void coalescing(chunk_t *new_node);

/* deallocates the space pointed to by return_ptr; it does nothing if
 * return_ptr is NULL.  
 *
 * This function assumes that the Rover pointer has already been 
 * initialized and points to some memory block in the free list.
 */
void Mem_free(void *return_ptr)
{
    // precondition
    assert(Rover != NULL && Rover->next != NULL);

    chunk_t *new_node = NULL;
    chunk_t *rec_ptr = NULL;
    char returned = FALSE;

    if (return_ptr != NULL) {
        new_node = (chunk_t *)return_ptr;
        new_node--;
        rec_ptr = &Dummy;
        rec_ptr = rec_ptr->next;

        /* Search for the space starting with the first block */
        while (rec_ptr != &Dummy) {
            /* If new_node belongs before rec_ptr */
            if (rec_ptr >= new_node + new_node->size) {
                new_node->prev = rec_ptr->prev;
                new_node->next = rec_ptr;
                rec_ptr->prev->next = new_node;
                rec_ptr->prev = new_node;
                returned = TRUE;
                break;
            }
            /* If new_node belongs on the end */
            else if ((rec_ptr->next == &Dummy) && (rec_ptr + rec_ptr->size <= new_node)) {
                new_node->prev = rec_ptr;
                new_node->next = rec_ptr->next;
                rec_ptr->next->prev = new_node;
                rec_ptr->next = new_node;
                returned = TRUE;
                break;
            }
            rec_ptr = rec_ptr->next;
        }
        /* If the list is empty */
        if (rec_ptr->next == rec_ptr) {
            new_node->prev = rec_ptr;
            new_node->next = rec_ptr;
            rec_ptr->prev = new_node;
            rec_ptr->next = new_node;
            returned = TRUE;
        }

        assert(returned == TRUE);
        rec_ptr = NULL;

        /* Coalescing */
        if (Coalescing == TRUE) coalescing(new_node);
    }
}

void coalescing(chunk_t *new_node)
{
    chunk_t *fused_node = new_node;
    chunk_t *next_node = NULL;

    if (fused_node->prev + fused_node->prev->size == fused_node) {
        fused_node = new_node->prev;
        /* Disconnect new_node from the list */
        new_node->prev->next = new_node->next;
        new_node->next->prev = new_node->prev;
        new_node->prev = NULL;
        new_node->next = NULL;
        /* Update fused_node */
        fused_node->size = fused_node->size + new_node->size;
    }
    if (fused_node + fused_node->size == fused_node->next) {
        next_node = fused_node->next;
        /* Disconnect next_node from the list */
        next_node->prev->next = next_node->next;
        next_node->next->prev = next_node->prev;
        next_node->prev = NULL;
        next_node->next = NULL;
        /* Update fused_node */
        fused_node->size = fused_node->size + next_node->size;
    }
}

chunk_t *find_space_first(const int nsize);
chunk_t *find_space_best(const int nsize);
void remove_block(chunk_t *p);
void carve_block(chunk_t *p, const int nsize);

/* returns a pointer to space for an object of size nbytes, or NULL if the
 * request cannot be satisfied.  The memory is uninitialized.
 *
 * This function assumes that there is a Rover pointer that points to
 * some item in the free list.  By default, Rover is initialized to the
 * address of the dummy block whose size is one, but set the size field
 * to zero so this block can never be removed from the list.  Rover can
 * never be null.
 */
void *Mem_alloc(const int nbytes)
{
    // precondition
    assert(nbytes > 0);
    assert(Rover != NULL && Rover->next != NULL);

    chunk_t *p = NULL;
    chunk_t *rec_ptr = NULL;
    long nsize = (nbytes-1)/sizeof(chunk_t) + 2;
    int pages;

    if (SearchPolicy == FIRST_FIT) p = find_space_first(nsize);
    else {
        assert(SearchPolicy == BEST_FIT);
        p = find_space_best(nsize);
    }

    /* If no space was found make new page(s) into a block */
    if (p == NULL) {
        pages = (nbytes+sizeof(chunk_t)-1)/PAGESIZE + 1;
        p = morecore(pages*PAGESIZE);
        p->size = pages*PAGESIZE/sizeof(chunk_t);
        p->debug = 0;
        rec_ptr = &Dummy;
        p->next = rec_ptr;
        p->prev = rec_ptr->prev;
        rec_ptr->prev->next = p;
        rec_ptr->prev = p;
        rec_ptr = NULL;
    }

    /* Take p off the list */
    if (p->size == nsize) remove_block(p);
    else carve_block(p, nsize);

    /* Assert p is valid and set */
    assert((p->size - 1)*sizeof(chunk_t) >= nbytes);
    assert((p->size - 1)*sizeof(chunk_t) <= nbytes + sizeof(chunk_t));
    assert(p->prev == NULL);
    assert(p->next == NULL);
    assert(p->debug == 0);

    return (p + 1);
}

/*  This function searches for the first space on the list that is large
 *  enough for allocating. It will return NULL if one isn't found.
 */
chunk_t *find_space_first(const int nsize)
{
    chunk_t *rec_ptr = NULL;
    chunk_t *space_ptr = NULL;

    rec_ptr = &Dummy;
    rec_ptr = rec_ptr->next;
    /* Starting at the first block, search for a large enough space */
    while (rec_ptr != &Dummy) {
        if (rec_ptr->size >= nsize) {
            space_ptr = rec_ptr;
            break;
        }
        rec_ptr = rec_ptr->next;
    }
    rec_ptr = NULL;

    return space_ptr;
}

/*  This function searches for the best fit on the list and returns NULL
 *  if one isn't found.
 */
chunk_t *find_space_best(const int nsize)
{
    chunk_t *rec_ptr = NULL;
    chunk_t *space_ptr = NULL;
    long best_size = 0;

    rec_ptr = &Dummy;
    rec_ptr = rec_ptr->next;
    /* Starting at the first block, search for the best space */
    while (rec_ptr != &Dummy) {
        if (rec_ptr->size >= nsize) {
            /* For the first fit */
            if (best_size == 0) {
                space_ptr = rec_ptr;
                best_size = rec_ptr->size;
            }
            /* If rec_ptr->size is better than the best found */
            else if (rec_ptr->size < best_size) {
                space_ptr = rec_ptr;
                best_size = rec_ptr->size;
            }
            /* If a perfect fit is found */
            if (best_size == nsize) break;
        }
        rec_ptr = rec_ptr->next;
    }
    rec_ptr = NULL;

    return space_ptr;
}

// Removes the block with address p from the free list
void remove_block(chunk_t *p)
{
    p->prev->next = p->next;
    p->next->prev = p->prev;
    p->prev = NULL;
    p->next = NULL;
}

// Carves out nsize units from the block with address p
void carve_block(chunk_t *p, const int nsize)
{
    chunk_t *carved = NULL;
    /* Create the carved block */
    carved = p + nsize;
    carved->prev = p->prev;
    carved->next = p->next;
    carved->size = p->size - nsize;
    carved->debug = 0;
    /* Replace p with carved on the list */
    p->prev->next = carved;
    p->next->prev = carved;
    /* Disconnect and update p */
    p->prev = NULL;
    p->next = NULL;
    p->size = nsize;
}

/* prints stats about the current free list
 *
 * -- number of items in the linked list including dummy item
 * -- min, max, and average size of each item (in bytes)
 * -- total memory in list (in bytes)
 * -- number of calls to sbrk and number of pages requested
 *
 * A min and max size of 0 means an empty list or an error
 *
 * A message is printed if all the memory is in the free list
 */
void Mem_stats(void)
{
    long total_size, avg_size, min_size, max_size;
    long item_num = 0;
    chunk_t *p = Rover;
    chunk_t *start = p;
    total_size = 0;
    min_size = 0;
    max_size = 0;

    /* Find the total size, min/max, and the number of items on the list */
    do {
        total_size += p->size;
        item_num++;
        /* Check for min and max */
        if ((p->size < min_size) && (p->size > 0)) min_size = p->size;
        if (min_size == 0) min_size = p->size;
        if (p->size > max_size) max_size = p->size;
        /* Increment p */
        p = p->next;
    } while (p != start);

    /* Find the average size */
    if (item_num > 1) {
        avg_size = total_size/(item_num-1);
        if (total_size % (item_num-1) >= item_num/2) avg_size++;
    }

    printf("sizeof(chunk_t) = %ld\n", sizeof(chunk_t));
    if ((min_size > 0) && (max_size > 0)) {
        printf("items = %ld, min = %ld, max = %ld, avg = %ld, total = %ld\n",
            item_num, min_size*sizeof(chunk_t), max_size*sizeof(chunk_t), 
                avg_size*sizeof(chunk_t), total_size*sizeof(chunk_t));
        printf("pages = %d, sbrk calls = %d\n", NumPages, NumSbrkCalls);
    }
    else printf("The list is empty with only the dummy node.\n");
}

/* print table of memory in free list 
 *
 * The print should include the dummy item in the list 
 */
void Mem_print(void)
{
    assert(Rover != NULL && Rover->next != NULL);
    chunk_t *p = Rover;
    chunk_t *start = p;
    do {
        // example format.  Modify for your design
        printf("p=%p, size=%ld, end=%p, next=%p %s\n", 
                p, p->size, p + p->size, p->next, p->size!=0?"":"<-- dummy");
        p = p->next;
    } while (p != start);
    mem_validate();
}

/* This is an experimental function to attempt to validate the free
 * list when coalescing is used.  It is not clear that these tests
 * will be appropriate for all designs.  If your design utilizes a different
 * approach, that is fine.  You do not need to use this function and you
 * are not required to write your own validate function.
 */
void mem_validate(void)
{
    assert(Rover != NULL && Rover->next != NULL);
    assert(Rover->size >= 0);
    int wrapped = FALSE;
    int found_dummy = FALSE;
    int found_rover = FALSE;
    chunk_t *p, *largest, *smallest;

    // for validate begin at Dummy
    p = &Dummy;
    do {
        if (p->size == 0) {
            assert(found_dummy == FALSE);
            found_dummy = TRUE;
        } else {
            assert(p->size > 0);
        }
        if (p == Rover) {
            assert(found_rover == FALSE);
            found_rover = TRUE;
        }
        p = p->next;
    } while (p != &Dummy);
    assert(found_dummy == TRUE);
    assert(found_rover == TRUE);

    if (Coalescing) {
        do {
            if (p >= p->next) {
                // this is not good unless at the one wrap
                if (wrapped == TRUE) {
                    printf("validate: List is out of order, already found wrap\n");
                    printf("first largest %p, smallest %p\n", largest, smallest);
                    printf("second largest %p, smallest %p\n", p, p->next);
                    assert(0);   // stop and use gdb
                } else {
                    wrapped = TRUE;
                    largest = p;
                    smallest = p->next;
                }
            } else {
                assert(p + p->size < p->next);
            }
            p = p->next;
        } while (p != &Dummy);
        assert(wrapped == TRUE);
    }
}

/* vi:set ts=8 sts=4 sw=4 et: */

