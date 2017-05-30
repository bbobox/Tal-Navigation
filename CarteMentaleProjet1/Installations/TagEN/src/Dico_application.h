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
#ifndef Dico_applicationH
#define Dico_applicationH
//---------------------------------------------------------------------------
#include "unicode.h"
#include "Alphabet.h"
#include "Text_tokens.h"
#include "DELA.h"
#include "Token_tree.h"


struct offset_list {
  int offset;
  unichar* contenu_provisoire;
  struct offset_list* suivant;
};

struct word_struct {
  struct offset_list* list;
  struct word_transition* trans;
};


struct word_transition {
  int token_number;
  struct word_struct* arr;
  struct word_transition* suivant;
};


struct word_struct_array {
  struct word_struct** tab;
  int N;
};



#define BUFFER_SIZE 200000

struct word_struct_array* word_array;
FILE* DLF;
FILE* DLC;
FILE* ERR;
Alphabet* ALPH;
unsigned char* BIN;
struct INF_codes* INF;
FILE* TEXT;
struct text_tokens* TOK;
int buffer[BUFFER_SIZE];
int LENGTH;
int origine_courante;
struct token_tree_node* token_tree_root;
unsigned char* part_of_a_word;
unsigned char* has_been_processed;
int COMPOUND_WORDS=0;
int* n_occur=NULL;
int WORDS_HAVE_BEEN_COUNTED=0;
int ERRORS=0;
int SIMPLE_WORDS=0;
int CURRENT_BLOCK=1;


void liberer_word_struct(struct word_struct*);
void dico_application(unsigned char*,struct INF_codes*,
                             FILE*,struct text_tokens*,Alphabet*,
                             FILE*,FILE*,FILE*,int);
void init_dico_application(struct text_tokens*,FILE* dlf,FILE* dlc,FILE* err,FILE* text,Alphabet* alph);
void free_dico_application();
#endif
