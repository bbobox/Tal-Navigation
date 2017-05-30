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
#ifndef MatchesH
#define MatchesH
//---------------------------------------------------------------------------

#include "unicode.h"
#include "Text_parsing.h"

#define LONGUEST_MATCHES 0
#define SHORTEST_MATCHES 1
#define ALL_MATCHES 2
#define IGNORE_TRANSDUCTIONS 0
#define MERGE_TRANSDUCTIONS 1
#define REPLACE_TRANSDUCTIONS 2

struct liste_matches {
  int debut;
  int fin;
  unichar* output;
  struct liste_matches *suivant;
};


struct liste_matches *liste_match=NULL;
int nombre_match=0;
int longueur_avant=0;
int longueur_apres=0;
int statut_match;
int transduction_mode;
int SEARCH_LIMITATION=-1;


void  afficher_match_fst2(int,unichar*);
struct liste_matches* nouveau_match(unichar*);
void free_liste_matches(struct liste_matches*);
struct liste_matches* eliminer_shortest_match_fst2(struct liste_matches*,int,int*);
struct liste_matches* ecrire_index_des_matches(struct liste_matches*,int,
                                               long int*,FILE*);
struct liste_matches* load_match_list(FILE*,int*);

#endif
