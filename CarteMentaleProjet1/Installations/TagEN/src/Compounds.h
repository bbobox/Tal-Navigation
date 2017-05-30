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
#ifndef CompoundsH
#define CompoundsH
//---------------------------------------------------------------------------
#include "unicode.h"
#include "Alphabet.h"
#include "DELA.h"
#include "String_hash.h"


unsigned char* tableau_bin;
char* tableau_sia;
char* tableau_correct_right_component;
FILE* debug_file;
FILE* result_file;
struct INF_codes* inf_codes;


void get_first_sia_code(int,unichar*);
void ecrire_ligne_dico_sortie(unichar*,int);
int next_word(FILE*,unichar*);

#endif













