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
#ifndef Arbre_mots_composesH
#define Arbre_mots_composesH
//---------------------------------------------------------------------------

#include "unicode.h"
#include "Alphabet.h"
#include "String_hash.h"
#include "Liste_nombres.h"

struct transition_dlc {
  int token;
  struct etat_dlc* arr;
  struct transition_dlc* suivant;
};

struct etat_dlc {
  struct liste_nombres* patterns;
  int nombre_patterns;
  int* tab_patterns;
  struct transition_dlc *liste;
  int nombre_transitions;
  int* tab_token;
  struct etat_dlc **tab_arr;
};


struct etat_dlc* nouveau_noeud_dlc();
int decouper_chaine_en_tokens(unichar*,int*,Alphabet*,struct string_hash*);
void ajouter_a_dlc_sans_code(unichar*,Alphabet*,struct string_hash*);
void ajouter_a_dlc_avec_code(unichar*,int,Alphabet*,struct string_hash*);
int remplacer_dans_dlc(unichar*,int,int,Alphabet*,struct string_hash*);



struct etat_dlc* racine_dlc=nouveau_noeud_dlc();
int n_dlc=0;
int t_dlc=0;
struct etat_dlc* tableau_dlc[1000000];

#endif
