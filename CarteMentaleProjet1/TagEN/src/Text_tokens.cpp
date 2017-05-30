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
#include "Text_tokens.h"
//---------------------------------------------------------------------------


struct text_tokens* new_text_tokens() {
struct text_tokens* tmp;
tmp=(struct text_tokens*)malloc(sizeof(struct text_tokens));
tmp->N=0;
tmp->SENTENCE_MARKER=-1;
tmp->token=NULL;
return tmp;
}


struct text_tokens* load_text_tokens(char* nom) {
FILE* f;
f=u_fopen(nom,U_READ);
if (f==NULL) {
   return NULL;
}
struct text_tokens* res;
res=new_text_tokens();
res->N=u_read_int(f);
res->token=(unichar**)malloc((res->N)*sizeof(unichar*));
unichar tmp[1000];
res->SENTENCE_MARKER=-1;
int i=0;
while (u_read_line(f,tmp)) {
  res->token[i]=(unichar*)malloc(sizeof(unichar)*(1+u_strlen(tmp)));
  u_strcpy(res->token[i],tmp);
  if (!u_strcmp_char(tmp,"{S}")) {
     res->SENTENCE_MARKER=i;
  } else if (!u_strcmp_char(tmp," ")) {
            (res->SPACE)=i;
         }
  i++;
}
u_fclose(f);
return res;
}



struct string_hash* load_text_tokens_hash(char* nom) {
FILE* f;
f=u_fopen(nom,U_READ);
if (f==NULL) {
   return NULL;
}
u_read_int(f);
struct string_hash* res;
res=new_string_hash_N(400000);
unichar tmp[1000];
while (u_read_line(f,tmp)) {
  get_hash_number(tmp,res);
}
u_fclose(f);
return res;
}



struct string_hash* load_text_tokens_hash(char* nom,int *SENTENCE_MARKER) {
FILE* f;
f=u_fopen(nom,U_READ);
if (f==NULL) {
   return NULL;
}
u_read_int(f);
struct string_hash* res;
res=new_string_hash_N(400000);
unichar tmp[1000];
int x;
while (u_read_line(f,tmp)) {
   x=get_hash_number(tmp,res);
   if (!u_strcmp_char(tmp,"{S}")) {
      (*SENTENCE_MARKER)=x;
   }
}
u_fclose(f);
return res;
}




void free_text_tokens(struct text_tokens* tok) {
for (int i=0;i<tok->N;i++) {
  free(tok->token[i]);
}
free(tok);
}




void explorer_token_tree(int pos,unichar* sequence,Alphabet* alph,struct arbre_hash* n,struct liste_nombres** l) {
if (sequence[pos]=='\0') {
   // if we are at the end of the sequence
   if (n->final!=-1) {
      // if the sequence is a text token, we add its number to the list
      (*l)=inserer_dans_liste_nombres(n->final,*l);
   }
   return;
}
struct arbre_hash_trans* trans=n->trans;
while (trans!=NULL) {
  if (is_equal_or_case_equal(sequence[pos],trans->c,alph)) {
     // if we can follow the transition
     explorer_token_tree(pos+1,sequence,alph,trans->arr,l);
  }
  trans=trans->suivant;
}
}



struct liste_nombres* get_token_list_for_sequence(unichar* sequence,Alphabet* alph,struct string_hash* hash) {
struct liste_nombres* l=NULL;
explorer_token_tree(0,sequence,alph,hash->racine,&l);
return l;
}



int get_token_number(unichar* s,struct text_tokens* tok) {
for (int i=0;i<tok->N;i++) {
    if (!u_strcmp(tok->token[i],s)) return i;
}
return -1;
}



//
// return 1 if s is a digit sequence, 0 else
//
int is_a_digit_token(unichar* s) {
int i=0;
while (s[i]!='\0') {
   if (s[i]<'0' || s[i]>'9') {
      return 0;
   }
   i++;
}
return 1;
}
//---------------------------------------------------------------------------

