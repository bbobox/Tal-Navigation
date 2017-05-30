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
// ENGLISH "c:\unitex-visiteur\english\corpus\ivanhoe_snt\text.mft"
//


//
// the usage function is in the file CodePages.cpp
//


int main(int argc, char **argv) {
FILE* entree;
FILE* sortie;
char res[1000];
int i;
int UTF8=0;
if (argc<2) {                                                          //$CD:20021206
   usage(0);
   return 0;
}
else if (argc<3) {                                                     //$CD:20021206
    if (!strcmp(argv[1],"LATIN") || !strcmp(argv[1],"windows-1252"))   //$CD:20021206
        usage_LATIN(0);                                                 //$CD:20021206
    else if (!strcmp(argv[1],"windows-1250"))                          //$CD:20021206
        usage_windows1250(0);                                           //$CD:20021206
    else if (!strcmp(argv[1],"windows-1257"))                          //$CD:20021206
        usage_windows1257(0);                                           //$CD:20021206
    else if (!strcmp(argv[1],"windows-1251"))                          //$CD:20021206
        usage_windows1251(0);                                           //$CD:20021206
    else if (!strcmp(argv[1],"windows-1254"))                          //$CD:20021206
        usage_windows1254(0);                                           //$CD:20021206
    else if (!strcmp(argv[1],"windows-1258"))                          //$CD:20021206
        usage_windows1258(0);                                           //$CD:20021206
    else if (!strcmp(argv[1],"iso-8859-1"))                            //$CD:20021206
        usage_iso88591(0);                                              //$CD:20021206
    else if (!strcmp(argv[1],"iso-8859-15"))                           //$CD:20021206
        usage_iso885915(0);                                             //$CD:20021206
    else if (!strcmp(argv[1],"iso-8859-2"))                            //$CD:20021206
        usage_iso88592(0);                                              //$CD:20021206
    else if (!strcmp(argv[1],"iso-8859-3"))                            //$CD:20021206
        usage_iso88593(0);                                              //$CD:20021206
    else if (!strcmp(argv[1],"iso-8859-4"))                            //$CD:20021206
        usage_iso88594(0);                                              //$CD:20021206
    else if (!strcmp(argv[1],"iso-8859-5"))                            //$CD:20021206
        usage_iso88595(0);                                              //$CD:20021206
    else if (!strcmp(argv[1],"iso-8859-7"))                            //$CD:20021206
        usage_iso88597(0);                                              //$CD:20021206
    else if (!strcmp(argv[1],"iso-8859-9"))                            //$CD:20021206
        usage_iso88599(0);                                              //$CD:20021206
    else if (!strcmp(argv[1],"iso-8859-10"))                           //$CD:20021206
        usage_iso885910(0);                                             //$CD:20021206
    else usage(0);                                                      //$CD:20021206
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
else if (!strcmp(argv[1],"UTF-8")) {
     UTF8=1;
}
else {
     fprintf(stderr,"Invalid language parameter %s\n",argv[1]);
     return 1;
}

if (!UTF8) {
   create_uni2asc_code_page_array();
}

int REPLACE=0;
if (!strcmp(argv[2],"-r")) REPLACE=1;

for (i=2+REPLACE;i<argc;i++) {
    strcpy(res,argv[i]);
    if (!UTF8) {
       strcat(res,".ascii");
    } else {
       strcat(res,".utf8");
    }
    entree=u_fopen(argv[i],U_READ);
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
       if (!UTF8) {
          convert_unicode_to_ascii(entree,sortie);
       } else {
          convert_unicode_to_UTF_8(entree,sortie);
       }
       u_fclose(entree);
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
return 0;
}




