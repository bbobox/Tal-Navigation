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
#include "Matches.h"
//---------------------------------------------------------------------------


void afficher_match_fst2(int fin,unichar* s) {
int i;
struct liste_matches *l;
switch (statut_match) {
case LONGUEST_MATCHES: {
                       if (liste_match==NULL) {
                         l=nouveau_match(s);
                         l->debut=origine_courante+N_INT_ALLREADY_READ;
                         l->fin=fin;
                         l->suivant=NULL;
                         liste_match=l;
                       }
                       else if (liste_match->fin<fin) {
                         if (liste_match->debut==origine_courante+N_INT_ALLREADY_READ) {
                           liste_match->fin=fin;
                           if (liste_match->output!=NULL) free(liste_match->output);
                           if (s==NULL) {
                              liste_match->output=NULL;
                           } else {
                              liste_match->output=(unichar*)malloc((1+u_strlen(s))*sizeof(unichar));
                              u_strcpy(liste_match->output,s);
                           }
                         }
                         else {
                           l=nouveau_match(s);
                           l->debut=origine_courante+N_INT_ALLREADY_READ;
                           l->fin=fin;
                           l->suivant=liste_match;
                           liste_match=l;
                         }
                       }
                       break;
				     }
case ALL_MATCHES: {
                  if (liste_match==NULL) {
                    l=nouveau_match(s);
                    l->debut=origine_courante+N_INT_ALLREADY_READ;
                    l->fin=fin;
                    l->suivant=NULL;
                    liste_match=l;
                  }
                  else {
                    l=liste_match;
                    while ((l!=NULL)&&!((l->debut==origine_courante+N_INT_ALLREADY_READ)&&(l->fin==fin)))
                      l=l->suivant;
                    if (l==NULL) {
                      l=nouveau_match(s);
                      l->debut=origine_courante+N_INT_ALLREADY_READ;
                      l->fin=fin;
                      l->suivant=liste_match;
                      liste_match=l;
                    }
                  }
                  break;
                }
case SHORTEST_MATCHES: {
                       if (liste_match==NULL) {
                         l=nouveau_match(s);
                         l->debut=origine_courante+N_INT_ALLREADY_READ;
                         l->fin=fin;
                         l->suivant=NULL;
                         liste_match=l;
                       }
                       else {
                         i=0;
                         liste_match=eliminer_shortest_match_fst2(liste_match,fin,&i);
                         if (i==0) {
                           l=nouveau_match(s);
                           l->debut=origine_courante+N_INT_ALLREADY_READ;
                           l->fin=fin;
                           l->suivant=liste_match;
                           liste_match=l;
                         }
                       }
                       break;
                     }
}
}



struct liste_matches* nouveau_match(unichar* s) {
struct liste_matches *l;
l=(struct liste_matches*)malloc(sizeof(struct liste_matches));
if (s==NULL) {
   l->output=NULL;
} else {
   l->output=(unichar*)malloc(sizeof(unichar)*(1+u_strlen(s)));
   u_strcpy(l->output,s);
}
l->suivant=NULL;
return l;
}



struct liste_matches* eliminer_shortest_match_fst2(struct liste_matches *ptr,
                                              int fin,int *i) {
struct liste_matches *l;
if (ptr==NULL) return NULL;
if ((ptr->debut<=origine_courante+N_INT_ALLREADY_READ)&&(ptr->fin>=fin)) {
  if (*i) {
    l=ptr->suivant;
    free_liste_matches(ptr);
    return eliminer_shortest_match_fst2(l,fin,&(*i));
  } else {
      ptr->debut=origine_courante+N_INT_ALLREADY_READ;
      ptr->fin=fin;
      (*i)=1;
      ptr->suivant=eliminer_shortest_match_fst2(ptr->suivant,fin,&(*i));
      return ptr;
    }
}
else if ((ptr->debut>=origine_courante+N_INT_ALLREADY_READ)&&(ptr->fin<=fin)) {
     (*i)=1;
     return ptr;
     }
     else {
       ptr->suivant=eliminer_shortest_match_fst2(ptr->suivant,fin,&(*i));
       return ptr;
     }
}



struct liste_matches* ecrire_index_des_matches(struct liste_matches *l,int pos,
                                               long int* N,FILE* fichier_match) {
struct liste_matches *ptr;
unichar tmp[100];
if (l==NULL) return NULL;
if (l->fin<pos) {
   // we can save the match
   u_int_to_string(l->debut,tmp);
   u_fprints(tmp,fichier_match);
   u_fprints_char(" ",fichier_match);
   u_int_to_string(l->fin,tmp);
   u_fprints(tmp,fichier_match);
   if (l->output!=NULL) {
      // if there is an output
      u_fprints_char(" ",fichier_match);
      u_fprints(l->output,fichier_match);
   }
   u_fprints_char("\n",fichier_match);
   nombre_match++;
   nombre_unites_reconnues=nombre_unites_reconnues+(l->fin+1)-(l->debut);
   if (nombre_match==SEARCH_LIMITATION) {
      // if we have reached the search limitation, we free the remaining
      // matches and return
      while (l!=NULL) {
         ptr=l;
         l=l->suivant;
         free_liste_matches(ptr);
      }
      return NULL;
   }
   ptr=l->suivant;
   free_liste_matches(l);
   return ecrire_index_des_matches(ptr,pos,N,fichier_match);
}
l->suivant=ecrire_index_des_matches(l->suivant,pos,N,fichier_match);
return l;
}



struct liste_matches* load_match_list(FILE* f,int *TRANDUCTION_MODE) {
struct liste_matches* l=NULL;
struct liste_matches* end_of_list=l;
int c;
int start,end;
unichar output[3000];
char is_an_output;
u_fgetc(f);   // we jump the # char
c=u_fgetc(f);
switch(c) {
  case 'I': *TRANDUCTION_MODE=IGNORE_TRANSDUCTIONS; break;
  case 'M': *TRANDUCTION_MODE=MERGE_TRANSDUCTIONS; break;
  case 'R': *TRANDUCTION_MODE=REPLACE_TRANSDUCTIONS; break;
}
u_fgetc(f);
while ((c=u_fgetc(f))!=EOF) {
   start=0;
   do {
      start=start*10+(c-'0');
   } while (u_is_digit((unichar)(c=u_fgetc(f))));
   c=u_fgetc(f);
   end=0;
   do {
      end=end*10+(c-'0');
   } while (u_is_digit((unichar)(c=u_fgetc(f))));
   if (c==' ') {
      // if we have an output to read
      int i=0;
      while ((c=u_fgetc(f))!='\n') {
         output[i++]=(unichar)c;
      }
      output[i]='\0';
      is_an_output=1;
   } else {
      is_an_output=0;
   }
   if (end_of_list==NULL) {
      if (is_an_output)
         l=nouveau_match(output);
      else l=nouveau_match(NULL);
      l->debut=start;
      l->fin=end;
      end_of_list=l;
   } else {
      if (is_an_output)
         end_of_list->suivant=nouveau_match(output);
      else end_of_list->suivant=nouveau_match(NULL);
      end_of_list->suivant->debut=start;
      end_of_list->suivant->fin=end;
      end_of_list=end_of_list->suivant;
   }
}
return l;
}



void free_liste_matches(struct liste_matches* l) {
if (l->output!=NULL) free(l->output);
free(l);
}

