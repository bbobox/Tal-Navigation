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
#ifndef FlattenFst2H
#define FlattenFst2H
//---------------------------------------------------------------------------

#include "unicode.h"
#include "AutomateFst2.h"
#include "Liste_nombres.h"

#define FLATTEN_ERROR 0
#define EQUIVALENT_FST 1
#define APPROXIMATIVE_FST 2
#define EQUIVALENT_RTN 3



struct flattened_main_graph_info {
   Etat_comp* states;
   int size;
   int current_pos;
};


struct liste_nombres** dependences;
int* new_graph_number;

int flatten_fst2(Automate_fst2*,int,char*,int);
void compute_dependences(Automate_fst2*);
void compute_dependences_for_subgraph(Automate_fst2*,int,struct liste_nombres**);
void print_dependences(Automate_fst2*);
void check_for_graphs_to_keep(Automate_fst2*,int);
int renumber_graphs_to_keep(Automate_fst2*);
int flatten_main_graph(Automate_fst2*,int,struct flattened_main_graph_info*,int);
struct flattened_main_graph_info* new_flattened_main_graph_info();
void free_flattened_main_graph_info(struct flattened_main_graph_info*);
int flatten_sub_graph_recursively(Automate_fst2*,int,int,int,
                                  struct flattened_main_graph_info* new_main_graph,
                                  int,int,int*,int*);
void remove_epsilon_transitions_in_flattened_graph(struct flattened_main_graph_info*);
void compute_reverse_transitions_of_main_graph(struct flattened_main_graph_info*);
int determinisation_new_main_graph(FILE*,Etat_comp*);
void save_graphs_to_keep(Automate_fst2*,FILE*);
void save_graph_to_be_kept(int,Automate_fst2*,FILE*);
void copy_tags_into_file(Automate_fst2*,FILE*);

#endif
