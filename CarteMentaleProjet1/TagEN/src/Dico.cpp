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
#include "DELA.cpp"
#include "String_hash.cpp"
#include "unicode.cpp"
#include "Alphabet.cpp"
#include "Text_tokens.cpp"
#include "Dico_application.cpp"
#include "Liste_nombres.cpp"
#include "FileName.cpp"
#include "Token_tree.cpp"
#include "Copyright.h"
//---------------------------------------------------------------------------

//
// "e:\my unitex\french\corpus\femme.snt" "e:\my unitex\french\alphabet.txt" e:\unitex\french\dela\DELA.bin
//
// THAI:
// e:\unitex\thai\corpus\corpus.txt e:\unitex\thai\alphabet_thai.txt e:\DELA\thai\DELA.txt
//

//---------------------------------------------------------------------------
void usage() {
printf("%s",COPYRIGHT);
printf("Usage: Dico <text> <alphabet> <dic_1> [<dic_2> <dic_3> ...]\n%s",CR);
printf("     <text> : the text file\n%s",CR);
printf("     <alphabet> : the alphabet file\n%s",CR);
printf("     <dic_i> : name of dictionary to be applied\n\n%s",CR);
printf("Applies dictionaries to the text and produces \n%s",CR);
printf("3 files, saved in the text directory. These files are:\n\n%s",CR);
printf(" DLF : simple entry dictionary\n%s",CR);
printf(" DLC : compound entry dictionary\n%s",CR);
printf(" ERR : unrecognized simple words\n\n%s",CR);
printf("There are 3 levels of priority. If the dictionary name ends with \"-\",\n%s",CR);
printf("it will be applied with the maximum priority. If the suffix\n%s",CR);
printf("is \"+\", the priority is minimal. If there is no suffix, the priority\n%s",CR);
printf("is normal. The numbers of simple, compound and unknown forms are saved\n%s",CR);
printf("in a file named stat_dic.n which is created in the text directory.\n%s",CR);
printf("\nExample:    Dico \"c:\\tutu.snt\" \"c:\\Alphabet.txt\" Dela.bin MyFilter-.bin\n%s",CR);
printf("\nThis command will apply first MyFilter-.bin and then Dela.bin.\n%s",CR);
printf("\nNote: the 3 resulting files (DLF, DLC and ERR) are stored in the text\n%s",CR);
printf("directory. THEY ARE NOT SORTED. Use the SortTxt program to.\n\n%s",CR);
}



int main(int argc, char **argv) {
unsigned char* bin;
struct INF_codes* INF;
struct text_tokens* tok;
char nom_inf[1000];
char nom_bin[1000];
char nom_dlf[1000];
char nom_dlc[1000];
char nom_err[1000];
FILE *dlc,*dlf,*err,*text;
Alphabet* alph;
if (argc<4) {
   usage();
   return 0;
}
get_snt_path(argv[1],nom_dlf);
strcat(nom_dlf,"dlf");
get_snt_path(argv[1],nom_dlc);
strcat(nom_dlc,"dlc");
get_snt_path(argv[1],nom_err);
strcat(nom_err,"err");
alph=load_alphabet(argv[2]);
if (alph==NULL) {
   fprintf(stderr,"Cannot open alphabet file %s\n",argv[2]);
   return 1;
}
char text_cod[2000];
char tokens_txt[2000];
get_snt_path(argv[1],text_cod);
strcat(text_cod,"text.cod");
get_snt_path(argv[1],tokens_txt);
strcat(tokens_txt,"tokens.txt");
tok=load_text_tokens(tokens_txt);
if (tok==NULL) {
   free_alphabet(alph);
   fprintf(stderr,"Cannot open token file %s\n",tokens_txt);
   return 1;
}
dlf=u_fopen(nom_dlf,U_WRITE);
if (dlf==NULL) {
   free_alphabet(alph);
   free_text_tokens(tok);
   fprintf(stderr,"Cannot create %s\n",nom_dlf);
   return 1;
}
dlc=u_fopen(nom_dlc,U_WRITE);
if (dlc==NULL) {
   free_alphabet(alph);
   free_text_tokens(tok);
   u_fclose(dlf);
   fprintf(stderr,"Cannot create %s\n",nom_dlc);
   return 1;
}
err=u_fopen(nom_err,U_WRITE);
if (err==NULL) {
   free_alphabet(alph);
   free_text_tokens(tok);
   u_fclose(dlf);
   u_fclose(dlc);
   fprintf(stderr,"Cannot create %s\n",nom_err);
   return 1;
}
text=fopen(text_cod,"rb");
if (text==NULL) {
   free_alphabet(alph);
   free_text_tokens(tok);
   u_fclose(dlf);
   u_fclose(dlc);
   u_fclose(err);
   fprintf(stderr,"Cannot open coded text file %s\n",text_cod);
   return 1;
}
printf("Initializing...\n%s",CR);
init_dico_application(tok,dlf,dlc,err,text,alph);
for (int priority=1;priority<4;priority++) {
   for (int i=3;i<argc;i++) {
      // we do a loop to apply several dictionaries
      name_without_extension(argv[i],nom_inf);
      char C=nom_inf[strlen(nom_inf)-1];
      if ((priority==1 && C=='-') ||
          (priority==2 && C!='-' && C!='+') ||
          (priority==3 && C=='+')) {
         printf("Applying %s...\n%s",argv[i],CR);
         strcat(nom_inf,".inf");
         name_without_extension(argv[i],nom_bin);
         strcat(nom_bin,".bin");
         INF=load_INF_file(nom_inf);
         if (INF==NULL) {
            fprintf(stderr,"Cannot open %s\n",nom_inf);
         }
         else {
            bin=load_BIN_file(nom_bin);
            if (bin==NULL) {
               free_INF_codes(INF);
               fprintf(stderr,"Cannot open %s\n",nom_bin);
               }
            else {
               dico_application(bin,INF,text,tok,alph,dlf,dlc,err,priority);
               free_INF_codes(INF);
               free(bin);
            }
         }
      }
   }
}
printf("Saving unknown words...\n%s",CR);
sauver_mots_inconnus();
printf("Done.\n%s",CR);
free_dico_application();
free_alphabet(alph);
free_text_tokens(tok);
u_fclose(dlf);
u_fclose(dlc);
u_fclose(err);
fclose(text);
get_snt_path(argv[1],tokens_txt);
strcat(tokens_txt,"stat_dic.n");
dlf=u_fopen(tokens_txt,U_WRITE);
if (dlf==NULL) {
   fprintf(stderr,"Cannot write %s\n",tokens_txt);
}
else {
   unichar tmp[100];
   u_int_to_string(SIMPLE_WORDS,tmp);
   u_strcat_char(tmp,"\n");
   u_fprints(tmp,dlf);
   u_int_to_string(COMPOUND_WORDS,tmp);
   u_strcat_char(tmp,"\n");
   u_fprints(tmp,dlf);
   u_int_to_string(ERRORS,tmp);
   u_strcat_char(tmp,"\n");
   u_fprints(tmp,dlf);
   u_fclose(dlf);
}
return 0;
}
//---------------------------------------------------------------------------
