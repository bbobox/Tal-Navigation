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
#include <stdio.h>

#include "autalmot.h"
#include "minim.h"
#include "fst2autalmot.h"
#include "utils.h"



int main(int argc, char ** argv) {

  argv++, argc--;

  if (argc < 1) { die("bad args\n"); }

  tAutAlMot * a1, * a2;

  if ((a1 = load_grammar_automaton(*argv)) == NULL) { die("unable to open %s\n", *argv); }


  argv++, argc--;

  a2 = minimise(a1);

  char * outname = argc ? *argv : (char *) "minim-out.fst2";

  autalmot_dump_fst2_fname(a2, outname);

  printf("minimisation in %s\n", outname);

  return 0;
}
