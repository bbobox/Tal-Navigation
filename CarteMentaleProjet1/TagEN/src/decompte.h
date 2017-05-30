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
/* decompte.h */
/* Decompte des chemins valides d un automate acyclique par la methode des */
/* etats factorisants */

#ifndef _DECOMPTE_H_
#define _DECOMPTE_H_

typedef struct evaluation {
  tAutAlMot * aut ;   /* Automate acyclique */
  int    numSymbole ;
  int    numEtat ;
  int    tailleDirect ;
  double  tailleLog ;
  int *  factorisants ;
  int ** direct ;
  int *  dicoInverse ;
} tEvaluation ;



void prepareEval(tAutAlMot * A, tEvaluation * evaluation) ;
double evalPhrase(tEvaluation *) ;
tAutAlMot * tri_topologique(tAutAlMot * a, int * dicoInverse) ;
void autalmot_tri_topologique(tAutAlMot * A);
float visu(FILE * f, tEvaluation * e) ;


tEvaluation * evaluation_new(tAutAlMot * A);
void evaluation_delete(tEvaluation * evaluation);
double eval_phrase(tEvaluation * evaluation);

#endif
