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
#include "list_aut.h"
#include "entrsort.h"
#include "fst2autalmot.h"
#include "utils.h"


int main(int argc, char ** argv) {

  if (argc < 2) { die("usage: %s <txtauto>\n", *argv); }

  argv++, argc--;

  list_aut * txtauto = load_text_automaton(*argv);

  int len = strlen(*argv);
  char buf[len + 5];

  if (strcmp(*argv + len - 5, ".fst2") == 0) {
    strcpy(buf, *argv);
    strcpy(buf + len - 5, "-exp.fst2");
  } else {
    sprintf(buf, "%s.exp", *argv);
  }

  if (text_output_fst2_fname(txtauto, buf) == -1) { die("unable to explode fst in %s\n", buf); }

  printf("%s exploded in %s.\n", *argv, buf);

  return 0;
}
