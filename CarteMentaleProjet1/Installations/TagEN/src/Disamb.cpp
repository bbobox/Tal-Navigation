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
#include "unicode.cpp"
#include "AutomateFst2.cpp"
#include "LiberationFst2.cpp"
#include "Alphabet.cpp"
#include "Contraintes.cpp"
#include "Copyright.h"
//---------------------------------------------------------------------------


//
// "e:\my unitex\french\corpus\femme_snt\text.fst2" "e:\my unitex\french\disamb\disamb.fst2" "e:\my unitex\french\alphabet.txt"
//
// "c:\unitex-visiteur\portuguese\corpus\Os Pobres_snt\text.fst2" "c:\unitex-visiteur\portuguese\disamb\disamb.fst2" "c:\unitex-visiteur\portuguese\alphabet.txt"
//


//---------------------------------------------------------------------------
void usage() {
printf("%s",COPYRIGHT);
printf("Usage: Disamb <text FST> <fst2> <alphabet>\n%s",CR);
printf("     <text FST> : the text automaton\n%s",CR);
printf("     <fst2> : the grammar to be applied to the text automaton\n%s",CR);
printf("     <alphabet> : the alphabet file for the current language.\n\n%s",CR);
printf("Applies a disambiguation grammar to a text automaton. The text\n%s",CR);
printf("automaton file is modified.\n%s",CR);
}


int main(int argc, char **argv) {
if (argc!=4) {
  usage();
  return 0;
}
Automate_fst2* text;
Automate_fst2* grammar;
Alphabet* alph;
printf("Loading text automaton...\n%s",CR);
text=load_fst2(argv[1],1);
if (text==NULL) {
   fprintf(stderr,"Cannot load text automaton %s\n",argv[1]);
   return 1;
}
printf("Loading disambiguation grammar...\n%s",CR);
grammar=load_fst2(argv[2],0);
if (grammar==NULL) {
   fprintf(stderr,"Cannot load grammar %s\n",argv[2]);
   return 1;
}
printf("Loading alphabet...\n%s",CR);
alph=load_alphabet(argv[3]);
if (alph==NULL) {
   fprintf(stderr,"Cannot load alphabet file %s\n",argv[3]);
   return 1;
}
printf("Preparing tags for intersection...\n%s",CR);
contrainte1=allouer_contraintes(grammar->nombre_etiquettes);
contrainte2=allouer_contraintes(grammar->nombre_etiquettes);
etiq=allouer_contraintes(text->nombre_etiquettes);
calculer_contraintes(text,grammar);

free_fst2(text);
free_fst2(grammar);
free_alphabet(alph);
  // a virer
  printf("fini");
  getchar();
  //
return 0;
}



