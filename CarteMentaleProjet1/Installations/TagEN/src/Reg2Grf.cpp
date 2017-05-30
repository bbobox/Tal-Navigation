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
#include "Regular_expression.cpp"
#include "Copyright.h"
//---------------------------------------------------------------------------


void usage() {
printf("%s",COPYRIGHT);
printf("Usage: Reg2Grf <file>\n%s",CR);
printf("     <file> : unicode text file where the regular expression is stored.\n%s",CR);
printf("              We must use a file, because we cannot give Unicode\n%s",CR);
printf("              parameters on a command line.\n\n%s",CR);
printf("Converts the regular expression into a graph, named REGEXP.GRF\n%s",CR);
printf("and stored in the same directory that <file>. You can use the following\n%s",CR);
printf("operators:\n%s",CR);
printf(" A+B          matches either the expression A or B\n%s",CR);
printf(" A.B or A B   matches the concatenation of A and B\n%s",CR);
printf(" A*           matches 0 more more times the expression A\n%s",CR);
printf(" (A)          matches the expression A\n%s",CR);
printf("If you want to match any character of ( ) * + .\n%s",CR);
printf("you must use the \\ char: \\* will match the char *\n%s",CR);   
printf("\nExample: \"(le+la) (<A:s>+<E>) <N:s>\"\n%s",CR);
}

//
// test parameter
//
// "e:\my unitex\english\regexp.txt"
//


int main(int argc, char **argv) {
if (argc!=2) {
   usage();
   return 0;
}
FILE* f=u_fopen(argv[1],U_READ);
if (f==NULL) {
   fprintf(stderr,"Cannot open file %s\n",argv[1]);
   return 1;
}
// we read the regular expression in the file
unichar exp[1000];
int i=0;
int c;
while ((c=u_fgetc(f))!=EOF && c!='\n') {
   exp[i++]=(unichar)c;
}
exp[i]='\0';
u_fclose(f);
// we extract regexp file pathname
char nom_grf[1000];
strcpy(nom_grf,argv[1]);
i=strlen(nom_grf);
while (i>0 && nom_grf[i]!='/' && nom_grf[i]!='\\') i--;
if (i==0) {
   strcpy(nom_grf,"regexp.grf");
} else {
   nom_grf[i+1]='\0';
   strcat(nom_grf,"regexp.grf");
}

if (!reg2grf(exp,nom_grf)) {
   fprintf(stderr,"Error in the regular expression\n");
   return 1;
}
printf("Expression converted.\n%s",CR);
return 0;
}
//---------------------------------------------------------------------------
