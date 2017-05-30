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
#include <string.h>
#include <stdlib.h>
#include "unicode.cpp"
#include "AutomateFst2.cpp"
#include "LiberationFst2.cpp"
#include "VerifierRecursion.cpp"
#include "Copyright.h"
#include "Grf2Fst2_lib.cpp"
//---------------------------------------------------------------------------


/////////////////////////////////////////////////////////////
/////////PROGRAMME PRINCIPAL ////////////////////////////////
/////////////////////////////////////////////////////////////


void usage() {
printf("%s",COPYRIGHT);
printf("Usage : Grf2Fst2 <grf> [y/n]\n%s",CR);
printf("      <grf> : main graph of grammar (must be an absolute path)\n%s",CR);
printf("      [y/n] : enable or not the loops/left-recursion detection\n%s",CR);
printf("Compile the grammar and saves the result in a FST2 file stored\n%s",CR);
printf("in the same directory that <grf>.\n%s",CR);
}



int main(int argc,char *argv[]) {
  char temp[TAILLE_MOT_GRAND_COMP];
  char temp1[TAILLE_MOT_GRAND_COMP];
  int l;

  if(argc<2) {
    usage();
    return 0;
  }
  donnees=(struct donnees_comp *) malloc_comp(sizeof(struct donnees_comp));
  init_generale_comp();
  init_arbres_comp();
  strcpy(temp,argv[1]);

  if(ouverture_fichier_sortie(temp) == 0)
  {
    free_comp(donnees);
    libere_arbres_comp();
    fprintf(stderr,"Cannot open file %s\n",temp);
    return 1;
   }
  u_fprints_char("0000000000\n",fs_comp);
 if (compilation(temp) == 0)
  {
    fprintf(stderr,"Compilation has failed\n");
    libere_arbres_comp();
    free_comp(donnees);
    u_fclose(fs_comp);
    return 1;
  }
  sauvegarder_etiquettes_comp();
  libere_arbres_comp();
  free_comp(donnees);
  u_fclose(fs_comp);
  strcpy(temp1,temp);
  l = strlen(temp1);
  temp1[l-4] = '\0';
  strcat(temp1,".fst2");
  ecrire_fichier_sortie_nb_graphes(temp1);
  if (argc==2 || (!strcmp(argv[2],"y"))) {
    if (!pas_de_recursion(temp1)) {
      return 1;
    }
  }
  printf("Compilation has succeeded\n%s",CR);
  return 0;
 }



