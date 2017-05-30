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
#ifndef NorwegianCompoundsH
#define NorwegianCompoundsH
//---------------------------------------------------------------------------
#include "unicode.h"
#include "Alphabet.h"
#include "DELA.h"
#include "String_hash.h"
#include "Compounds.h"

Alphabet* norwegian_alphabet;

struct word_decomposition {
   int n_parts;
   unichar decomposition[2000];
   unichar dela_line[2000];
   int is_an_N;
   int is_an_a;
};

struct word_decomposition_list {
   struct word_decomposition* element;
   struct word_decomposition_list* suivant;
};


void analyse_norwegian_compounds(Alphabet*,unsigned char*,struct INF_codes*,FILE*,FILE*,FILE*,FILE*);
void check_valid_right_component(char*,struct INF_codes*);
void check_valid_left_component(char*,struct INF_codes*);
char check_valid_left_component_for_an_INF_line(struct token_list*);
char check_valid_left_component_for_one_INF_code(unichar*);
char check_valid_right_component_for_an_INF_line(struct token_list*);
char check_valid_right_component_for_one_INF_code(unichar*);
void analyse_norwegian_word_list(unsigned char*,struct INF_codes*,FILE*,FILE*,FILE*,FILE*);
int analyse_norwegian_word(unichar*);
void explorer_etat_decomposition(int,unichar*,int,int,unichar*,unichar*,unichar*,int,int*);
void explorer_etat(int,unichar*,int,int,unichar*,unichar*,unichar*,int,int*,int);
void get_first_sia_code(int,unichar*);
void ecrire_ligne_dico_sortie(unichar*,int);
char check_Nsia(unichar*);
char check_Asio(unichar*);
char check_VW(unichar*);
char check_ADV(unichar*);

struct word_decomposition* new_word_decomposition();
void free_word_decomposition(struct word_decomposition*);
struct word_decomposition_list* new_word_decomposition_list();
void free_word_decomposition_list(struct word_decomposition_list*);
void explore_state(int,unichar*,int,unichar*,int,unichar*,unichar*,struct word_decomposition_list**,int);

#endif
