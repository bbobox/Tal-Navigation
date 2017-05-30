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
#ifndef ContraintesH
#define ContraintesH
//---------------------------------------------------------------------------


#include "unicode.h"
#include "AutomateFst2.h"
#include "Alphabet.h"


struct contrainte {
   char controle; 
   unichar* raw;
   unichar* flechi;
   unichar* canonique;
   unichar** codes_gramm;
   int n_codes_gramm;
   unichar** codes_flex;
   int n_codes_flex;
   struct contrainte* contrainte_alternative;
};



struct contrainte** contrainte1=NULL;
struct contrainte** contrainte2=NULL;
struct contrainte** etiq=NULL;


struct contrainte* new_contrainte();
void free_contrainte(struct contrainte*);
struct contrainte* contruire_contrainte_depuis_etiquette(struct etiq_*);
struct contrainte** allouer_contraintes(int);
void free_contraintes(int,struct contrainte**);
void calculer_contraintes(Automate_fst2*,Automate_fst2*);
int contrainte1_verifiee(struct contrainte*,struct contrainte*,struct contrainte*,int*,Alphabet*);
int contrainte_verifiee(struct contrainte*,struct contrainte*,Alphabet* alph);
int codes_gramm_compatibles(struct contrainte*,struct contrainte*);
int codes_flex_compatibles(struct contrainte*,struct contrainte*);
int infos_gramm_compatibles(struct contrainte*,struct contrainte*);
int single_codes_flex_compatibles(unichar*,unichar*);
void afficher_contrainte(struct contrainte*);


#endif
