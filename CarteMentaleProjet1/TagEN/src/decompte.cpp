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
/* decompte.c */
/* Decompte des chemins d un automate acyclique par la methode des */
/* etats factorisants */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "general.h"
#include "autalmot.h"
#include "decompte.h"

#include "utils.h"

#define TAILLE_MAX_SYMBOL  80

static void representation_relative(tEvaluation *) ;
//static void aff_mat(tEvaluation * e) ;
static void etat_factorisant(tEvaluation *) ;
//static int denombrer_transitions(int,int, tAutAlMot *) ;

static unsigned int denombrer_sequences(etat,etat, tAutAlMot *) ;
static unsigned int denombrer(tEvaluation * e) ;
static double denombrer_log(tEvaluation * e);

static void enumerer_sequences(FILE *, etat,etat,unichar *, tAutAlMot *) ;
static void enumerer(FILE *, tEvaluation *) ;




tEvaluation * evaluation_new(tAutAlMot * A) {

  tEvaluation * eval = (tEvaluation *) xmalloc(sizeof(tEvaluation));

  //  eval->aut = initAutAlMot(A->nbEtats);
  prepareEval(A, eval);

  return eval;
}


void evaluation_delete(tEvaluation * evaluation) {

  libereAutAlMot(evaluation->aut);
  free(evaluation->factorisants);

  for (int n = 0; n <= evaluation->tailleDirect; n ++) { free(evaluation->direct[n]); }

  free(evaluation->direct);
  free(evaluation->dicoInverse);
  free(evaluation);
}




/* Preconditions : l automate A a au moins un etat et il est acyclique ;
 * l'autre argument est vide.
 */

void prepareEval(tAutAlMot * A, tEvaluation * evaluation) {

  evaluation->numSymbole  = 0;
  evaluation->numEtat     = 0;
  evaluation->dicoInverse = (int *) xmalloc(A->nbEtats * sizeof(int));

  evaluation->aut = tri_topologique(A, evaluation->dicoInverse);

  representation_relative(evaluation);

  etat_factorisant(evaluation);
}





/* Calcule le logarithme du nombre de sequences reconnues.
 * Preconditions : l automate a ete traite par prepareEval.
 */


double evalPhrase(tEvaluation * evaluation) {

  unsigned int cardinal = denombrer(evaluation) ;

  //  debug("cardinal=%d\n", cardinal);

  if (cardinal > 0) {

    evaluation->tailleLog =  log((double) cardinal) ;

  }  else {

    error("evalPhrase; cardinal = %d?\n", cardinal);
    evaluation->tailleLog = 0;
  }

  return evaluation->tailleLog ;
}


double eval_phrase(tEvaluation * evaluation) {

  evaluation->tailleLog = denombrer_log(evaluation);

  if (evaluation->tailleLog < 0.0) {
    error("eval_phrase; taillelog=%d\n", (int) evaluation->tailleLog);
    evaluation->tailleLog = 0;
  }

  return evaluation->tailleLog ;
}



/* Calcule le logarithme du nombre de sequences reconnues.
 * Affiche sous forme compacte les sequences reconnues.
 * Precondition : l'argument e a ete traite par prepareEval.
 * Renvoie la taille logarithmique de l'automate.
 */

float visu(FILE * f, tEvaluation * e) {

  float cardinal ;

  fprintf(f,"## Line display:\n") ;

  if (e -> tailleDirect)
    enumerer(f, e) ;
  cardinal = denombrer(e) ;

  if (cardinal > 0.0)
    e -> tailleLog = (float) log(cardinal) ;
  else erreurInt("visu") ;

  fprintf(f, "## Lexical ambiguity : %.1f\n", e -> tailleLog) ;

  return e -> tailleLog ;
}




/* Preconditions : autTrie est vide.
 * Ne modifie pas le premier automate d'entree A.
 * Place dans autTrie un automate equivalent dans lequel les etats sont
 * numerotes suivant un tri topologique. Postconditions :
 * il n'y a pas de partage de memoire entre A et autTrie ;
 * la taille est egale au nombre d'etats dans autTrie.
 * remplissage du tableau du degre entrant
 */

tAutAlMot * tri_topologique(tAutAlMot * A, int * dicoInverse) {

  int * dico ;
  etat i, nouveau, ancien ;
  tTransitions * T;

  int * entrants = (int *) xmalloc(A->nbEtats * sizeof(int)) ;

  for (i = 0; i < A->nbEtats; i ++) { entrants[i] = 0; }

  for (i = 0; i < A->nbEtats; i ++) {
    for (T = A->etats[i]; T; T = T->suivant) { entrants[T->but]++; }
  }

  /* tri topologique */

  dico = (int *) xcalloc(A->nbEtats, sizeof(int));

  for (nouveau = 0 ; nouveau < A->nbEtats ; nouveau++) {

    ancien = 0;

    while (entrants[ancien] != 0) { ancien++; }

    dico[ancien]         = nouveau;
    dicoInverse[nouveau] = ancien;
    entrants[ancien]     = -1;

    for (T = A->etats[ancien]; T != NULL; T = T->suivant) { entrants[T->but]--; }
  }


  /* creation du nouvel automate */

  tAutAlMot * autTrie = initAutAlMot(A->nbEtats);


  if (A->nbEtatsInitiaux != 1 || A->initial[0]) { erreurInt("tri_topologique"); }

  for (ancien = 0; ancien < autTrie->nbEtats; ancien++) {

    autTrie->type[dico[ancien]] = A->type[ancien];

    autTrie->etats[dico[ancien]] = NULL;

    for (T = A->etats[ancien]; T; T = T->suivant) {
      nouvTrans(autTrie, dico[ancien], T->etiq, dico[T->but]);
    }
  }
  
  free(dico);
  free(entrants);

  return autTrie;
}




void autalmot_tri_topologique(tAutAlMot * A) {

  int * dicoInverse = (int *) xmalloc(A->nbEtats * sizeof(int));

  tAutAlMot * tri = tri_topologique(A, dicoInverse);

  tTransitions ** tmptrans = A->etats;
  A->etats   = tri->etats;
  tri->etats = tmptrans;

  char * tmpsorte = A->type;
  A->type         = tri->type;
  tri->type       = tmpsorte;

  free(A->entrantesEtats);
  A->entrantesEtats = NULL;

  etat tmptaille = A->taille;
  A->taille      = tri->taille;
  tri->taille    = tmptaille;

  etat * tmpinit = A->initial;
  A->initial     = tri->initial;
  tri->initial   = tmpinit;


  free(dicoInverse);
  libereAutAlMot(tri);
}



static void representation_relative(tEvaluation * e) {

  etat i ;
  int j ;
  tTransitions * T ;
  e->tailleDirect = 0 ;


  /* Recherche du plus grand saut */

  for(i = 0; i < e->aut->nbEtats; i ++) {

    for (T = e->aut->etats[i]; T; T = T->suivant) {

      j = T->but - i;
      if (e->tailleDirect < j) { e->tailleDirect = j; }
    }
  }


  /* construction de la matrice */

  e->direct = (int **) xmalloc(e->aut->nbEtats * sizeof(int *)) ;


  for (i = 0; i < e->aut->nbEtats; i++) {

    e->direct[i] = (int *) xmalloc((e->tailleDirect + 1) * sizeof(int)) ;

    /* remplissage de la matrice direct */

    for (j = 0; j < e->tailleDirect + 1; j++) { e->direct[i][j] = 0; }

    for (T = e->aut->etats[i]; T; T = T->suivant) { e->direct[i][T->but - i] = 1; }
  }
}



#if 0
static void aff_mat(tEvaluation * e) {
  etat i ; int j;
  printf("\nAffichage de la matrice direct :\n") ;
  for (i=0;i<e->aut ->nbEtats;i++)	{
    for (j = 1; j <= e->tailleDirect;j++)
      printf("  %d  ",e -> direct[i][j]);
    printf("\n") ;	}
}

#endif

static void etat_factorisant(tEvaluation * e) {

  etat i, k;
  int j ;


  e->factorisants = (int *) xmalloc(e->aut->nbEtats * sizeof(int)) ;

  for (i = 0; i < e->aut->nbEtats; i++) { e->factorisants[i] = TRUE; }


  for (i = 0; i < e->aut->nbEtats; i++) {

    for (j = 2; j <= e->tailleDirect; j++) {

      if (e->direct[i][j] == 1) {
	for (k = i + 1; k < j + i; k++) { e->factorisants[k] = FALSE ; }
      }
    }
  }
}



#if 0
static int denombrer_transitions(int i,int j, tAutAlMot * B) {
  tTransitions * T;
  int denombrement=0;
  int k;
  for (k=i;k<j;k++) {
    T=B->etats[k];
    while (T!=NULL) {
      T=T->suivant;
      denombrement++;
    }
  }
  return(denombrement);
}
#endif


static unsigned int denombrer_sequences(etat i,etat j, tAutAlMot * B){

  tTransitions * T;
  unsigned int denombrement = 0;

  for (T = B->etats[i]; T != NULL; T = T->suivant) {

    if (T->but == j) {

      denombrement++;

    } else {

      denombrement += denombrer_sequences(T->but, j, B);
    }
  }

  return denombrement ;
}




static unsigned int denombrer(tEvaluation * e) {

  etat deEtat1, aEtat2;
  unsigned int denombrement = 1;

  aEtat2 = 0;

  while (aEtat2 < e->aut->nbEtats - 1)	{

    unsigned int fl;
    deEtat1 = aEtat2;
    aEtat2++;

    while (e->factorisants[aEtat2] == FALSE)
      aEtat2++;

    fl = denombrer_sequences(deEtat1, aEtat2, e->aut);

    denombrement *= fl ;

    //    debug("fl=%d, den=%d\n", fl, denombrement);
  }

  return denombrement ;
}


static double denombrer_log(tEvaluation * e) {

  etat q1, q2;
  double denombrement = 0.0;

  q2 = 0;

  while (q2 < e->aut->nbEtats - 1) {

    unsigned int dn;
    q1 = q2;
    q2++;

    while (e->factorisants[q2] == FALSE) { q2++; }

    dn = denombrer_sequences(q1, q2, e->aut);

    denombrement += log((double) dn);
  }

  return denombrement ;
}



static void enumerer_sequences(FILE * f, etat i, etat j, unichar * chaine, tAutAlMot * B) {

  tTransitions * T ;
  unichar * finChaine ;

  if (i == j) {
    u_fprintf(f,"## %S\n", chaine);
    fflush(f) ;
    return ;
  }

  finChaine = chaine + u_strlen(chaine) ;

  for (T = B -> etats[i] ; T ; T = T -> suivant) {
    u_sprintf(finChaine, " {%S,%S.%S}", T->etiq->flechie, T->etiq->canonique, T->etiq->gramm) ;
    enumerer_sequences(f, T -> but, j, chaine, B) ;
  }

  finChaine = 0 ;
}



/* Precondition : tailleDirect > 0. */

static void enumerer(FILE * f, tEvaluation * e) {

  etat deEtat1, aEtat2 ;

  unichar * chaine ;
  chaine = (unichar *) calloc(e->tailleDirect, (TAILLE_MAX_SYMBOL + 2) * sizeof(unichar)) ;

  if (! chaine)
    erreurMem("enumerer") ;

  aEtat2 = 0 ;

  while (aEtat2 < e->aut->nbEtats - 1) {

    chaine[0] = 0 ;
    deEtat1 = aEtat2 ;

    do { aEtat2 ++ ;
    } while (e->factorisants[aEtat2] == 0) ;

    fprintf(f,"## Etat %d\n", e->dicoInverse[deEtat1] + 1) ;
    enumerer_sequences(f, deEtat1, aEtat2, chaine,  e->aut) ;
  }

  fprintf(f,"## Etat %d\n", e -> dicoInverse[aEtat2] + 1) ;
  free(chaine) ;
}
