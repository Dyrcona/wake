/*
 * liblist - A library to implement doubly-linked lists in C.
 * Copyright © 2012 Jason J.A. Stephenson <jason@sigio.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIST_INCL
#define LIST_INCL 1

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Doubly linked list implementation. */

/* Create the list_t type so we have a shorthand for our struct. */
typedef struct doubly_linked_list_elem list_t;

/* Implementation of a linked list node. You can manipulate these
 * directly or use the handy functions below to help with more common
 * tasks. */
struct doubly_linked_list_elem {
	void *data;
	list_t *previous;
	list_t *next;
};

/*
 * Initialize a list with count members. The count argument should be
 * followed by count number of pointers to the data to be stored in
 * the corresponding list member from left to right.
 *
 * Returns a pointer to the first list member or NULL if there is an error.
 */
list_t *initialize_list(size_t count, ...);

/*
 * Rewind list back to the beginning. It modifies its list argument to
 * point at the first member of the list if it does not
 * already. Returns a pointer to the first member of the list, also.
 */
list_t *rewind_list(list_t *list);

/*
 * Unwind list to the end. It modifies its list argument to point at
 * the last member of the list if it does not already do so. Retuns a
 * pointer to the last list member.
 */
list_t *unwind_list(list_t *list);

/*
 * Adds data to the end of the list. Returns a pointer to the newly
 * created list member or NULL if it is unable to allocate space.
 */
list_t *append_list_data(list_t *list, void *data);

/*
 * Adds data to the beginning of the list. Returns a pointer to the
 * newly created list member or NULL if it is unable to allocate
 * space.
 */
list_t *prepend_list_data(list_t *list, void *data);

list_t *insert_list_data_before(list_t *list, void *data);

list_t *insert_list_data_after(list_t *list, void *data);

list_t *delete_list_member_before(list_t *list);

list_t *delete_list_member_after(list_t *list);

list_t *delete_list_member(list_t *list);

/*
 * Returns the count of items in the list.
 */
size_t count_list(list_t *list);

list_t *get_list_index(list_t *list, size_t index);

void swap_list_data(list_t *first, list_t *second);

list_t *search_list(list_t *list, void *des, int (*func)(void *, void *));

void sort_list_data(list_t *list, int (*func)(void *, void*));

/*
 * Frees the space allocated to hold the list_t information in the
 * entire list that contains the argument pointer. This function does
 * NOT free the memory pointed to by the data member. This is mainly
 * because we don't know and have no way to find out if that member
 * points to static memory or to memory that was allocated with
 * free(). It is your responsibility to keep track of and to free this
 * data if necessary.
 */
void free_list(list_t *list);

#ifdef __cplusplus
}
#endif

#endif
