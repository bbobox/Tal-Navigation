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
//--------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unicode.cpp"
#include "Copyright.h"
#include "CodePages.cpp"
//--------------------------------------------------------------


//
// the usage function is in the file CodePages.cpp
//


int main(int argc, char **argv) {
FILE* entree;
FILE* sortie;
char res[1000];
int i;
if (argc<2) {                                                          //$CD:20021206
   usage(1);
   return 0;
}
else if (argc<3) {                                                     //$CD:20021206
    if (!strcmp(argv[1],"LATIN") || !strcmp(argv[1],"windows-1252"))   //$CD:20021206
        usage_LATIN(1);                                                 //$CD:20021206
    else if (!strcmp(argv[1],"windows-1250"))                          //$CD:20021206
        usage_windows1250(1);                                           //$CD:20021206
    else if (!strcmp(argv[1],"windows-1257"))                          //$CD:20021206
        usage_windows1257(1);                                           //$CD:20021206
    else if (!strcmp(argv[1],"windows-1251"))                          //$CD:20021206
        usage_windows1251(1);                                           //$CD:20021206
    else if (!strcmp(argv[1],"windows-1254"))                          //$CD:20021206
        usage_windows1254(1);                                           //$CD:20021206
    else if (!strcmp(argv[1],"windows-1258"))                          //$CD:20021206
        usage_windows1258(1);                                           //$CD:20021206
    else if (!strcmp(argv[1],"iso-8859-1"))                            //$CD:20021206
        usage_iso88591(1);                                              //$CD:20021206
    else if (!strcmp(argv[1],"iso-8859-15"))                           //$CD:20021206
        usage_iso885915(1);                                             //$CD:20021206
    else if (!strcmp(argv[1],"iso-8859-2"))                            //$CD:20021206
        usage_iso88592(1);                                              //$CD:20021206
    else if (!strcmp(argv[1],"iso-8859-3"))                            //$CD:20021206
        usage_iso88593(1);                                              //$CD:20021206
    else if (!strcmp(argv[1],"iso-8859-4"))                            //$CD:20021206
        usage_iso88594(1);                                              //$CD:20021206
    else if (!strcmp(argv[1],"iso-8859-5"))                            //$CD:20021206
        usage_iso88595(1);                                              //$CD:20021206
    else if (!strcmp(argv[1],"iso-8859-7"))                            //$CD:20021206
        usage_iso88597(1);                                              //$CD:20021206
    else if (!strcmp(argv[1],"iso-8859-9"))                            //$CD:20021206
        usage_iso88599(1);                                              //$CD:20021206
    else if (!strcmp(argv[1],"iso-8859-10"))                           //$CD:20021206
        usage_iso885910(1);                                             //$CD:20021206
    else usage(1);                                                      //$CD:20021206
   return 0;                                                           //$CD:20021206
}                                                                      //$CD:20021206
if (!strcmp(argv[1],"FRENCH") ||
    !strcmp(argv[1],"ENGLISH") ||
    !strcmp(argv[1],"GERMAN") ||
    !strcmp(argv[1],"SPANISH") ||
    !strcmp(argv[1],"PORTUGUESE") ||
    !strcmp(argv[1],"ITALIAN") ||
    !strcmp(argv[1],"NORWEGIAN") ||
    !strcmp(argv[1],"LATIN") ||                                        //$CD:20021206
    !strcmp(argv[1],"windows-1252"))                                   //$CD:20021206
   init_ansi();
else if (!strcmp(argv[1],"THAI"))
     init_thai();
else if (!strcmp(argv[1],"GREEK"))
     init_grec();
else if (!strcmp(argv[1],"CZECH"))
     init_tcheque();
else if (!strcmp(argv[1],"windows-1250"))                              //$CD:20021206
     init_windows1250();                                               //$CD:20021206
else if (!strcmp(argv[1],"windows-1257"))                              //$CD:20021206
     init_windows1257();                                               //$CD:20021206
else if (!strcmp(argv[1],"windows-1251"))                              //$CD:20021206
     init_windows1251();                                               //$CD:20021206
else if (!strcmp(argv[1],"windows-1254"))                              //$CD:20021206
     init_windows1254();                                               //$CD:20021206
else if (!strcmp(argv[1],"windows-1258"))                              //$CD:20021206
     init_windows1258();                                               //$CD:20021206
else if (!strcmp(argv[1],"iso-8859-1"))                                //$CD:20021206
     init_iso88591();                                                  //$CD:20021206
else if (!strcmp(argv[1],"iso-8859-15"))                               //$CD:20021206
     init_iso885915();                                                 //$CD:20021206
else if (!strcmp(argv[1],"iso-8859-2"))                                //$CD:20021206
     init_iso88592();                                                  //$CD:20021206
else if (!strcmp(argv[1],"iso-8859-3"))                                //$CD:20021206
     init_iso88593();                                                  //$CD:20021206
else if (!strcmp(argv[1],"iso-8859-4"))                                //$CD:20021206
     init_iso88594();                                                  //$CD:20021206
else if (!strcmp(argv[1],"iso-8859-5"))                                //$CD:20021206
     init_iso88595();                                                  //$CD:20021206
else if (!strcmp(argv[1],"iso-8859-7"))                                //$CD:20021206
     init_iso88597();                                                  //$CD:20021206
else if (!strcmp(argv[1],"iso-8859-9"))                                //$CD:20021206
     init_iso88599();                                                  //$CD:20021206
else if (!strcmp(argv[1],"iso-8859-10"))                               //$CD:20021206
     init_iso885910();                                                 //$CD:20021206

else {
     fprintf(stderr,"Invalid language parameter %s\n",argv[1]);
     return 1;
}

int REPLACE=0;
if (!strcmp(argv[2],"-r")) REPLACE=1;


for (i=2+REPLACE;i<argc;i++) {
    strcpy(res,argv[i]);
    strcat(res,".uni");
    if (u_is_a_unicode_file(argv[i])) {
       // we don't try to convert the file if
       // it is already a unicode one 
       fprintf(stderr,"%s is already a Unicode file\n",argv[i]);
    }
    else {
       entree=fopen(argv[i],"rb");
       sortie=fopen(res,"wb");
       int problem=0;
       if (entree==NULL) {
          fprintf(stderr,"Cannot open %s\n",argv[i]);
          problem=1;
       }
       if (sortie==NULL) {
          fprintf(stderr,"Cannot write %s\n",res);
          problem=1;
          fclose(entree);
       }
       if (!problem) {
          fprintf(sortie,"%c%c",0xFF,0xFE);
          convert_ascii_to_unicode(entree,sortie);
          fclose(entree);
          fclose(sortie);
          if (REPLACE) {
             remove(argv[i]);
             rename(res,argv[i]);
          }
          printf("%s converted\n%s",argv[i],CR);
       } else {
          remove(res);
       }
    }
}
return 0;
}



