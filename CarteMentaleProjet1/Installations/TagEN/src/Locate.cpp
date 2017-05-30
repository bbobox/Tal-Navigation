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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unicode.cpp"
#include "Alphabet.cpp"
#include "DELA.cpp"
#include "String_hash.cpp"
#include "LocatePattern.cpp"
#include "AutomateFst2.cpp"
#include "Text_tokens.cpp"
#include "Liste_nombres.cpp"
#include "Grammatical_codes.cpp"
#include "Flexional_codes.cpp"
#include "Facteurs_interdits.cpp"
#include "Loading_dic.cpp"
#include "Arbre_mots_composes.cpp"
#include "Fst2_tags_optimization.cpp"
#include "DLC_optimization.cpp"
#include "Optimized_fst2.cpp"
#include "Pattern_transitions.cpp"
#include "Text_parsing.cpp"
#include "Matches.cpp"
#include "TransductionVariables.cpp"
#include "TransductionStack.cpp"
#include "Liste_num.cpp"
#include "FileName.cpp"
#include "Copyright.h"
//---------------------------------------------------------------------------

//
// "E:\My Unitex\French\Corpus\Ceci est un test.snt" "E:\My Unitex\French\regexp.fst2" "E:\My Unitex\French\Alphabet.txt" l i 200
//
// "e:\my unitex\french\corpus\la peau de chagrin.snt" "e:\my unitex\french\graphs\vaux\adjprep.fst2" "e:\my unitex\french\Alphabet.txt" l i all
//
//---------------------------------------------------------------------------

void usage() {
printf("%s",COPYRIGHT);
printf("Usage: Locate <text> <fst2> <alphabet> <sla> <imr> <n> [-thai] [-space]\n%s",CR);
printf("     <text>: the text file\n%s",CR);
printf("     <fst2> : the grammar to be applied\n%s",CR);
printf("     <alphabet> : the language alphabet file\n%s",CR);
printf("     <sla> : defines the matching mode: s=shortest matches\n%s",CR);
printf("                                        l=longuest matches\n%s",CR);
printf("                                        a=all matches\n%s",CR);
printf("     <imr> : defines the transduction mode: i=ignore outputs\n%s",CR);
printf("                                            m=merge outputs\n%s",CR);
printf("                                            r=replace outputs\n%s",CR);
printf("     <n> : defines the search limitation:\n%s",CR);
printf("                                        all=compute all matches\n%s",CR);
printf("                                        N=stop after N matches\n%s",CR);
printf("     [-thai] : option to use to work on thai\n%s",CR);
printf("     [-space] : enables morphological use of space\n%s",CR);
printf("\nApplies a grammar to a text, and saves the matching sequence index in a\n%s",CR);
printf("file named \"concord.ind\" stored in the text directory. A result info file\n%s",CR);
printf("named \"concord.n\" is also saved in the same directory.\n%s",CR);
}



int main(int argc, char **argv) {
if (argc!=7 && argc!=8 && argc!=9) {
   usage();
   return 0;
}
char tokens_txt[2000];
char text_cod[2000];
char dlf[2000];
char dlc[2000];
get_snt_path(argv[1],tokens_txt);
strcat(tokens_txt,"tokens.txt");
get_snt_path(argv[1],text_cod);
strcat(text_cod,"text.cod");
get_snt_path(argv[1],dlf);
strcat(dlf,"dlf");
get_snt_path(argv[1],dlc);
strcat(dlc,"dlc");
int mode;
int output_mode;
if (!strcmp(argv[4],"s")) {
   mode=SHORTEST_MATCHES;
} else if (!strcmp(argv[4],"a")) {
          mode=ALL_MATCHES;
       }
  else if (!strcmp(argv[4],"l")) {
          mode=LONGUEST_MATCHES;
       }
  else {
     fprintf(stderr,"Invalid parameter %s\n",argv[4]);
     return 1;
  }
if (!strcmp(argv[5],"i")) {
   output_mode=IGNORE_TRANSDUCTIONS;
} else if (!strcmp(argv[5],"m")) {
          output_mode=MERGE_TRANSDUCTIONS;
       }
  else if (!strcmp(argv[5],"r")) {
          output_mode=REPLACE_TRANSDUCTIONS;
       }
  else {
     fprintf(stderr,"Invalid parameter %s\n",argv[5]);
     return 1;
  }
if (!strcmp(argv[6],"all")) {
   SEARCH_LIMITATION=-1;
}
else {
   if (!sscanf(argv[6],"%d",&SEARCH_LIMITATION)) {
      fprintf(stderr,"Invalid parameter %s\n",argv[6]);
      return 1;
   }
}
CHAR_BY_CHAR=OCCIDENTAL;
if (argc==8) {
   if (strcmp(argv[7],"-thai") && strcmp(argv[7],"-space")) {
      fprintf(stderr,"Invalid parameter %s\n",argv[7]);
      return 1;
   }
  if (!strcmp(argv[7],"-thai")) CHAR_BY_CHAR=THAI;
  if (!strcmp(argv[7],"-space")) GESTION_DE_L_ESPACE=MODE_MORPHO;
}
if (argc==9) {
   if (strcmp(argv[8],"-thai") && strcmp(argv[8],"-space")) {
      fprintf(stderr,"Invalid parameter %s\n",argv[8]);
      return 1;
   }
  if (!strcmp(argv[8],"-thai")) CHAR_BY_CHAR=THAI;
  if (!strcmp(argv[8],"-space")) GESTION_DE_L_ESPACE=MODE_MORPHO;
}

locate_pattern(text_cod,tokens_txt,argv[2],dlf,dlc,argv[3],mode,output_mode);
return 0;
}
//---------------------------------------------------------------------------
