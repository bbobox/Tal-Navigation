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
#ifndef Extract_unitsH
#define Extract_unitsH
//---------------------------------------------------------------------------

#define MAX_TOKENS_BY_SENTENCE 100000

int sentence_buffer[MAX_TOKENS_BY_SENTENCE];


void extract_units(char,FILE*,struct text_tokens*,FILE*,FILE*);
void read_one_sentence(int*,FILE*,struct text_tokens*,int*,int*);
struct liste_matches* is_a_match_in_the_sentence(struct liste_matches*,int*,int,int);


#endif
