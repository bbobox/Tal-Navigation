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
#include "unicode.cpp"
#include "AutomateFst2.cpp"
#include "LiberationFst2.cpp"
#include "Copyright.h"
//---------------------------------------------------------------------------


//
// "E:\My Unitex\French\Dela\delas.dic" "E:\My Unitex\French\Dela\delasflx.dic"  "E:\My Unitex\French\Inflection"
//


#define N_FST2 3000 // maximum number of flexional transducers

char repertoire[1000];
FILE *f;
FILE *f_out;
int line=0;
Automate_fst2* fst2[N_FST2];
int n_fst2=0;
struct node* root;
int ADD_TWO_POINTS=0;
int REMOVE_DIGITS_FROM_GRAMM_CODE=1;


void traiter_dico();
void inflect(unichar*,char*,unichar*);
int get_transducer(char*);
void free_transducer_tree();
struct node* new_node();
void free_node(struct node*);
void free_transition(struct transition*);
void explore_state(unichar*,unichar*,unichar*,Automate_fst2*,int,unichar*);



void usage() {
printf("%s",COPYRIGHT);
printf("Usage: Inflect <delas> <result> <dir> [-a] [-k] \n%s",CR);
printf("     <delas> : the unicode delas file to be inflected\n%s",CR);
printf("     <result> : the unicode resulting dictionary \n%s",CR);
printf("     <dir> : the inflectional graphs directory.\n%s",CR);
printf("     -a : this optional parameter indicates that a':'\n%s",CR);
printf("          character must be inserted if the produced\n%s",CR);
printf("          flexional information sequence does not begin\n%s",CR);
printf("          by this character.\n%s",CR);
printf("     -k : this optional parameter indicates that\n%s",CR);
printf("          flexional grammar names must not be reduced\n%s",CR);
printf("          by removing digits (N32 is not turned to N)\n%s",CR);
printf("\n%sInflects a DELAS.\n%s",CR,CR);
}




int main(int argc, char **argv) {
if (argc<4 || argc>6) {
   usage();
   return 0;
}
if (argc>4) {
   if (!strcmp(argv[4],"-a")) {
      ADD_TWO_POINTS=1;
      if (argc==6) {
         if (!strcmp(argv[5],"-k")) {
            REMOVE_DIGITS_FROM_GRAMM_CODE=0;
         } else {
            fprintf(stderr,"Invalid parameter: %s\n",argv[5]);
            return 1;
         }
      }
   }
   else if (!strcmp(argv[4],"-k")) {
           REMOVE_DIGITS_FROM_GRAMM_CODE=0;
           if (argc==6) {
              if (!strcmp(argv[5],"-a")) {
                 ADD_TWO_POINTS=1;
              } else {
                 fprintf(stderr,"Invalid parameter: %s\n",argv[5]);
                 return 1;
              }
           }
   } else {
     fprintf(stderr,"Invalid parameter: %s\n",argv[4]);
     return 1;
   }
}
strcpy(repertoire,argv[3]);
strcat(repertoire,"/");
f=u_fopen(argv[1],U_READ);
f_out=u_fopen(argv[2],U_WRITE);
if (f==NULL) {
  fprintf(stderr,"Cannot open %s\n",argv[1]);
  return 1;
}
if (f_out==NULL) {
  fprintf(stderr,"Cannot open %s\n",argv[2]);
  return 1;
}
traiter_dico();
u_fclose(f);
u_fclose(f_out);
printf("Done.\n%s",CR);
return 0;
}


//
// This function extract the grammatical code from the flexional
// transducer name: N32 -> N. It returns the length of it
//
int get_grammatical_code(char* code,unichar* res,int REMOVE) {
int i=0;
if (REMOVE) {
   // N32 -> N
   while (u_is_letter((unichar)code[i])) {
      res[i]=(unichar)code[i++];
   }
   res[i]='\0';
} else {
   // we don't modify the code
   u_strcpy_char(res,code);
   i=u_strlen(res);
}
return i;
}


//
// reads and processes a line of the DELAS
//
int read_line() {
unichar lemme[1000];
char code[100];
unichar code_gramm[1000];
int c;
int i;
int fini=1;
line++;
i=0;
while ((c=u_fgetc(f))!=',' && c!='\n' && c!=EOF) {
    if (c=='\\') {
        c=u_fgetc(f);
        if (c=='\n') {
            fprintf(stderr,"Line %d: '\\' at end of line\n",line);
            return 1;
        }
        if (c==EOF) {
            fprintf(stderr,"Line %d: unexpected end of file\n",line);
            return 1;
        }
    }
    lemme[i++]=(unichar)c;
}
if (c==EOF) return 0;
lemme[i]='\0';
if (c=='\n') {
    fprintf(stderr,"Line %d: unexpected end of line\n",line);
    return 1;
}
// we read the flexional reference (N31, V2, ...)
i=0;
while ((c=u_fgetc(f))!='+' && c!='\n' && c!=EOF) {
    if (c=='\\') {
        c=u_fgetc(f);
        if (c=='\n') {
            fprintf(stderr,"Line %d: '\\' at end of line\n",line);
            return 1;
        }
        if (c==EOF) {
            fprintf(stderr,"Line %d: unexpected end of file\n",line);
            return 0;
        }
    }
    code[i++]=(char)c;
}
code[i]='\0';
if (c==EOF) fini=0;
if (i==0) {
    fprintf(stderr,"Line %d: missing flexional code\n",line);
    return fini;
}
i=get_grammatical_code(code,code_gramm,REMOVE_DIGITS_FROM_GRAMM_CODE);
if (i==0) {
   fprintf(stderr,"Line %d: empty flexional transducer name\n",line);
   return fini;
}
if (c=='+') {
    // we read the semantic codes if any (+Conc+z2)
    code_gramm[i++]='+';
    while ((c=u_fgetc(f))!='\n' && c!=EOF) {
        if (c=='\\') {
            c=u_fgetc(f);
            if (c=='\n') {
                fprintf(stderr,"Line %d: '\\' at end of line\n",line);
                return 1;
            }
            if (c==EOF) {
                fprintf(stderr,"Line %d: unexpected end of file\n",line);
                return 0;
            }
        }
        code_gramm[i++]=(unichar)c;
    }
    code_gramm[i]='\0';
    if (c==EOF) fini=0;
}
inflect(lemme,code,code_gramm);
return fini;
}



//
// reads the DELAS and produces the DELAF
//
void traiter_dico() {
root=new_node();
while (read_line());
free_transducer_tree();
}



//
// inflect the lemma 'lemme', using the flexional transducer 'flex', and
// taking 'code' as the basic grammatical code to be written in the DELAF
//
void inflect(unichar* lemme,char* flex,unichar* code) {
unichar flechi[1000];
unichar canonique[1000];
unichar code_gramm[1000];
unichar temp[1000];
temp[0]='\0';
int T=get_transducer(flex);
if (fst2[T]==NULL) {
    // if the automaton has not been loaded
    return;
}
u_strcpy(flechi,lemme);
u_strcpy(canonique,lemme);
u_strcpy(code_gramm,code);
explore_state(flechi,canonique,temp,fst2[T],0,code_gramm);
}






struct node {
    int final;
    struct transition* t;
};

struct transition {
    char c;
    struct node* n;
    struct transition* suivant;
};



struct node* new_node() {
struct node* n=(struct node*)malloc(sizeof(struct node));
n->final=-1;
n->t=NULL;
return n;
}


struct transition* new_transition(char c) {
struct transition* t=(struct transition*)malloc(sizeof(struct transition));
t->c=c;
t->n=NULL;
t->suivant=NULL;
return t;
}


//
// free the transduction t
//
void free_transition(struct transition* t) {
struct transition* tmp;
while (t!=NULL) {
    free_node(t->n);
    tmp=t;
    t=t->suivant;
    free(tmp);
}
}


//
// free the node n
//
void free_node(struct node* n) {
if (n==NULL) {
  fprintf(stderr,"Erreur dans free_node\n");
  return;
}
free_transition(n->t);
free(n);
}


//
// free the transducer tree memory
//
void free_transducer_tree() {
free_node(root);
}


//
// looks for a transition by the char c
// creates it if it does not exists
//
struct transition* get_transition(char c,struct transition* t,struct node** n) {
struct transition* tmp;
while (t!=NULL) {
    if (t->c==c) return t;
    t=t->suivant;
}
tmp=new_transition(c);
tmp->suivant=(*n)->t;
tmp->n=NULL;
(*n)->t=tmp;
return tmp;
}


//
// this function look for the path to 'flex', creating it if necessary
// the current node is n, and pos is the position in the flex string
//
int get_node(char* flex,int pos,struct node* n) {
if (flex[pos]=='\0') {
    // we are at the final node for flex
    if (n->final!=-1) {
        // if the automaton allready exists we returns its position
        return n->final;
    } else {
        // else we load it
        if (n_fst2==N_FST2) {
            fprintf(stderr,"Memory error: too much flexional transducers\n");
            exit(1);
        }
        char s[1000];
        strcpy(s,repertoire);
        strcat(s,flex);
        strcat(s,".fst2");
        fst2[n_fst2]=load_fst2(s,1);
        n->final=n_fst2;
        return n_fst2++;
        }
}
// if we are not at the end of the string flex
struct transition* trans=get_transition(flex[pos],n->t,&n);
if (trans->n==NULL) {
    trans->n=new_node();
}
return get_node(flex,pos+1,trans->n);
}



//
// try to load the transducer flex and returns its position in the
// 'fst2' array. Returns -1 if the transducer cannot be loaded
//
int get_transducer(char* flex) {
return get_node(flex,0,root);
}



struct couple_string {
   unichar flechi[1000];
   unichar out[1000];
   struct couple_string* suivant;
};

void explore_state_recursion(unichar*,unichar*,unichar*,Automate_fst2*,int,struct couple_string**,unichar*);



//
// explore the tag of the transition T
//
void explore_tag(struct transition_fst* T,unichar* flechi,unichar* canonique,unichar* sortie,
                 Automate_fst2* a,unichar* code_gramm) {
if (T->etiquette < 0) {
   // if we are in the case of a call to a sub-graph
   struct couple_string* L=NULL;
   struct couple_string* temp;
   explore_state_recursion(flechi,canonique,sortie,a,a->debut_graphe_fst2[-(T->etiquette)],&L,code_gramm);
   while (L!=NULL) {
      explore_state(L->flechi,canonique,L->out,a,T->arr,code_gramm);
      temp=L;
      L=L->suivant;
      free(temp);
   }
   return;
}
Etiquette e=a->etiquette[T->etiquette];
int pos=u_strlen(flechi);
unichar out[1000];
unichar pile[1000];
unichar etiq[1000];
int pos_etiq;
u_strcpy(out,sortie);
if (u_strcmp_char(e->transduction,"<E>")) {
   u_strcat(out,e->transduction);
}
u_strcpy(pile,flechi);
u_strcpy(etiq,e->contenu);
if (u_strcmp_char(etiq,"<E>")) {
    // if the tag is not <E>, we process it
    for (pos_etiq=0;etiq[pos_etiq]!='\0';) {
        switch (etiq[pos_etiq]) {
            case 'L':   {
                            if (pos!=0) {
                                // if the stack is not empty, we decrease the
                                // stack pointer
                                pos--;
                            }
                            pos_etiq++;
                        };break;
            case 'R':   {
                            pos++;
                            pos_etiq++;
                        };break;
            default:    {
                            pile[pos++]=etiq[pos_etiq++];
                        };break;
        }
    }
}
// then, we go the next state
pile[pos]='\0';
explore_state(pile,canonique,out,a,T->arr,code_gramm);
}



//
// explore the transducer a
//
void explore_state(unichar* flechi,unichar* canonique,unichar* sortie,
                   Automate_fst2* a,int etat_courant,unichar* code_gramm) {
Etat_fst e=a->etat[etat_courant];
if (e->controle & 1) {
    // if we are in a final state, we save the computed things
    u_fprints(flechi,f_out);
    u_fprints_char(",",f_out);
    u_fprints(canonique,f_out);
    u_fprints_char(".",f_out);
    u_fprints(code_gramm,f_out);
    if (ADD_TWO_POINTS && sortie[0]!=':') {
       u_fprints_char(":",f_out);    
    }
    u_fprints(sortie,f_out);
    u_fprints_char("\n",f_out);
}
struct transition_fst* t=e->trans;
while (t!=NULL) {
    explore_tag(t,flechi,canonique,sortie,a,code_gramm);
    t=t->suivant;
}
}



//
// explore the tag of the transition T
//
void explore_tag_recursion(struct transition_fst* T,unichar* flechi,unichar* canonique,unichar* sortie,
                 Automate_fst2* a,struct couple_string** LISTE,unichar* code_gramm) {
if (T->etiquette < 0) {
   // if we are in the case of a call to a sub-graph
   struct couple_string* L=NULL;
   struct couple_string* temp;
   explore_state_recursion(flechi,canonique,sortie,a,a->debut_graphe_fst2[-(T->etiquette)],&L,code_gramm);
   while (L!=NULL) {
      explore_state_recursion(L->flechi,canonique,L->out,a,T->arr,LISTE,code_gramm);
      temp=L;
      L=L->suivant;
      free(temp);
   }
   return;
}
Etiquette e=a->etiquette[T->etiquette];
int pos=u_strlen(flechi);
unichar out[1000];
unichar pile[1000];
unichar etiq[1000];
int pos_etiq;
u_strcpy(out,sortie);
if (u_strcmp_char(e->transduction,"<E>")) {
   u_strcat(out,e->transduction);
}
u_strcpy(pile,flechi);
u_strcpy(etiq,e->contenu);
if (u_strcmp_char(etiq,"<E>")) {
    // if the tag is not <E>, we process it
    for (pos_etiq=0;etiq[pos_etiq]!='\0';) {
        switch (etiq[pos_etiq]) {
            case 'L':   {
                            if (pos!=0) {
                                // if the stack is not empty, we decrease the
                                // stack pointer
                                pos--;
                            }
                            pos_etiq++;
                        };break;
            case 'R':   {
                            pos++;
                            pos_etiq++;
                        };break;
            default:    {
                            pile[pos++]=etiq[pos_etiq++];
                        };break;
        }
    }
}
// then, we go the next state
pile[pos]='\0';
explore_state_recursion(pile,canonique,out,a,T->arr,LISTE,code_gramm);
}



//
// explore the sub-transducer a
//
void explore_state_recursion(unichar* flechi,unichar* canonique,unichar* sortie,
                   Automate_fst2* a,int etat_courant,struct couple_string** L,
                   unichar* code_gramm) {
Etat_fst e=a->etat[etat_courant];
if (e->controle & 1) {
    // if we are in a final state, we save the computed things
    struct couple_string* res=(struct couple_string*)malloc(sizeof(struct couple_string));
    u_strcpy(res->flechi,flechi);
    u_strcpy(res->out,sortie);
    res->suivant=(*L);
    (*L)=res;
}
struct transition_fst* t=e->trans;
while (t!=NULL) {
    explore_tag_recursion(t,flechi,canonique,sortie,a,L,code_gramm);
    t=t->suivant;
}
}


