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
#include "Minimize_tree.h"
//---------------------------------------------------------------------------


void minimize_tree(struct arbre_dico* racine) {
printf("Minimizing...                      \n%s",CR);
init_tab_by_hauteur();
int H;
H=sort_by_height(racine);
float z;
for (int k=0;k<=H;k++) {
   int size=convert_list_to_array(k);
   quicksort(0,size-1);
   fusionner(size);
   z=100.0*(float)(k)/(float)H;
   if (z>100.0) z=100.0;
   printf("%2.0f%% completed...    %s",z,CR);
}
printf("Minimization done.                     \n%s",CR);
free_tab_by_hauteur();
}



//
// this function compares two tree nodes as follow:
// 1) by the unichar that lead to them
// 2) by their hash_number (n° of line in INF file)
// 3) by the transition that get out of them
//
int compare_nodes(struct arbre_dico_trans* a,struct arbre_dico_trans* b) {
if (a==NULL || b==NULL || a->noeud==NULL || b->noeud==NULL) {
   fprintf(stderr,"Probleme dans compares_nodes\n");
   getchar();
   exit(1);
}
// then, the hash numbers
if (a->noeud->arr!=NULL && b->noeud->arr==NULL) return -1;
if (a->noeud->arr==NULL && b->noeud->arr!=NULL) return 1;

if (a->noeud->arr!=NULL && b->noeud->arr!=NULL &&
/*if (*/a->noeud->hash_number != b->noeud->hash_number)
   return (a->noeud->hash_number - b->noeud->hash_number);
// and finally the transitions
a=a->noeud->trans;
b=b->noeud->trans;
while(a!=NULL && b!=NULL) {
   // if the unichars are different then nodes are different
   if (a->c != b->c) return (a->c - b->c);
   // if the unichars are equal and destination nodes are different...
   if (a->noeud != b->noeud) return (a->noeud - b->noeud);
   a=a->suivant;
   b=b->suivant;
}
if (a==NULL && b==NULL) {
   // if the transition lists are equal
   return 0;
}
if (a==NULL) {
   // if the first list is shorter than the second
   return -1;
}
// if the first list is longuer then the second
return 1;
}



void init_tab_by_hauteur() {
int i;
for (i=0;i<HAUTEUR_MAX;i++) {
   tab_by_hauteur[i]=NULL;
}
}



void free_node_list(struct node_list* l) {
struct node_list* ptr;
while (l!=NULL) {
   ptr=l;
   l=l->suivant;
   free(ptr);
}
}



void free_tab_by_hauteur() {
int i;
for (i=0;i<HAUTEUR_MAX;i++) {
   free_node_list(tab_by_hauteur[i]);
}
}



int sort_by_height(struct arbre_dico* n) {
if (n==NULL) {
   fprintf(stderr,"Probleme dans sort_by_height\n");
   exit(1);
}
if (n->trans==NULL) {
   // if the node is a leaf
   return 0;
}
struct arbre_dico_trans* trans=n->trans;
int res=-1;
int k;
while (trans!=NULL) {
   k=sort_by_height(trans->noeud);
   if (k>res) res=k;
   insert_trans_in_tab_by_hauteur(trans,k);
   trans=trans->suivant;
}
return 1+res;
}



void insert_trans_in_tab_by_hauteur(struct arbre_dico_trans* trans,int k) {
struct node_list* tmp;
tmp=new_node_list();
tmp->trans=trans;
tmp->suivant=tab_by_hauteur[k];
tab_by_hauteur[k]=tmp;
}



struct node_list* new_node_list() {
struct node_list* n;
n=(struct node_list*)malloc(sizeof(struct node_list));
n->trans=NULL;
n->suivant=NULL;
return n;
}



int convert_list_to_array(int k) {
int size=0;
struct node_list* l=tab_by_hauteur[k];
struct node_list* ptr;
tab_by_hauteur[k]=NULL;
while (l!=NULL) {
   tab_trans[size++]=l->trans;
   ptr=l;
   l=l->suivant;
   free(ptr);
}
return size;
}




//
// on partitionne le tableau t
//
int partition_pour_quicksort(int m, int n) {
struct arbre_dico_trans* pivot;
struct arbre_dico_trans* tmp;
int i = m-1;
int j = n+1;         // indice final du pivot
pivot=tab_trans[(m+n)/2];
while (true) {
  do j--;
  while ((j>(m-1))&&(compare_nodes(pivot,tab_trans[j]) < 0));
  do i++;
  while ((i<n+1)&&(compare_nodes(tab_trans[i],pivot) < 0));
  if (i<j) {
    tmp=tab_trans[i];
    tab_trans[i]=tab_trans[j];
    tab_trans[j]=tmp;
  } else return j;
}
}



void quicksort(int debut,int fin) {
int p;
if (debut<fin) {
  p=partition_pour_quicksort(debut,fin);
  quicksort(debut,p);
  quicksort(p+1,fin);
}
}



void fusionner(int size) {
int i=1;
struct arbre_dico_trans* base=tab_trans[0];
while (i<size) {
   if (compare_nodes(base,tab_trans[i])==0) {
      // if the base trans is equivalent to the current one
      // then we must destroy the current one's destination node
      //free_arbre_dico(tab_trans[i]->noeud);
      // ans modify the current one's destination node
      tab_trans[i]->noeud=base->noeud;
      i++;
   }
   else {
      base=tab_trans[i];
      i++;
   }
}
}

