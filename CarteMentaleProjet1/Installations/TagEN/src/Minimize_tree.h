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
//---------------------------------------------------------------------------
#ifndef Minimize_treeH
#define Minimize_treeH
//---------------------------------------------------------------------------

#define HAUTEUR_MAX 10000
#define MAX_TRANS 4000000

struct node_list {
   struct arbre_dico_trans* trans;
   struct node_list* suivant;
};

struct node_list* tab_by_hauteur[HAUTEUR_MAX];
struct arbre_dico_trans* tab_trans[MAX_TRANS];


void minimize_tree(struct arbre_dico*);
int compare_nodes(struct arbre_dico_trans*,struct arbre_dico_trans*);
void init_tab_by_hauteur();
void free_tab_by_hauteur();
int sort_by_height(struct arbre_dico*);
void insert_trans_in_tab_by_hauteur(struct arbre_dico_trans*,int);
struct node_list* new_node_list();
int convert_list_to_array(int);
void quicksort(int,int);
void fusionner(int);

#endif
