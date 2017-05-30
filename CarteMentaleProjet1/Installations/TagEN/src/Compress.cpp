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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unicode.cpp"
#include "DELA.cpp"
#include "Arbre_dico.cpp"
#include "String_hash.cpp"
#include "Arbre_to_bin.cpp"
#include "FileName.cpp"
#include "Minimize_tree.cpp"
#include "Copyright.h"
//---------------------------------------------------------------------------

//
// e:\test_dico.dic
//


//---------------------------------------------------------------------------


void usage() {
printf("%s",COPYRIGHT);
printf("Usage: Compress <dictionary> [-flip]\n%s",CR);
printf("   <dictionary> : any unicode DELAF or DELACF dictionary\n%s",CR);
printf("   -flip : this optional parameter specifies that the inflected and lemma\n%s",CR);
printf("           forms must be swapped\n\n%s",CR);
printf("Compresses a dictionary into an finite state automaton. This automaton\n%s",CR);
printf("is stored is a .bin file, and the associated flexional codes are\n%s",CR);
printf("written in a .inf file.\n\n%s",CR);
}



void ecrire_nombre_lignes_INF(char name[],int n)
{
  FILE *f;
  int i;
  i=2+9*2; // *2 because of unicode +2 because of FF FE at file start
  f=fopen((char*)name,"r+b");
  do
    {
      fseek(f,i,0);
      i=i-2;
      u_fputc((unichar)((n%10)+'0'),f);
      n=n/10;
    }
  while (n);
  fclose(f);
}




int main(int argc, char **argv) {
FILE *f;
FILE *INF;
unichar s[1000];
dic_entry *e;
char bin[1000];
char inf[1000];
struct arbre_dico* racine;
struct string_hash* hash;
int i=1;

if (argc!=2 && argc!=3) {
   usage();
   return 0;
}
f=u_fopen(argv[1],U_READ);
if (f==NULL) {
   fprintf(stderr,"Cannot open %s\n",argv[1]);
   return 1;
}
int FLIP=0;
if (argc==3) {
   if (!strcmp(argv[2],"-flip")) {
        FLIP=1;
   } else {
      fprintf(stderr,"Invalid parameter: %s\n",argv[2]);
   }
}

name_without_extension(argv[1],bin);
strcat(bin,".bin");
name_without_extension(argv[1],inf);
strcat(inf,".inf");
INF=u_fopen(inf,U_WRITE);
if (INF==NULL) {
   fprintf(stderr,"Cannot create %s\n",inf);
   u_fclose(f);
   return 1;
}
u_fprints_char("0000000000\n",INF);
racine=new_arbre_dico();
hash=new_string_hash();
unichar tmp[1000];
printf("Compressing...\n%s",CR);
while(read_DELA_line(f,s)) {
  if (s[0]=='\0') {
     fprintf(stderr,"Line %d: empty line\n",i);
  }
  else {
     e=tokenize_DELA_line(s);
     if (FLIP) {
        unichar* o=e->inflected;
        e->inflected=e->lemma;
        e->lemma=o;
     }
     if (e!=NULL) {
        if (contains_unprotected_equal_sign(e->inflected)
            || contains_unprotected_equal_sign(e->lemma)) {
           // if the inflected form or lemma contains any unprotected = sign,
           // we must insert the space entry and the - entry:
           // pomme=de=terre,.N  ->  pomme de terre,pomme de terre.N
           //                        pomme-de-terre,pomme-de-terre.N
           unichar inf_tmp[1000];
           unichar lem_tmp[1000];
           u_strcpy(inf_tmp,e->inflected);
           u_strcpy(lem_tmp,e->lemma);
           replace_unprotected_equal_sign(e->inflected,(unichar)' ');
           replace_unprotected_equal_sign(e->lemma,(unichar)' ');
           // we insert pomme de terre,pomme de terre.N
           get_compressed_line(e,tmp);
           inserer_entree(e->inflected,tmp,racine,hash);
           // and pomme-de-terre,pomme-de-terre.N
           u_strcpy(e->inflected,inf_tmp);
           u_strcpy(e->lemma,lem_tmp);
           replace_unprotected_equal_sign(e->inflected,(unichar)'-');
           replace_unprotected_equal_sign(e->lemma,(unichar)'-');
           get_compressed_line(e,tmp);
           inserer_entree(e->inflected,tmp,racine,hash);
        }
        else {
           // normal case
           unprotect_equal_signs(e->inflected);
           unprotect_equal_signs(e->lemma);
           get_compressed_line(e,tmp);
           inserer_entree(e->inflected,tmp,racine,hash);
        }
     }
  }
  if (i%10000==0) {
     printf("%d line%s read...       %s",i,(i>1)?"s":"",CR);
  }
  i++;
}
u_fclose(f);
sauver_lignes_hash(INF,hash);
u_fclose(INF);
minimize_tree(racine);
creer_et_sauver_bin(racine,bin);
printf("%d line%s read            \n%s%d INF entr%s created\n%s",i,(i>1)?"s":"",CR,hash->N,(hash->N>1)?"ies":"y",CR);
printf("%d states, %d transitions\n%s",N_STATES,N_TRANSITIONS,CR);
ecrire_nombre_lignes_INF(inf,hash->N);
// the following line had been removed because of a slowness problem with
// very large INF lines
//free_string_hash(hash);
return 0;
}

