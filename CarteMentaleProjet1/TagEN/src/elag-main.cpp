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
/*  NOM : ligcomm.c               */
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
#include "elag-functions.h"

#include "utils.h"

// .cpp inclusion


#include "autalmot.cpp"
#include "general.cpp"
#include "convcomm.cpp"
#include "elag-functions.cpp"
#include "entrsort.cpp"
#include "inter.cpp"
#include "emonde.cpp"
#include "deter.cpp"
#include "minim.cpp"
#include "decompte.cpp"
#include "list_aut.cpp"
#include "variable.cpp"
#include "unicode.cpp"
#include "fst2autalmot.cpp"
#include "AutomateFst2.cpp"
#include "String_hash.cpp"



//


#ifndef MAX_PATH
#define MAX_PATH 1024
#endif


//static tConvComm convComm ;
//static tLigneComm ligneComm ;




int main(int argc, char ** argv) {


  list_aut * gramm = 0;

  char buf[MAX_PATH];

  char * progname   = *argv;
  char * txtauto    = NULL;
  char * grammardir = NULL;
  char * grammars   = NULL;
  char * onegrammar = NULL;
  char * output     = NULL;



  argv++, argc--;

  while (argc) {

    if (**argv != '-') { // text automaton
      
      txtauto = *argv;

    } else {

      if (strcmp(*argv, "-o") == 0) { // output
	
	argv++, argc--;
	if (argc == 0) { die("bad args\n"); }

	output = *argv;

      } else if (strcmp(*argv, "-d") == 0) { // grammars directory

	argv++, argc--;
	if (argc == 0) { die("bad args\n"); }

	grammardir = *argv;

      } else if (strcmp(*argv, "-c") == 0) { // file of compiled grammar names

	argv++, argc--;
	if (argc == 0) { die("bad args\n"); }

	grammars = *argv;

      } else if (strcmp(*argv, "-g") == 0) { // 1 grammar already compiled

	argv++, argc--;
	if (argc == 0) { die("bad args\n"); }

	onegrammar = *argv;

      } else if (strcmp(*argv, "-h") == 0) {

	printf("\nusage: %s <txtauto> -d <gramdir> [ -c <gramlist> | -g <grammar> ] -o <output>\n\n", progname);
	return 0;

      }	else { die("unknow arg: '%s'\n", *argv); }

    }

    argv++, argc--;
  }	


  if (txtauto == NULL) { die("no text automaton specified\n"); }

  if ((grammars && onegrammar) || !(grammars || onegrammar)) { die("either -c, either -g option should be used\n"); }

  if (output == NULL) {
    sprintf(buf, "%s.elag", txtauto);
    output = buf;
  }


  if (grammardir == NULL) {
    grammardir = dirname(strdup(grammars));
    grammars   = basename(grammars);
  }

  debug("changing to %s directory\n", grammardir);

  if (chdir(grammardir) == -1) { error("unable to change to %s directory.\n", grammardir); }


  if (grammars) { /* Une série de grammaires compilées */

    if ((gramm = chargeGramm(grammars)) == NULL) { die("unable to load grammar %s", grammars); }

    printf("Grammars are loaded.\n") ;

  } else {  /* une seule grammaire compilee */

    if ((gramm = chargeUneGramm(onegrammar)) == NULL) { die("unable to load grammar %s", grammars); }

    printf("Grammar is loaded.\n");
  }
  
  leve_ambiguite(txtauto, gramm, output);

  return 0;
}





#if 0
int oldmain(int argc, char * argv []) {

  list_aut * gramm = 0 ;
  convComm.nbOpt = 6 ;

  convComm.identif[0][0] = 0; /* parametre principal */
  convComm.parametre[0]  = 1;

  strcpy(convComm.identif[2], "e") ; /* fichier d'erreurs */
  convComm.parametre[2] = 1 ;

  strcpy(convComm.identif[1], "c") ; /* fichier des noms des grammaires compilees */
  convComm.parametre[1] = 1 ;

  strcpy(convComm.identif[3], "d") ; /* repertoire des regles */
  convComm.parametre[3] = 1 ;

  strcpy(convComm.identif[4], "g") ; /* une grammaires deja compilee */
  convComm.parametre[4] = 1 ;

  strcpy(convComm.identif[5], "s") ; /* fichier de sortie */
  convComm.parametre[5] = 1 ;

  convComm.tiret = 'o' ;
  convComm.separees = 1 ;

  lireLigneComm(& convComm, & ligneComm, argc, argv) ;
  if (ligneComm.activee[2] && ligneComm.parametre[2]) {
    fErr = fopen(ligneComm.parametre[2], "w") ;
    if (! fErr) {
      fprintf(stderr,"\nError: opening file %s\n", ligneComm.parametre[2]) ;
      return 0 ; }

  } else {
    fprintf(stderr,"\nTo use Elag, specify name of error file.\n") ;
    return 0 ;
  }

  if (! ligneComm.activee[0] || ! ligneComm.parametre[0]) {
    fprintf(fErr, "\nError: name of text automaton not found\n") ;
    fclose(fErr) ;
    exit(1) ;
  }

  if (! ligneComm.activee[3] || ! ligneComm.parametre[3]) {
    fprintf(fErr, "\nError: name of Elag directory not found\n") ;
    fclose(fErr) ;
    exit(1) ;
  }

  if (! ligneComm.activee[5] || ! ligneComm.parametre[5]) {
    fprintf(fErr, "\nError: name of output file not found\n") ;
    fclose(fErr) ;
    exit(1) ;
  }

  if (ligneComm.activee[1] && ligneComm.parametre[1]) {

    /* Une série de grammaires compilées */
    gramm = chargeGramm(ligneComm.parametre[3], ligneComm.parametre[1]) ;
    if (gramm)
      fprintf(fErr, "\nGrammar loaded.\n") ;

  } else if (ligneComm.activee[4] && ligneComm.parametre[4]) {

    /* une seule grammaire compilee */
    gramm = chargeUneGramm(ligneComm.parametre[3], ligneComm.parametre[4]) ;
    if (gramm)
      fprintf(fErr, "\nGrammar loaded.\n") ;

  } else {
    fprintf(fErr, "\nError: name of grammars not found\n") ;
    fclose(fErr) ;
    exit(1) ;
  }

  if (gramm) {
 
    debug("%d gram auto:\n", gramm->nb_aut);

    leve_ambiguite(ligneComm.parametre[0], gramm, ligneComm.parametre[5]) ;
  }

  fclose(fErr) ;

  return 0 ;
}
#endif
