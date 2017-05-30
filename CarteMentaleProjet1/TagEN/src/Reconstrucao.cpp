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
#include "Alphabet.cpp"
#include "DELA.cpp"
#include "FileName.cpp"
#include "Copyright.h"
#include "Matches.cpp"
#include "PortugueseNormalization.cpp"
#include "String_hash.cpp"
#include "String_list.cpp"
#include "Sentence_to_grf.cpp"
#include "AutomateFst2.cpp"
#include "Normalization_transducer.cpp"
#include "Text_tokens.cpp"
#include "Liste_nombres.cpp"

//---------------------------------------------------------------------------

//
// "E:\My Unitex\Portuguese\Alphabet.txt" "E:\My Unitex\Portuguese\Corpus\Os Pobres_snt\\concord.ind" "E:\Unitex\Portuguese\Dela\Raiz.bin" "E:\Unitex\Portuguese\Dela\FuturoCondicional.bin" "E:\NormalizePronouns.fst2" "E:\NasalSuffixPronouns.fst2" "E:\N.grf"
//

//---------------------------------------------------------------------------
void usage() {
printf("%s",COPYRIGHT);
printf("Usage: Reconstrucao <alph> <list> <root> <dic> <pro> <nasalpro> <res>\n%s",CR);
printf("   <alph> : the alphabet file to use\n%s",CR);
printf("   <list> : the match list that describes the forms to be normalized. This\n%s",CR);
printf("            list must have been computed in MERGE or REPLACE mode.\n%s",CR);
printf("   <root> : the .bin dictionary containing the radical forms\n%s",CR);
printf("   <dic> : the .bin dictionary containing the complete forms\n%s",CR);
printf("   <pro> : the .fst2 grammar describing pronoun rewriting rules\n%s",CR);
printf("   <nasalpro> : the .fst2 grammar describing nasal pronoun rewriting rules\n%s",CR);
printf("   <res> :  the name of the .grf graph to be generated\n\n%s",CR);
printf("Takes a list of multi-part verbs and creates an apropriate normalization\n%sgrammar.\n%s",CR,CR);
}



int main(int argc, char **argv) {
if (argc!=8) {
   usage();
   return 0;
}
printf("Loading alphabet...\n%s",CR);
Alphabet* alph=load_alphabet(argv[1]);
if (alph==NULL) {
   fprintf(stderr,"Cannot load alphabet file %s\n",argv[1]);
   return 1;
}
printf("Loading match list...\n%s",CR);
FILE* f_list=u_fopen(argv[2],U_READ);
if (f_list==NULL) {
   fprintf(stderr,"Cannot load match list %s\n",argv[2]);
   free_alphabet(alph);
   return 1;
}
int TRANSDUCTION_MODE;
struct liste_matches* list=load_match_list(f_list,&TRANSDUCTION_MODE);
u_fclose(f_list);
if (TRANSDUCTION_MODE==IGNORE_TRANSDUCTIONS) {
   fprintf(stderr,"Invalid match list %s\n",argv[2]);
   free_alphabet(alph);
   return 1;
}
printf("Loading radical form dictionary...\n%s",CR);
unsigned char* root_bin=load_BIN_file(argv[3]);
if (root_bin==NULL) {
   fprintf(stderr,"Cannot load radical form dictionary %s\n",argv[3]);
   free_alphabet(alph);
   return 1;
}
char root_inf_file[2000];
name_without_extension(argv[3],root_inf_file);
strcat(root_inf_file,".inf");
struct INF_codes* root_inf=load_INF_file(root_inf_file);
if (root_bin==NULL) {
   fprintf(stderr,"Cannot load radical form dictionary %s\n",root_inf_file);
   free_alphabet(alph);
   free(root_bin);
   return 1;
}
printf("Loading inflected form dictionary...\n%s",CR);
unsigned char* inflected_bin=load_BIN_file(argv[4]);
if (inflected_bin==NULL) {
   fprintf(stderr,"Cannot load inflected form dictionary %s\n",argv[4]);
   free_alphabet(alph);
   free(root_bin);
   free_INF_codes(root_inf);
   return 1;
}
char inflected_inf_file[2000];
name_without_extension(argv[4],inflected_inf_file);
strcat(inflected_inf_file,".inf");
struct INF_codes* inflected_inf=load_INF_file(inflected_inf_file);
if (inflected_inf==NULL) {
   fprintf(stderr,"Cannot load inflected form dictionary %s\n",inflected_inf_file);
   free_alphabet(alph);
   free(root_bin);
   free(inflected_bin);
   free_INF_codes(root_inf);
   return 1;
}
printf("Loading pronoun rewriting rule grammar...\n%s",CR);
struct noeud_arbre_normalization* rewriting_rules=load_normalization_transducer_string(argv[5]);
if (rewriting_rules==NULL) {
   fprintf(stderr,"Cannot load pronoun rewriting grammar %s\n",argv[5]);
   free_alphabet(alph);
   free(root_bin);
   free(inflected_bin);
   free_INF_codes(root_inf);
   free_INF_codes(inflected_inf);
   return 1;
}
printf("Loading nasal pronoun rewriting rule grammar...\n%s",CR);
struct noeud_arbre_normalization* nasal_rewriting_rules=load_normalization_transducer_string(argv[6]);
if (rewriting_rules==NULL) {
   fprintf(stderr,"Cannot load nasal pronoun rewriting grammar %s\n",argv[6]);
   free_alphabet(alph);
   free(root_bin);
   free(inflected_bin);
   free_INF_codes(root_inf);
   free_INF_codes(inflected_inf);
   free_noeud_arbre_normalization(rewriting_rules);
   return 1;
}


printf("Constructing normalization grammar...\n%s",CR);
build_portuguese_normalization_grammar(alph,list,root_bin,root_inf,inflected_bin,inflected_inf,argv[7],
                                       rewriting_rules,nasal_rewriting_rules);

free_alphabet(alph);
free(root_bin);
free_INF_codes(root_inf);
free(inflected_bin);
free_INF_codes(inflected_inf);
free_noeud_arbre_normalization(rewriting_rules);
free_noeud_arbre_normalization(nasal_rewriting_rules);
return 0;
}
//---------------------------------------------------------------------------


