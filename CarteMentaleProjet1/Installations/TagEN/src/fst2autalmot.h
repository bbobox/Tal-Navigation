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
#ifndef _FST2AUTALMOT_H_
#define _FST2AUTALMOT_H_

#include "AutomateFst2.h"
#include "autalmot.h"
#include "list_aut.h"

tAutAlMot * fst2AutAlMot(Automate_fst2 * A, int nb, tAlphMot * alphabetLu);

list_aut  * load_text_automaton(char * fname);
tAutAlMot * load_grammar_automaton(char * fname);

void text_output_fst2(list_aut * txt, FILE * f);
int  text_output_fst2_fname(list_aut * txt, char * fname);

#endif
