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
#include "Optimized_fst2.h"
//---------------------------------------------------------------------------


struct etat_opt* nouveau_etat_opt() {
struct etat_opt* e;
e=(struct etat_opt*)malloc(sizeof(struct etat_opt));
e->liste_sous_graphes=NULL;
e->liste_inter_graphes=NULL;
e->liste_metas=NULL;
e->liste_patterns=NULL;
e->liste_patterns_composes=NULL;
e->liste_tokens=NULL;
e->tableau_de_tokens=NULL;
e->tableau_liste_arr=NULL;
e->nombre_de_tokens=0;
return e;
}



struct appel_a_sous_graphe* nouvelle_liste_sous_graphes() {
struct appel_a_sous_graphe* a;
a=(struct appel_a_sous_graphe*)malloc(sizeof(struct appel_a_sous_graphe));
a->numero_de_graphe=-2000;
a->liste_arr=NULL;
a->suivant=NULL;
return a;
}



struct liste_arrivees* nouvelle_liste_arrivees() {
struct liste_arrivees* l;
l=(struct liste_arrivees*)malloc(sizeof(struct liste_arrivees));
l->etiquette_origine=-2222;
l->arr=-3333;
l->suivant=NULL;
return l;
}



void ajouter_si_pas_deja_present(struct liste_arrivees **l,int n,int etiq_origine) {
struct liste_arrivees* ptr;
if (*l==NULL) {
  *l=nouvelle_liste_arrivees();
  (*l)->arr=n;
  (*l)->etiquette_origine=etiq_origine;
  return;
}
ptr=*l;
while (ptr->suivant!=NULL && !(ptr->arr==n && ptr->etiquette_origine==etiq_origine))
  ptr=ptr->suivant;
if (!(ptr->arr==n && ptr->etiquette_origine==etiq_origine)) {
  ptr->suivant=nouvelle_liste_arrivees();
  ptr->suivant->arr=n;
  ptr->suivant->etiquette_origine=etiq_origine;
  return;
}
}



void ajouter_sous_graphe(struct transition_fst* trans,struct appel_a_sous_graphe** a) {
struct appel_a_sous_graphe* ptr;
int E;
E=-(trans->etiquette);
if (*a==NULL) {
  // 1er cas: liste vide
  *a=nouvelle_liste_sous_graphes();
  (*a)->numero_de_graphe=E;
  ajouter_si_pas_deja_present(&((*a)->liste_arr),trans->arr,-2000);
  return;
}
if ((*a)->numero_de_graphe==E) {
  // 2eme cas: mise a jour du premier element
  ajouter_si_pas_deja_present(&((*a)->liste_arr),trans->arr,-2000);
  return;
}
ptr=*a;
while (ptr->suivant!=NULL && ptr->suivant->numero_de_graphe!=E)
  ptr=ptr->suivant;
if (ptr->suivant==NULL) {
  // on insere en fin de liste
  ptr->suivant=nouvelle_liste_sous_graphes();
  ptr->suivant->numero_de_graphe=E;
  ajouter_si_pas_deja_present(&(ptr->suivant->liste_arr),trans->arr,-2000);
  return;
}
// on met a jour l'element
ajouter_si_pas_deja_present(&(ptr->suivant->liste_arr),trans->arr,-2000);
}



struct appel_a_pattern* nouvelle_liste_patterns() {
struct appel_a_pattern* a;
a=(struct appel_a_pattern*)malloc(sizeof(struct appel_a_pattern));
a->numero_de_pattern=-2000;
a->negation=0;
a->liste_arr=NULL;
a->suivant=NULL;
return a;
}



void ajouter_pattern(int pattern,int etiq_origine,struct appel_a_pattern** a,int arr,int controle) {
struct appel_a_pattern* ptr;
int negation;
negation=controle&2;
if (*a==NULL) {
  // 1er cas: liste vide
  *a=nouvelle_liste_patterns();
  (*a)->numero_de_pattern=pattern;
  (*a)->negation=negation;
  ajouter_si_pas_deja_present(&((*a)->liste_arr),arr,etiq_origine);
  return;
}
if ((*a)->numero_de_pattern==pattern && (*a)->negation==negation) {
  // 2eme cas: mise a jour du premier element
  ajouter_si_pas_deja_present(&((*a)->liste_arr),arr,etiq_origine);
  return;
}
ptr=*a;
while (ptr->suivant!=NULL && !(ptr->suivant->numero_de_pattern==pattern && (*a)->negation==negation))
  ptr=ptr->suivant;
if (ptr->suivant==NULL) {
  // on insere en fin de liste
  ptr->suivant=nouvelle_liste_patterns();
  ptr->suivant->numero_de_pattern=pattern;
  ptr->suivant->negation=negation;
  ajouter_si_pas_deja_present(&(ptr->suivant->liste_arr),arr,etiq_origine);
  return;
}
// on met a jour l'element
ajouter_si_pas_deja_present(&(ptr->suivant->liste_arr),arr,etiq_origine);
}



struct liste_de_tokens* nouvelle_liste_tokens() {
struct liste_de_tokens* l;
l=(struct liste_de_tokens*)malloc(sizeof(struct liste_de_tokens));
l->tok=NULL;
l->suivant=NULL;
return l;
}



struct token* nouveau_token() {
struct token* t;
t=(struct token*)malloc(sizeof(struct token));
t->numero_de_token=-2000;
t->liste_arr=NULL;
return t;
}



void ajouter_token_a_liste_tokens(int token,int n_etiq,struct liste_de_tokens** l,int arr,int* N) {
struct liste_de_tokens *ptr;
struct liste_de_tokens *tmp;
if (*l==NULL) {
  // 1er cas liste vide
  *l=nouvelle_liste_tokens();
  (*l)->tok=nouveau_token();
  (*l)->tok->numero_de_token=token;
  (*N)++;
  ajouter_si_pas_deja_present(&((*l)->tok->liste_arr),arr,n_etiq);
  return;
}
if (token<((*l)->tok->numero_de_token)) {
  // 2eme cas: insertion en tete de liste
  ptr=nouvelle_liste_tokens();
  ptr->tok=nouveau_token();
  ptr->tok->numero_de_token=token;
  (*N)++;
  ajouter_si_pas_deja_present(&(ptr->tok->liste_arr),arr,n_etiq);
  ptr->suivant=*l;
  *l=ptr;
  return;
}
if (token==((*l)->tok->numero_de_token)) {
  // 2eme cas prime : mise a jour de la tete de liste
  ajouter_si_pas_deja_present(&((*l)->tok->liste_arr),arr,n_etiq);
  return;
}
// 3eme cas: cas general
ptr=*l;
while (ptr->suivant!=NULL && token>ptr->suivant->tok->numero_de_token)
  ptr=ptr->suivant;
if (ptr->suivant==NULL) {
  // si on insere en fin de liste
  ptr->suivant=nouvelle_liste_tokens();
  ptr->suivant->tok=nouveau_token();
  ptr->suivant->tok->numero_de_token=token;
  (*N)++;
  ajouter_si_pas_deja_present(&(ptr->suivant->tok->liste_arr),arr,n_etiq);
  return;
}
if (token==ptr->suivant->tok->numero_de_token) {
  // si on met a jour le token
  ajouter_si_pas_deja_present(&(ptr->suivant->tok->liste_arr),arr,n_etiq);
  return;
}
// dernier cas: on insere apres ptr
tmp=nouvelle_liste_tokens();
tmp->tok=nouveau_token();
tmp->tok->numero_de_token=token;
(*N)++;
ajouter_si_pas_deja_present(&(tmp->tok->liste_arr),arr,n_etiq);
tmp->suivant=ptr->suivant;
ptr->suivant=tmp;
}



void ajouter_liste_de_tokens(Etiquette e,int n_etiq,Etat_opt *e2,int arr,int* N) {
struct liste_nombres* l;
l=e->numeros;
while (l!=NULL) {
  ajouter_token_a_liste_tokens(l->n,n_etiq,&((*e2)->liste_tokens),arr,N);
  l=l->suivant;
}
}



struct appel_a_meta* nouvelle_liste_metas() {
struct appel_a_meta* a;
a=(struct appel_a_meta*)malloc(sizeof(struct appel_a_meta));
a->numero_de_meta=-2000;
a->negation=0;
a->numero_de_variable=-1;
a->liste_arr=NULL;
a->suivant=NULL;
return a;
}



void ajouter_meta(int meta,int etiq_origine,struct appel_a_meta** a,int arr,int controle,
                  int numero_de_variable) {
struct appel_a_meta* ptr;
int negation;
negation=controle&2;
if (*a==NULL) {
  // 1er cas: liste vide
  *a=nouvelle_liste_metas();
  (*a)->numero_de_meta=meta;
  (*a)->negation=negation;
  (*a)->numero_de_variable=numero_de_variable;
  ajouter_si_pas_deja_present(&((*a)->liste_arr),arr,etiq_origine);
  return;
}
if ((*a)->numero_de_meta==meta && (*a)->negation==negation
    && (*a)->numero_de_variable==numero_de_variable) {
  // 2eme cas: mise a jour du premier element
  ajouter_si_pas_deja_present(&((*a)->liste_arr),arr,etiq_origine);
  return;
}
ptr=*a;
while (ptr->suivant!=NULL
       && !(ptr->suivant->numero_de_meta==meta
            && (*a)->negation==negation
            && (*a)->numero_de_variable==numero_de_variable))
  ptr=ptr->suivant;
if (ptr->suivant==NULL) {
  // on insere en fin de liste
  ptr->suivant=nouvelle_liste_metas();
  ptr->suivant->numero_de_meta=meta;
  ptr->suivant->negation=negation;
  ptr->suivant->numero_de_variable=numero_de_variable;
  ajouter_si_pas_deja_present(&(ptr->suivant->liste_arr),arr,etiq_origine);
  return;
}
// on met a jour l'element
ajouter_si_pas_deja_present(&(ptr->suivant->liste_arr),arr,etiq_origine);
}



void optimiser_trans(struct transition_fst* ptr,Etat_opt* e2,Etiquette* etiquette) {
Etiquette e;
int controle;
if (ptr->etiquette<0) {
  // cas d'un sous-graphe
  ajouter_sous_graphe(ptr,&((*e2)->liste_sous_graphes));
  return;
}
e=etiquette[ptr->etiquette];
if (e==NULL) {
   fprintf(stderr,"Internal problem in optimiser_trans\n");
   exit(1);
}
controle=e->controle;
//---pattern mot compose
if (e->pattern_compose!=-777) {
  ajouter_pattern(e->pattern_compose,ptr->etiquette,&((*e2)->liste_patterns_composes),ptr->arr,controle);
}
//----------------------
if (controle&64) {
  // liste de mots
  ajouter_liste_de_tokens(e,ptr->etiquette,e2,ptr->arr,&((*e2)->nombre_de_tokens));
  return;
}
if ((controle&32)&&(e->numero!=-1)) {
  // mot tout seul
  ajouter_token_a_liste_tokens(e->numero,ptr->etiquette,&((*e2)->liste_tokens),ptr->arr,&((*e2)->nombre_de_tokens));
  if (e->numeros!=NULL) {
     ajouter_liste_de_tokens(e,ptr->etiquette,e2,ptr->arr,&((*e2)->nombre_de_tokens));
  }
  return;
}
if ((controle&8)&&(e->numero!=-1)) {
  // pattern
  ajouter_pattern(e->numero,ptr->etiquette,&((*e2)->liste_patterns),ptr->arr,controle);
  return;
}
if (controle&16) {
  // meta
  int k=-1;
  if (e->numero==VAR_START || e->numero==VAR_END) {
     // if the meta is $a( or $a)
     k=get_token_number(e->contenu,transduction_variable_index);
  }
  ajouter_meta(e->numero,ptr->etiquette,&((*e2)->liste_metas),ptr->arr,controle,k);
  return;
}
}



void convertir_liste_tokens_en_tableau(Etat_opt *e2,int e) {
int i;
struct liste_de_tokens* l;
struct liste_de_tokens* tmp;
if ((*e2)->nombre_de_tokens==0) return;
(*e2)->tableau_de_tokens=(int*)malloc(sizeof(int)*(*e2)->nombre_de_tokens);
(*e2)->tableau_liste_arr=(struct liste_arrivees**)malloc(sizeof(struct liste_arrivees*)*(*e2)->nombre_de_tokens);
i=0;
l=(*e2)->liste_tokens;
while (l!=NULL) {
  (*e2)->tableau_de_tokens[i]=l->tok->numero_de_token;
  (*e2)->tableau_liste_arr[i]=l->tok->liste_arr;
  i++;
  tmp=l;
  l=l->suivant;
  free(tmp->tok);
  free(tmp);
}
if (i!=(*e2)->nombre_de_tokens) {
   fprintf(stderr,"Probleme interne dans la fonction convertir_liste_tokens_en_tableau\n");
}
(*e2)->liste_tokens=NULL;
}



void optimiser_etat(Etat_fst e1,Etat_opt* e2,int e,Etiquette* etiquette) {
struct transition_fst* ptr;
if (e1==NULL) {
  *e2=NULL;
  return;
}
*e2=nouveau_etat_opt();
(*e2)->controle=e1->controle;
ptr=e1->trans;
while (ptr!=NULL) {
  optimiser_trans(ptr,e2,etiquette);
  ptr=ptr->suivant;
}
convertir_liste_tokens_en_tableau(e2,e);
}



void optimize_fst2(Automate_fst2* automate) {
int i;
for (i=0;i<automate->nombre_etats;i++) {
  graphe_opt[i]=NULL;
  optimiser_etat(automate->etat[i],&graphe_opt[i],i,automate->etiquette);
}
}
