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
#include "DLC_optimization.h"
//---------------------------------------------------------------------------


void optimiser_etat_dlc(struct etat_dlc* n) {
struct liste_nombres* tmp;
struct transition_dlc* t;
int i;
if (n==NULL) return;
if (n->nombre_patterns!=0) {
  n->tab_patterns=(int*)malloc(sizeof(int)*n->nombre_patterns);
  i=0;
  while (n->patterns!=NULL) {
    n->tab_patterns[i++]=n->patterns->n;
    tmp=n->patterns;
    n->patterns=n->patterns->suivant;
    free(tmp);
  }
}
if (n->nombre_transitions!=0) {
  n->tab_token=(int*)malloc(sizeof(int)*n->nombre_transitions);
  n->tab_arr=(struct etat_dlc**)malloc(sizeof(struct etat_dlc*)*n->nombre_transitions);
  i=0;
  while (n->liste!=NULL) {
    n->tab_token[i]=n->liste->token;
    n->tab_arr[i]=n->liste->arr;
    t=n->liste;
    n->liste=n->liste->suivant;
    optimiser_etat_dlc(n->tab_arr[i]);
    i++;
    free(t);
  }
}
}



void optimize_dlc() {
optimiser_etat_dlc(racine_dlc);
} 

