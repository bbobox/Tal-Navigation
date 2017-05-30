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


/* compgr.c */

/* Date 	: juin 98 */
/* Auteur(s) 	: MAYER Laurent et al */
/* compilation des grammaires de levee d ambiguites */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "general.h"
#include "autalmot.h"
#include "list_aut.h"
#include "compgr.h"
#include "entrsort.h"
#include "deter.h"
#include "concat.h"
#include "inter.h"
#include "emonde.h"
#include "minim.h"

#include "utils.h"


static void lireRegle(tRegle * regle, char * nomFRegle, int numRegle) ;
static BOOL prepareRegle(tRegle * regle) ;
static int compteContraintes(tAutAlMot * aut, etat * contrainte) ;
static BOOL compileRegle(tRegle * regle, int numero) ;
static int separeAut(tAutAlMot * autLu, tAutAlMot * r, etat depart, char lim) ;
static void separeCont(tAutAlMot * autLu, tAutAlMot * r, etat depart, etat arrivee) ;
static int suivre(tAutAlMot * autLu, tAutAlMot * r, etat e, unichar delim, int * corresp) ;
static BOOL suivreCont(tAutAlMot * autLu, tAutAlMot * r, etat depart, etat arrivee, int * corresp) ;
static tAutAlMot * combinaison(tRegle * regle, int ens, tAutAlMot * AetoileR1, tAutAlMot * R2Aetoile) ;
static list_aut * regroupeRegles(tRegle * regle, int premiere, int nbRegles, FILE * compos, int nbGramm, char * nomFichNomRegles) ;





/* Lit les regles de levee d'ambiguites et les compile sous la forme d'automates. 
 * nomFichNomRegles est le fichier des noms des grammaires. 
 * nomFichNomGramm est le fichier de la composition des grammaires compilées.
 */


list_aut * compileGramm(char * nomRepertoire, char * nomFichNomRegles, char * nomFichNomGramm) {

  tRegle regle[maxRegles] ;
  int nbRegles = 0, r, nomLu, premiere, nbGramm = 0 ;
  list_aut * gramm = 0, * cumul = 0 ;
  char  nom[maxNomFich] ;
  FILE * fGramm, * compos ;

  if ((fGramm = fopen(nomFichNomRegles, "r")) == NULL) { die("cannot open file %s\n", nomFichNomRegles); }

  if ((compos = fopen(nomFichNomGramm, "w")) == NULL) { die("cannot open file %s\n", nomFichNomGramm); }


  do {
    nomLu = fscanf(fGramm, "%s", nom);
  } while (nomLu == 1 && nom[0] == '#');  /* On passe les commentaires. */


  while (nomLu == 1) {  /* Il reste au moins une grammaire a compiler.  */

    do {  /* On lit une serie d'au plus maxRegles regles. */

      debug("%s\n", nom);
      lireRegle(regle, nom, nbRegles);
      nbRegles++;

      do {
	nomLu = fscanf(fGramm, "%s", nom);   /* On passe les commentaires. */
      } while (nomLu == 1 && nom[0] == '#');

    } while (nomLu == 1 && (nbRegles % maxRegles));


    if (nbRegles % maxRegles) {
      premiere = nbRegles - nbRegles % maxRegles;
    } else {
      premiere = nbRegles - maxRegles;
    }

    for (r = premiere ; r < nbRegles ; r++) {
      if (! prepareRegle(regle + (r % maxRegles))) { die("cannot prepareRegle\n"); }
    }

    printf("\n%d grammars read.\n", nbRegles - premiere) ;

    for (r = premiere; r < nbRegles; r ++) { compileRegle(regle + (r % maxRegles), r); }

    printf("%d grammars compiled.\n", nbRegles - premiere) ;

    // dobreak = 1;

    gramm = regroupeRegles(regle, premiere, nbRegles, compos, nbGramm, nomFichNomRegles) ;

    if (gramm) {

      printf("\n%d grammars compiled into %d grammars. \n", nbRegles - premiere, gramm->nb_aut) ;

      if (cumul) {

	fusion(cumul, gramm);
	gramm = 0;

      } else { cumul = gramm; }

      nbGramm = cumul->nb_aut;
    }
  }

  fclose(fGramm);
  fclose(compos);

  return cumul ;
}




static void lireRegle(tRegle * regle, char * nomFRegle, int numRegle) {

  //  debug("lireRegle %s, %d\n", nomFRegle, numRegle);

  int l = strlen(nomFRegle);

  if (strcmp(nomFRegle + l - 5, ".fst2") != 0) {

    if (strcmp(nomFRegle + l - 4, ".grf") == 0) { die("%s: the compiled graph should be specified.\n", nomFRegle); }

    strcat(nomFRegle, ".fst2");
  }

  if (strlen(nomFRegle) >= maxNomFich) { die("Name %s too long.\n", nomFRegle); }

  strcpy(regle[numRegle % maxRegles].nom, nomFRegle);
  regle[numRegle % maxRegles].autLu = chargeAutGramm(nomFRegle);

  /*
  autalmot_dump_plain(regle[numRegle % maxRegles].autLu);

  char buf[128];
  sprintf(buf, "regle%d.fst2", numRegle);

  autalmot_dump_fst2_fname(regle[numRegle % maxRegles].autLu, buf);
  */
} 



/* make autalmot for pattern matching of context ...
 */

static tAutAlMot * make_locate_auto(tRegle * regle) {

  debug("make_locate_auto\n");

  tAutAlMot * res = copieAutomate(regle->contexte[0].G);
  autalmot_concat(res, regle->contexte[0].D);

  autalmot_dump_plain(res);

  return res;
}


/* Reconnait les 4 parties d'une regle
 */

static BOOL prepareRegle(tRegle * regle) {

  tTransitions * t;
  int finR1, finR2, finC2, nbContraintes, c;
  etat contrainte[maxContraintes];	/* Etats buts des transitions etiquetees par les '=' medians des contraintes. */
  BOOL succes = TRUE;

  nbContraintes      = compteContraintes(regle->autLu, contrainte);
  regle->nbContextes = nbContraintes + 1;
  regle->contexte    = (tContexte *) xcalloc(regle->nbContextes, sizeof(tContexte));

  regle->contexte[0].G = regle->contexte[0].D = NULL;
  regle->contexte[1].G = regle->contexte[1].D = NULL;
  finR1 = finR2 = finC2 = NEXISTEPAS ;


  debug("prepareRegle\n");
  autalmot_dump_plain(regle->autLu);
  autalmot_dump_fst2_fname(regle->autLu, "regle.fst2");

  //#warning "what does it mean?"

  //  regle->autLu->sorte[0] += 4 ;    /* on le marque */
  regle->autLu->type[0] |= AUT_MARK;

  for (t = regle->autLu->etats[0]; t ; t = t->suivant) {

    switch (t->etiq->gramm[0]) {

    case '!' :

      if (regle->contexte[0].G) { die("too much '!'\n", regle->nom); }

      debug("contexte G\n");
      regle->contexte[0].G = (tAutAlMot *) xcalloc(1, sizeof(tAutAlMot));
      finR1 = separeAut(regle->autLu, regle->contexte[0].G, t->but, '!');

      debug("contexte D\n");
      regle->contexte[0].D = (tAutAlMot *) xcalloc(1, sizeof(tAutAlMot));  
      finR2 = separeAut(regle->autLu, regle->contexte[0].D, (etat) finR1, '!');

      debug("OK");

      if (finR1 == NEXISTEPAS || finR2 == NEXISTEPAS || ! final(regle->autLu, finR2)) {	succes = FALSE; }
      break ;


    case '=' :

      if (regle->contexte[1].G) { die("nondeterministic .fst file\n") ; }

      for (c = 0 ; c < nbContraintes ; c++) {

	regle->contexte[c + 1].G = (tAutAlMot *) xcalloc(1, sizeof(tAutAlMot));

	separeCont(regle->autLu, regle->contexte[c + 1].G, t->but, contrainte[c]) ;
	regle->contexte[c + 1].D = (tAutAlMot *) xcalloc(1, sizeof(tAutAlMot)) ;  

	finC2 = separeAut(regle->autLu, regle->contexte[c + 1].D, contrainte[c], '=') ;

	if (finC2 == NEXISTEPAS || ! final(regle -> autLu, finC2)) { succes = FALSE ; }
      }
      break ;

    default :
      die("in grammar: left delimitor '!' or '=' lacking, %S,%S.%S\n", t->etiq->flechie, t->etiq->canonique, t->etiq->gramm) ;
    }
  }

  libereAutAlMot(regle->autLu) ;

  if (! succes) { die("prepareRegle: ! success\n") ; }


  //  debug("prepare Regle: %d contexts\n", regle->nbContextes);

  /*
  for (int i = 0; i < regle->nbContextes; i++) {

    debug("%d.D:\n", i); autalmot_dump_plain(regle->contexte[i].D);
    debug("%d.G:\n", i); autalmot_dump_plain(regle->contexte[i].G);

      char buf[64];
      sprintf(buf, "context-%d.D.dot", i); autalmot_dump_dot_fname(regle->contexte[i].D, buf);
      sprintf(buf, "context-%d.G.dot", i); autalmot_dump_dot_fname(regle->contexte[i].G, buf);
  }
  */

  char buf[MAX_PATH];
  strcpy(buf, regle->nom);
  strcpy(buf + strlen(buf) - 5, "-conc.fst2");

  debug("outputing something interesting in %s ...\n", buf);

  tAutAlMot * locate = make_locate_auto(regle);

  autalmot_dump_fst2_fname(locate, buf);

  libereAutAlMot(locate);

  return succes;
}








/* Compte les contraintes dans aut.
 * Place dans "contrainte" les etats buts des transitions
 * etiquetees par les '=' medians des contraintes.
 * Verifie que toutes les transitions entrant dans ces etats
 * sont etiquetees par '='.
 */

static int compteContraintes(tAutAlMot * aut, etat * contrainte) {

  etat source = 0, e ;
  tTransitions * t ;
  int c, nbContraintes = 0 ;

  for (t = aut->etats[0]; t && ! source; t = t->suivant) {

    if (t->etiq->gramm[0] == '=') {
      if (t->but == 0) { die("illegal cycle in grammar\n"); }
      source = t->but;
    }
  }

  if (! source) { die("left delimitor '=' not found\n"); }

  for (e = 1; e < aut->nbEtats; e++) {
    for (t = aut->etats[e]; t; t = t->suivant) {

      if (t->but != source && t->etiq->gramm[0] == '=' && ! final(aut, t->but)) {

	for (c = 0; c < nbContraintes; c++)
	  if (contrainte[c] == t->but)
	    break ;

	if (c == nbContraintes) {
	  if (++nbContraintes >= maxContraintes) { die("too many constraints with same condition\n"); }
	  contrainte[c] = t->but ;
	}
      }
    }
  }

  if (nbContraintes == 0) { die("middle delimitor '=' not found\n"); }

  for (c = 0; c < nbContraintes; c++)
    for (e = 0; e < aut->nbEtats; e++)
      for (t = aut->etats[e]; t; t = t->suivant)
	if (t->but == contrainte[c] && t->etiq->gramm[0] != '=') { die("middle delimitor '=' not found\n") ; }

  return nbContraintes;
}



/* Copie autLu dans r depuis depart jusqu a delim. 
 * Ne copie pas les transitions etiquetees par delim.
 * Preconditions : autLu a une transition etiquetee delim et dont le but est depart.
 * Renvoie l'etat but des transitions etiquetees par delim.
 */

static int separeAut(tAutAlMot * autLu, tAutAlMot * r, etat depart, char delim) {

   int * corresp;                /* et non unsigned int, car NEXISTEPAS < 0 */
   int fin = NEXISTEPAS;         /* Etat but des transitions etiquetees par delim. */
   etat e;                       /* Etat de autLu. */
   tTransitions * t;


   debug("separeAut: dep=%d, delim=%c\n", depart, delim);

   initAutAlMotAlloue(r, autLu->nbEtats);
   r->nbEtats = 1;
   r->type[0] = AUT_INITIAL;
   corresp = (int *) xmalloc(autLu->nbEtats * sizeof(etat));

   for (e = 0; e < autLu->nbEtats ; e++) { corresp[e] = NEXISTEPAS; }

   corresp[depart] = 0;
   fin = suivre(autLu, r, depart, delim, corresp);
   free(corresp);

   if (fin == NEXISTEPAS) { /* suivre n'a pas rencontre delim. */
     die("in grammar: middle or end delimitor '%c' lacking\n", delim) ;
   }

   /* Verifie que toutes les transitions entrant dans fin sont etiquetees par delim. */

   if (! final(autLu, fin))
     for (e = 0; e < autLu->nbEtats; e++)
       for (t = autLu->etats[e]; t; t = t->suivant)
	 if (t->but == fin && t->etiq->gramm[0] != delim)
	   fprintf(fErr, "\nInternal error [separeAut], '%c'\n", delim);

   /*printf("separeAut '%c' %d, %d etats\n", delim, depart + 1, r -> nbEtats) ;*/
   /*printf(" %d ", r -> nbEtats) ;*/

   return fin;
}


/* Copie autLu dans r depuis depart jusqu a arrivee
 * Ne copie pas les transitions etiquetees par '='.
 * Preconditions : dans autLu, une transition etiquetee '=' entre dans
 * depart et une autre dans arrivee ; depart est dans la partie gauche
 * d'une contrainte et arrivee dans la partie droite d'une contrainte.
 */

static void separeCont(tAutAlMot * autLu, tAutAlMot * r, etat depart, etat arrivee) {

  int * corresp ;   /* et non unsigned int, car NEXISTEPAS < 0 */
  etat e ;   /* Etat de autLu. */
  BOOL trouve ;

  initAutAlMotAlloue(r, autLu -> nbEtats) ;
  r -> nbEtats = 1 ;
  r -> type[0] = AUT_INITIAL ;
  corresp = (int *) xcalloc(autLu->nbEtats, sizeof(etat)) ;

  for (e = 0 ; e < autLu -> nbEtats ; e ++)
    corresp[e] = NEXISTEPAS ;
  corresp[depart] = 0 ;

  trouve = suivreCont(autLu, r, depart, arrivee, corresp) ;

  if (! trouve) /* suivreCont n'a pas rencontre arrivee. */
    erreurInt("separeCont") ;

  free(corresp) ;
}



/* Copie autLu dans r depuis l'etat e de autLu jusqu'a delim. 
 * corresp donne pour chaque etat de autLu son equiv. dans r. 
 * Ne copie pas les transitions etiquetees par delim. 
 * Rend l'etat but des transitions etiquetees par delim 
 * s'il est rencontre. Rend NEXISTEPAS s'il n'est pas rencontre.
 */

static int suivre(tAutAlMot * autLu, tAutAlMot * r, etat e, unichar delim, int * corresp) {

   tTransitions * t;
   int f, fin = NEXISTEPAS;

   static int indent = 0;

   indent++;
   for (int i = 0; i < indent; i++) { fputc(' ', stderr); }

   debug("suivre(_,_,%d,%C)\n", e, delim);

   for (t = autLu->etats[e]; t; t = t->suivant) {

     for (int i = 0; i < indent; i++) { fputc(' ', stderr); }
     fprintf(stderr, "(%d, ", e); Affiche_Symbole(t->etiq); fprintf(stderr, ", %d)\n", t->but);

     if (t->etiq->gramm[0] == delim) { /* on a trouve delim */

       debug("delim!");

       f = t->but;
       if (fin != NEXISTEPAS && f != fin) { die("[suivre]: too much '%C' in rule, %d, %d\n", delim, fin, f) ; }
       fin = f;
       r->type[corresp[e]] |= AUT_TERMINAL;    /* corresp[e] devient terminal */

     } else { /* transition normale, on va la copier */

       if (corresp[t->but] == NEXISTEPAS) {   /* nouvel etat */

	 corresp[t->but] = (int) r->nbEtats++;
	 r->type[corresp[t->but]] = 0;
	 nouvTrans(r, corresp[e], t->etiq, corresp[t->but]) ;

	 f = suivre(autLu, r, t->but, delim, corresp);

	 if (f != NEXISTEPAS) {

	   if ((fin != NEXISTEPAS) && (f != fin)) { die("suivre: found too much '%C' in rule (in state %d & %d)\n", delim, fin, f); }

	   fin = f ;
	 }

	 // if (fin != NEXISTEPAS && f != fin) { die("Internal error [suivre], '%C', state=%d, fin=%d, f=%d\n", delim, e, fin, f); }

       } else { nouvTrans(r, corresp[e], t->etiq, corresp[t->but]) ; }

     }
   }

   /* Si toutes les transitions sortant de e entrent dans des etats deja
    * explores auparavant, on peut avoir une activation de suivre qui
    * ne rencontre pas le delimiteur de fin. Dans ce cas, fin == NEXISTEPAS.
    */

   for (int i = 0; i < indent; i++) { fputc(' ', stderr); }
   debug("out [state=%d, fin=%d]\n", e, fin);

   indent--;
   return fin;
}


/* Copie autLu dans r depuis l etat depart de autLu jusqu a arrivee. */
/* corresp donne pour chaque etat de autLu son equiv. dans r. */
/* Ne copie pas les transitions etiquetees par '='. */

static BOOL suivreCont(tAutAlMot * autLu, tAutAlMot * r, etat depart, etat arrivee, int * corresp) {

  BOOL trouve = FALSE ;
  tTransitions * t ;

  for (t = autLu -> etats[depart] ; t ; t = t -> suivant) {
    if (t -> etiq -> gramm[0] == '=') {
      if (t->but == arrivee && !(final(r,corresp[depart]))) {
	trouve = TRUE ;
	r->type[corresp[depart]] |= AUT_FINAL;    /* corresp[e] devient terminal */
      }
    } else {                   /* transition normale, on va la copier */

      if (corresp[t -> but] == NEXISTEPAS) {   /* nouvel etat */

	corresp[t -> but] = (int) r -> nbEtats ++ ;
	r->type[corresp[t -> but]] = 0;
	nouvTrans(r, corresp[depart], t -> etiq, corresp[t -> but]) ;

	if (suivreCont(autLu, r, t -> but, arrivee, corresp)) { trouve = TRUE ; }

      } else { nouvTrans(r, corresp[depart], t -> etiq, corresp[t -> but]) ; }
    }
  }

  /* Si toutes les transitions sortant de e entrent dans des etats deja
   * explores auparavant, on peut avoir une activation de suivreCont qui
   * ne rencontre pas arrivee. Dans ce cas, trouve == FALSE.
   */

  return trouve ;
}





static BOOL compileRegle(tRegle * regle, int numero) {

  //  char buf[128];

  tAutAlMot * a1, * a2;  /* automates temporaires */
  tAutAlMot * AetoileR1, * R2Aetoile;
  int c, p, ens ;


  printf("Compiling %s ... ", regle->nom) ;
  fflush(stdout);

  /*  
  for (c = 0; c < regle->nbContextes; c++) {
    sprintf(buf, "compile1-%dG.dot", c);
    autalmot_dump_dot_fname(regle->contexte[c].G, buf);
    sprintf(buf, "compile1-%dD.dot", c);
    autalmot_dump_dot_fname(regle->contexte[c].D, buf);
  }
  */

  for (c = 0; c < regle->nbContextes; c++) {

    regle->contexte[c].G = deterCompl(regle->contexte[c].G);
    regle->contexte[c].G = minimise(regle->contexte[c].G);

    regle->contexte[c].D = deterCompl(regle->contexte[c].D);
    regle->contexte[c].D = minimise(regle->contexte[c].D);
  }


  /*
  for (c = 0; c < regle->nbContextes; c++) {
  sprintf(buf, "context%d-G.dot", c);
  autalmot_dump_dot_fname(regle->contexte[c].G, buf);
  sprintf(buf, "context%d-D.dot", c);
  autalmot_dump_dot_fname(regle->contexte[c].D, buf);
  }
  */

  // autalmot_dump_dot_fname(regle->contexte[0].G, "R1.dot");
  // autalmot_dump_dot_fname(regle->contexte[0].D, "R2.dot");


  /* Construction de A*R1 : */

  AjoutBoucle(regle->contexte[0].G, etatInitial);
  AetoileR1 = deterCompl(regle->contexte[0].G);
  AetoileR1 = minimise(AetoileR1);

  //autalmot_dump_dot_fname(AetoileR1, "AEtoileR1.dot");


  /* Construction de R2A* : */

  AjoutBoucle(regle->contexte[0].D, etatsFinaux) ;
  R2Aetoile = deterCompl(regle->contexte[0].D) ;
  R2Aetoile = minimise(R2Aetoile) ;


  // autalmot_dump_dot_fname(R2Aetoile, "R2AEtoile.dot");

  p = (int) pow(2, regle->nbContextes - 1) ;
  a2 = 0 ;

  for (ens = 0 ; ens < p ; ens ++) {

    a1 = combinaison(regle, ens, AetoileR1, R2Aetoile);

    /*
    sprintf(buf, "compile-a1-%d.dot", ens);
    autalmot_dump_dot_fname(a1, buf);
    */

    if (a2) {

      a2 = unionAut(a1, a2);

      /*
      sprintf(buf, "compile-a2-union%d.dot", ens);
      autalmot_dump_dot_fname(a2, buf);
      */

      if (a2->nbEtats) {

	a2 = deterCompl(a2);

	/*
	sprintf(buf, "compile-a2-detercompl%d.dot", ens);
	autalmot_dump_dot_fname(a2, buf);
	*/
	
	a2 = minimise(a2);

	/*
	sprintf(buf, "compile-a2-minim%d.dot", ens);
	autalmot_dump_dot_fname(a2, buf);
	*/

      }
    } else a2 = a1;

    /*
    sprintf(buf, "compile-a2-%d.dot", ens);
    autalmot_dump_dot_fname(a2, buf);
    */
  }

  complementation(a2) ;
  EmondeAutomate(a2, TRUE) ;

  if (a2->nbEtats == 0) { warning("grammar %s forbids everything.\n", regle->nom); }

  regle->compilee = a2 ;

  printf(" done. (%d states)\n", regle->compilee->nbEtats) ;

  /*
    autalmot_dump_plain(regle->compilee);
  */

  /*
  sprintf(buf, "compil%d-end.dot", numero);
  autalmot_dump_dot_fname(regle->compilee, buf);
  */

  return TRUE ;
}




/* Parcours de l'ensemble des contraintes. Chaque contrainte est
 * representee par un bit dans la representation binaire de ens.
 * Postcondition : l'automate resultat est deterministe et complet.
 * Il peut ne pas avoir d'etats.
 * 
 * retourne:  (A*.R1 \ (U_{i in ens} (A*.Ci,1))) (R2.A* \ (U_{i not in ens} (Ci,2.A*)))
 */


static tAutAlMot * combinaison(tRegle * regle, int ens,	tAutAlMot * AetoileR1, tAutAlMot * R2Aetoile) {

  tAutAlMot * a1, * a2, * a3, * a4, * a5 ; /* automates temporaires */
  int c, dpc ; /* dpc = pow(2, c - 1) */

  //  char buf[1024];

  a1 = a2 = 0 ;

  for (c = dpc = 1; c < regle->nbContextes; c++, dpc *= 2) {

    if (dpc & ens) {    /* le c-ieme bit de ens vaut 1 */

      if (a1) {

	a5 = copieAutomate(regle->contexte[c].G) ;
	a1 = unionAut(a5, a1) ;
	if (a1->nbEtats == 0)
	  erreurInt("combinaison") ;
	a1 = deterCompl(a1) ;
	a1 = minimise(a1) ;

      } else a1 = copieAutomate(regle->contexte[c].G) ;

    } else {    /* le c-ieme bit de ens vaut 0 */

      if (a2) {

	a5 = copieAutomate(regle->contexte[c].D) ;
	a2 = unionAut(a5, a2) ;
	if (a2 -> nbEtats == 0)
	  erreurInt("combinaison") ;
	a2 = deterCompl(a2) ;
	a2 = minimise(a2) ;

      } else a2 = copieAutomate(regle->contexte[c].D) ;
    }
  }

  /* L(a1) = U_{i in I} R1_i
   * L(a2) = U_{i not in I} R2_i
   */

  /*
  sprintf(buf, "combi%d_a1.dot", ens);
  autalmot_dump_dot_fname(a1, buf);

  sprintf(buf, "combi%d_a2.dot", ens);
  autalmot_dump_dot_fname(a2, buf);
  */

  if (a1) {

    AjoutBoucle(a1, etatInitial) ;
    a1 = deterCompl(a1) ;
    a1 = minimise(a1) ;

    complementation(a1) ;

    verifVide(a1) ;	/* emonder a1 ?  */


    a3 = interAut(a1, AetoileR1) ;
    libereAutAlMot(a1) ;

    /*libereAutAlMot(a5) ; */
    /*a3 = deterCompl(a3) ; */

    if (a3->nbEtats)
      a3 = minimise(a3) ;  /* emonder a3 ? */

  } else a3 = copieAutomate(AetoileR1) ;

  /*
  sprintf(buf, "combi%d_a3.dot", ens);
  autalmot_dump_dot_fname(a3, buf);
  */

  if (a2) {

    AjoutBoucle(a2, etatsFinaux) ;

    a2 = deterCompl(a2) ;

    a2 = minimise(a2) ;

    complementation(a2) ;

    verifVide(a2) ;	/* emonder a2 ?  */
    a4 = interAut(a2, R2Aetoile) ;
    libereAutAlMot(a2) ;

    /*libereAutAlMot(a5) ;*/
    /*a4 = deterCompl(a4) ; */

    if (a4 -> nbEtats)
      a4 = minimise(a4) ;  /* emonder a4 ? */

  } else a4 = copieAutomate(R2Aetoile) ;


  /*
  sprintf(buf, "combi%d_a4.dot", ens);
  autalmot_dump_dot_fname(a4, buf);
  */

  a5 = concatenation(a3, a4) ;

  if (a5->nbEtats) {
    a5 = deterCompl(a5) ;
    a5 = minimise(a5) ;
    EmondeAutomate(a5, TRUE) ;  /* utile quand tous les etats sont inutiles */
  }

  /*
  sprintf(buf, "combi%d_a5.dot", ens);
  autalmot_dump_dot_fname(a5, buf);
  */

  return a5 ;
}






/* Regroupe les regles compilees en un nombre d automates inferieur
 * ou egal au nombre de regles.
 * Preconditions : premiere < nbRegles et nbRegles > 0.
 * nbGramm est le nombre de grammaires compilees deja existantes.
 */

static list_aut * regroupeRegles(tRegle * regle, int premiere, int nbRegles, FILE * compos, int nbGramm, char * nomFichNomRegles) {

  int r ;
  char nomFich[maxNomFich] ;
  FILE * svg ;


  debug("regroupe(pre=%d, nb=%d, nbGr=%d, fich=%s)\n", premiere, nbRegles, nbGramm, nomFichNomRegles);

  list_aut * gramm = (list_aut *) xcalloc(1, sizeof(list_aut)) ;
  gramm->les_aut = (tAutAlMot **) xcalloc(nbRegles - premiere, sizeof(tAutAlMot *)) ;
  gramm->nb_aut  = 0;

  tAutAlMot * temp = NULL;

  for (r = premiere; r < nbRegles; r++) {

    printf("Grammar %d : %d states\n", r, regle[r % maxRegles].compilee->nbEtats) ;

    if (temp) {

      gramm->les_aut[gramm->nb_aut] = interAut(temp, regle[r % maxRegles].compilee) ;

      if (! gramm->les_aut[gramm->nb_aut]) { die("regroupeRegles: interAut error"); }

      libereAutAlMot(temp) ;
      libereAutAlMot(regle[r % maxRegles].compilee) ;

      if (! gramm->les_aut[gramm->nb_aut]->nbEtats) { warning("grammars forbid everything.\n"); }

      /* habituellement minimiser l'automate ici ne change rien */

      EmondeAutomate(gramm->les_aut[gramm->nb_aut], TRUE);

      if (! gramm->les_aut[gramm->nb_aut]->nbEtats) { warning("grammars forbid everything.\n"); }

      printf("So far, %d states.\n", gramm->les_aut[gramm->nb_aut]->nbEtats);

    } else {

      fprintf(compos, "<%s%d.elg>\n", nomFichNomRegles, nbGramm + gramm->nb_aut);
      gramm->les_aut[gramm->nb_aut] = regle[r % maxRegles].compilee;
    }

    temp = gramm->les_aut[gramm->nb_aut];
    fprintf(compos, "\t%s\n", regle[r % maxRegles].nom);

    if (temp->nbEtats >= maxEtats) {

      gramm->les_aut[gramm->nb_aut] = temp;

      sprintf(nomFich, "%s%d.elg", nomFichNomRegles, nbGramm + gramm->nb_aut);

      svg = u_fopen(nomFich, U_WRITE);

      if (! svg) { die("cannot open file %s.\n", nomFich); }

      autalmot_dump_fst2(temp, svg);
      fclose(svg);

      gramm->nb_aut++;
      printf("next automaton.\n");
      temp = NULL;
    }
  }


  if (temp) {

    gramm->les_aut[gramm->nb_aut] = temp;

    sprintf(nomFich, "%s%d.elg", nomFichNomRegles, nbGramm + gramm->nb_aut);

    if ((svg = u_fopen(nomFich, U_WRITE)) == NULL) { die("cannot open file %s.\n", nomFich); }

    autalmot_dump_fst2(temp, svg);
    fclose(svg);
    gramm->nb_aut++;
  }

  return gramm ;
}
