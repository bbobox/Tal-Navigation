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
#ifndef GermanCompoundsH
#define GermanCompoundsH
//---------------------------------------------------------------------------
#include "unicode.h"
#include "Alphabet.h"
#include "DELA.h"
#include "String_hash.h"

Alphabet* german_alphabet;

struct german_word_decomposition {
   int n_parts;
   unichar decomposition[2000];
   unichar dela_line[2000];
};

struct german_word_decomposition_list {
   struct german_word_decomposition* element;
   struct german_word_decomposition_list* suivant;
};


void analyse_german_compounds(Alphabet*,unsigned char*,struct INF_codes*,FILE*,FILE*,FILE*,FILE*);
void check_valid_right_component_german(char*,struct INF_codes*);
void check_valid_left_component_german(char*,struct INF_codes*);
char check_valid_left_component_for_an_INF_line_german(struct token_list*);
char check_valid_left_component_for_one_INF_code_german(unichar*);
char check_valid_right_component_for_an_INF_line_german(struct token_list*);
char check_valid_right_component_for_one_INF_code_german(unichar*);
void analyse_german_word_list(unsigned char*,struct INF_codes*,FILE*,FILE*,FILE*,FILE*);
int analyse_german_word(unichar*);
void get_first_sia_code_german(int,unichar*);

struct german_word_decomposition* new_german_word_decomposition();
void free_german_word_decomposition(struct german_word_decomposition*);
struct german_word_decomposition_list* new_german_word_decomposition_list();
void free_german_word_decomposition_list(struct german_word_decomposition_list*);
void explore_state_german(int,unichar*,int,unichar*,int,unichar*,unichar*,struct german_word_decomposition_list**,int);

#endif
