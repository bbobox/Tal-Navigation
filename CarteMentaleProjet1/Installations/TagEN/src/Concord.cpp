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
//---------------------------------------------------------------------------
#include "unicode.cpp"
#include "Text_tokens.cpp"
#include "String_hash.cpp"
#include "Liste_nombres.cpp"
#include "Alphabet.cpp"
#include "Matches.cpp"
#include "Concordance.cpp"
#include "FileName.cpp"
#include "Copyright.h"
//---------------------------------------------------------------------------


//
//
//
// "e:\corpus monde\_unicode_lm94_snt\concord.ind" "Courier" "3" 40 40 CL html NULL
//
//---------------------------------------------------------------------------
void usage() {
printf("%s",COPYRIGHT);
printf("Usage: Concord <concor> <font> <fontsize> <l> <r> <order> <mode> <alph> [-thai]\n%s",CR);
printf("   <concord> : the concord.ind file generated by the locate program\n%s",CR);
printf("   <font> : the font you want to use\n%s",CR);
printf("   <fontsize> : the font size in HTML (1-7)\n%s",CR);
printf("   <l> : left context length in chars\n%s",CR);
printf("   <r> : right context length in chars\n%s",CR);
printf("   <order> : sort order:\n%s",CR);
printf("           TO : text order\n%s",CR);
printf("           LC : left, center\n%s",CR);
printf("           LR : left, right\n%s",CR);
printf("           CL : center, left\n%s",CR);
printf("           CR : center, right\n%s",CR);
printf("           RL : right, left\n%s",CR);
printf("           RC : right, center\n%s",CR);
printf("           NULL : specifies no order (used for non-html <mode>)\n%s",CR);
printf("   <mode> : the mode to be used:\n%s",CR);
printf("            html : produces an HTML concordance file\n%s",CR);
printf("            text : produces a plain text concordance file\n%s",CR);
printf("            glossanet : produces a glossanet HTML concordance file\n%s",CR);
printf("            (txt) : produces a file named (txt) which is the SNT file\n%s",CR);
printf("                    merged with the match results\n%s",CR);
printf("   <alph> : the char order file used for sorting\n%s",CR);
printf("            NULL if no alphabet is given\n%s",CR);
printf("   [-thai] : option to use for thai concordances\n%s",CR);
printf("\nExtracts the matches stored in <concor>, and stores them into a UTF-8\n%s",CR);
printf("HTML file saved in the <concor> directory or produces a text file, according\n%s",CR);
printf("to the <mode> parameter\n%s",CR);
printf("\nExamples:\n%s",CR);
printf("Concord tutu.ind \"Courier New\" 3 40 40 TO html \"d:\\My dir\\alph.txt\"\n%s",CR);
printf("    -> produces an HTML file\n%s",CR);
printf("\n%s",CR);
printf("Concord tutu.ind NULL 0 0 0 NULL \"C:\\My dir\\RES.SNT\" NULL\n%s",CR);
printf("    -> produces a text file named \"C:\\My dir\\RES.SNT\"\n%s",CR);
}


#define MAX_ENTER_CHAR 1000000
int enter_pos[MAX_ENTER_CHAR];
int n_enter_char;


int main(int argc, char **argv) {
if (argc!=9 && argc!=10) {
   usage();
   return 0;
}
if (strlen(argv[3])!=1 || argv[3][0]<'0' || argv[3][0]>'7') {
   fprintf(stderr,"Invalid font size parameter %s\n",argv[3]);
   return 1;
}
FILE* concor=u_fopen(argv[1],U_READ);
if (concor==NULL) {
   fprintf(stderr,"Cannot open file %s\n",argv[1]);
   return 1;
}
char text_cod[2000];
char tokens_txt[2000];
char enter[2000];
get_filename_path(argv[1],text_cod);
strcat(text_cod,"text.cod");
get_filename_path(argv[1],tokens_txt);
strcat(tokens_txt,"tokens.txt");
get_filename_path(argv[1],enter);
strcat(enter,"enter.pos");

FILE* text=fopen(text_cod,"rb");
if (text==NULL) {
   fprintf(stderr,"Cannot open file %s\n",text_cod);
   u_fclose(concor);
   return 1;
}
struct text_tokens* tok=load_text_tokens(tokens_txt);
if (tok==NULL) {
   fprintf(stderr,"Cannot load text token file %s\n",tokens_txt);
   u_fclose(concor);
   fclose(text);
   return 1;
}
FILE* f_enter=fopen(enter,"rb");
if (f_enter==NULL) {
   fprintf(stderr,"Cannot open file %s\n",enter);
   n_enter_char=0;
}
else {
   n_enter_char=fread(&enter_pos,sizeof(int),MAX_ENTER_CHAR,f_enter);
   fclose(f_enter);
}

int longueur_avant;
int longueur_apres;
if (1!=sscanf(argv[4],"%d",&longueur_avant)) {
   fprintf(stderr,"Invalid left context length %s\n",argv[4]);
   u_fclose(concor);
   fclose(text);
   free_text_tokens(tok);
   return 1;
}
if (1!=sscanf(argv[5],"%d",&longueur_apres)) {
   fprintf(stderr,"Invalid right context length %s\n",argv[5]);
   u_fclose(concor);
   fclose(text);
   free_text_tokens(tok);
   return 1;
}
int sort_mode;
if (!strcmp(argv[6],"TO") || !strcmp(argv[6],"NULL"))
   sort_mode=TEXT_ORDER;
else if (!strcmp(argv[6],"LC"))
   sort_mode=LEFT_CENTER;
else if (!strcmp(argv[6],"LR"))
   sort_mode=LEFT_RIGHT;
else if (!strcmp(argv[6],"CL"))
   sort_mode=CENTER_LEFT;
else if (!strcmp(argv[6],"CR"))
   sort_mode=CENTER_RIGHT;
else if (!strcmp(argv[6],"RL"))
   sort_mode=RIGHT_LEFT;
else if (!strcmp(argv[6],"RC"))
   sort_mode=RIGHT_CENTER;
else {
   fprintf(stderr,"Invalid sort mode %s\n",argv[6]);
   u_fclose(concor);
   fclose(text);
   free_text_tokens(tok);
   return 1;
}
if (argc==10) {
   if (strcmp(argv[9],"-thai")) {
      fprintf(stderr,"Invalid optional parameter _%s_\n",argv[9]);
      u_fclose(concor);
      fclose(text);
      free_text_tokens(tok);
      return 1;
   }
  CHAR_BY_CHAR=THAI;
} else CHAR_BY_CHAR=OCCIDENTAL;
char f[2000];
get_filename_path(argv[1],f);
create_concordance(concor,text,tok,sort_mode,longueur_avant,longueur_apres,argv[2],argv[3],f,argv[7],argv[8],n_enter_char,enter_pos);
u_fclose(concor);
fclose(text);
free_text_tokens(tok);
printf("Done.\n%s",CR);
return 0;
}
//---------------------------------------------------------------------------