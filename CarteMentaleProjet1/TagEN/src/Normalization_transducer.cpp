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

#include "Normalization_transducer.h"
//---------------------------------------------------------------------------


struct noeud_arbre_normalization* new_noeud_arbre_normalization() {
struct noeud_arbre_normalization* tmp;
tmp=(struct noeud_arbre_normalization*)malloc(sizeof(struct noeud_arbre_normalization));
tmp->liste_arrivee=NULL;
tmp->trans=NULL;
return tmp;
}


struct trans_arbre_normalization* new_trans_arbre_normalization(int token) {
struct trans_arbre_normalization* tmp;
tmp=(struct trans_arbre_normalization*)malloc(sizeof(struct trans_arbre_normalization));
tmp->token=token;
tmp->s=NULL;
tmp->arr=NULL;
tmp->suivant=NULL;
return tmp;
}



struct trans_arbre_normalization* new_trans_arbre_normalization_string(unichar* s) {
struct trans_arbre_normalization* tmp;
tmp=(struct trans_arbre_normalization*)malloc(sizeof(struct trans_arbre_normalization));
tmp->token=-1;
tmp->s=(unichar*)malloc(sizeof(unichar)*(1+u_strlen(s)));
u_strcpy(tmp->s,s);
tmp->arr=NULL;
tmp->suivant=NULL;
return tmp;
}


void free_noeud_arbre_normalization(struct noeud_arbre_normalization* n) {
if (n==NULL) return;
free_trans_arbre_normalization(n->trans);
free(n);
}


void free_trans_arbre_normalization(struct trans_arbre_normalization* t) {
struct trans_arbre_normalization* tmp;
while (t!=NULL) {
  tmp=t;
  if (tmp->s!=NULL) {
     free(tmp->s);
  }
  free_noeud_arbre_normalization(tmp->arr);
  t=t->suivant;
  free(tmp);
}
}



//
// this function looks for a transition by a token represented by an int
//
struct trans_arbre_normalization* get_trans_arbre_normalization(int n,struct trans_arbre_normalization* t) {
if (t==NULL) return NULL;
if (t->token==n) return t;
return get_trans_arbre_normalization(n,t->suivant);
}



//
// this function looks for a transition by a token represented by a string
//
struct trans_arbre_normalization* get_trans_arbre_normalization_string(unichar* s,struct trans_arbre_normalization* t) {
if (t==NULL) return NULL;
if (!u_strcmp(t->s,s)) {
   return t;
}
return get_trans_arbre_normalization_string(s,t->suivant);
}



struct temp_list* new_temp_list(unichar* output,struct noeud_arbre_normalization* n) {
struct temp_list* t;
t=(struct temp_list*)malloc(sizeof(struct temp_list));
t->output=(unichar*)malloc(sizeof(unichar)*(1+u_strlen(output)));
u_strcpy(t->output,output);
t->arr=n;
t->suivant=NULL;
return t;
}



struct temp_list* inserer_dans_temp_list(unichar* output,
                                         struct noeud_arbre_normalization* n,
                                         struct temp_list* l) {
if (l==NULL) return new_temp_list(output,n);
if (l->arr==n && !u_strcmp(l->output,output)) return l;
l->suivant=inserer_dans_temp_list(output,n,l->suivant);
return l;
}



void free_temp_list(struct temp_list* t) {
if (t==NULL) return;
if (t->output!=NULL) free(t->output);
free(t);
}



void explorer_automate_normalization(Automate_fst2*,int,
                                     struct noeud_arbre_normalization*,
                                     struct string_hash*,unichar*,
                                     Alphabet*);


//
// this function explores a sub-graph, considering tokens as int
//
void explorer_sub_automate_normalization(Automate_fst2* automate,int n,
                                     struct noeud_arbre_normalization* noeud_normalization,
                                     struct string_hash* hash,unichar* output,
                                     Alphabet* alph,struct temp_list** TEMP_LIST) {
Etat_fst etat;
etat=automate->etat[n];
if (etat->controle&1) {
   // if we are in a final state
   (*TEMP_LIST)=inserer_dans_temp_list(output,noeud_normalization,(*TEMP_LIST));
}
struct transition_fst* trans;
trans=etat->trans;
unichar tmp[1000];
while (trans!=NULL) {
   if (trans->etiquette<0) {
      // case of a sub-graph
      struct temp_list* TMP=NULL;
      explorer_sub_automate_normalization(automate,automate->debut_graphe_fst2[-(trans->etiquette)],noeud_normalization,
                                        hash,output,alph,&TMP);
      while (TMP!=NULL) {
         // we continue to explore the current automaton
         explorer_sub_automate_normalization(automate,trans->arr,TMP->arr,
                                        hash,TMP->output,alph,TEMP_LIST);
         struct temp_list* z=TMP;
         TMP=TMP->suivant;
         free_temp_list(z);
      }
   }
   else {
      // normal transition
      Etiquette etiq;
      etiq=automate->etiquette[trans->etiquette];
      u_strcpy(tmp,output);
      u_strcat_char(tmp," ");
      if (etiq->transduction!=NULL && u_strcmp_char(etiq->transduction,"")
          && u_strcmp_char(etiq->transduction,"<E>") && !only_spaces(etiq->transduction)) {
         // we append the output if it exists and is not epsilon
         u_strcat(tmp,etiq->transduction);
      }
      struct liste_nombres* liste=get_token_list_for_sequence(etiq->contenu,alph,hash);
      while (liste!=NULL) {
        struct trans_arbre_normalization* trans_norm;
        trans_norm=get_trans_arbre_normalization(liste->n,noeud_normalization->trans);
        if (trans_norm==NULL) {
           // if the transition does not exist in the tree, we create it
           trans_norm=new_trans_arbre_normalization(liste->n);
           // we also create the destination node
           trans_norm->arr=new_noeud_arbre_normalization();
           trans_norm->suivant=noeud_normalization->trans;
           noeud_normalization->trans=trans_norm;
        }
        explorer_sub_automate_normalization(automate,trans->arr,trans_norm->arr,
                                        hash,tmp,alph,TEMP_LIST);
        struct liste_nombres* L=liste;
        liste=liste->suivant;
        free(L);
      }
   }
   trans=trans->suivant;
}
}



//
// this function explores a sub-graph, considering tokens as strings
//
void explorer_sub_automate_normalization_string(Automate_fst2* automate,int n,
                                     struct noeud_arbre_normalization* noeud_normalization,
                                     unichar* output,struct temp_list** TEMP_LIST) {
Etat_fst etat;
etat=automate->etat[n];
if (etat->controle&1) {
   // if we are in a final state
   (*TEMP_LIST)=inserer_dans_temp_list(output,noeud_normalization,(*TEMP_LIST));
}
struct transition_fst* trans;
trans=etat->trans;
unichar tmp[1000];
while (trans!=NULL) {
   if (trans->etiquette<0) {
      // case of a sub-graph
      struct temp_list* TMP=NULL;
      explorer_sub_automate_normalization_string(automate,automate->debut_graphe_fst2[-(trans->etiquette)],noeud_normalization,
                                        output,&TMP);
      while (TMP!=NULL) {
         // we continue to explore the current automaton
         explorer_sub_automate_normalization_string(automate,trans->arr,TMP->arr,
                                        TMP->output,TEMP_LIST);
         struct temp_list* z=TMP;
         TMP=TMP->suivant;
         free_temp_list(z);
      }
   }
   else {
      // normal transition
      Etiquette etiq;
      etiq=automate->etiquette[trans->etiquette];
      u_strcpy(tmp,output);
      u_strcat_char(tmp," ");
      if (etiq->transduction!=NULL && u_strcmp_char(etiq->transduction,"")
          && u_strcmp_char(etiq->transduction,"<E>") && !only_spaces(etiq->transduction)) {
         // we append the output if it exists and is not epsilon
         u_strcat(tmp,etiq->transduction);
      }
      struct trans_arbre_normalization* trans_norm;
      trans_norm=get_trans_arbre_normalization_string(etiq->contenu,noeud_normalization->trans);
      if (trans_norm==NULL) {
         // if the transition does not exist in the tree, we create it
         trans_norm=new_trans_arbre_normalization_string(etiq->contenu);
         // we also create the destination node
         trans_norm->arr=new_noeud_arbre_normalization();
         trans_norm->suivant=noeud_normalization->trans;
         noeud_normalization->trans=trans_norm;
      }
      explorer_sub_automate_normalization_string(automate,trans->arr,trans_norm->arr,
                                        tmp,TEMP_LIST);
   }
   trans=trans->suivant;
}
}



//
// this function explore the normalization grammar to construct the token tree
//
void explorer_automate_normalization(Automate_fst2* automate,int n,
                                     struct noeud_arbre_normalization* noeud_normalization,
                                     struct string_hash* hash,unichar* output,
                                     Alphabet* alph) {
Etat_fst etat;
etat=automate->etat[n];
if (etat->controle&1) {
   // if we are in a final state
   noeud_normalization->liste_arrivee=insert_in_string_list(output,noeud_normalization->liste_arrivee);
}
struct transition_fst* trans;
trans=etat->trans;
unichar tmp[1000];
while (trans!=NULL) {
   if (trans->etiquette<0) {
      // case of a sub-graph
      struct temp_list* TMP=NULL;
      explorer_sub_automate_normalization(automate,automate->debut_graphe_fst2[-(trans->etiquette)],noeud_normalization,
                                        hash,output,alph,&TMP);
      while (TMP!=NULL) {
         // we continue to explore the current automaton
         explorer_automate_normalization(automate,trans->arr,TMP->arr,
                                        hash,TMP->output,alph);
         struct temp_list* z=TMP;
         TMP=TMP->suivant;
         free_temp_list(z);
      }
   }
   else {
      // normal transition
      Etiquette etiq;
      etiq=automate->etiquette[trans->etiquette];
      u_strcpy(tmp,output);
      u_strcat_char(tmp," ");
      if (etiq->transduction!=NULL && u_strcmp_char(etiq->transduction,"")
          && u_strcmp_char(etiq->transduction,"<E>") && !only_spaces(etiq->transduction)) {
         // we append the output if it exists and is not epsilon
         u_strcat(tmp,etiq->transduction);
      }
      struct liste_nombres* liste=get_token_list_for_sequence(etiq->contenu,alph,hash);
      while (liste!=NULL) {
        struct trans_arbre_normalization* trans_norm;
        trans_norm=get_trans_arbre_normalization(liste->n,noeud_normalization->trans);
        if (trans_norm==NULL) {
           // if the transition does not exist in the tree, we create it
           trans_norm=new_trans_arbre_normalization(liste->n);
           // we also create the destination node
           trans_norm->arr=new_noeud_arbre_normalization();
           trans_norm->suivant=noeud_normalization->trans;
           noeud_normalization->trans=trans_norm;
        }
        explorer_automate_normalization(automate,trans->arr,trans_norm->arr,
                                        hash,tmp,alph);
        struct liste_nombres* L=liste;
        liste=liste->suivant;
        free(L);
      }
   }
   trans=trans->suivant;
}
}



//
// this function explore the normalization grammar to construct the token tree
//
void explorer_automate_normalization_string(Automate_fst2* automate,int n,
                                     struct noeud_arbre_normalization* noeud_normalization,
                                     unichar* output) {
Etat_fst etat;
etat=automate->etat[n];
if (etat->controle&1) {
   // if we are in a final state
   noeud_normalization->liste_arrivee=insert_in_string_list(output,noeud_normalization->liste_arrivee);
}
struct transition_fst* trans;
trans=etat->trans;
unichar tmp[1000];
while (trans!=NULL) {
   if (trans->etiquette<0) {
      // case of a sub-graph
      struct temp_list* TMP=NULL;
      explorer_sub_automate_normalization_string(automate,automate->debut_graphe_fst2[-(trans->etiquette)],noeud_normalization,
                                        output,&TMP);
      while (TMP!=NULL) {
         // we continue to explore the current automaton
         explorer_automate_normalization_string(automate,trans->arr,TMP->arr,TMP->output);
         struct temp_list* z=TMP;
         TMP=TMP->suivant;
         free_temp_list(z);
      }
   }
   else {
      // normal transition
      Etiquette etiq;
      etiq=automate->etiquette[trans->etiquette];
      u_strcpy(tmp,output);
      u_strcat_char(tmp," ");
      if (etiq->transduction!=NULL && u_strcmp_char(etiq->transduction,"")
          && u_strcmp_char(etiq->transduction,"<E>") && !only_spaces(etiq->transduction)) {
         // we append the output if it exists and is not epsilon
         u_strcat(tmp,etiq->transduction);
      }
      struct trans_arbre_normalization* trans_norm;
      trans_norm=get_trans_arbre_normalization_string(etiq->contenu,noeud_normalization->trans);
      if (trans_norm==NULL) {
         // if the transition does not exist in the tree, we create it
         trans_norm=new_trans_arbre_normalization_string(etiq->contenu);
         // we also create the destination node
         trans_norm->arr=new_noeud_arbre_normalization();
         trans_norm->suivant=noeud_normalization->trans;
         noeud_normalization->trans=trans_norm;
      }
      explorer_automate_normalization_string(automate,trans->arr,trans_norm->arr,tmp);
   }
   trans=trans->suivant;
}
}



//
// this function constructs a token tree from a normalization grammar
// tokens are represented by integers
//
struct noeud_arbre_normalization* load_normalization_transducer(char* nom,Alphabet* alph,struct text_tokens* tok) {
Automate_fst2* automate=load_fst2(nom,0);
if (automate==NULL) {
   // if the loading of the normalization transducer has failed, we return
   return NULL;
}
struct string_hash* hash=new_string_hash();
// we create the token tree to speed up the consultation
for (int i=0;i<tok->N;i++) {
   get_hash_number_without_insert(tok->token[i],hash);
}
struct noeud_arbre_normalization* root=new_noeud_arbre_normalization();
unichar a[1];
a[0]='\0';
explorer_automate_normalization(automate,automate->debut_graphe_fst2[1],root,hash,a,alph);
free_fst2(automate);
free_string_hash_without_insert(hash);
return root;
}



//
// this function constructs a token tree from a normalization grammar
// tokens are represented by strings
//
struct noeud_arbre_normalization* load_normalization_transducer_string(char* nom) {
Automate_fst2* automate=load_fst2(nom,0);
if (automate==NULL) {
   // if the loading of the normalization transducer has failed, we return
   return NULL;
}
struct noeud_arbre_normalization* root=new_noeud_arbre_normalization();
unichar a[1];
a[0]='\0';
explorer_automate_normalization_string(automate,automate->debut_graphe_fst2[1],root,a);
free_fst2(automate);
return root;
}



//
// this function takes an output s like " {de,.PREP} {le,.DET} "
// and returns the string list of the tags that must be produced:
// "{de,.PREP}" and "{le,.DET}"
//
struct string_list* tokenize_normalization_output(unichar* s,Alphabet* alph) {
if (s==NULL) return NULL;
struct string_list* result=NULL;
unichar tmp[1000];
int i;
int j;
i=0;
while (s[i]!='\0') {
   while (s[i]==' ') {
      // we ingore spaces
      i++;
   }
   if (s[i]!='\0') {
      // if we are not at the end of the string
      if (s[i]=='{') {
         // case of a tag like "{de,.PREP}"
         j=0;
         while (s[i]!='\0' && s[i]!='}') {
            tmp[j++]=s[i++];
         }
         if (s[i]!='\0') {
            // the end of string is an error (no closing '}'), so we save the
            // tag only if it is a valid one
            tmp[j]='}';
            tmp[j+1]='\0';
            // we go on the next char
            i++;
            result=insert_at_end_of_string_list(tmp,result);
         }
      }
      else
      if (is_letter(s[i],alph)) {
         // case of a letter sequence like "Rivoli"
         j=0;
         while (is_letter(s[i],alph)) {
            tmp[j++]=s[i++];
         }
         tmp[j]='\0';
         // we don't have to go on the next char, we are allready on it
         result=insert_at_end_of_string_list(tmp,result);
      }
      else {
         // case of a single non-space char like "-"
         tmp[0]=s[i];
         tmp[1]='\0';
         // we go on the next char of the string
         i++;
         result=insert_at_end_of_string_list(tmp,result);
      }
   }
}


return result;
}
