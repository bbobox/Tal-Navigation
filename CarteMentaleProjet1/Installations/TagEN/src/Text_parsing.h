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
//---------------------------------------------------------------------------
#ifndef Text_parsingH
#define Text_parsingH
//---------------------------------------------------------------------------

#include "Matches.h"
#include "AutomateFst2.h"
#include "TransductionStack.h"
#include "String_hash.h"
#include "Liste_num.h"


#define NBRE_ARR_MAX 50
#define TAILLE_PILE 200
#define BUFFER_SIZE 1000000
#define MODE_MORPHO 0
#define MODE_NON_MORPHO 1

int GESTION_DE_L_ESPACE=MODE_NON_MORPHO;
int texte[BUFFER_SIZE];
int LENGTH;
int N_INT_ALLREADY_READ;
int origine_courante;
long int nombre_unites_reconnues=0;
int* debut_graphe;
struct string_hash* TOKENS;
Etiquette* ETIQUETTE;
int SENTENCE_DELIMITER_INDICE=-1;


void launch_locate(FILE*,Automate_fst2*,int,struct string_hash*,FILE*,int,long int,FILE*);
void parcourir_initial_opt(int,int,int);
void parcourir_opt(int,int,int,struct liste_num**,int);
int dichotomie(int,int*,int);
int trouver_mot_compose_DIC(int,int);

#endif
