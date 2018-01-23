/* lab1.c
 * Prof. Jon Calhoun 
 * calhou3                              <--- replace with your user name!
 * ECE 2230 Spring 2018
 * MP1
 *
 *
 * Purpose: Driver for MP1 
 *
 * Assumptions: The functions to collect input
 * for a record and to print a record specify the format that is required for
 * grading.
 *
 * The program accepts one command line arguement that is the size of the list.
 *
 *
 * Bugs: None that I know.
 *
 * See the ECE 223 programming guide
 *
 * NOTE: One of the requirements is to verify you program does not have any 
 * memory leaks or other errors that can be detected by valgrind.  Run with
 * your test script(s):
 *      valgrind --leak-check=full ./lab1 5 < your_test_script
 *  and replace "5" with the value that makes sense for your test script
 * 
 * Are you unhappy with the way this code is formatted?  You can easily
 * reformat (and automatically indent) your code using the astyle 
 * command.  If it is not installed use the Ubuntu Software Center to 
 * install astyle.  Then in a terminal on the command line do
 *     astyle --style=kr lab1.c
 *
 * See "man astyle" for different styles.  Replace "kr" with one of
 * ansi, java, gnu, linux, or google to see different options.  Or, set up 
 * your own style.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "atom_list.h"

#define MAXLINE 256
#define FALSE 0
#define TRUE 1

// function prototypes for lab1.c
void fill_atom_record(struct atom_t *new, int atom_id);
void print_atom_rec_short(struct atom_t *rec);
void print_atom_rec_long(struct atom_t *rec);
void get_bounding_box(float* minX, float* maxX, float* minY, float* maxY);

int main(int argc, char *argv[])
{
    char line[MAXLINE];
    char command[MAXLINE];
    char junk[MAXLINE];
    int list_size = -1;
    int num_items = 0;
    int atomic_num = 0;
    int atom_id = 0;
    int i = 0;
    int found = FALSE;
    struct atom_t *rec_ptr = NULL;

    if (argc != 2) {
        printf("Usage: ./lab1 list_size\n");
        exit(1);
    }
    list_size = atoi(argv[1]);
    if (list_size < 1) {
        printf("Invalid list size %d\n", list_size);
        exit(2);
    } 
    printf("The list size is %d. Possible commands:\n", list_size);
    printf("INSERT\nFIND atomic#\nREMOVE atomic#\nPRINT\nSTATS\nMIGRATE\nQUIT\n");

    struct atom_list_t *my_list = atom_list_construct(list_size);

    // remember fgets includes newline \n unless line too long 
    while (fgets(line, MAXLINE, stdin) != NULL) {
        num_items = sscanf(line, "%s %d %s", command, &atomic_num, junk);
        

        if (num_items == 1 && strcmp(command, "QUIT") == 0) {
            printf("Goodbye\n");

            /* remove list from back to front to advoid shifting */
            int num_in_list = atom_list_number_entries(my_list);
            for (i = num_in_list-1; i>0; i--) {
                rec_ptr = atom_list_remove(my_list, i);
                free(rec_ptr);
            }
            
            atom_list_destruct(my_list);
            break;
        }


        
        else if (num_items == 1 && strcmp(command, "INSERT") == 0) {
            rec_ptr = NULL;   
            rec_ptr = (struct atom_t *) malloc(sizeof(struct atom_t));
            fill_atom_record(rec_ptr, atom_id);
            atom_id++;

            int added_return = -2;
            added_return = atom_list_add(my_list, rec_ptr);

            if (added_return == 1) {
                printf("Inserted: %d\n", rec_ptr->atomic_num);
            } else if (added_return == 0) {
                printf("Updated: %d\n", rec_ptr->atomic_num);
            } else if (added_return == -1) {
                printf("Rejected: %d\n", rec_ptr->atomic_num);
                free(rec_ptr);
            } else {
                printf("Error with return value! Fix your code.\n");
            }
            rec_ptr = NULL;
        }


        else if (num_items == 2 && strcmp(command, "FIND") == 0) {
            int index = -1;
            rec_ptr = NULL;  

            index = atom_list_lookup_first_atomic_num(my_list, atomic_num);
            rec_ptr = atom_list_access(my_list, index);

            if (rec_ptr == NULL) {
                printf("Did not find: %d\n", atomic_num);
            } else {
                /* print items in structure */
                found = TRUE;
                do {
                    printf("Found: %d at index %d\n", atomic_num, index);
                    print_atom_rec_long(rec_ptr);

                    // check next index position to see if same atomic num
                    index++;
                    rec_ptr = atom_list_access(my_list, index);
                    if (rec_ptr == NULL || rec_ptr->atomic_num != atomic_num) {
                        found = FALSE;
                    }
                } while (found);
            }
            rec_ptr = NULL;
        }




        else if (num_items == 1 && strcmp(command, "MIGRATE") == 0) {
            struct atom_list_t * migrate_ptr = NULL;
            float minX, maxX, minY, maxY = 0.0;

            get_bounding_box(&minX, &maxX, &minY, &maxY);
            migrate_ptr = atom_list_form_migrate_list(my_list, minX, maxX, minY, maxY);

            if (migrate_ptr == NULL) {
                printf("Did not find atoms to migrate in : %e %e %e %e\n", minX, maxX, minY, maxY);
            } else {
                /* print items in structure */
                printf("Found atoms to migrate:\n");
            
                int num_in_list = atom_list_number_entries(migrate_ptr);
                for (i=0; i < num_in_list; i++) {
                     rec_ptr = atom_list_access(migrate_ptr, i);
                    if (rec_ptr != NULL) {
                        print_atom_rec_long(rec_ptr);
                    }
                    else {
                        printf("Error in migrate list: NULL value\n");
                        break;
                    }
                }
            
                // remove list from back to front to advoid shifting
                for (i = num_in_list-1; i>0; i--) {
                    rec_ptr = atom_list_remove(migrate_ptr, i);
                    free(rec_ptr);
                }
                atom_list_destruct(migrate_ptr);
                rec_ptr = NULL;
            }
        }


        else if (num_items == 2 && strcmp(command, "REMOVE") == 0) {
            int index = -1;
            rec_ptr = NULL;   

            index = atom_list_lookup_first_atomic_num(my_list, atomic_num);
 
            if (index == -1) {
                printf("Did not remove: %d\n", atomic_num);
            } else {
                rec_ptr = atom_list_remove(my_list, index);
                printf("Removed: %d\n", atomic_num);
                print_atom_rec_short(rec_ptr);
                print_atom_rec_long(rec_ptr);
                free(rec_ptr);
                /*
                found = TRUE;
                do {
                    rec_ptr = atom_list_remove(my_list, index);
                    printf("Removed: %d\n", atomic_num);
                    print_atom_rec_long(rec_ptr);
                    free(rec_ptr);
                    rec_ptr = atom_list_access(my_list, index);
                    if (rec_ptr == NULL || rec_ptr->atomic_num != atomic_num) {
                        found = FALSE;
                    }
                } while (found);
                */
            }
            rec_ptr = NULL;
        }

        
        else if (num_items == 1 && strcmp(command, "STATS") == 0) {
            // get the number in list and order of the list
            int num_in_list = 0;   
            num_in_list = atom_list_number_entries(my_list);
            printf("Number records: %d\n", num_in_list);
        }

        
        else if (num_items == 1 && strcmp(command, "PRINT") == 0) {
            int num_in_list = 0;   
            num_in_list = atom_list_number_entries(my_list);
            
            if (num_in_list == 0) {
                printf("List empty\n");
            } else {
                printf("List has %d records\n", num_in_list);
                for (i = 0; i < num_in_list; i++) {
                    printf("%4d: ", i);
                    rec_ptr = NULL;    
                    rec_ptr = atom_list_access(my_list, i);
                    print_atom_rec_short(rec_ptr);
                }
            }
            rec_ptr = NULL;
        } else {
            printf("UNKNOWN COMMAND: %s", line);
        }
    }
    return 0;
}

/* If a string ends with an end-of-line \n, remove it.
 */
void chomp(char *str) 
{
    int lastchar = strlen(str) - 1;
    if (lastchar >= 0 && str[lastchar] == '\n') {
        str[lastchar] = '\0';
    }
}

/* Prompts user for atom record input.
 * The input is not checked for errors but will default to an acceptable value
 * if the input is incorrect or missing.
 *
 * The input to the function assumes that the structure has already been
 * created.  The contents of the structure are filled in.
 *
 * There is no output.
 *
 * DO NOT CHANGE THIS FUNCTION!
 */
void fill_atom_record(struct atom_t *new, int atom_id)
{
    char line[MAXLINE];
    assert(new != NULL);

    new->atom_id = atom_id;

    printf("Atomic Number:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%u", &new->atomic_num);

    printf("Atomic Mass:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%e", &new->mass);
    

    printf("Atom X Position:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%e", &new->x_position);
    printf("Atom Y Position:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%e", &new->y_position);
    

    printf("Atom X Velocity:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%e", &new->x_velocity);
    printf("Atom Y Velocity:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%e", &new->y_velocity);
    

    printf("\n");
}

/* Prompts user for x and y values to create a box in 2D space. The lower left
 * corner is defined as (x_min, y_min) and the upper right corner is defined as
 * (x_max, y_max).  The input is not checked for errors but will default to an
 * acceptable value if the input is incorrect or missing.
 *
 * The input to the function assumes that the structure has already been
 * created.  The contents of the structure are filled in.
 *
 * There is no output.
 *
 * DO NOT CHANGE THIS FUNCTION!
 */
void get_bounding_box(float* minX, float* maxX, float* minY, float* maxY) {

    char line[MAXLINE];
    
    printf("Min X:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%e", minX);
    
    printf("Max X:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%e", maxX);
    

    printf("Min Y:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%e", minY);
    
    printf("Max Y:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%e", maxY);

}

/* print the information for a particular atom record in short form
 *
 * Input is a pointer to a record, and no entries are changed.
 *
 * DO NOT CHANGE THIS FUNCTION!
 */
void print_atom_rec_short(struct atom_t *rec)
{
    assert(rec != NULL);
    printf("Atom ID: %d position = (%e, %e)\n", rec->atom_id, rec->x_position, rec->y_position);
}

/* Long form to print a particular atom record 
 *
 * Input is a pointer to a record, and no entries are changed.
 *
 * DO NOT CHANGE THIS FUNCTION!
 */
void print_atom_rec_long(struct atom_t *rec)
{
    assert(rec != NULL);
    printf("Atom ID: %d\n", rec->atom_id);
    printf("     atomic # = %d\n", rec->atomic_num);
    printf("     mass     = %e\n", rec->mass);
    printf("     position = (%e, %e)\n", rec->x_position, rec->y_position);
    printf("     velocity = (%e, %e)\n", rec->x_velocity, rec->y_velocity);
    printf("\n");
}


/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
