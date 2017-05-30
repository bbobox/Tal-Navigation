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
#include "Arbre_mots_composes.h"
//---------------------------------------------------------------------------


int decouper_chaine_en_tokens(unichar* ch,int t[],Alphabet* alph,struct string_hash* tok) {
int i,c,n_token,j,k;
struct liste_nombres* ptr;
unichar m[400];
k=0;
n_token=0;
if (CHAR_BY_CHAR==THAI) {
   // if we are processing a thai dictionary, we go char by char
   n_token=0;
   i=0;
   m[1]='\0';
   while (ch[i]!='\0') {
      m[0]=ch[i];
      j=get_token_number(m,tok);
      if (j==-1) {
         t[n_token++]=-3;
         t[n_token++]=-5;
      }
      else t[n_token++]=j;
      i++;
   }
   t[n_token]=-1;
   return 1;
}
// if we are not in THAI mode
while ((c=ch[k])!='\0') {
  if (c==' ') {
              j=ESPACE;
              if (j==-1) {
                t[n_token++]=-3;
                t[n_token++]=-5;
              }
              else t[n_token++]=j;
              while ((c=ch[++k])==' ');
            }
    else if (is_letter((unichar)c,alph)) {
              // on est dans le cas d'un mot
              i=0;
              do {
                m[i++]=(unichar)c;
                c=ch[++k];
              }
              while (is_letter((unichar)c,alph));
              m[i]='\0';

              if (n_token>0 /*&& toutes les variantes sont autorisees*/ ) {
                ////////////// TEST SUR LES VARIANTES MIN/MAJ AUTORISEES
                // si on traite le premier token, on autorise qd meme les variantes
                // au cas ou il apparaisse en d�but de phrase
                j=get_token_number(m,tok);
                if (j==-1) {
                  t[n_token++]=-3;
                  t[n_token++]=-5;
                }
                else t[n_token++]=j;
                ////////////////// TEST
              }
              else {
                // on calcule les variantes min-maj
                t[n_token++]=-3; // debut de liste de tokens

                ptr=get_token_list_for_sequence(m,alph,tok);

                while (ptr!=NULL) {
                  j=ptr->n;
                  t[n_token++]=j;
                  ptr=ptr->suivant;
                }
                t[n_token++]=-5; // fin de liste de tokens
              }
            }
    else {
               m[0]=(unichar)c;
               m[1]='\0';
               k++;
               j=get_token_number(m,tok);
               if (j==-1) {
                  t[n_token++]=-3;
                  t[n_token++]=-5;
               } else t[n_token++]=j;
    }
}
t[n_token]=-1;
return 1;
}



void ajouter_code_a_etat_dlc(struct etat_dlc* n,int code) {
struct liste_nombres *l,*precedent;
int stop;

if (n->patterns==NULL) {
  // 1er cas liste vide
  l=(struct liste_nombres*)malloc(sizeof(struct liste_nombres));
  l->n=code;
  l->suivant=n->patterns;
  n->patterns=l;
  (n->nombre_patterns)++;
  return;
}
if (n->patterns->n==code)
  // 2eme cas: 1er element=code
  return;
if (n->patterns->n>code) {
  // 3eme cas: on doit inserer en tete de liste
  l=(struct liste_nombres*)malloc(sizeof(struct liste_nombres));
  l->n=code;
  l->suivant=n->patterns;
  n->patterns=l;
  (n->nombre_patterns)++;
  return;
}
// cas general
precedent=n->patterns;
stop=0;
while (!stop && precedent->suivant!=NULL) {
  if (precedent->suivant->n==code) return;
  else if (precedent->suivant->n<code) precedent=precedent->suivant;
  else stop=1;
}
l=(struct liste_nombres*)malloc(sizeof(struct liste_nombres));
l->n=code;
l->suivant=precedent->suivant;
precedent->suivant=l;
(n->nombre_patterns)++;
return;
}



struct transition_dlc* nouvelle_transition_dlc() {
struct transition_dlc* t;
t=(struct transition_dlc*)malloc(sizeof(struct transition_dlc));
t_dlc++;
t->token=-4444;
t->arr=NULL;
t->suivant=NULL;
return t;
}



struct etat_dlc* nouveau_noeud_dlc() {
struct etat_dlc* n;
n=(struct etat_dlc*)malloc(sizeof(struct etat_dlc));
n_dlc++;
n->patterns=NULL;
n->nombre_patterns=0;
n->tab_patterns=NULL;
n->liste=NULL;
n->nombre_transitions=0;
n->tab_token=NULL;
n->tab_arr=NULL;
return n;
}



struct etat_dlc* get_trans_dlc_sans_creer(struct etat_dlc* n,int t) {
struct transition_dlc* l;
l=n->liste;
while (l!=NULL) {
  if (l->token==t) return l->arr;
  l=l->suivant;
}
return NULL;
}



struct etat_dlc* get_trans_dlc(struct etat_dlc* n,int t) {
struct transition_dlc* l;
struct transition_dlc* precedent;
int stop;
if (n->liste==NULL) {
  // 1er cas liste vide
  l=nouvelle_transition_dlc();
  l->token=t;
  l->suivant=n->liste;
  l->arr=nouveau_noeud_dlc();
  n->liste=l;
  (n->nombre_transitions)++;
  return l->arr;
}
if (n->liste->token==t)
  // 2eme cas: 1er element=code
  return n->liste->arr;
if (n->liste->token>t) {
  // 3eme cas: on doit inserer en tete de liste
  l=nouvelle_transition_dlc();
  l->token=t;
  l->suivant=n->liste;
  l->arr=nouveau_noeud_dlc();
  n->liste=l;
  (n->nombre_transitions)++;
  return l->arr;
}
// cas general
precedent=n->liste;
stop=0;
while (!stop && precedent->suivant!=NULL) {
  if (precedent->suivant->token==t) return precedent->suivant->arr;
  else if (precedent->suivant->token<t) precedent=precedent->suivant;
  else stop=1;
}
l=nouvelle_transition_dlc();
l->token=t;
l->arr=nouveau_noeud_dlc();
l->suivant=precedent->suivant;
precedent->suivant=l;
(n->nombre_transitions)++;
return l->arr;
}



void ajouter_dlc(int* token_dlc,int i,struct etat_dlc* n,int code) {
struct etat_dlc* ptr;
int pos,premier_token;
if (token_dlc[i]==-1) {
  // on est au bout de la branche
  ajouter_code_a_etat_dlc(n,code);
  return;
}
premier_token=(i==0);
if (token_dlc[i]!=-3) {
  ptr=get_trans_dlc(n,token_dlc[i]);
  if (premier_token) {
     tableau_dlc[token_dlc[i]]=ptr;
  }
  ajouter_dlc(token_dlc,i+1,ptr,code);
  return;
}
pos=i;
do pos++;
while (token_dlc[pos]!=-5);
pos++;
// pos est l'endroit auquel on cherchera le token suivant
i++;
while (token_dlc[i]!=-5)  {
  ptr=get_trans_dlc(n,token_dlc[i]);
  if (premier_token) {
     tableau_dlc[token_dlc[i]]=ptr;
  }
  ajouter_dlc(token_dlc,pos,ptr,code);
  i++;
}
}



void ajouter_a_dlc_sans_code(unichar* m,Alphabet* alph,struct string_hash* tok) {
int token_dlc_temp[40];
decouper_chaine_en_tokens(m,token_dlc_temp,alph,tok);
ajouter_dlc(token_dlc_temp,0,racine_dlc,-555);
}



void ajouter_a_dlc_avec_code(unichar* m,int code,Alphabet* alph,struct string_hash* tok) {
int token_dlc_temp[40];
decouper_chaine_en_tokens(m,token_dlc_temp,alph,tok);
ajouter_dlc(token_dlc_temp,0,racine_dlc,code);
}



int inserer_code_dans_etat_dlc(struct etat_dlc* n,int code,int code2) {
if (appartient_a_liste(code,n->patterns)) {
  ajouter_code_a_etat_dlc(n,code2);
  return 1;
}
return 0;
}



int inserer_dans_dlc(int token_dlc[],int i,struct etat_dlc* n,int code,int code2) {
struct etat_dlc* ptr;
int pos,resultat;
resultat=0;
if (token_dlc[i]==-1) {
  // on est au bout de la branche
  return inserer_code_dans_etat_dlc(n,code,code2);
}
if (token_dlc[i]!=-3) {
  ptr=get_trans_dlc_sans_creer(n,token_dlc[i]);
  if (ptr!=NULL) return inserer_dans_dlc(token_dlc,i+1,ptr,code,code2);
  else return 0;
}
pos=i;
do pos++;
while (token_dlc[pos]!=-5);
pos++;
// pos est l'endroit auquel on cherchera le token suivant
i++;
while (token_dlc[i]!=-5)  {
  ptr=get_trans_dlc_sans_creer(n,token_dlc[i]);
  if (ptr!=NULL) resultat=resultat+inserer_dans_dlc(token_dlc,pos,ptr,code,code2);
  i++;
}
return resultat;
}



int remplacer_dans_dlc(unichar* m,int code,int code2,Alphabet* alph,struct string_hash* tok) {
int token_dlc[40];
decouper_chaine_en_tokens(m,token_dlc,alph,tok);
return inserer_dans_dlc(token_dlc,0,racine_dlc,code,code2);
}
