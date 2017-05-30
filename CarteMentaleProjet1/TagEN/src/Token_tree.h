 /*
  * Unitex
  *
  * Copyright (C) 2001-2003 Universit� de Marne-la-Vall�e <unitex@univ-mlv.fr>
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
//---------------------------------------------------------------------------
#ifndef Token_treeH
#define Token_treeH
//---------------------------------------------------------------------------

struct token_tree_node {
   char final;
   struct token_tree_trans* trans;
};

struct token_tree_trans {
   int token;
   struct token_tree_node* arr;
   struct token_tree_trans* suivant;
};

struct token_tree_node* new_token_tree_node();
void free_token_tree_node(struct token_tree_node*);
struct token_tree_trans* new_token_tree_trans();
void free_token_tree_trans(struct token_tree_trans*);
int was_allready_in_token_tree(int*,int,struct token_tree_node*,int);

#endif
