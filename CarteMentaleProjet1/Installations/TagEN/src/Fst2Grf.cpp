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
#include "AutomateFst2.cpp"
#include "LiberationFst2.cpp"
#include "Sentence_to_grf.cpp"
#include "FileName.cpp"
#include "Liste_nombres.cpp"
#include "Copyright.h"
//---------------------------------------------------------------------------

//
// "e:\my unitex\english\corpus\ivanhoe_snt\text.fst2" 1
//

void usage() {
printf("%s",COPYRIGHT);
printf("Usage: Fst2Grf <text automata> <sentence> [<output>]\n%s",CR);
printf("     <text automata> :  the FST2 file that contains the text automata.\n%s",CR);
printf("     <sentence> :       the number of the sentence to be converted.\n%s",CR);
printf("     <output> :         name GRF file as <output>.grf and the TXT one as <output>.txt (default cursentence)\n\n%s", CR); 
printf("Converts a sentence automaton into a GRF file that can be viewed. The\n%s",CR);
printf("resulting file, named cursentence.grf, is stored in the same directory\n%s",CR);
printf("that <text automata>. The text of the sentence is saved in the same\n%s",CR);
printf("directory, in a file named cursentence.txt.\n%s",CR);
}



int main(int argc, char **argv) {

  if (argc < 3 || argc > 4) {
    usage();
    return 0;
  }

  int SENTENCE;
  char nom_grf[2000];
  char nom_txt[2000];
  FILE * txt, * f;


  if (!sscanf(argv[2],"%d",&SENTENCE)) {
    fprintf(stderr,"Invalid sentence number %s\n",argv[2]);
    return 1;
  }

  get_filename_path(argv[1],nom_grf);
  get_filename_path(argv[1],nom_txt);

  if (argc == 3) {

    strcat(nom_grf,"cursentence.grf");
    strcat(nom_txt,"cursentence.txt");

  } else {

    strcat(nom_grf, argv[3]);
    strcat(nom_grf, ".grf");

    strcat(nom_txt, argv[3]);
    strcat(nom_txt, ".txt");

  }


  f = u_fopen(nom_grf,U_WRITE);
  if (f==NULL) {
    fprintf(stderr,"Cannot write file %s\n",nom_grf);
    return 1;
  }

  txt = u_fopen(nom_txt,U_WRITE);
  if (txt==NULL) {
    fprintf(stderr,"Cannot write file %s\n",nom_txt);
    u_fclose(f);
    return 1;
  }



Automate_fst2* automate;
printf("Loading %s...\n%s",argv[1],CR);
automate=load_one_sentence_of_fst2(argv[1],SENTENCE,txt);
if (automate==NULL) {
   fprintf(stderr,"Cannot load text automata file %s\n",argv[1]);
   u_fclose(f);
   u_fclose(txt);
   return 1;
}
printf("Creating GRF...\n%s",CR);
sentence_to_grf(automate,SENTENCE,f);
u_fclose(f);
u_fclose(txt);
free_fst2(automate);
printf("Done.\n%s",CR);
return 0;
}
//---------------------------------------------------------------------------
