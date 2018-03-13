/* list.c
 * John Woods
 * jzwoods
 * ECE 2230 Spring 2018
 * MP3
 *
 * Purpose: This is simply an extention of the list.c in mp2 that is compatable
 *      with a third type of list where atomic ids are randomly generated.
 *
 * Assumptions: Any input variable for the functions here from list.c or
 *      atom_support.c were checked for validity first. However, input
 *      variables from lab3.c or geninput.c might not be valid.
 *
 * Bugs: The program works as intended, but the recursive sort function is
 *      horrendously slow compared to the other sort functions. The other
 *      three sort functions are also a bit slower, but it might be that
 *      I'm using a virtual network.
 */

#include <stdlib.h>
#include <assert.h>

#include "datatypes.h"   /* defines data_t */
#include "list.h"        /* defines public functions for list ADT */

/* definitions for private constants used in list.c only */

#define LIST_SORTED_ASCENDING   -1234567
#define LIST_SORTED_DESCENDING  -7654321
#define LIST_UNSORTED           -4444444
int VALIDATE_STATUS = 1;

/* prototypes for private functions used in list.c only */
void list_debug_validate(list_t *L);

/* ----- below are the functions  ----- */

/* Obtains a pointer to an element stored in the specified list, at the
 * specified list position
 * 
 * list_ptr: pointer to list-of-interest.  A pointer to an empty list is
 *           obtained from list_construct.
 *
 * pos_index: position of the element to be accessed.  Index starts at 0 at
 *            head of the list, and incremented by one until the tail is
 *            reached.  Can also specify LISTPOS_HEAD and LISTPOS_TAIL
 *
 * return value: pointer to element accessed within the specified list.  A
 * value NULL is returned if the pos_index does not correspond to an element in
 * the list.  For example, if the list is empty, NULL is returned.
 */
data_t * list_access(list_t *list_ptr, int pos_index)
{
    int count;
    list_node_t *L;
 
    assert(list_ptr != NULL);

    /* debugging function to verify that the structure of the list is valid */
    list_debug_validate(list_ptr);

    /* handle three special cases.
     *   1.  The list is empty
     *   2.  Asking for the head 
     *   3.  Asking for the tail
     */
    if (list_ptr->current_list_size == 0) {
        return NULL;  /* list is empty */
    }
    else if (pos_index == LISTPOS_HEAD || pos_index == 0) {
        return list_ptr->head->data_ptr;
    }
    else if (pos_index == LISTPOS_TAIL || 
             pos_index == list_ptr->current_list_size - 1) {
        return list_ptr->tail->data_ptr;
    }
    else if (pos_index < 0 || pos_index >= list_ptr->current_list_size)
        return NULL;   /* does not correspond to position in list */

    /* we now know pos_index is for an interal element */
    /* loop through the list until find correct position index */
    L = list_ptr->head;
    for (count = 0; count < pos_index; count++) {
        L = L->next;
    }
    assert(L != NULL);
    return L->data_ptr;
}

/* Allocates a new, empty list 
 *
 * By convention, the list is initially assumed to be sorted.  The field sorted
 * can only take values LIST_SORTED_ASCENDING LIST_SORTED_DESCENDING or 
 * LIST_UNSORTED
 *
 * Use list_free to remove and deallocate all elements on a list (retaining the
 * list itself).
 *
 * comp_proc = pointer to comparison function
 *
 * Use the standard function free() to deallocate a list which is no longer
 * useful (after freeing its elements).
 */
list_t * list_construct(int (*fcomp)(const data_t *, const data_t *),
        void (*dataclean)(data_t *))
{
    list_t *L;
    L = (list_t *) malloc(sizeof(list_t));
    L->head = NULL;
    L->tail = NULL;
    L->current_list_size = 0;
    L->list_sorted_state = LIST_SORTED_ASCENDING;
    L->comp_proc = fcomp;
    L->data_clean = dataclean;

    /* the last line of this function must call validate */
    list_debug_validate(L);
    return L;
}

/* Finds an element in a list and returns a pointer to it.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: element against which other elements in the list are compared.
 *           Note: use the comp_proc function pointer found in the list_t 
 *           header block. 
 *
 * The function returns a pointer to the matching element with lowest index if
 * a match if found.  If a match is not found the return value is NULL.
 *
 * The function also returns the integer position of matching element with the
 *           lowest index.  If a matching element is not found, the position
 *           index that is returned should be -1. 
 *
 * pos_index: used as a return value for the position index of matching element
 *
 *
 */
data_t * list_elem_find(list_t *list_ptr, data_t *elem_ptr, int *pos_index)
{
    list_debug_validate(list_ptr);

    int i = 0;
    data_t *rec_ptr = NULL;
    *pos_index = -1;

    /* Search for the element */
    while (i<list_ptr->current_list_size) {
        rec_ptr = list_access(list_ptr, i);
        if (list_ptr->comp_proc(rec_ptr, elem_ptr) == 0) {
            *pos_index = i;
            break;
        }
        i++;
    }

    /* If the element isn't found */
    if (*pos_index == -1) rec_ptr = NULL;

    return rec_ptr;
}

/* Deallocates the contents of the specified list, releasing associated memory
 * resources for other purposes.
 *
 * Free all elements in the list, and the header block.  Use the data_clean
 * function point to free the data_t items in the list.
 */
void list_destruct(list_t *list_ptr)
{
    /* the first line must validate the list */
    list_debug_validate(list_ptr);

    /* Frees the tail each time it goes through the loop until only one item is left */
    while (list_ptr->current_list_size > 1) {
        /* Makes the second from the tail the new tail */
        list_ptr->tail = list_ptr->tail->prev;

        /* Frees the old tail */
        (list_ptr->data_clean)(list_ptr->tail->next->data_ptr);
        free(list_ptr->tail->next);
        list_ptr->tail->next = NULL;
        list_ptr->current_list_size--;
    }

    /* Once there is one item left */
    if (list_ptr->current_list_size == 1) {
	(list_ptr->data_clean)(list_ptr->head->data_ptr);
	free(list_ptr->head);
	list_ptr->current_list_size--;
    }

    /* Once there is nothing left */
    free(list_ptr);
    list_ptr = NULL;
}

/* Inserts the specified data element into the specified list at the specified
 * position.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: pointer to the element to be inserted into list.
 *
 * pos_index: numeric position index of the element to be inserted into the 
 *            list.  Index starts at 0 at head of the list, and incremented by 
 *            one until the tail is reached.  The index can also be equal
 *            to LISTPOS_HEAD or LISTPOS_TAIL (these are special negative 
 *            values use to provide a short cut for adding to the head
 *            or tail of the list).
 *
 * If pos_index is greater than the number of elements currently in the list, 
 * the element is simply appended to the end of the list (no additional elements
 * are inserted).
 *
 * Note that use of this function results in the list to be marked as unsorted,
 * even if the element has been inserted in the correct position.  That is, on
 * completion of this subroutine the list_ptr->list_sorted_state must be equal 
 * to LIST_UNSORTED.
 */
void list_insert(list_t *list_ptr, data_t *elem_ptr, int pos_index)
{
    assert(list_ptr != NULL);

    /* Create the new node */
    list_node_t *NewNode = NULL;
    NewNode = (list_node_t *)malloc(sizeof(list_node_t));

    /* If the list is empty to start with */
    if (list_ptr->current_list_size == 0) {
        /* Initialize the new node */
        NewNode->data_ptr = elem_ptr;
        NewNode->prev = NULL;
        NewNode->next = NULL;
        /* Place it on the list */
        list_ptr->head = NewNode;
        list_ptr->tail = NewNode;
        list_ptr->current_list_size++;
    }

    /* If the index is on the head */
    else if ((pos_index == 0) || (pos_index == LISTPOS_HEAD)) {
        /* Initialize the new node */
        NewNode->data_ptr = elem_ptr;
        NewNode->prev = NULL;
        NewNode->next = list_ptr->head;
        /* Place it on the list */
        list_ptr->head->prev = NewNode;
        list_ptr->head = NewNode;
        list_ptr->current_list_size++;
    }

    /* If the index is in the middle of the list */
    else if ((pos_index > 0) && (pos_index < list_ptr->current_list_size)) {
        /* Make a pointer to the place of interest */
        int i;
        list_node_t *rec_ptr = NULL;
        rec_ptr = list_ptr->head;
        for (i=0;i<pos_index;i++) {
            rec_ptr = rec_ptr->next;
        }
        /* Initialize the new node */
        NewNode->data_ptr = elem_ptr;
        NewNode->prev = rec_ptr->prev;
        NewNode->next = rec_ptr;
        /* Place it on the list */
        rec_ptr->prev->next = NewNode;
        rec_ptr->prev = NewNode;
        list_ptr->current_list_size++;
    }

    /* If the index is on the tail */
    else if ((pos_index >= list_ptr->current_list_size) || (pos_index = LISTPOS_TAIL)) {
        /* Initialize the new node */
        NewNode->data_ptr = elem_ptr;
        NewNode->prev = list_ptr->tail;
        NewNode->next = NULL;
        /* Place it on the list */
        list_ptr->tail->next = NewNode;
        list_ptr->tail = NewNode;
        list_ptr->current_list_size++;
    }

    /* the last two lines of this function must be the following */
    if (list_ptr->list_sorted_state != LIST_UNSORTED)
        list_ptr->list_sorted_state = LIST_UNSORTED;
    list_debug_validate(list_ptr);
}

/* Inserts the element into the specified sorted list at the proper position,
 * as defined by the comp_proc function pointer found in the header block.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: pointer to the element to be inserted into list.
 *
 * If you use list_insert_sorted, the list preserves its sorted nature.
 *
 * If you use list_insert, the list will be considered to be unsorted, even
 * if the element has been inserted in the correct position.
 *
 * If the list is not sorted and you call list_insert_sorted, this subroutine
 * should generate a system error and the program should immediately stop.
 *
 * The comparison procedure must accept two arguments (A and B) which are both
 * pointers to elements of type data_t.  The comparison procedure returns an
 * integer code which indicates the precedence relationship between the two
 * elements.  The integer code takes on the following values:
 *    1: A should be closer to the list head than B
 *   -1: B should be closer to the list head than A
 *    0: A and B are equal in rank
 * This definition results in the list being in ascending order.  To insert
 * in decending order, change the sign of the value that is returned.
 *
 * Note: if the element to be inserted is equal in rank to an element already
 * in the list, the newly inserted element will be placed after all the
 * elements of equal rank that are already in the list.
 */
void list_insert_sorted(list_t *list_ptr, data_t *elem_ptr)
{
    assert(list_ptr != NULL);
    assert(list_ptr->list_sorted_state != LIST_UNSORTED);

    /* insert your code here */
    /* Save the list order */
    int order = list_ptr->list_sorted_state;
    /* Find where the new element belongs */
    int i, pos_index;
    list_node_t *rec_ptr = NULL;
    rec_ptr = list_ptr->head;
    i = 0;
    if (order == LIST_SORTED_ASCENDING) {
        while (i < list_ptr->current_list_size) {
            if (list_ptr->comp_proc(rec_ptr->data_ptr, elem_ptr) == -1) break;
            i++;
            rec_ptr = rec_ptr->next;
        }
        pos_index = i;
    }
    else if (order == LIST_SORTED_DESCENDING) {
        while (i < list_ptr->current_list_size) {
            if (list_ptr->comp_proc(rec_ptr->data_ptr, elem_ptr) == 1) break;
            i++;
            rec_ptr = rec_ptr->next;
        }
        pos_index = i;
    }

    /* Insert the item on the list */
    list_ptr->list_sorted_state = LIST_UNSORTED;
    list_insert(list_ptr, elem_ptr, pos_index);
    list_ptr->list_sorted_state = order;

    /* the last line of this function must be the following */
    list_debug_validate(list_ptr);
}

/* Removes an element from the specified list, at the specified list position,
 * and returns a pointer to the element.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * pos_index: position of the element to be removed.  Index starts at 0 at
 *            head of the list, and incremented by one until the tail is
 *            reached.  Can also specify LISTPOS_HEAD and LISTPOS_TAIL
 *
 * Attempting to remove an element at a position index that is not contained in
 * the list will result in no element being removed, and a NULL pointer will be
 * returned.
 */
data_t * list_remove(list_t *list_ptr, int pos_index)
{
    list_debug_validate(list_ptr);

    list_node_t *rec_ptr = NULL;
    data_t *elem_ptr = NULL;

    /* If the list is not empty */
    if (list_ptr->current_list_size > 0) {
        /* If there is only one item on the list */
        if (list_ptr->current_list_size == 1) {
            /* Initialize the pointers */
            rec_ptr = list_ptr->head;
            elem_ptr = list_ptr->head->data_ptr;
            /* Adjust the list */
            list_ptr->head = NULL;
            list_ptr->tail = NULL;
            list_ptr->current_list_size--;
            /* Free the node */
            free(rec_ptr);
            rec_ptr = NULL;
        }

        /* If the item is on the head of the list */
        else if ((pos_index == 0) || (pos_index == LISTPOS_HEAD)) {
            /* Initialize the pointers */
            rec_ptr = list_ptr->head;
            elem_ptr = list_ptr->head->data_ptr;
            /* Adjust the list */
            list_ptr->head = list_ptr->head->next;
            list_ptr->head->prev = NULL;
            list_ptr->current_list_size--;
            /* Free the node */
            free(rec_ptr);
            rec_ptr = NULL;
        }

        /* If the item is in the middle of the list */
        else if ((pos_index > 0) && (pos_index < list_ptr->current_list_size-1)) {
            int i;
            /* Initialize the pointers */
            rec_ptr = list_ptr->head;
            for (i=0;i<pos_index;i++) {
                rec_ptr = rec_ptr->next;
            }
            elem_ptr = rec_ptr->data_ptr;
            /* Adjust the list */
            rec_ptr->prev->next = rec_ptr->next;
            rec_ptr->next->prev = rec_ptr->prev;
            list_ptr->current_list_size--;
            /* Free the node */
            free(rec_ptr);
            rec_ptr = NULL;
        }

        /* If the item is on the tail of the list */
        else if ((pos_index == list_ptr->current_list_size-1) || (pos_index == LISTPOS_TAIL)) {
            /* Initialize the pointers */
            rec_ptr = list_ptr->tail;
            elem_ptr = list_ptr->tail->data_ptr;
            /* Adjust the list */
            list_ptr->tail = list_ptr->tail->prev;
            list_ptr->tail->next = NULL;
            list_ptr->current_list_size--;
            /* Free the node */
            free(rec_ptr);
            rec_ptr = NULL;
        }
    }

    return elem_ptr;
}

/* Reverse the order of the elements in the list.  Also change the 
 * list_sorted_state flag.  This function can only be called on a list
 * that is sorted.
 *
 * list_ptr: pointer to list-of-interest.  
 */
void list_reverse(list_t *list_ptr)
{
    assert(list_order(list_ptr) != 0);

    /* Move the elements to their right locations */
    int i, size, state;
    data_t *rec_ptr = NULL;
    size = list_ptr->current_list_size;
    state = list_ptr->list_sorted_state; // Saving the sorting state
    i = 0;
    list_ptr->list_sorted_state = LIST_UNSORTED; // This is TEMPORARY
    for (i=0;i<size-1;i++) {
        rec_ptr = list_remove(list_ptr, LISTPOS_TAIL);
        list_insert(list_ptr, rec_ptr, i);
    }
    rec_ptr = NULL;

    /* Swap the sorted state on the list */
    if (state == LIST_SORTED_ASCENDING) 
        list_ptr->list_sorted_state = LIST_SORTED_DESCENDING;
    else list_ptr->list_sorted_state = LIST_SORTED_ASCENDING;

    // after the list is reversed verify it is valid.
    list_debug_validate(list_ptr);
}

/* Obtains the length of the specified list, that is, the number of elements
 * that the list contains.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * Returns an integer equal to the number of elements stored in the list.  An
 * empty list has a size of zero.
 */
int list_size(list_t *list_ptr)
{
    assert(list_ptr != NULL);
    assert(list_ptr->current_list_size >= 0);
    return list_ptr->current_list_size;
}

/* Obtains the sort status and order of the specified list. 
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * Returns 
 *    1: the list is sorted in ascending order
 *   -1: descending order
 *    0: the list is not sorted but a queue
 */
int list_order(list_t *list_ptr)
{
    assert(list_ptr != NULL);
    if (list_ptr->list_sorted_state == LIST_SORTED_ASCENDING)
        return 1;
    else if (list_ptr->list_sorted_state == LIST_SORTED_DESCENDING)
        return -1;
    else if (list_ptr->list_sorted_state == LIST_UNSORTED)
        return 0;
    else 
        exit(5);  // this should not happen
}


/* This function verifies that the pointers for the two-way linked list are
 * valid, and that the list size matches the number of items in the list.
 *
 * If the linked list is sorted it also checks that the elements in the list
 * appear in the proper order.
 *
 * The function produces no output if the two-way linked list is correct.  It
 * causes the program to terminate and print a line beginning with "Assertion
 * failed:" if an error is detected.
 *
 * The checks are not exhaustive, so an error may still exist in the
 * list even if these checks pass.
 *
 * YOU MUST NOT CHANGE THIS FUNCTION.  WE USE IT DURING GRADING TO VERIFY THAT
 * YOUR LIST IS CONSISTENT.
 */
void list_debug_validate(list_t *L)
{
    list_node_t *N;
    int count = 0;
    assert(L != NULL);
    if (VALIDATE_STATUS != 1)
        return;

    if (L->head == NULL)
        assert(L->tail == NULL && L->current_list_size == 0);
    if (L->tail == NULL)
        assert(L->head == NULL && L->current_list_size == 0);
    if (L->current_list_size == 0)
        assert(L->head == NULL && L->tail == NULL);
    assert(L->list_sorted_state == LIST_SORTED_ASCENDING 
            || L->list_sorted_state == LIST_SORTED_DESCENDING
            || L->list_sorted_state == LIST_UNSORTED);

    if (L->current_list_size == 1) {
        assert(L->head == L->tail && L->head != NULL);
        assert(L->head->next == NULL && L->head->prev == NULL);
        assert(L->head->data_ptr != NULL);
    }
    if (L->head == L->tail && L->head != NULL)
        assert(L->current_list_size == 1);
    if (L->current_list_size > 1) {
        assert(L->head != L->tail && L->head != NULL && L->tail != NULL);
        N = L->head;
        assert(N->prev == NULL);
        while (N != NULL) {
            assert(N->data_ptr != NULL);
            if (N->next != NULL)
                assert(N->next->prev == N);
            else
                assert(N == L->tail);
            count++;
            N = N->next;
        }
        assert(count == L->current_list_size);
    }
    if (L->list_sorted_state != LIST_UNSORTED && L->head != NULL) {
        N = L->head;
        int comp_val = -1 * list_order(L);
        while (N->next != NULL) {
            assert((L->comp_proc)(N->data_ptr, N->next->data_ptr) != comp_val);
            N = N->next;
        }
    }
}
/* commands for vim. ts: tabstop, sts: softtabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */

