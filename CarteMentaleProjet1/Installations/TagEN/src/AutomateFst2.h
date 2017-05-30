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
//---------------------------------------------------------------------------
#ifndef AutomateFst2H
#define AutomateFst2H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unicode.h"

//----------CONSTANTES-------------------------------------------



#define NBRE_ETIQUETTES 100000          // nbre max d'etiquettes (%etiq1%etiq2%....)
#define NBRE_ETATS      500000          // nbre max d'etats dans le FST2




//----------TYPES------------------------------------------
struct etiq_{
  int numero;
  unsigned char controle;
  // 1: transduction
  // 2: negation
  // 4: respect minuscules/majuscules
  // 64: debut de variable $a(
  // 128: fin de variable $a)
  unichar *contenu;
  unichar *transduction;
  unichar *flechi;
  unichar *canonique;
  unichar *infos_gramm;
  struct liste_nombres* numeros;
  int nombre_mots;
  int pattern_compose;

  // valeurs pour controle apres numerote_tags(...) :
  // 16: #, ESPACE, <E>, <MOT>, <DIC>, <MAJ>, <MIN>, <PRE>, $a(, $a), <NB>
  // 8: <V>
  // 8|64 : <manger.V>, <mange,manger.V>
  // 32|64: <manger>
  // 32: lundi
};

typedef struct etiq_* Etiquette;


struct etat_fst {
  unsigned char controle;        // etat final ou pas
  // 1: est terminal
  // 2: est initial
  // 4: bit de marquage
  struct transition_fst *trans;     // transitions partant de cet etat
};

typedef struct etat_fst* Etat_fst;


struct transition_fst {
  int etiquette;                // etiquette de la transition : un entier
  int arr;                      // etat d'arrivee de la transition
  struct transition_fst *suivant;   // transition suivante
};

typedef struct transition_fst *liste_transition;


struct variable_list {
  unichar* name;
  int start;
  int end;
  struct variable_list* suivant;
};


struct automate_fst2 {
    Etat_fst* etat;
    Etiquette* etiquette;
    int nombre_graphes;
    int nombre_etats;
    int nombre_etiquettes;
    int* debut_graphe_fst2;
    unichar** nom_graphe;
    int* nombre_etats_par_grf;
    struct variable_list* variables;
};

typedef struct automate_fst2 Automate_fst2;


//----------PROTOTYPES-------------------------------------------
void charger_graphe_fst2(FILE*,Etat_fst[],Etiquette[],int*,int*,int*,int**,
                         char***,int,int**);

liste_transition nouvelle_transition_mat();
Automate_fst2* load_fst2(char*,int);
void free_fst2(Automate_fst2*);
struct variable_list* get_variable(unichar*,struct variable_list*);
Automate_fst2* load_one_sentence_of_fst2(char*,int,FILE*);

#endif
