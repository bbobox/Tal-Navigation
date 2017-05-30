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



#ifndef _STACK_H_
#define _STACK_H_


#define STACK_DEFAULT_SIZE  512

typedef struct stack_t {

  void ** data;
  int head;
  int size;
} stack_t;


stack_t * stack_new(int size = STACK_DEFAULT_SIZE);
void stack_delete(stack_t * stack);

void stack_clear(stack_t * stack);

int stack_empty(stack_t * stack);

void stack_push(stack_t * stack, void * elem);
void * stack_head(stack_t * stack);
void * stack_pop(stack_t * stack);

#endif
