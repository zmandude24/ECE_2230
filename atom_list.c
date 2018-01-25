// See ECE 223 programming guide for required comments!
//
// You must have comments at the top of the file.
//
// EVERY function must have comments


#include <stdlib.h>
#include <assert.h>

#include "atom_list.h"

void atom_copy(struct atom_t *atom, struct atom_t *dest);

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

/* This function will determine if the atom's position is within a box or not. It will return 1 if it's in the box and 0 if it's not. */
int atom_list_determine_inside_box(struct atom_t *atom_ptr, float x_min,
                        float x_max, float y_min, float y_max)
{
    if ((atom_ptr->x_position >= x_min) && (atom_ptr->x_position <= x_max)) { // Check x-axis
	if ((atom_ptr->y_position >= y_min) && (atom_ptr->y_position <= y_max)) { // Check y-axis
	    return 1;
	}
	else return 0;
    }
    else return 0;
}

struct atom_list_t *atom_list_form_migrate_list(struct atom_list_t *list_ptr,
                        float x_min, float x_max, float y_min, float y_max)
{
    int migrate_list_size = 0;
    int i;

    for (i=0;i<list_ptr->atom_count;i++) {
	if (atom_list_determine_inside_box(list_ptr->atom_ptr[i],x_min,x_max,y_min,y_max) == 0) {
	    migrate_list_size++;
	}
    }

    if (migrate_list_size == 0) return NULL;
    else {
	struct atom_list_t *migrate_list;
	struct atom_t *rec_ptr = NULL;
	migrate_list = atom_list_construct(migrate_list_size);

	i = 0;
	while (i < list_ptr->atom_count) {
	    if (atom_list_determine_inside_box(list_ptr->atom_ptr[i],x_min,x_max,y_min,y_max) == 0) {
		rec_ptr = atom_list_remove(list_ptr,i);
		atom_list_add(migrate_list, rec_ptr);
		free(rec_ptr);
		rec_ptr = NULL;
	    }
	    else {
		i++;
	    }
	}
	return migrate_list;
    }
}

/* Finds the pointer for the matching index if it exists. If it doesn't exist, it will return a null pointer. */
struct atom_t *atom_list_access(struct atom_list_t *list_ptr, int index)
{
    if (index == -1) return NULL;
    else return list_ptr->atom_ptr[index];
}

/* Places the atom info to be deleted at the tail end of the list and returns a pointer to the tail if it's not there already. If the index is -1, then it will return a null pointer. */
struct atom_t *atom_list_remove(struct atom_list_t *list_ptr, int index)
{
    int i;
    struct atom_t *atom;

    if (index == -1) return NULL; // If the index is -1

    else if (index == list_ptr->atom_count-1) { // If it's already on the tail end
	list_ptr->atom_count--;
	return list_ptr->atom_ptr[list_ptr->atom_count];
    }

/* If it's not at the tail end, then it will copy the atom to be deleted to a dummy struct, shift the atoms on the right of it one index to the left, then put the atom to be deleted at the tail end.  */
    else {
	atom = (struct atom_t *)malloc(sizeof(struct atom_t));
	atom_copy(list_ptr->atom_ptr[index],atom); // Copy to dummy
	
	/* Shift the atoms on the right one index to the left */
	for (i=index+1;i<list_ptr->atom_count;i++) {
	    atom_copy(list_ptr->atom_ptr[i],list_ptr->atom_ptr[i-1]);
	}

	/* Stick the atom to be deleted on the tail end */
	atom_copy(atom,list_ptr->atom_ptr[list_ptr->atom_count-1]);
	
	free(atom);
	list_ptr->atom_count--;
	return list_ptr->atom_ptr[list_ptr->atom_count];
    }
}

/* This will copy the information of an atom to another atom */
void atom_copy(struct atom_t *atom, struct atom_t *dest)
{
    dest->atom_id = atom->atom_id;
    dest->atomic_num = atom->atomic_num;
    dest->mass = atom->mass;

    dest->x_position = atom->x_position;
    dest->y_position = atom->y_position;

    dest->x_velocity = atom->x_velocity;
    dest->y_velocity = atom->y_velocity;
}
