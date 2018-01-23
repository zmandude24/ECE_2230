/* atom_list.h 
 * Prof. Calhoun 
 * ECE 2230 Spring 2018
 * MP1
 *
 * Purpose: The interface definition for MP1.  DO NOT CHANGE THIS FILE
 *
 * Assumptions: structure definitions and public functions as defined for
 * assignmnet.  There are no extensions.
 *
 * Bugs:
 */



struct atom_t {
    unsigned int atom_id;       // Unique atom id
    unsigned int atomic_num;    // Atomic number for atom
    float mass;                // Atomic mass
    
    float x_position;          // x coordinate of position
    float y_position;          // y coordinate of position

    float x_velocity;          // x component of velocity
    float y_velocity;          // y component of velocity
};


// Header block for the list
struct atom_list_t {
    int atom_count;       // current number of records in list
    int atom_size;        // size of the list
    struct atom_t **atom_ptr;
};

//  The sequential list ADT must have the following interface:

struct atom_list_t *atom_list_construct(int size);
void atom_list_destruct(struct atom_list_t *);
int atom_list_number_entries(struct atom_list_t *);
int atom_list_add(struct atom_list_t *, struct atom_t *);
int atom_list_lookup_first_atomic_num(struct atom_list_t *, int atomic_num);
int atom_list_determine_inside_box(struct atom_t *, float x_min, float x_max,
                        float y_min, float y_max);
struct atom_list_t *atom_list_form_migrate_list(struct atom_list_t *,
                        float x_min, float x_max, float y_min, float y_max);
struct atom_t *atom_list_access(struct atom_list_t *, int index);
struct atom_t *atom_list_remove(struct atom_list_t *, int index);


