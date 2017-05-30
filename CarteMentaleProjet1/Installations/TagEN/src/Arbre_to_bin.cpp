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
#include "Arbre_to_bin.h"
#include "Arbre_dico.h"
//---------------------------------------------------------------------------

unsigned char* bin;
int taille;


void numeroter_noeuds(struct arbre_dico* a) {
if (a==NULL) return;
if (a->offset!=-1) {
   return;
}
a->offset=taille;
a->n_trans=0;
taille=taille+2;  // 2 bytes for the number of transitions
if (a->arr!=NULL) {
   // if the node is a final one, we count 3 bytes for the adress of the INF line
   taille=taille+3;
}
struct arbre_dico_trans* tmp;
tmp=a->trans;
while (tmp!=NULL) {
  taille=taille+5; // for each transition, we count 2 bytes for the unichar and 3 bytes for the dest adress
  tmp=tmp->suivant;
  (a->n_trans)++;  // we also count the number of transitions
}
tmp=a->trans;
while (tmp!=NULL) {
  numeroter_noeuds(tmp->noeud);
  tmp=tmp->suivant;
}
}



void remplir_tableau_bin(struct arbre_dico* a) {
if (a==NULL) return;
if (a->hash_number==-1) {
   return;
}
N_STATES++;
unichar n=(unichar)a->n_trans;
if (a->arr==NULL) {
   // if the node is not a final one, we put to 1 the the heaviest bit
   n=(unichar)(n|32768);
}
int pos=a->offset;
// we write the 2 bytes info about the node
bin[pos]=(unsigned char)(n/256);
bin[pos+1]=(unsigned char)(n%256);
pos=pos+2;
if (a->arr!=NULL) {
   int adr=a->hash_number;
   bin[pos++]=(unsigned char)(adr/(256*256));
   adr=adr%(256*256);
   bin[pos++]=(unsigned char)(adr/256);
   adr=adr%256;
   bin[pos++]=(unsigned char)(adr);
}
a->hash_number=-1;
struct arbre_dico_trans* tmp;
tmp=a->trans;
while (tmp!=NULL) {
   N_TRANSITIONS++;
   bin[pos++]=(unsigned char)((tmp->c)/256);
   bin[pos++]=(unsigned char)((tmp->c)%256);
   int adr=tmp->noeud->offset;
   bin[pos++]=(unsigned char)(adr/(256*256));
   adr=adr%(256*256);
   bin[pos++]=(unsigned char)(adr/256);
   adr=adr%256;
   bin[pos++]=(unsigned char)(adr);
   remplir_tableau_bin(tmp->noeud);
   tmp=tmp->suivant;
}
}



void creer_et_sauver_bin(struct arbre_dico* a,char* nom) {
FILE *f;
f=fopen(nom,"wb");
if (f==NULL) {
  fprintf(stderr,"Cannot write automaton file %s\n",nom);
  exit(1);
}
taille=4; // we jump after the 4 bytes describing the automaton size
numeroter_noeuds(a);
bin=(unsigned char*)malloc(taille*sizeof(unsigned char));
if (bin==NULL) {
   fprintf(stderr,"Not enough memory to create the dictionary automaton\n");
   exit(1);
}
int n=taille;
printf("Binary file: %d bytes\n%s",n,CR);
bin[0]=(unsigned char)(n/(256*256*256));
n=n%(256*256*256);
bin[1]=(unsigned char)(n/(256*256));
n=n%(256*256);
bin[2]=(unsigned char)(n/256);
n=n%256;
bin[3]=(unsigned char)(n);
remplir_tableau_bin(a);
if (fwrite(bin,1,taille,f)!=(unsigned)taille) {
  fprintf(stderr,"Error while writing file %s\n",nom);
}
fclose(f);
free(bin);
}
