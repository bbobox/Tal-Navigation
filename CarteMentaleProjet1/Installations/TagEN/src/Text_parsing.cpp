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
#include "Text_parsing.h"
//---------------------------------------------------------------------------



void block_change(FILE* f) {
int i;
for (i=origine_courante;i<BUFFER_SIZE;i++) {
  // first, we copy the end of the buffer at the beginning
  texte[i-origine_courante]=texte[i];
}
N_INT_ALLREADY_READ=N_INT_ALLREADY_READ+origine_courante;
int N=BUFFER_SIZE-origine_courante;
int l=fread(texte+N,sizeof(int),origine_courante,f);
origine_courante=0;
LENGTH=N+l;
}



void launch_locate(FILE* f,Automate_fst2* automate,int mode,struct string_hash* tok,FILE* out,
                   int output_mode,long int text_size,FILE* info) {
LENGTH=fread(texte,sizeof(int),BUFFER_SIZE,f);
statut_match=mode;
transduction_mode=output_mode;
int debut=automate->debut_graphe_fst2[1];
debut_graphe=automate->debut_graphe_fst2;
N_INT_ALLREADY_READ=0;
origine_courante=0;
TOKENS=tok;
ETIQUETTE=automate->etiquette;
int n_read;
while (origine_courante<LENGTH && nombre_match!=SEARCH_LIMITATION) {
   if (LENGTH==BUFFER_SIZE && origine_courante>(LENGTH-2000)) {
      // if must change of block
      block_change(f);
   }
   if ((text_size/100)!=0) {
      n_read=((origine_courante+N_INT_ALLREADY_READ)%(text_size/100));
      if (n_read==0) {
         printf("%2.0f%% done        %s",100.0*(float)(N_INT_ALLREADY_READ+origine_courante)/(float)text_size,CR);
      }
   }
   if (!(texte[origine_courante]==ESPACE && GESTION_DE_L_ESPACE==MODE_NON_MORPHO)) {
      StackPointer=0;
      parcourir_initial_opt(debut,0,0);
   }
   liste_match=ecrire_index_des_matches(liste_match,N_INT_ALLREADY_READ+origine_courante,&nombre_unites_reconnues,out);
   origine_courante++;
}
printf("100%% done      \n\n%s",CR);
printf("%d match%s\n%s",nombre_match,(nombre_match<=1)?"":"es",CR);
printf("%d recognized units\n%s",nombre_unites_reconnues,CR);
printf("(%2.3f%% of the text is covered)\n%s",((float)nombre_unites_reconnues*100.0)/text_size,CR);
if (info!=NULL) {
   char tmp[3000];
   unichar unitmp[3000];
   sprintf(tmp,"%d match%s",nombre_match,(nombre_match<=1)?"":"es");
   u_strcpy_char(unitmp,tmp);
   u_strcat_char(unitmp,"\n");
   sprintf(tmp,"%d recognized units",nombre_unites_reconnues);
   u_strcat_char(unitmp,tmp);
   u_strcat_char(unitmp,"\n");
   sprintf(tmp,"(%2.3f%% of the text is covered)",((float)nombre_unites_reconnues*100.0)/text_size);
   u_strcat_char(unitmp,tmp);
   u_strcat_char(unitmp,"\n");
   u_fprints(unitmp,info);
}
}






void parcourir_initial_opt(int e,int pos,int profondeur) {
Etat_opt etat_courant;
int pos2,k,pos3,pos4,k_pas_decale;
//int tmp[NBRE_ARR_MAX];
struct liste_arrivees* arr;
struct appel_a_sous_graphe* a_sous_graphe;
struct appel_a_meta* a_meta;
struct appel_a_pattern* a_pattern;
int SOMMET=StackPointer;
unichar* sortie;

if ((profondeur++)>TAILLE_PILE) {
  return;
}
etat_courant=graphe_opt[e];
// si l'etat courant est final...
if (etat_courant->controle&1) {
   if (transduction_mode==IGNORE_TRANSDUCTIONS) {
      if (pos>0)
         afficher_match_fst2(pos+origine_courante+N_INT_ALLREADY_READ-1,NULL);
      else afficher_match_fst2(pos+origine_courante+N_INT_ALLREADY_READ,NULL);
   } else {
      stack[SOMMET]='\0';
      if (pos>0)
         afficher_match_fst2(pos+origine_courante+N_INT_ALLREADY_READ-1,stack);
      else afficher_match_fst2(pos+origine_courante+N_INT_ALLREADY_READ,stack);
   }
}

if (pos+origine_courante>LENGTH) return;

// sous-graphes
a_sous_graphe=etat_courant->liste_sous_graphes;
{ // this block is used to desallocate old_stack after computing sub-graphs
unichar old_stack[STACK_SIZE];
u_strcpy(old_stack,stack);
while (a_sous_graphe!=NULL) {
  arr=a_sous_graphe->liste_arr;
  while (arr!=NULL) {
    //parcourir_opt(debut_graphe[a_sous_graphe->numero_de_graphe],pos,profondeur,tmp,&n_matches);
    struct liste_num* L=NULL;
    parcourir_opt(debut_graphe[a_sous_graphe->numero_de_graphe],pos,profondeur,&L,0/*tmp,&n_matches*/);
    /*for (i=0;i<n_matches;i++)
      parcourir_initial_opt(arr->arr,tmp[i],profondeur);
    */
    while (L!=NULL) {
       u_strcpy(stack,L->pile);
       StackPointer=L->sommet;
       parcourir_initial_opt(arr->arr,L->n,profondeur);
       u_strcpy(stack,old_stack);
       StackPointer=SOMMET;
       struct liste_num* l_tmp=L;
       L=L->suivant;
       free(l_tmp);
    }
    arr=arr->suivant;
  }
  a_sous_graphe=a_sous_graphe->suivant;
}
}

// gestion de l'espace
if (texte[pos+origine_courante]==ESPACE) {
   pos2=pos+1;
}
else pos2=pos;
if (pos2+origine_courante>LENGTH) return;
pos4=pos2+1;

// metas
a_meta=etat_courant->liste_metas;
if (a_meta!=NULL) {
  k=index_controle[texte[pos2+origine_courante]];
  k_pas_decale=index_controle[texte[pos+origine_courante]];
}
while (a_meta!=NULL) {
  arr=a_meta->liste_arr;
  while (arr!=NULL) {
    sortie=ETIQUETTE[arr->etiquette_origine]->transduction;
    switch (a_meta->numero_de_meta) {
      case SPACE_TAG:if (texte[pos+origine_courante]==ESPACE) {
                        if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                        if (transduction_mode==MERGE_TRANSDUCTIONS) push_char(' ');
                        parcourir_initial_opt(arr->arr,pos+1,profondeur);
                        StackPointer=SOMMET;
                      }
                      break;
      case DIESE: if (texte[pos+origine_courante]!=ESPACE) {
                     if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                     parcourir_initial_opt(arr->arr,pos,profondeur);
                     StackPointer=SOMMET;
                  }
                  break;
      case EPSILON: if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                    parcourir_initial_opt(arr->arr,pos,profondeur);
                    StackPointer=SOMMET;
                    break;
      case VAR_START: {//int old=get_variable_start(a_meta->numero_de_variable);
                      set_variable_start(a_meta->numero_de_variable,pos2);
                      if (transduction_mode==MERGE_TRANSDUCTIONS) {
                         if (pos2!=pos) push_char(' ');
                      }
                      parcourir_initial_opt(arr->arr,pos2,profondeur);
                      StackPointer=SOMMET;
                      //set_variable_start(a_meta->numero_de_variable,old);
                      }
                      break;
      case VAR_END: {//int old=get_variable_end(a_meta->numero_de_variable);
                      set_variable_end(a_meta->numero_de_variable,pos-1);
                      if (transduction_mode==MERGE_TRANSDUCTIONS) {
                         if (pos2!=pos) push_char(' ');
                      }
                      parcourir_initial_opt(arr->arr,pos2,profondeur);
                      StackPointer=SOMMET;
                      //set_variable_end(a_meta->numero_de_variable,old);
                      }
                      break;
      case NB: {     int z=pos2;
                     while (is_a_digit_token(TOKENS->tab[texte[z+origine_courante]])) z++;
                     if (z!=pos2) {
                        if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                        if (transduction_mode==MERGE_TRANSDUCTIONS) {
                           if (pos2!=pos) push_char(' ');
                           for (int y=pos2;y<z;y++) {
                              push_string(TOKENS->tab[texte[y+origine_courante]]);
                           }
                        }
                        parcourir_initial_opt(arr->arr,z,profondeur);
                        StackPointer=SOMMET;
                     }
               }
               break;
      case MOT: if ((GESTION_DE_L_ESPACE==MODE_MORPHO)&&(pos2!=pos)&&(!(k_pas_decale&1))
                    &&(a_meta->negation)) {
                   // on est dans le cas ou on veut attraper l'espace avec <!MOT>
                   if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                   if (transduction_mode==MERGE_TRANSDUCTIONS) {
                      if (pos2!=pos) push_char(' ');
                   }
                   parcourir_initial_opt(arr->arr,pos+1,profondeur);
                   StackPointer=SOMMET;
                }
                else if (((k&1)&&!(a_meta->negation))
                    ||((!(k&1))&&(a_meta->negation)&&(texte[pos2+origine_courante]!=SENTENCE_DELIMITER_INDICE))) {
                   if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                   if (transduction_mode==MERGE_TRANSDUCTIONS) {
                      if (pos2!=pos) push_char(' ');
                      push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                   }
                   parcourir_initial_opt(arr->arr,pos4,profondeur);
                   StackPointer=SOMMET;
                }
                break;
      case DIC: if (!(a_meta->negation)) {
                  pos3=trouver_mot_compose_DIC(pos2,-555);
                  if (pos3!=-1) {
                     if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                     if (transduction_mode==MERGE_TRANSDUCTIONS) {
                        if (pos2!=pos) push_char(' ');
                        for (int x=pos2;x<=pos3;x++)
                          push_string(TOKENS->tab[texte[x+origine_courante]]);
                     }
                     parcourir_initial_opt(arr->arr,pos3+1,profondeur);
                     StackPointer=SOMMET;
                  }
                  if (k&2) {
                     if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                     if (transduction_mode==MERGE_TRANSDUCTIONS) {
                        if (pos2!=pos) push_char(' ');
                        push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                     }
                     parcourir_initial_opt(arr->arr,pos4,profondeur);
                     StackPointer=SOMMET;
                     break;
                  }
      			}
                else { if ((!(k&2))&&(k&1)) {
                          if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                          if (transduction_mode==MERGE_TRANSDUCTIONS) {
                             if (pos2!=pos) push_char(' ');
                             push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                          }
                          parcourir_initial_opt(arr->arr,pos4,profondeur);
                          StackPointer=SOMMET;
                       }
                       break;
                } break;
      case SDIC: if ((k&2) && !(k&32)) {
                     if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                     if (transduction_mode==MERGE_TRANSDUCTIONS) {
                        if (pos2!=pos) push_char(' ');
                        push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                     }
                     parcourir_initial_opt(arr->arr,pos4,profondeur);
                     StackPointer=SOMMET;
                  }
                  break;
      case CDIC: pos3=trouver_mot_compose_DIC(pos2,-555);
                  if (pos3!=-1) {
                     if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                     if (transduction_mode==MERGE_TRANSDUCTIONS) {
                        if (pos2!=pos) push_char(' ');
                        for (int x=pos2;x<=pos3;x++)
                          push_string(TOKENS->tab[texte[x+origine_courante]]);
                     }
                     parcourir_initial_opt(arr->arr,pos3+1,profondeur);
                     StackPointer=SOMMET;
                  }
                  if (k&32) {
                     if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                     if (transduction_mode==MERGE_TRANSDUCTIONS) {
                        if (pos2!=pos) push_char(' ');
                        push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                     }
                     parcourir_initial_opt(arr->arr,pos4,profondeur);
                     StackPointer=SOMMET;
                     break;
                  }
                 break;
      case MAJ: if (!(a_meta->negation)) {
                  if (k&4) {
                     if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                     if (transduction_mode==MERGE_TRANSDUCTIONS) {
                        if (pos2!=pos) push_char(' ');
                        push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                     }
                     parcourir_initial_opt(arr->arr,pos4,profondeur);
                     StackPointer=SOMMET;
                     break;
                  }
      			}
                else {if ((!(k&4))&&(k&1)) {
                         if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                         if (transduction_mode==MERGE_TRANSDUCTIONS) {
                             if (pos2!=pos) push_char(' ');
                             push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                         }
                         parcourir_initial_opt(arr->arr,pos4,profondeur);
                         StackPointer=SOMMET;
                      }
                      break;
                } break;
      case MIN: if (!(a_meta->negation)) {
                  if (k&8) {
                     if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                     if (transduction_mode==MERGE_TRANSDUCTIONS) {
                        if (pos2!=pos) push_char(' ');
                        push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                     }
                     parcourir_initial_opt(arr->arr,pos4,profondeur);
                     StackPointer=SOMMET;
                     break;
                  }
      			}
                else {if ((!(k&8))&&(k&1)) {
                         if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                         if (transduction_mode==MERGE_TRANSDUCTIONS) {
                             if (pos2!=pos) push_char(' ');
                             push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                         }
                         parcourir_initial_opt(arr->arr,pos4,profondeur);
                         StackPointer=SOMMET;
                      }
                      break;
                } break;
      case PRE: if (!(a_meta->negation)) {
                  if (k&16) {
                     if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                     if (transduction_mode==MERGE_TRANSDUCTIONS) {
                        if (pos2!=pos) push_char(' ');
                        push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                     }
                     parcourir_initial_opt(arr->arr,pos4,profondeur);
                     StackPointer=SOMMET;
                     break;
                  }
      			}
                else {if ((!(k&16))&&(k&1)) {
                         if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                         if (transduction_mode==MERGE_TRANSDUCTIONS) {
                             if (pos2!=pos) push_char(' ');
                             push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                         }
                         parcourir_initial_opt(arr->arr,pos4,profondeur);
                         StackPointer=SOMMET;
                      }
                      break;
                } break;
    }
    arr=arr->suivant;
  }
  a_meta=a_meta->suivant;
}

// patterns composes
a_pattern=etat_courant->liste_patterns_composes;
while (a_pattern!=NULL) {
  arr=a_pattern->liste_arr;
  while (arr!=NULL) {
    sortie=ETIQUETTE[arr->etiquette_origine]->transduction;
    k=a_pattern->numero_de_pattern;
    pos3=trouver_mot_compose_DIC(pos2,k);
    if (pos3!=-1 && !(a_pattern->negation)) {
       if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
       if (transduction_mode==MERGE_TRANSDUCTIONS) {
          if (pos2!=pos) push_char(' ');
          for (int x=pos2;x<=pos3;x++)
            push_string(TOKENS->tab[texte[x+origine_courante]]);
       }
       parcourir_initial_opt(arr->arr,pos3+1,profondeur);
       StackPointer=SOMMET;
    }
    arr=arr->suivant;
  }
  a_pattern=a_pattern->suivant;
}


// patterns simples
a_pattern=etat_courant->liste_patterns;
while (a_pattern!=NULL) {
  arr=a_pattern->liste_arr;
  while (arr!=NULL) {
    sortie=ETIQUETTE[arr->etiquette_origine]->transduction;
    k=a_pattern->numero_de_pattern;
      //---mots composes
      pos3=trouver_mot_compose_DIC(pos2,k);
      if (pos3!=-1 && !(a_pattern->negation)) {
         if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
         if (transduction_mode==MERGE_TRANSDUCTIONS) {
            if (pos2!=pos) push_char(' ');
            for (int x=pos2;x<=pos3;x++)
              push_string(TOKENS->tab[texte[x+origine_courante]]);
         }
         parcourir_initial_opt(arr->arr,pos3+1,profondeur);
         StackPointer=SOMMET;
      }
      //---mots simples
    if (index_code_gramm[texte[pos2+origine_courante]]!=NULL) {
        if ((index_code_gramm[texte[pos2+origine_courante]][k/8]&(1<<(k%8)) && !a_pattern->negation)
            || (!(index_code_gramm[texte[pos2+origine_courante]][k/8]&(1<<(k%8))) && a_pattern->negation)) {
           if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
           if (transduction_mode==MERGE_TRANSDUCTIONS) {
              if (pos2!=pos) push_char(' ');
              push_string(TOKENS->tab[texte[pos2+origine_courante]]);
           }
           parcourir_initial_opt(arr->arr,pos4,profondeur);
           StackPointer=SOMMET;
        }
    } else {
        // if there is no code, we can try to look for a negation
        if (a_pattern->negation && (index_controle[texte[pos2+origine_courante]]&1)) {
           if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
              if (transduction_mode==MERGE_TRANSDUCTIONS) {
                 if (pos2!=pos) push_char(' ');
                 push_string(TOKENS->tab[texte[pos2+origine_courante]]);
              }
              parcourir_initial_opt(arr->arr,pos4,profondeur);
              StackPointer=SOMMET;
        }
    }
    arr=arr->suivant;
  }
  a_pattern=a_pattern->suivant;
}

// tokens
if (etat_courant->nombre_de_tokens!=0) {
  k=dichotomie(texte[pos2+origine_courante],etat_courant->tableau_de_tokens,etat_courant->nombre_de_tokens);
  if (k!=-1) {
    arr=etat_courant->tableau_liste_arr[k];
    while (arr!=NULL) {
      sortie=ETIQUETTE[arr->etiquette_origine]->transduction;
      if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
      if (transduction_mode==MERGE_TRANSDUCTIONS) {
         if (pos2!=pos) push_char(' ');
         push_string(TOKENS->tab[texte[pos2+origine_courante]]);
      }
      parcourir_initial_opt(arr->arr,pos4,profondeur);
      StackPointer=SOMMET;
      arr=arr->suivant;
    }
  }
}
}



int appartient_deja_aux_matches(int t[],int max,int n) {
int i;
for (i=0;i<max;i++)
  if (t[i]==n) return 1;
return 0;
}



void parcourir_opt(int e,int pos,int profondeur,struct liste_num** L,int n_matches/*int retours[],int *last*/) {
Etat_opt etat_courant;
int pos2,k,pos3,pos4,k_pas_decale;
/*int tmp[NBRE_ARR_MAX];
int i;*/
struct liste_arrivees* arr;
struct appel_a_sous_graphe* a_sous_graphe;
struct appel_a_meta* a_meta;
struct appel_a_pattern* a_pattern;
int SOMMET=StackPointer;
unichar* sortie;

if ((profondeur++)>TAILLE_PILE) {
  return;
}
etat_courant=graphe_opt[e];
// si l'etat courant est final...
if (etat_courant->controle&1) {
  if (/**last*/n_matches==(NBRE_ARR_MAX-1)) {
    //erreur("Infinite recognition");
    // BUGFIX (HT,JD, LIPN/P13: r�cursion infinie)
    fprintf(stderr,"Unitex WARNING: has entered infinite recursion loop - forcing unfold\n");
    exit(-1);
    return;
  }
  else {
    (*L)=inserer_si_absent(pos,(*L),StackPointer,stack);
    n_matches++;
  }
}

if (pos+origine_courante>LENGTH) return;

// sous-graphes
a_sous_graphe=etat_courant->liste_sous_graphes;
{
unichar old_stack[STACK_SIZE];
u_strcpy(old_stack,stack);
while (a_sous_graphe!=NULL) {
  arr=a_sous_graphe->liste_arr;
  while (arr!=NULL) {
    struct liste_num* LISTE=NULL;
    parcourir_opt(debut_graphe[a_sous_graphe->numero_de_graphe],pos,profondeur,&LISTE,0/*tmp,&n_matches*/);
    /*for (i=0;i<n_matches;i++) {
      parcourir_opt(arr->arr,tmp[i],profondeur,retours,&(*last));
    }*/
    while (LISTE!=NULL) {
       u_strcpy(stack,LISTE->pile);
       StackPointer=LISTE->sommet;
       parcourir_opt(arr->arr,LISTE->n,profondeur,L,n_matches);
       u_strcpy(stack,old_stack);
       StackPointer=SOMMET;
       struct liste_num* l_tmp=LISTE;
       LISTE=LISTE->suivant;
       free(l_tmp);
    }
    arr=arr->suivant;
  }
  a_sous_graphe=a_sous_graphe->suivant;
}
u_strcpy(stack,old_stack);
StackPointer=SOMMET;
}

// gestion de l'espace
if (texte[pos+origine_courante]==ESPACE)
  pos2=pos+1;
else pos2=pos;
if (pos2+origine_courante>LENGTH) return;
pos4=pos2+1;

// metas
a_meta=etat_courant->liste_metas;
if (a_meta!=NULL) {
  k=index_controle[texte[pos2+origine_courante]];
  k_pas_decale=index_controle[texte[pos+origine_courante]];
}
while (a_meta!=NULL) {
  arr=a_meta->liste_arr;
  while (arr!=NULL) {
    sortie=ETIQUETTE[arr->etiquette_origine]->transduction;
    switch (a_meta->numero_de_meta) {
      case SPACE_TAG: if (texte[pos+origine_courante]==ESPACE) {
                         if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                         if (transduction_mode==MERGE_TRANSDUCTIONS) push_char(' ');
                         parcourir_opt(arr->arr,pos+1,profondeur,L,n_matches/*retours,&(*last)*/);
                         StackPointer=SOMMET;
                      }
                      break;
      case DIESE: if (texte[pos+origine_courante]!=ESPACE) {
                     if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                     parcourir_opt(arr->arr,pos,profondeur,L,n_matches/*retours,&(*last)*/);
                     StackPointer=SOMMET;
                  }
                  break;
      case EPSILON: if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                    parcourir_opt(arr->arr,pos,profondeur,L,n_matches/*retours,&(*last)*/);
                    StackPointer=SOMMET;
                    break;
      case VAR_START: {//int old=get_variable_start(a_meta->numero_de_variable);
                      if (transduction_mode==MERGE_TRANSDUCTIONS) {
                         if (pos2!=pos) push_char(' ');
                      }
                      set_variable_start(a_meta->numero_de_variable,pos2);
                      parcourir_opt(arr->arr,pos2,profondeur,L,n_matches/*retours,&(*last)*/);
                      StackPointer=SOMMET;
                      //set_variable_start(a_meta->numero_de_variable,old);
                      }
                      break;
      case VAR_END: {//int old=get_variable_end(a_meta->numero_de_variable);
                      if (transduction_mode==MERGE_TRANSDUCTIONS) {
                         if (pos2!=pos) push_char(' ');
                      }
                      set_variable_end(a_meta->numero_de_variable,pos-1);
                      parcourir_opt(arr->arr,pos2,profondeur,L,n_matches/*retours,&(*last)*/);
                      StackPointer=SOMMET;
                      //set_variable_end(a_meta->numero_de_variable,old);
                      }
                      break;
      case NB: {     int z=pos2;
                     while (is_a_digit_token(TOKENS->tab[texte[z+origine_courante]])) z++;
                     if (z!=pos2) {
                        if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                        if (transduction_mode==MERGE_TRANSDUCTIONS) {
                           if (pos2!=pos) push_char(' ');
                           for (int y=pos2;y<z;y++) {
                              push_string(TOKENS->tab[texte[y+origine_courante]]);
                           }
                        }
                        parcourir_opt(arr->arr,z,profondeur,L,n_matches);
                        StackPointer=SOMMET;
                     }
               }
               break;
      case MOT: if ((GESTION_DE_L_ESPACE==MODE_MORPHO)&&(pos2!=pos)&&(!(k_pas_decale&1))
                    &&(a_meta->negation)) {
                   // on est dans le cas ou on veut attraper l'espace avec <!MOT>
                   if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                   if (transduction_mode==MERGE_TRANSDUCTIONS) {
                      if (pos2!=pos) push_char(' ');
                   }
                   parcourir_opt(arr->arr,pos+1,profondeur,L,n_matches/*retours,&(*last)*/);
                   StackPointer=SOMMET;
                }
                if (((k&1)&&!(a_meta->negation))
                    ||((!(k&1))&&(a_meta->negation))) {
                   if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                   if (transduction_mode==MERGE_TRANSDUCTIONS) {
                      if (pos2!=pos) push_char(' ');
                      push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                   }
                   parcourir_opt(arr->arr,pos4,profondeur,L,n_matches/*retours,&(*last)*/);
                   StackPointer=SOMMET;
                   break;
                }
      case DIC: if (!(a_meta->negation)) {
                  pos3=trouver_mot_compose_DIC(pos2,-555);
                  if (pos3!=-1) {
                     if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                     if (transduction_mode==MERGE_TRANSDUCTIONS) {
                        if (pos2!=pos) push_char(' ');
                        for (int x=pos2;x<=pos3;x++)
                          push_string(TOKENS->tab[texte[x+origine_courante]]);
                     }
                     parcourir_opt(arr->arr,pos3+1,profondeur,L,n_matches/*retours,&(*last)*/);
                     StackPointer=SOMMET;
                  }
                  if (k&2) {
                     if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                     if (transduction_mode==MERGE_TRANSDUCTIONS) {
                        if (pos2!=pos) push_char(' ');
                        push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                     }
                     parcourir_opt(arr->arr,pos4,profondeur,L,n_matches/*retours,&(*last)*/);
                     StackPointer=SOMMET;
                     break;
                  }
      			}
                else {if ((!(k&2))&&(k&1)) {
                         if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                         if (transduction_mode==MERGE_TRANSDUCTIONS) {
                            if (pos2!=pos) push_char(' ');
                            push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                         }
                         parcourir_opt(arr->arr,pos4,profondeur,L,n_matches/*retours,&(*last)*/);
                         StackPointer=SOMMET;
                         break;
                      }
                } break;
      case SDIC: if ((k&2) && !(k&32)) {
                     if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                     if (transduction_mode==MERGE_TRANSDUCTIONS) {
                        if (pos2!=pos) push_char(' ');
                        push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                     }
                     parcourir_opt(arr->arr,pos4,profondeur,L,n_matches/*retours,&(*last)*/);
                     StackPointer=SOMMET;
                  }
                  break;
      case CDIC: pos3=trouver_mot_compose_DIC(pos2,-555);
                  if (pos3!=-1) {
                     if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                     if (transduction_mode==MERGE_TRANSDUCTIONS) {
                        if (pos2!=pos) push_char(' ');
                        for (int x=pos2;x<=pos3;x++)
                          push_string(TOKENS->tab[texte[x+origine_courante]]);
                     }
                     parcourir_opt(arr->arr,pos3+1,profondeur,L,n_matches/*retours,&(*last)*/);
                     StackPointer=SOMMET;
                  }
                  if (k&32) {
                     if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                     if (transduction_mode==MERGE_TRANSDUCTIONS) {
                        if (pos2!=pos) push_char(' ');
                        push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                     }
                     parcourir_opt(arr->arr,pos4,profondeur,L,n_matches/*retours,&(*last)*/);
                     StackPointer=SOMMET;
                     break;
                  }
                 break;
      case MAJ: if (!(a_meta->negation)) {
                  if (k&4) {
                     if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                     if (transduction_mode==MERGE_TRANSDUCTIONS) {
                        if (pos2!=pos) push_char(' ');
                        push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                     }
                     parcourir_opt(arr->arr,pos4,profondeur,L,n_matches/*retours,&(*last)*/);
                     StackPointer=SOMMET;
                     break;
                  }
      			}
                else {if ((!(k&4))&&(k&1)) {
                         if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                         if (transduction_mode==MERGE_TRANSDUCTIONS) {
                             if (pos2!=pos) push_char(' ');
                             push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                         }
                         parcourir_opt(arr->arr,pos4,profondeur,L,n_matches/*retours,&(*last)*/);
                         StackPointer=SOMMET;
                         break;
                      }
                } break;
      case MIN: if (!(a_meta->negation)) {
                  if (k&8) {
                     if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                     if (transduction_mode==MERGE_TRANSDUCTIONS) {
                        if (pos2!=pos) push_char(' ');
                        push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                     }
                     parcourir_opt(arr->arr,pos4,profondeur,L,n_matches/*retours,&(*last)*/);
                     StackPointer=SOMMET;
                     break;
                  }
      			}
                else {if ((!(k&8))&&(k&1)) {
                         if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                         if (transduction_mode==MERGE_TRANSDUCTIONS) {
                             if (pos2!=pos) push_char(' ');
                             push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                         }
                         parcourir_opt(arr->arr,pos4,profondeur,L,n_matches/*retours,&(*last)*/);
                         StackPointer=SOMMET;
                         break;
                      }
                } break;
      case PRE: if (!(a_meta->negation)) {
                  if (k&16) {
                     if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                     if (transduction_mode==MERGE_TRANSDUCTIONS) {
                        if (pos2!=pos) push_char(' ');
                        push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                     }
                     parcourir_opt(arr->arr,pos4,profondeur,L,n_matches/*retours,&(*last)*/);
                     StackPointer=SOMMET;
                     break;
                  }
      			}
                else {if ((!(k&16))&&(k&1)) {
                         if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
                         if (transduction_mode==MERGE_TRANSDUCTIONS) {
                             if (pos2!=pos) push_char(' ');
                             push_string(TOKENS->tab[texte[pos2+origine_courante]]);
                         }
                         parcourir_opt(arr->arr,pos4,profondeur,L,n_matches/*retours,&(*last)*/);
                         StackPointer=SOMMET;
                         break;
                      }
                } break;
    }
    arr=arr->suivant;
  }
  a_meta=a_meta->suivant;
}

// patterns composes
a_pattern=etat_courant->liste_patterns_composes;
while (a_pattern!=NULL) {
  arr=a_pattern->liste_arr;
  while (arr!=NULL) {
    sortie=ETIQUETTE[arr->etiquette_origine]->transduction;
    k=a_pattern->numero_de_pattern;
    pos3=trouver_mot_compose_DIC(pos2,k);
    if (pos3!=-1 && !(a_pattern->negation)) {
       if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
       if (transduction_mode==MERGE_TRANSDUCTIONS) {
          if (pos2!=pos) push_char(' ');
          for (int x=pos2;x<=pos3;x++)
            push_string(TOKENS->tab[texte[x+origine_courante]]);
       }
       parcourir_opt(arr->arr,pos3+1,profondeur,L,n_matches/*retours,&(*last)*/);
       StackPointer=SOMMET;
    }
    arr=arr->suivant;
  }
  a_pattern=a_pattern->suivant;
}


// patterns
a_pattern=etat_courant->liste_patterns;
while (a_pattern!=NULL) {
  arr=a_pattern->liste_arr;
  while (arr!=NULL) {
    sortie=ETIQUETTE[arr->etiquette_origine]->transduction;
    k=a_pattern->numero_de_pattern;
      //---mots composes
      pos3=trouver_mot_compose_DIC(pos2,k);
      if (pos3!=-1 && !(a_pattern->negation)) {
         if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
         if (transduction_mode==MERGE_TRANSDUCTIONS) {
            if (pos2!=pos) push_char(' ');
            for (int x=pos2;x<=pos3;x++)
              push_string(TOKENS->tab[texte[x+origine_courante]]);
         }
         parcourir_opt(arr->arr,pos3+1,profondeur,L,n_matches/*retours,&(*last)*/);
         StackPointer=SOMMET;
      }
      //---mots simples
      if (index_code_gramm[texte[pos2+origine_courante]]!=NULL) {
        if ((index_code_gramm[texte[pos2+origine_courante]][k/8]&(1<<(k%8)) && !a_pattern->negation)
            || (!(index_code_gramm[texte[pos2+origine_courante]][k/8]&(1<<(k%8))) && a_pattern->negation)) {
           if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
           if (transduction_mode==MERGE_TRANSDUCTIONS) {
              if (pos2!=pos) push_char(' ');
              push_string(TOKENS->tab[texte[pos2+origine_courante]]);
           }
           parcourir_opt(arr->arr,pos4,profondeur,L,n_matches/*retours,&(*last)*/);
           StackPointer=SOMMET;
        }
      }
      else {
           // if there is no code, we can look for a negation
           if (a_pattern->negation && (index_controle[texte[pos2+origine_courante]]&1)) {
              if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
              if (transduction_mode==MERGE_TRANSDUCTIONS) {
                 if (pos2!=pos) push_char(' ');
                 push_string(TOKENS->tab[texte[pos2+origine_courante]]);
              }
              parcourir_opt(arr->arr,pos4,profondeur,L,n_matches/*retours,&(*last)*/);
              StackPointer=SOMMET;
           }
      }
    arr=arr->suivant;
  }
  a_pattern=a_pattern->suivant;
}

// tokens
if (etat_courant->nombre_de_tokens!=0) {
  k=dichotomie(texte[pos2+origine_courante],etat_courant->tableau_de_tokens,etat_courant->nombre_de_tokens);
  if (k!=-1) {
    arr=etat_courant->tableau_liste_arr[k];
    while (arr!=NULL) {
      sortie=ETIQUETTE[arr->etiquette_origine]->transduction;
      if (transduction_mode!=IGNORE_TRANSDUCTIONS) process_transduction(sortie);
      if (transduction_mode==MERGE_TRANSDUCTIONS) {
         if (pos2!=pos) push_char(' ');
         push_string(TOKENS->tab[texte[pos2+origine_courante]]);
      }
      parcourir_opt(arr->arr,pos4,profondeur,L,n_matches/*retours,&(*last)*/);
      StackPointer=SOMMET;
      arr=arr->suivant;
    }
  }
}
}




int dichotomie(int a,int* t,int n) {
int debut,milieu;
if (n==0||t==NULL) return -1;
if (a<t[0]) return -1;
if (a>t[n-1]) return -1;
n=n-1;
debut=0;
while (debut<=n) {
  milieu=(debut+n)/2;
  if (t[milieu]==a) return milieu;
  if (t[milieu]<a) {
    debut=milieu+1;
  }
  else {
    n=milieu-1;
  }
}
return -1;
}




int trouver_mot_compose(int pos,struct etat_dlc* n,int code) {
int position_max,p,res;
if (n==NULL)
  return -1;
if (-1!=dichotomie(code,n->tab_patterns,n->nombre_patterns))
  position_max=pos-1;
else position_max=-1;
if (pos+origine_courante==LENGTH)
  return position_max;
res=dichotomie(texte[pos+origine_courante],n->tab_token,n->nombre_transitions);
if (res==-1)
  return position_max;
p=trouver_mot_compose(pos+1,n->tab_arr[res],code);
if (p>position_max)
  return p;
else return position_max;
}



int trouver_mot_compose_DIC(int pos,int code) {
int position_max,p,res;
struct etat_dlc *n;

if (pos+origine_courante==LENGTH) {
   return -1;
}
if ((n=tableau_dlc[texte[pos+origine_courante]])==NULL) {
   return -1;
}
if (-1!=dichotomie(code,n->tab_patterns,n->nombre_patterns)) {
   position_max=pos;
}
else position_max=-1;
pos++;
if (pos+origine_courante==LENGTH) {
   return -1;
}
res=dichotomie(texte[pos+origine_courante],n->tab_token,n->nombre_transitions);
if (res==-1) {
   return position_max;
}
p=trouver_mot_compose(pos+1,n->tab_arr[res],code);
if (p>position_max) {
   return p;
}
else return position_max;
}
