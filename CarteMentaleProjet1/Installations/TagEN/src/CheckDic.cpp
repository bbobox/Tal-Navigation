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
#include "FileName.cpp"
#include "DELA.cpp"
#include "String_hash.cpp"
#include "Copyright.h"
//---------------------------------------------------------------------------

//
// e:\dela\french\delaf.dic DELAF
//

void usage() {
printf("%s",COPYRIGHT);
printf("Usage: CheckDic <dela> <type>\n%s",CR);
printf("     <dela> : name of the unicode text dictionary (must be a full path\n%s",CR);
printf("     <type> : dictionary type. Two values are possible:\n%s",CR);
printf("              DELAS : check any non inflected dictionary\n%s",CR);
printf("              DELAF : check any inflected dictionary\n%s",CR);
printf("\n%sChecks the format of <dela> and produces a file named CHECK_DIC.TXT\n%s",CR,CR);
printf("that contains check result informations. This file is stored in the\n%s",CR);
printf("<dela> directory.\n%s",CR);
}

#define DELAS 0
#define DELAF 1

int main(int argc, char **argv) {
if (argc!=3) {
   usage();
   return 0;
}
FILE* dic;
FILE* out;
int TYPE;
if (!strcmp(argv[2],"DELAS")) {
   TYPE=DELAS;
}
else if (!strcmp(argv[2],"DELAF")) {
     TYPE=DELAF;
}
else {
     fprintf(stderr,"Invalid dictionary type %s\n",argv[2]);
     return 1;
}
dic=u_fopen(argv[1],U_READ);
if (dic==NULL) {
   fprintf(stderr,"Cannot open dictionary %s\n",argv[1]);
   return 1;
}
char res[1024];
get_filename_path(argv[1],res);
strcat(res,"CHECK_DIC.TXT");
out=u_fopen(res,U_WRITE);
if (out==NULL) {
   fprintf(stderr,"Cannot create %s\n",res);
   u_fclose(dic);
   return 1;
}
printf("Checking %s...\n%s",argv[1],CR);
unichar s[5000];
int line=1;
char alphabet[65536];
for (int i=0;i<65536;i++) {alphabet[i]=0;}
struct string_hash* semantic=new_string_hash();
struct string_hash* inflectional=new_string_hash();
while (read_DELA_line(dic,s)) {
   if (s[0]=='\0') {
      char temp[1000];
      sprintf(temp,"Line %d: empty line\n",line);
      u_fprints_char(temp,out);
   }
   else {
      if (TYPE==DELAS) check_DELAS_line(s,out,line,alphabet,semantic,inflectional);
      else check_DELAF_line(s,out,line,alphabet,semantic,inflectional);
   }
   if (line%10000==0) {
     printf("%d lines read...       %s",line,CR);
  }
  line++;
}
printf("%d lines read\n%s",line-1,CR);
u_fclose(dic);
u_fprints_char("-----------------------------------\n",out);
u_fprints_char("----  All chars used in forms  ----\n",out);
u_fprints_char("-----------------------------------\n",out);
unichar r[4];
unichar r2[7];
r[1]=' ';
r[2]='(';
r[3]='\0';
r2[5]='\n';
r2[6]='\0';
for (int i=0;i<65536;i++) {
  if (alphabet[i]) {
     r[0]=(unichar)i;
     u_char_to_hexa((unichar)i,r2);
     r2[4]=')';
     u_fprints(r,out);
     u_fprints(r2,out);
  }
}
u_fprints_char("-------------------------------------------------------------\n",out);
char tmp[1000];
sprintf(tmp,"----  %3d grammatical/semantic code%s",semantic->N,(semantic->N>1)?"s used in dictionary  ----\n":" used in dictionary  -----\n");
u_fprints_char(tmp,out);
u_fprints_char("-------------------------------------------------------------\n",out);
unichar comment[2000];
for (int i=0;i<semantic->N;i++) {
   u_fprints(semantic->tab[i],out);
   if (warning_on_code(semantic->tab[i],comment)) {
      u_fprints_char(" ",out);
      u_fprints(comment,out);
   }
   u_fprints_char("\n",out);
}
u_fprints_char("-----------------------------------------------------\n",out);
sprintf(tmp,"----  %3d inflectional code%s",inflectional->N,(inflectional->N>1)?"s used in dictionary  ----\n":" used in dictionary  -----\n");
u_fprints_char(tmp,out);
u_fprints_char("-----------------------------------------------------\n",out);
for (int i=0;i<inflectional->N;i++) {
   u_fprints(inflectional->tab[i],out);
   if (warning_on_code(inflectional->tab[i],comment)) {
      u_fprints_char(" ",out);
      u_fprints(comment,out);
   }
   u_fprints_char("\n",out);
}
u_fclose(out);
printf("Done.\n%s",CR);
return 0;
}

