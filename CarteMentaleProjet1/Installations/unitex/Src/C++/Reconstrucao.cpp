/*
 * Unitex
 *
 * Copyright (C) 2001-2012 Université Paris-Est Marne-la-Vallée <unitex@univ-mlv.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Unicode.h"
#include "Alphabet.h"
#include "DELA.h"
#include "AbstractDelaLoad.h"
#include "File.h"
#include "Copyright.h"
#include "LocateMatches.h"
#include "PortugueseNormalization.h"
#include "String_hash.h"
#include "Sentence2Grf.h"
#include "Fst2.h"
#include "NormalizationFst2.h"
#include "Text_tokens.h"
#include "List_int.h"
#include "Error.h"
#include "UnitexGetOpt.h"
#include "Reconstrucao.h"
#include "CompressedDic.h"

#ifndef HAS_UNITEX_NAMESPACE
#define HAS_UNITEX_NAMESPACE 1
#endif

namespace unitex {

const char* usage_Reconstrucao =
         "Usage: Reconstrucao [OPTIONS] <index>\n"
         "\n"
         "  <index> : the match list that describes the forms to be normalized. This\n"
         "            list must have been computed by Locate in MERGE or REPLACE mode.\n"
         "\n"
         "OPTIONS:\n"
         "  -a ALPH/--alphabet=ALPH: the alphabet file to use\n"
         "  -r ROOT/--root=ROOT: the .bin dictionary containing the radical forms\n"
         "  -d BIN/--dictionary=BIN: the .bin dictionary containing the complete forms\n"
         "  -p PRO/--pronoun_rules=PRO: the .fst2 grammar describing pronoun rewriting rules\n"
         "  -n NAS/--nasal_pronoun_rules=NAS: the .fst2 grammar describing nasal pronoun rewriting rules\n"
         "  -o OUT/--output=OUT:  the name of the .grf graph to be generated\n"
         "  -h/--help: this help\n"
         "\n"
         "Takes a list of multi-part verbs and creates an apropriate normalization grammar.\n";


static void usage() {
u_printf("%S",COPYRIGHT);
u_printf(usage_Reconstrucao);
}


const char* optstring_Reconstrucao=":a:r:d:p:n:o:hk:q:";
const struct option_TS lopts_Reconstrucao[]= {
      {"alphabet",required_argument_TS,NULL,'a'},
      {"root",required_argument_TS,NULL,'r'},
      {"dictionary",required_argument_TS,NULL,'d'},
      {"pronoun_rules",required_argument_TS,NULL,'p'},
      {"nasal_pronoun_rules",required_argument_TS,NULL,'n'},
      {"output",required_argument_TS,NULL,'o'},
      {"input_encoding",required_argument_TS,NULL,'k'},
      {"output_encoding",required_argument_TS,NULL,'q'},
      {"help",no_argument_TS,NULL,'h'},
      {NULL,no_argument_TS,NULL,0}
};


int main_Reconstrucao(int argc,char* const argv[]) {
if (argc==1) {
   usage();
   return 0;
}

char alphabet[FILENAME_MAX]="";
char root[FILENAME_MAX]="";
char dictionary[FILENAME_MAX]="";
char pronoun_rules[FILENAME_MAX]="";
char nasal_pronoun_rules[FILENAME_MAX]="";
char output[FILENAME_MAX]="";
VersatileEncodingConfig vec=VEC_DEFAULT;
int val,index=-1;
struct OptVars* vars=new_OptVars();
while (EOF!=(val=getopt_long_TS(argc,argv,optstring_Reconstrucao,lopts_Reconstrucao,&index,vars))) {
   switch(val) {
   case 'a': if (vars->optarg[0]=='\0') {
                fatal_error("You must specify a non empty alphabet file name\n");
             }
             strcpy(alphabet,vars->optarg);
             break;
   case 'r': if (vars->optarg[0]=='\0') {
                fatal_error("You must specify a non empty root dictionary file name\n");
             }
             strcpy(root,vars->optarg);
             break;
   case 'd': if (vars->optarg[0]=='\0') {
                fatal_error("You must specify a non empty dictionary file name\n");
             }
             strcpy(dictionary,vars->optarg);
             break;
   case 'p': if (vars->optarg[0]=='\0') {
                fatal_error("You must specify a non empty pronoun rewriting rule file name\n");
             }
             strcpy(pronoun_rules,vars->optarg);
             break;
   case 'n': if (vars->optarg[0]=='\0') {
                fatal_error("You must specify a non empty nasal pronoun rewriting rule file name\n");
             }
             strcpy(nasal_pronoun_rules,vars->optarg);
             break;
   case 'o': if (vars->optarg[0]=='\0') {
                fatal_error("You must specify a non empty output file name\n");
             }
             strcpy(output,vars->optarg);
             break;
   case 'k': if (vars->optarg[0]=='\0') {
                fatal_error("Empty input_encoding argument\n");
             }
             decode_reading_encoding_parameter(&(vec.mask_encoding_compatibility_input),vars->optarg);
             break;
   case 'q': if (vars->optarg[0]=='\0') {
                fatal_error("Empty output_encoding argument\n");
             }
             decode_writing_encoding_parameter(&(vec.encoding_output),&(vec.bom_output),vars->optarg);
             break;
   case 'h': usage(); return 0;
   case ':': if (index==-1) fatal_error("Missing argument for option -%c\n",vars->optopt);
             else fatal_error("Missing argument for option --%s\n",lopts_Reconstrucao[index].name);
   case '?': if (index==-1) fatal_error("Invalid option -%c\n",vars->optopt);
             else fatal_error("Invalid option --%s\n",vars->optarg);
             break;
   }
   index=-1;
}

if (vars->optind!=argc-1) {
   fatal_error("Invalid arguments: rerun with --help\n");
}

if (root[0]=='\0') {
   fatal_error("You must specify the root .bin dictionary\n");
}
if (dictionary[0]=='\0') {
   fatal_error("You must specify the .bin dictionary to use\n");
}
if (pronoun_rules[0]=='\0') {
   fatal_error("You must specify the pronoun rule file\n");
}
if (nasal_pronoun_rules[0]=='\0') {
   fatal_error("You must specify the nasal pronoun rule file\n");
}
if (output[0]=='\0') {
   fatal_error("You must specify the output dictionary file name\n");
}

Alphabet* alph=NULL;
if (alphabet[0]!='\0') {
   u_printf("Loading alphabet...\n");
   alph=load_alphabet(&vec,alphabet);
   if (alph==NULL) {
      fatal_error("Cannot load alphabet file %s\n",alphabet);
      return 1;
   }
}
u_printf("Loading match list...\n");
U_FILE* f_list=u_fopen(&vec,argv[vars->optind],U_READ);
if (f_list==NULL) {
   error("Cannot load match list %s\n",argv[vars->optind]);
   free_alphabet(alph);
   return 1;
}
OutputPolicy output_policy;
struct match_list* list=load_match_list(f_list,&output_policy,NULL);
u_fclose(f_list);
if (output_policy==IGNORE_OUTPUTS) {
   error("Invalid match list %s\n",argv[vars->optind]);
   free_alphabet(alph);
   return 1;
}
char root_inf_file[FILENAME_MAX];
remove_extension(root,root_inf_file);
strcat(root_inf_file,".inf");
u_printf("Loading radical form dictionary...\n");
Dictionary* root_dic=new_Dictionary(&vec,root,root_inf_file);
if ((*root)=='\0') {
	free_alphabet(alph);
	return 1;
}
u_printf("Loading inflected form dictionary...\n");
char inflected_inf_file[FILENAME_MAX];
remove_extension(dictionary,inflected_inf_file);
strcat(inflected_inf_file,".inf");
Dictionary* inflected_dic=new_Dictionary(&vec,dictionary,inflected_inf_file);
if (inflected_dic==NULL) {
	free_Dictionary(root_dic);
	free_alphabet(alph);
	return 1;
}
u_printf("Loading pronoun rewriting rule grammar...\n");
struct normalization_tree* rewriting_rules=load_normalization_transducer_string(&vec,pronoun_rules);
if (rewriting_rules==NULL) {
   error("Cannot load pronoun rewriting grammar %s\n",pronoun_rules);
   free_alphabet(alph);
   free_Dictionary(root_dic);
   free_Dictionary(inflected_dic);
   return 1;
}
u_printf("Loading nasal pronoun rewriting rule grammar...\n");
struct normalization_tree* nasal_rewriting_rules=load_normalization_transducer_string(&vec,nasal_pronoun_rules);
if (rewriting_rules==NULL) {
   error("Cannot load nasal pronoun rewriting grammar %s\n",nasal_pronoun_rules);
   free_alphabet(alph);
   free_Dictionary(root_dic);
   free_Dictionary(inflected_dic);
   free_normalization_tree(rewriting_rules);
   return 1;
}
u_printf("Constructing normalization grammar...\n");
build_portuguese_normalization_grammar(alph,list,root_dic,inflected_dic,output,
                                       &vec,rewriting_rules,nasal_rewriting_rules);
free_alphabet(alph);
free_Dictionary(root_dic);
free_Dictionary(inflected_dic);
free_normalization_tree(rewriting_rules);
free_normalization_tree(nasal_rewriting_rules);
free_OptVars(vars);
return 0;
}

} // namespace unitex

