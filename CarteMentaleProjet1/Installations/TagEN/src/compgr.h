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
/* Nom 		: compgr.h */
/* Date 	: juin 98 */
/* Auteur(s) 	: MAYER Laurent et al */

#ifndef __compgr_h
#define __compgr_h

#define maxRegles 16
#define maxEtats 128
#define maxContraintes 8
#define NEXISTEPAS - 1

typedef struct {
  tAutAlMot * G, * D;
} tContexte;

typedef struct {
  tAutAlMot * autLu;      /* Automate lu dans un fichier */
  tAutAlMot * compilee;
  int nbContextes;        /* nombre de contextes, au moins 2 */
  tContexte * contexte;   /* liste des contextes */  
  char nom[maxNomFich];
} tRegle ;

list_aut * compileGramm (char * nomRepertoire, char * nomFichNomRegles, char * nomFichNomGramm);

#endif
