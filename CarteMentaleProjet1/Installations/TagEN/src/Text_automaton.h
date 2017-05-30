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
#ifndef Text_automatonH
#define Text_automatonH
//---------------------------------------------------------------------------

#include "unicode.h"
#include "Text_tokens.h"
#include "Alphabet.h"
#include "Load_DLF_DLC.h"
#include "String_hash.h"
#include "Normalization_transducer.h"

#define MAX_TOKENS 2000


struct trans_text_automaton {
  unichar* chaine;
  int indice_noeud_arrivee;
  struct trans_text_automaton* suivant;
};


struct noeud_text_automaton {
  int numero;
  char final;
  char controle;
  struct trans_text_automaton* trans;
};



int count_non_space_tokens(int*,int,int,int);
void construire_text_automaton(int*,int,struct text_tokens*,
                               struct noeud_dlf_dlc*,struct string_hash*,
                               Alphabet*,FILE*,int,int,int,int,
                               struct noeud_arbre_normalization*);


#endif
