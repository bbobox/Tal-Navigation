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
#ifndef Text_tokensH
#define Text_tokensH
//---------------------------------------------------------------------------
#include "Liste_nombres.h"
#include "String_hash.h"
#include "unicode.h"
#include "Alphabet.h"


struct text_tokens {
   unichar** token;
   int N;
   int SENTENCE_MARKER;
   int SPACE;
};


struct text_tokens* load_text_tokens(char*);
struct string_hash* load_text_tokens_hash(char*);
struct string_hash* load_text_tokens_hash(char*,int*);
void free_text_tokens(struct text_tokens*);
struct liste_nombres* get_token_list_for_sequence(unichar*,Alphabet*,struct string_hash*);
int get_token_number(unichar*,struct text_tokens*);

//---------------------------------------------------------------------------
#endif


