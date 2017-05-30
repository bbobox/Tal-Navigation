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
/********************************************************/
/*  NOM : ligcomcomp.c               */
/*  systeme de levee d'ambiguites lexicales           */
/*  Objet :  analyse de la ligne de commande          */
/*  18 juillet 1998               */
/*  Auteurs : OULD ahmed, MAYER Laurent, Eric Laporte         */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>

#include "general.h"
#include "convcomm.h"
#include "autalmot.h"
#include "list_aut.h"
#include "compgr.h"

#include "utils.h"


// cpp inclusion

#include "convcomm.cpp"
#include "general.cpp"
#include "compgr.cpp"
#include "list_aut.cpp"
#include "entrsort.cpp"
#include "autalmot.cpp"
#include "deter.cpp"
#include "minim.cpp"
#include "concat.cpp"
#include "emonde.cpp"
#include "inter.cpp"
#include "decompte.cpp"
#include "variable.cpp"
#include "unicode.cpp"
#include "AutomateFst2.cpp"
#include "fst2autalmot.cpp"
#include "String_hash.cpp"

//



int main(int argc, char ** argv ) {

  static char buf[1024];

  char * progname    = *argv;
  char * compilename = NULL;
  char * ruledir     = NULL;
  char * rules       = NULL;

  for (int i = 0; i < argc; i++) { debug("-> %s\n", argv[i]); }

  argv++, argc--;


  while (argc) {

    if (**argv != '-') {

      rules = *argv;

    } else {

      if (strcmp(*argv, "-o") == 0) { // nom du fichier des grammaires compilee

	argv++, argc--;
	if (argc == 0) { die("-o argument needs a parameter\n"); }
      
	compilename = *argv;

      } else if (strcmp(*argv, "-d") == 0) { // rules directory

	argv++, argc--;
	if (argc == 0) { die("-d argument needs a parameter\n"); }

	ruledir = *argv;

      } else if (strcmp(*argv, "-r") == 0) { // rules file

	argv++, argc--;
	if (argc == 0) { die("-r argument needs a parameter\n"); }

	rules = *argv;

      } else if (strcmp(*argv, "-h") == 0) {

	printf("usage: %s -o <output> -d <rulesdir> [-r] <rulesfile>\n", progname); return 0;

      } else {

	die("unknow argument: '%s'\n", *argv);
      }
    }

    argv++, argc--;
  }


  if (! rules) { die("you must specified the rules file name\n"); }


  if (ruledir == NULL) {
    ruledir = dirname(strdup(rules));
    rules   = basename(rules);
  }

  debug("changing to %s directory\n", ruledir);
  if (chdir(ruledir) == -1) { error("unable to change to %s directory.\n", ruledir); }

  if (compilename == NULL) {

    int l = strlen(rules);

    if (strcmp(rules + l - 4, ".lst") == 0) {
      strcpy(buf, rules);
      strcpy(buf + l - 4, ".rul");
    } else {
      sprintf(buf, "%s.rul", rules);
    }
    compilename = buf;
  }


  list_aut * grammComp = compileGramm(ruledir, rules, compilename);

  if (! grammComp) {
    error("An error occured\n"); return 1;
  } else {
    printf("\nElag grammars are compiled in %s.\n%s", compilename, CR);
  }

  fflush(stdout);

  return 0 ;
}
