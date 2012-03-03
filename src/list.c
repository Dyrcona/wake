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
#include "list.h"
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

/* Doubly linked list implementation. */

list_t *initialize_list(size_t count, ...)
{
	extern int errno;
	int error;
	va_list ap;

	list_t *head, *current;
	head = malloc(sizeof(list_t));
	if (head != NULL) {
		head->data = NULL;
		head->previous = NULL;
		head->next = NULL;
		current = head;
		va_start(ap, count);
		while (count--) {
			current->data = va_arg(ap, void *);
			if (count) {
				current->next = malloc(sizeof(list_t));
				if (current->next != NULL) {
					current->next->previous = current;
					current->next->next = NULL;
					current->next->data = NULL;
					current = current->next;
				}
				else {
					error = errno;
					free_list(head);
					errno = error;
					break;
				}
			}
			else {
				current->next = NULL;
			}
		}
		va_end(ap);
	}
	return head;
}

void free_list(list_t *list)
{
	list_t *temp;
	list = unwind_list(list);
	while (list != NULL) {
		temp = list->previous;
		free(list);
		list = temp;
	}
}

size_t count_list(list_t *list)
{
	size_t count = 0;
	list_t *temp = list;

	if (temp != NULL) {
		while (temp->previous != NULL)
			temp = temp->previous;
		while (temp != NULL) {
			count++;
			temp = temp->next;
		}            
	}

	return count;
}

list_t *rewind_list(list_t *list)
{
	while (list->previous != NULL)
		list = list->previous;
	return list;
}

list_t *unwind_list(list_t *list)
{
	while (list->next != NULL)
		list = list->next;
	return list;
}

list_t *append_list_data(list_t *list, void *data)
{
	list_t *temp;
	temp = list;
	while (temp->next != NULL)
		temp = temp->next;
	temp->next = malloc(sizeof(list_t));
	if (temp->next != NULL) {
		temp->next->previous = temp;
		temp->next->next = NULL;
		temp->next->data = data;
	}
	temp = temp->next;

	return temp;
}

list_t *prepend_list_data(list_t *list, void *data)
{
	list_t *temp;
	temp = list;
	while (temp->previous != NULL)
		temp = temp->previous;
	temp->previous = malloc(sizeof(list_t));
	if (temp->previous != NULL) {
		temp->previous->previous = NULL;
		temp->previous->next = temp;
		temp->previous->data = data;
	}
	temp = temp->previous;

	return temp;
}

list_t *insert_list_data_before(list_t *list, void *data)
{
	list_t *new = malloc(sizeof(list_t));
	if (new != NULL) {
		new->next = list;
		new->previous = list->previous;
		list->previous = new;
		if (new->previous != NULL)
			new->previous->next = new;
		new->data = data;
	}
	return new;
}

list_t *insert_list_data_after(list_t *list, void *data)
{
	list_t *new = malloc(sizeof(list_t));
	if (new != NULL) {
		new->previous = list;
		new->next = list->next;
		list->next = new;
		if (new->next != NULL)
			new->next->previous = new;
		new->data = data;
	}
	return new;
}

list_t *delete_list_member_before(list_t *list)
{
	list_t *member = list->previous;
	if (member != NULL) {
		list->previous = member->previous;
		if (list->previous != NULL)
			list->previous->next = list;
		free(member);
	}
	return list;
}

list_t *delete_list_member_after(list_t *list)
{
	list_t *member = list->next;
	if (member != NULL) {
		list->next = member->next;
		if (list->next != NULL)
			list->next->previous = list;
		free(member);
	}
	return list;
}

list_t *delete_list_member(list_t *list)
{
	list_t *temp;
	if (list->previous != NULL) {
		temp = list->previous;
		temp->next = list->next;
		if (temp->next != NULL)
			temp->next->previous = temp;
	}
	else {
		temp = list->next;
		if (temp != NULL) {
			temp->previous = list->previous;
			if (temp->previous != NULL)
				temp->previous->next = temp;
		}
	}
	free(list);

	return temp;   
}

list_t *search_list(list_t *list, void *des, int (*func)(void *, void*))
{
	list_t *found = list;

	while (found != NULL)
		if ((*func)(des, found->data) == 0)
			break;
		else
			found = found->next;

	return found;
}

void swap_list_data(list_t *first, list_t *second)
{
	void *temp;
	temp = first->data;
	first->data = second->data;
	second->data = temp;
}

list_t *get_list_index(list_t *list, size_t index)
{
	list_t *temp = list;

	while (temp->previous != NULL)
		temp = temp->previous;

	while (index > 0 && temp != NULL) {
		temp = temp->next;
		index--;
	}

	return temp;
}

void sort_list_data(list_t *list, int (*func)(void *, void *))
{
	list_t *next, *first;
	size_t i, left;
	list = rewind_list(list);
	left = count_list(list);
	while (--left > 0) {
		first = list;
		for (i = 0; i < left; i++) {
			next = first->next;
			if ((*func)(first->data, next->data) > 0)
				swap_list_data(first, next);
			first = next;
		}
	}
}
