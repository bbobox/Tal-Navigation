 /*
  * Unitex 
  *
  * Copyright (C) 2001-2003 Université de Marne-la-Vallée <unitex@univ-mlv.fr>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 2
  * of the License, or (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  *
  */



#include <stdlib.h>

#include "const.h"
#include "stack.h"


stack_t * stack_new(int size) {

  if (size == 0) { size = 1; }

  stack_t * stack = (stack_t *) malloc(sizeof(stack_t));
  stack->size = size;
  stack->data = (void **) malloc(stack->size * sizeof(void *));
  stack->head = 0;

  return stack;
}


void stack_delete(stack_t * stack) {
  free(stack->data);
  free(stack);
}

void stack_clear(stack_t * stack) { stack->head = 0; }

int stack_empty(stack_t * stack) { return (stack->head == 0); }


void stack_push(stack_t * stack, void * e) {

  if (stack->head == stack->size) {
    stack->size = 2 * stack->size;
    stack->data = (void **) realloc(stack->data, stack->size * sizeof(void *));
  }

  stack->data[stack->head++] = e;
}


void * stack_head(stack_t * stack) {
  if (stack->head == 0) { return (void *) UNDEF; }
  return stack->data[stack->head - 1];
}


void * stack_pop(stack_t * stack) {
  if (stack->head == 0) { return (void *) UNDEF; }
  return stack->data[--stack->head];
}

