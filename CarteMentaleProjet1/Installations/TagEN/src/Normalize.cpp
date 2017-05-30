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
#include "FileName.cpp"
#include "Copyright.h"
#include "DELA.cpp"
#include "String_hash.cpp"
//---------------------------------------------------------------------------

//
// "e:\my unitex\greek\corpus\greek-corpus1_unicode.txt"
//
// "e:\my unitex\french\corpus\test.txt"
//


#define MAX_TAG_LENGTH 4000


void usage() {
printf("%s",COPYRIGHT);
printf("Usage: Normalize <text>\n%s",CR);
printf("     <text> : text file to be normalized\n\n%s",CR);
printf("Turns every sequence of separator chars (space, tab, new line) into one.\n%s",CR);
printf("If a separator sequence contains a new line char, it is turned to a single new\n%s",CR);
printf("line; if not, it is turned into a single space.\n%s",CR);
printf("The result is stored in a file named file_name.snt.\n%s",CR);
}


int N;


void update_position_in_file() {
N=N+2;
if ((N%(1024*1024))==0) {
   int l=N/(1024*1024);
   printf("%d megabyte%s read...%s",l,(l>1)?"s":"",CR);
}
}



void normalize(FILE* f,FILE* f_out) {
int c;
c=u_fgetc(f);
// 2 bytes for the unicode header and 2 bytes for the first character
N=4;
while (c!=EOF) {
   if (c==' ' || c=='\t' || c=='\n') {
      int enter=(c=='\n');
      if (enter) {
         // the new line sequence is coded with 2 characters
         update_position_in_file();
      }
      while ((c=u_fgetc(f))==' ' || c=='\t' || c=='\n') {
         update_position_in_file();
         if (c=='\n') {
            enter=1;
            update_position_in_file();
         }
      }
      update_position_in_file();
      if (enter) u_fputc((unichar)'\n',f_out);
         else u_fputc((unichar)' ',f_out);
   }
   else if (c=='{') {
      unichar s[MAX_TAG_LENGTH+1];
      s[0]='{';
      int z=1;
      while (z<(MAX_TAG_LENGTH-1) && (c=u_fgetc(f))!='}' && c!='{' && c!='\n') {
         s[z++]=(unichar)c;
         update_position_in_file();
      }
      update_position_in_file();
      if (z==(MAX_TAG_LENGTH-1) || c!='}') {
         // if the tag has no ending }
         fprintf(stderr,"\nError at char %d: a tag without ending } has been found\n",(N-1)/2);
         fprintf(stderr,"The { char was replaced by a [  char\n");
         s[0]='[';
         if (c=='{') {
            c='[';
         }
         s[z]=(unichar)c;
         s[z+1]='\0';
      }
      else if (c=='\n') {
         // if the tag contains a return
         fprintf(stderr,"\nError at char %d: a tag containing a new-line sequence has been found\n",(N-1)/2);
         fprintf(stderr,"The { char was replaced by a [  char\n");
         s[0]='[';
         s[z]='\n';
         s[z+1]='\0';
      }
      else {
         s[z]='}';
         s[z+1]='\0';
         if (!u_strcmp_char(s,"{S}")) {
            // if we have found a sentence delimiter
            // we have nothing special to do
         } else {
           if (!check_tag_token(s)) {
              // if a tag is incorrect, we exit
              fprintf(stderr,"\nError at char %d: the text contains an invalid tag\n",(N-1)/2);
              fprintf(stderr,"The { and } chars were replaced by the [ and ] chars\n");
              s[0]='[';
              s[z]=']';
           }
         }
      }
      u_fprints(s,f_out);
      c=u_fgetc(f);
      update_position_in_file();
   } else {
      u_fputc((unichar)c,f_out);
      c=u_fgetc(f);
      update_position_in_file();
   }
}
}




int main(int argc, char **argv) {
FILE* f;
FILE* f_out;
char n[1000];
if (argc!=2) {
   usage();
   return 0;
}
name_without_extension(argv[1],n);
strcat(n,".snt");
f=u_fopen(argv[1],U_READ);
if (f==NULL) {
   fprintf(stderr,"Cannot open file %s\n",argv[1]);
   return 1;
}
f_out=u_fopen(n,U_WRITE);
if (f_out==NULL) {
   fprintf(stderr,"Cannot create file %s\n",n);
   u_fclose(f);
   return 1;
}
printf("Normalizing %s...\n%s",argv[1],CR);
normalize(f,f_out);
printf("\n");
u_fclose(f);
u_fclose(f_out);
printf("\n");
return 0;
}
//---------------------------------------------------------------------------
