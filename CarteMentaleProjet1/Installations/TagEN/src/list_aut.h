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
/* #include "aut.h" */

#ifndef __list_aut_h
#define __list_aut_h

#include "autalmot.h"

/* Structure de liste d'automates */

typedef struct list_aut {
  int nb_aut;            /* Nombre d'automates */
  tAutAlMot ** les_aut;  /* Tableau des adresses des automates */
  double * tailleLog;     /* taille logarithmique de chaque aut. */
} list_aut;


void fusion(list_aut * liste1, list_aut * liste2) ;
void libereListAut(list_aut * liste) ;
#endif
