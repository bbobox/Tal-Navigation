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
#include "Copyright.h"
#include "Alphabet.cpp"
#include "DELA.cpp"
#include "String_hash.cpp"
#include "NorwegianCompounds.cpp"
#include "GermanCompounds.cpp"
//---------------------------------------------------------------------------

// NORWEGIAN e:\norvegien\alphabet.txt e:\norvegien\Delaf.bin e:\norvegien\inconnus.txt e:\norvegien\resultat.dic e:\norvegien\decomp.txt
// GERMAN "e:\my unitex\german\alphabet.txt" "e:\my unitex\german\Dela\dela.bin" "e:\my unitex\german\corpus\kritik der reinen vernunft_snt\err.txt" "e:\my unitex\german\corpus\kritik der reinen vernunft_snt\result.dic" "e:\my unitex\german\decomp.txt" 

//---------------------------------------------------------------------------
void usage() {
printf("%s",COPYRIGHT);
printf("Usage: PolyLex <lang> <alph> <dic> <list> <out> [<info>]\n%s",CR);
printf("     <lang> : language to wrok on. Possible values are:\n%s",CR);
printf("              GERMAN\n%s",CR);
printf("              NORWEGIAN\n%s",CR);
printf("     <alph> : alphabet file of the language\n%s",CR);
printf("     <dic> : dictionary .BIN to use\n%s",CR);
printf("     <list> : text file containing the words to be analysed\n%s",CR);
printf("     <out> : dictionary .DIC where the resulting lines will be stored. If\n%s",CR);
printf("             this file allready exists, the lines are added at the end of it.\n%s",CR);
printf("     [<info>] : if this optional parameter is precised, it is taken as\n%s",CR);
printf("                the name of a file which will contain information about\n%s",CR);
printf("                the analysis\n%s",CR);
printf("\n%s",CR);
printf("Tries to decompose some norwegian words as combinaisons of shortest words.\n%s",CR);
printf("This words are removed from the <list> files.\n%s",CR);
}


int main(int argc, char **argv) {
if (argc<6 || argc>7) {
   usage();
   return 0;
}
printf("Loading alphabet...\n%s",CR);
Alphabet* alph=load_alphabet(argv[2]);
if (alph==NULL) {
   fprintf(stderr,"Cannot load alphabet file %s\n",argv[2]);
   return 1;
}
printf("Loading BIN file...\n%s",CR);
unsigned char* bin=load_BIN_file(argv[3]);
if (bin==NULL) {
   fprintf(stderr,"Cannot load bin file %s\n",argv[3]);
   free_alphabet(alph);
   return 1;
}
char temp[1024];
strcpy(temp,argv[3]);
temp[strlen(argv[3])-3]='\0';
strcat(temp,"inf");
printf("Loading INF file...\n%s",CR);
struct INF_codes* inf=load_INF_file(temp);
if (inf==NULL) {
   fprintf(stderr,"Cannot load inf file %s\n",temp);
   free_alphabet(alph);
   free(bin);
   return 1;
}
char tmp[2000];
strcpy(tmp,argv[4]);
strcat(tmp,".tmp");
FILE* words=u_fopen(argv[4],U_READ);
if (words==NULL) {
   fprintf(stderr,"Cannot open word list file %s\n",argv[4]);
   free_alphabet(alph);
   free(bin);
   free_INF_codes(inf);
   // here we return 0 in order to do not block the preprocessing
   // in the Unitex Java interface, if no dictionary was applied
   // so that there is no "err" file
   return 0;
}
FILE* new_unknown_words=u_fopen(tmp,U_WRITE);
if (new_unknown_words==NULL) {
   fprintf(stderr,"Cannot open temporary word list file %s\n",tmp);
   free_alphabet(alph);
   free(bin);
   free_INF_codes(inf);
   u_fclose(words);
   return 1;
}

FILE* res=u_fopen(argv[5],U_APPEND);
if (res==NULL) {
   fprintf(stderr,"Cannot open result file %s\n",argv[5]);
   free_alphabet(alph);
   free(bin);
   free_INF_codes(inf);
   u_fclose(words);
   u_fclose(new_unknown_words);
   return 1;
}
FILE* debug=NULL;
if (argc==7) {
   debug=u_fopen(argv[6],U_WRITE);
   if (debug==NULL) {
      fprintf(stderr,"Cannot open debug file %s\n",argv[6]);
   }
}

if (!strcmp(argv[1],"GERMAN")) {
   analyse_german_compounds(alph,bin,inf,words,res,debug,new_unknown_words);
}
else if (!strcmp(argv[1],"NORWEGIAN")) {
   analyse_norwegian_compounds(alph,bin,inf,words,res,debug,new_unknown_words);
}
else {
   fprintf(stderr,"Invalid language argument: %s\n",argv[1]);
   free_alphabet(alph);
   free(bin);
   free_INF_codes(inf);
   u_fclose(words);
   u_fclose(new_unknown_words);
   u_fclose(res);
   if (debug!=NULL) {
      u_fclose(debug);
   }
   return 1;
}

free_alphabet(alph);
free(bin);
free_INF_codes(inf);
u_fclose(words);
u_fclose(new_unknown_words);
remove(argv[4]);
rename(tmp,argv[4]);
u_fclose(res);
if (debug!=NULL) {
   u_fclose(debug);
}
return 0;
}
//---------------------------------------------------------------------------
