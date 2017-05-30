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
#ifndef LocatePatternH
#define LocatePatternH
//---------------------------------------------------------------------------
#include "unicode.h"
#include "String_hash.h"
#include "AutomateFst2.h"
#include "Text_tokens.h"
#include "Liste_nombres.h"
#include "Alphabet.h"
#include "Grammatical_codes.h"
#include "Loading_dic.h"
#include "Fst2_tags_optimization.h"
#include "DLC_optimization.h"
#include "Optimized_fst2.h"
#include "Pattern_transitions.h"
#include "Text_parsing.h"
#include "TransductionVariables.h"

#define TAILLE_MOT 10000
#define MOT -10
#define DIC -11
#define MAJ -12
#define MIN -13
#define PRE	-14
#define DIESE -15
#define EPSILON -16
#define SPACE_TAG -17
#define VAR_START -18
#define VAR_END -19
#define NB -20
#define CDIC -21
#define SDIC -22

#define OCCIDENTAL 0
#define THAI 1


unsigned char index_controle[400000];
unsigned char* index_code_gramm[400000];
int pattern_compose_courant=0;
struct noeud_code_gramm *racine_code_gramm;
int ESPACE;
int CHAR_BY_CHAR;
struct liste_nombres* tag_token_list=NULL;

int locate_pattern(char*,char*,char*,char*,char*,char*,int,int);
void numerote_tags(Automate_fst2*,struct string_hash*,int*,struct string_hash*,Alphabet*,int*,int*,int*);
void decouper_entre_angles(unichar*,unichar*,unichar*,unichar*,struct string_hash*,Alphabet*);
unsigned char get_controle(unichar*,Alphabet*);
void compute_token_controls(struct string_hash*,Alphabet*);

#endif
