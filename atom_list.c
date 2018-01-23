// See ECE 223 programming guide for required comments!
//
// You must have comments at the top of the file.
//
// EVERY function must have comments


#include <stdlib.h>
#include <assert.h>

#include "atom_list.h"

struct atom_list_t *atom_list_construct(int size)
{
    return NULL;
}

void atom_list_destruct(struct atom_list_t *ptr)
{
}

int atom_list_number_entries(struct atom_list_t *list_ptr)
{
    return -1;
}

int atom_list_add(struct atom_list_t *list_ptr, struct atom_t *rec_ptr)
{
    return -1;
}

int atom_list_lookup_first_atomic_num(struct atom_list_t *list_ptr, int atomic_num)
{
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

struct atom_t *atom_list_access(struct atom_list_t *list_ptr, int index)
{
    return NULL;
}

struct atom_t *atom_list_remove(struct atom_list_t *list_ptr, int index)
{
    return NULL;
}



