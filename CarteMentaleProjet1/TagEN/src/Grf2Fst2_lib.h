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
#ifndef Grf2Fst2_libH
#define Grf2Fst2_libH
//---------------------------------------------------------------------------



#define NOMBRE_GRAPHES_COMP 5000        // nombre max de graphes
#define N_CAR_MAX_COMP 300               // nombre max de caracteres d'un mot normal
#define TAILLE_MOT_GRAND_COMP 10000       // nombre max de caracteres d'un long mot
#define NOMBRE_ETIQUETTES_COMP 50000    // nombre max d'étiquettes
#define NOMBRE_ETATS_COMP 10000          // nombre max d'états
#define NOMBRE_TRANSITIONS_COMP 10000    // nombre max de transitions
#define L_ASCII 256 //taille de l'alphabet
#define TAILLE_SEQUENCE_COMP 200        //nombre de mots max dans une sequence de mots entre deux + d'une boite
#define NBRE_ET  10000
#define NBRE_ETIQ_TRANSITION_COMP NOMBRE_ETIQUETTES_COMP+NOMBRE_GRAPHES_COMP

//
// structure d'un ensemble d'etats
//
struct char_etats_det {
  unsigned int num_char;     //  numero du char dont chaque bit est un etat
  unsigned int valeur;       //   valeur du char
  struct char_etats_det *suivant;
};


typedef struct char_etats_det* ensemble_det;


//
// structure d'un etat
//
struct etat_fst_det
{
  unsigned char controle;        // etat final ou pas
  struct char_etats_det *ens;            //ensemble des etats de graphe regroupes dans cet etat
  struct transition_fst_det *trans;     // transition_fsts partant de cet etat
};

typedef struct etat_fst_det* Etat_fst_det;


//
// structure d'une transition
//
struct transition_fst_det {
  int etiquette;                // etiquette de la transition : un entier
  int arr;                      // etat d'arrivee de la transition
  struct transition_fst_det *suivant;   // transition suivante
};

typedef struct transition_fst_det *liste_transition_det;


//
// structure d'un noeud num_char
//


struct noeud_num_char_det {
  int num_char;
  struct liste_branches_valeur_det *liste;
};


struct liste_branches_valeur_det {
  struct noeud_valeur_det *n;
  struct liste_branches_valeur_det *suivant;
};

struct noeud_valeur_det {
  int valeur;
  int indice;
  struct liste_branches_num_char_det *liste;
};


struct liste_branches_num_char_det {
  struct noeud_num_char_det *n;
  struct liste_branches_num_char_det *suivant;
};





//
// structure d'un etat
//


struct etat_comp
{
  unsigned char controle;          // octet de controle
  struct transition_comp *trans;         //liste des transitions
  struct transition_comp *transinv;         // liste des transitions inverses
};

typedef struct etat_comp* Etat_comp;



///
//Structure d'une transition
//

struct transition_comp
{
  int arr;                // etat d'arrivee
  int etiq;   // etiquettes a deplier
  struct transition_comp *suivant; //pointeur sur transition suivante
};

typedef struct transition_comp *Transition_comp;




//
// structures servant a manipuler l'arbre des graphes
//

struct noeud_g_comp
{
  int fin;
  unsigned char lettre;
  struct liste_feuilles_g_comp *l;
};

struct liste_feuilles_g_comp
{
  struct noeud_g_comp *node;
  struct liste_feuilles_g_comp *suivant;
};


//
// structures servant a manipuler l'arbre des etiquettes
//

struct noeud_comp
{
  int fin;
  unichar lettre;
  struct liste_feuilles_comp *l;
};

struct liste_feuilles_comp
{
  struct noeud_comp *node;
  struct liste_feuilles_comp *suivant;
};


struct donnees_comp
{
  unichar nom_graphe[NOMBRE_GRAPHES_COMP][N_CAR_MAX_COMP];  //0=vide 1=non_vide -1=non traite
  int statut_graphe[NOMBRE_GRAPHES_COMP];     //0=vide 1=non_vide -1=non traite
  unichar Etiquette_comp[NOMBRE_ETIQUETTES_COMP][N_CAR_MAX_COMP];
  unichar chemin_alphabet[TAILLE_MOT_GRAND_COMP];
  unichar chemin_graphe[TAILLE_MOT_GRAND_COMP];
};




struct donnees_comp *donnees;
int nombre_graphes_comp;
int nombre_etiquettes_comp;
struct noeud_g_comp *rac_graphe_comp; //racine de l'arbre des graphes
struct noeud_comp *rac_comp; //racine de l'arbre des étiquettes
FILE *fs_comp; //fichier de sortie
int EPSILON_comp;   // etiquette pour <E>
int n_malloc_comp=0;
int n_free_comp=0;
int compteur_char=0;
int compteur_free_char=0;
ensemble_det stock[NBRE_ETIQ_TRANSITION_COMP];
unsigned char final[NBRE_ETIQ_TRANSITION_COMP];
int hachage[NBRE_ETIQ_TRANSITION_COMP];
int hachageinv[NBRE_ETIQ_TRANSITION_COMP];
Etat_fst_det resultat[NBRE_ET];



void vider_noeud_num_char_det(struct noeud_num_char_det *);

#endif
