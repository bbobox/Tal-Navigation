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
/* variable.h */

/* Comparaison, intersection d etiquettes de variables */
/* Appartenance a une etiquette de variable */

#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#define nbCatGramm 45


typedef struct {
  char cat_gramm;
  char temps;
  char personne;  /* Vaut '1', '2' ou '3' */
  char genre;
  char nombre;
} tCode;


/* liste chainee de formes */

struct tListe {
  unichar * forme ;
  struct tListe * suiv ;
};

typedef struct tListe tListe ;


int appartient(tSymbole * etiqa, tSymbole * etiqb);
int appartientBis(tSymbole * etiqa, tSymbole * etiqb);
tSymbole * inter(tSymbole * etiq_a , tSymbole * etiq_b);
alphabet * sauf(tSymbole * etiq_a , tSymbole * etiq_b);
void ordonne(tSymbole * s);

#endif
