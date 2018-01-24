// See ECE 223 programming guide for required comments!
//
// You must have comments at the top of the file.
//
// EVERY function must have comments


#include <stdlib.h>
#include <assert.h>

#include "atom_list.h"

/* This will create the framework for the main structure by allocating the memory needed 
   for the size chosen, as well as initializing key values. */
struct atom_list_t *atom_list_construct(int size)
{
    struct atom_list_t *L;

    /* Allocates the memory for the main list */
    L = (struct atom_list_t *)malloc(sizeof(struct atom_list_t));

    L->atom_count = 0;    // Initializes the number of records as zero
    L->atom_size = size;  // Sets the max amount of possible records

    /* Allocates the memory for the array of the struct containing the atom info */
    L->atom_ptr = (struct atom_t **)malloc(size*sizeof(struct atom_t *));

    /* Allows the main program to make use of the new list */
    return L;
}


/* This will free the memory from the array containing the structs with the atom info */
void atom_list_destruct(struct atom_list_t *ptr)
{
    free(ptr->atom_ptr);
}

/* Since the number of entries is already stored in atom_count, we just have to return that value */
int atom_list_number_entries(struct atom_list_t *list_ptr)
{
    return list_ptr->atom_count;
}

/* This will add the information of an atom to the end of the array, if the array isn't full. If the array is full, it will alert the user of it. */
int atom_list_add(struct atom_list_t *list_ptr, struct atom_t *rec_ptr)
{
    /* First the function checks to see if the array is full or not */
    if (list_ptr->atom_count >= list_ptr->atom_size) return -1;
    /* If it's not full, then it inserts the information at the end of the array */
    else {
	list_ptr->atom_ptr[list_ptr->atom_count] = rec_ptr;
	list_ptr->atom_count++;
	return 1;
    }
}

/* This function will find the first index of the atom with the specified atomic number if it exists. If it does not exist, then it will return -1 meaning no atom in the list has that atomic number. */
int atom_list_lookup_first_atomic_num(struct atom_list_t *list_ptr, int atomic_num)
{
    int i = 0; // Starting from index 0

    for (i=0;i<list_ptr->atom_count;i++) {
	if (list_ptr->atom_ptr[i]->atomic_num == atomic_num) {
	    return i; // If there's a match, return with the index
	}
    }
    return -1;
}

int atom_list_determine_inside_box(struct atom_t *atom_ptr, float x_min,
                        float x_max, float y_min, float y_max)
{
    return 1;
}

struct atom_list_t *atom_list_form_migrate_list(struct atom_list_t *list_ptr,
                        float x_min, float x_max, float y_min, float y_max)
{
    return NULL;
}

/* Finds the pointer for the matching index if it exists. If it doesn't exist, it will return a null pointer. */
struct atom_t *atom_list_access(struct atom_list_t *list_ptr, int index)
{
    if (index == -1) return NULL;
    else return list_ptr->atom_ptr[index];
}

struct atom_t *atom_list_remove(struct atom_list_t *list_ptr, int index)
{
    return NULL;
}



