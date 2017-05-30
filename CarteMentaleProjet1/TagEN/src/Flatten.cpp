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
#include "Copyright.h"
#include "unicode.cpp"
#include "AutomateFst2.cpp"
#include "Grf2Fst2_lib.cpp"
#include "FlattenFst2.cpp"
#include "Liste_nombres.cpp"


//---------------------------------------------------------------------------
//
// "e:\my unitex\French\graphs\flatten.fst2" FST
//
//---------------------------------------------------------------------------
void usage() {
printf("%s",COPYRIGHT);
printf("Usage: Flatten <fst2> <type> [depth]\n%s",CR);
printf("     <fst2> : compiled grammar to flatten;\n%s",CR);
printf("     <type> : this parameter specifies the type of the resulting grammar\n%s",CR);
printf("              The 2 possibles values are:\n%s",CR);
printf("              FST : if the grammar is not a finite-state one, the program\n%s",CR);
printf("                    makes a finite-state approximation of it. The resulting\n%s",CR);
printf("                    FST2 will contain only one graph.\n%s",CR);
printf("              RTN : the grammar will be flattened according to the depth limit.\n%s",CR);
printf("                    The resulting grammar may not be finite-state.\n%s",CR);
printf("     [depth] : maximum subgraph depth to be flattened. If this parameter is\n%s",CR);
printf("               not precised, the value 10 is taken by default.\n%s",CR);
printf("\n\n%s",CR);
printf("Flattens a FST2 grammar into a finite state transducer in the limit of\n%s",CR);
printf("a recursion depth. The grammar <fst2> is replaced by its flattened equivalent.\n%s",CR);
printf("If the flattening process is complete, the resulting grammar contains only one\n%s",CR);
printf("graph.\n%s",CR);
}

int main(int argc, char **argv) {
if ((argc<3) || (argc>4)) {
   usage();
   return 0;
}
int RTN;
if (!strcmp(argv[2],"RTN")) {
   RTN=1;
}
else if (!strcmp(argv[2],"FST")) {
   RTN=0;
}
else {
   fprintf(stderr,"Invalid parameter: %s\n",argv[2]);
   return 1;
}
int depth=10;
if (argc==4) {
   if (1!=sscanf(argv[3],"%d",&depth) || (depth<1)) {
      fprintf(stderr,"Invalid depth parameter %s\n",argv[3]);
      return 1;
   }
}
printf("Loading %s...\n%s",argv[1],CR);
Automate_fst2* origin=load_fst2(argv[1],1);
if (origin==NULL) {
   fprintf(stderr,"Cannot load %s\n",argv[1]);
   return 1;
}

char temp[2000];
strcpy(temp,argv[1]);
strcat(temp,".tmp");

switch (flatten_fst2(origin,depth,temp,RTN)) {
   case EQUIVALENT_FST: printf("The resulting grammar is an equivalent finite-state transducer.\n%s",CR);
                        break;
   case APPROXIMATIVE_FST: printf("The resulting grammar is a finite-state approximation.\n%s",CR);
                        break;
   case EQUIVALENT_RTN: printf("The resulting grammar is an equivalent FST2.\n%s",CR);
                        break;
   default:;
}
free_fst2(origin);
remove(argv[1]);
rename(temp,argv[1]);
return 0;
}
//---------------------------------------------------------------------------
