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
/* elag-functions.cpp */
/* Date 	: juin 98 */
/* Auteur(s) 	: MAYER Laurent et al. */
/* Objet 	:  fonction principale de levee */
/*		  d'ambiguite */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "general.h"
#include "autalmot.h"
#include "list_aut.h"
#include "elag-functions.h"
#include "entrsort.h"
#include "deter.h"
#include "inter.h"
#include "emonde.h"
#include "decompte.h"
#include "minim.h"

#include "utils.h"
#include "fst2autalmot.h"


void leve_ambiguite(char * nom_fic_phrases, list_aut * gramm, char * nomSortie) {


  // debug("leve_ambiguite: phrases=%s, gramm=%d, out=%s\n", nom_fic_phrases, gramm, nomSortie);

  tAutAlMot * temp ;
  int i, j, nbPhrRej = 0;
  double apres, cumulApres = 0.0 ;
  time_t debut, fin ;

  if (! nom_fic_phrases) { erreurInt("leve_ambiguite"); }

  printf("\n* leve ambiguite: %d grammar%s.\n", gramm->nb_aut, gramm->nb_aut > 1 ?  "s" : "");

  tChargeurPhrases * chPhrases = constrChargeurPhrases(nom_fic_phrases) ;


  debut = time(0);

  printf("\nloading text ...%s\n", CR); fflush(stdout);

  list_aut * phrases = chargePhrases(chPhrases);

  printf("done.%s\n", CR);


  printf("\nprocessing ...\n%s", CR);

  for (i = 0; i < phrases->nb_aut; i++) {

    if (i % 100 == 0) { printf("phrase %d/%d ...\n%s", i + 1, phrases->nb_aut, CR); }

    if (phrases->les_aut[i]->nbEtats < 2) {

      fprintf(stderr, "Sentence %d is void.\n", i + 1) ;

    } else {

      for (j = 0; j < gramm->nb_aut; j++) {

	temp = interAutAtome(phrases->les_aut[i], gramm->les_aut[j]);
	EmondeAutomate(temp, FALSE);

	temp->name = u_strdup(phrases->les_aut[i]->name);
	libereAutAlMot(phrases->les_aut[i]);
	phrases->les_aut[i] = temp;

	if (phrases->les_aut[i]->nbEtats < 2) {
	  fprintf(stderr, "\nSentence %d rejected after grammar %d.\n\n", i + 1, j) ;
	  j = gramm->nb_aut;  /* on arrete pour cette phrase */
	  nbPhrRej++;
	}
      }
    }


    if (phrases->les_aut[i]->nbEtats > 1) {

      temp = deterCompl(phrases->les_aut[i]);
      phrases->les_aut[i] = minimise(temp);

      if (! EmondeAutomate(phrases->les_aut[i], FALSE))	{ erreurInt("EmondeAutomate"); }

      if (suppress_limphrase(phrases->les_aut[i]) == -1) {

	char buf[64];

	warning("an error occured while trying to remove sentence limits in sentence %d.\n", i + 1);

	sprintf(buf,"limphrase%d.dot", i);
	autalmot_dump_dot_fname(phrases->les_aut[i], buf);

	autalmot_tri_topologique(phrases->les_aut[i]);

	sprintf(buf,"limphrasetritopo%d.dot", i);
	autalmot_dump_dot_fname(phrases->les_aut[i], buf);
      }

      tEvaluation * evaluation = evaluation_new(phrases->les_aut[i]);

      //apres = evalPhrase(evaluation) ;
      apres = eval_phrase(evaluation);

      evaluation_delete(evaluation);

      /*
      printf("\n%d. Before: %.1f, after: %.1f. ", i + 1, phrases->tailleLog[i], apres) ;

      if (phrases->tailleLog[i] > 0.0 && apres > 0.0)
	printf("Residual: %.0f %%.\n", 100.0 * apres / phrases->tailleLog[i]) ;
      */

      cumulApres += apres;
    }
  }


  printf("\noutputing result in %s ...\n%s", nomSortie, CR);

  FILE * f;

  if (! (f = u_fopen(nomSortie, U_WRITE))) { die("cannot open %s for writing\n", nomSortie); }

  text_output_fst2(phrases, f);
  fclose(f) ;

  printf("done.\n%s", CR);


  printf("\nliberation ...\n%s", CR); fflush(stdout);
  libereListAut(phrases);
  printf("done.\n%s", CR);

  fin = time(0);

  printf("\nElapsed time: %.0f s.\n%s", difftime(fin, debut), CR);
  printf("Text. Before: %.1f, after: %.1f units per sentence. ", chPhrases->cumul / chPhrases->nbPhrases,
	 cumulApres / chPhrases->nbPhrases) ;

  if (chPhrases->cumul > 0.0) {

    if (cumulApres / (chPhrases->cumul) > 0.01) {

      printf("Residual: %.0f %%.\n%s", 100.0 * cumulApres / (chPhrases->cumul), CR) ;

    } else {

      printf("Residual: %.1f %%.\n%s", 100.0 * cumulApres / (chPhrases->cumul), CR) ;
    }
  }

  printf("%d sentences, %d rejected.\n\n", chPhrases->nbPhrases, nbPhrRej) ;


  destrChargeurPhrases(chPhrases) ;
}




#if 0

void leve_ambiguite_old(char * nom_fic_phrases, list_aut * gramm, char * nomSortie) {

  list_aut phrases ;
  tAutAlMot * temp ;
  tEvaluation * evaluation ;
  int i, j, nbPhrRej = 0, n, prochaine = 1 ;
  float apres, cumulApres = 0.0 ;
  FILE * f ; /* Fichier de resultats. */
  tChargeurPhrases * chPhrases ;
  time_t debut, fin ;


  if (! nom_fic_phrases)
    erreurInt("leve_ambiguite") ;

  printf("\n%d grammars\n", gramm->nb_aut) ;

  chPhrases = constrChargeurPhrases(nom_fic_phrases) ;

  if (! (f = fopen(nomSortie, "w"))) {
    die("opening %s for writing\n", nomSortie) ;
  }

  /* solution de debugage qui permet de visualiser sur l'ecran */

  debut = time(0) ;

  while (chargePhrases_old(chPhrases, &phrases)) {

    for (i = 0 ; i < phrases.nb_aut ; i ++) {

      if (i % 100 == 0)
	printf("phrase %d, %d etats...\n", i + prochaine, phrases.les_aut[i] -> nbEtats) ;

      /*sauvegAutAlMot(stdout, phrases.les_aut[i], i, FALSE) ;*/

      if (phrases.les_aut[i] -> nbEtats < 2) {

	printf("Sentence %d is void.\n", i + prochaine) ;
	fprintf(fErr, "\n%d void.", i + prochaine) ;

      } else {

	for (j = 0 ; j < gramm -> nb_aut ; j ++) {
	  /*
	    if (! phrases.les_aut[i] -> nbEtats)
	    printf("Erreur interne [leve_ambiguite] 1.\n") ;
	    if (! gramm -> les_aut[j] -> nbEtats)
	    printf("Erreur interne [leve_ambiguite] 2.\n") ;
	  */
	  temp = interAutAtome(phrases.les_aut[i], gramm -> les_aut[j]) ;
	  libereAutAlMot(phrases.les_aut[i]) ;
	  EmondeAutomate(phrases.les_aut[i] = temp, FALSE) ;

	  if (phrases.les_aut[i] -> nbEtats < 2) {
	    printf("Sentence %d rejected after grammar %d.\n", i + prochaine, j) ;
	    fprintf(fErr, "\n%d. Before: %.1f, rejected after grammar %d.",
		    i + prochaine, phrases.tailleLog[i], j) ;
	    j = gramm -> nb_aut ;  /* on arrete pour cette phrase */
	    nbPhrRej ++ ;
	  }
	}
      }


      if (phrases.les_aut[i]->nbEtats > 1) {

	temp = deterCompl(phrases.les_aut[i]) ;
	phrases.les_aut[i] = minimise(temp) ;

	if (! EmondeAutomate(phrases.les_aut[i], FALSE)) { erreurInt("EmondeAutomate"); }

	evaluation = (tEvaluation *) xcalloc(1, sizeof(tEvaluation)) ;  

	evaluation -> aut = initAutAlMot(phrases.les_aut[i] -> nbEtats) ;
	prepareEval(phrases.les_aut[i], evaluation) ;
	apres = evalPhrase(evaluation) ;

	fprintf(fErr, "\n%d. Before: %.1f, after: %.1f. ", i + prochaine, phrases.tailleLog[i], apres) ;

	if (phrases.tailleLog[i] > 0.0 && apres > 0.0)
	  fprintf(fErr, "Residual: %.0f %%.", 100.0 * apres/phrases.tailleLog[i]) ;

	cumulApres += apres ;
	libereAutAlMot(evaluation -> aut) ;
	free(evaluation -> factorisants) ;

	for (n = 0 ; n <= evaluation -> tailleDirect ; n ++)
	  free(evaluation -> direct[n]) ;

	free(evaluation -> direct) ;
	free(evaluation -> dicoInverse) ;
	free(evaluation) ; 
      }

      /*
	if (sprintf(sauvegarde, "temp%.03d.aut", i + 1))
	sauvegPhrase(sauvegarde, phrases.les_aut[i], i + prochaine,
	phrases.tailleLog[i]) ;
	else printf("Sauvegarde de secours impossible.\n") ;
      */
    }

    sauvegTexte(f, & phrases, prochaine) ;
    prochaine = chPhrases->nbPhrases + 1 ;
    libereListAut(& phrases) ;
  }

  fin = time(0);

  printf("\nElapsed time: %.0f s. ", difftime(fin, debut)) ;
  printf("\nText. Before: %.1f, after: %.1f units per sentence. ", chPhrases->cumul / chPhrases->nbPhrases,
	 cumulApres / chPhrases->nbPhrases);

  if (chPhrases -> cumul > 0.0) {

    if (cumulApres/(chPhrases -> cumul) > 0.01) {

      fprintf(fErr, "Residual: %.0f %%.\n", 100.0 * cumulApres/(chPhrases -> cumul)) ;

    } else {

      fprintf(fErr, "Residual: %.1f %%.\n", 100.0 * cumulApres/(chPhrases -> cumul)) ;
    }
  }

  printf("%d sentences, %d rejected.\n", chPhrases -> nbPhrases, nbPhrRej) ;
  destrChargeurPhrases(chPhrases) ;
  fclose(f) ;
}


#endif


/* Charge les grammaires deja compilees. */

list_aut * chargeGramm(char * nomFichGramm) {

  int g, nbGramm ;
  list_aut * gramm ;
  char nom[maxNomFich], * c ;
  static char nomComplet[maxNomFich] ;
  FILE * fGramm ;

  sprintf(nomComplet, "%s", nomFichGramm);

  if (strcmp(nomComplet + strlen(nomComplet) - 4, ".rul") != 0) {
    strcat(nomComplet, ".rul");
  } 

  fGramm = fopen(nomComplet, "r") ;
  if (! fGramm) {
    die("opening file %s\n", nomComplet) ;
  }

  gramm = (list_aut *) xcalloc(1, sizeof(list_aut)) ;

  nbGramm = 0 ;

  while (fgets(nom, maxNomFich, fGramm))
    if (nom[0] == '<')
      nbGramm ++ ;

  rewind(fGramm) ;

  gramm->les_aut = (tAutAlMot **) xcalloc(nbGramm, sizeof(tAutAlMot *)) ;
  gramm->nb_aut  = nbGramm ;

  for (g = 0; g < nbGramm; g ++) {

    do {
      if (! fgets(nom, maxNomFich, fGramm)) { die("\nInternal error: %s\n", nomFichGramm) ; }
    } while (nom[0] != '<');

    if(! (c = strchr(nom, '>'))) { die("in %s: delimitor '>' not found\n", nomFichGramm) ; }

    *c = 0 ;
    sprintf(nomComplet, "%s", nom + 1) ;
    printf("\nLecture de %s...\n", nomComplet) ;

    gramm->les_aut[g] = chargeAutGramm(nomComplet) ;

    /*printf("%s lu OK\n", tampon) ;*/
    /*sauvegAutAlMot(stdout, gramm -> les_aut[g], g, TRUE) ;*/
  }
  return gramm ;
}



/* Charge une seule grammaire deja compilee */

list_aut * chargeUneGramm(char * nomFichGramm) {

  list_aut * gramm ;
  static char nomComplet[maxNomFich] ;

  sprintf(nomComplet, "%s.elg", nomFichGramm);

  gramm = (list_aut *) xcalloc(1, sizeof(list_aut)) ;

  gramm->les_aut = (tAutAlMot **) xcalloc(1, sizeof(tAutAlMot *)) ;

  gramm->nb_aut = 1 ;
  printf("\nLecture de %s...\n", nomComplet) ;
  gramm->les_aut[0] = chargeAutGramm(nomComplet) ;
  return gramm ;
}
