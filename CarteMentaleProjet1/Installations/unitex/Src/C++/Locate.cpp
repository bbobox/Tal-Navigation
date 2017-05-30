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
#include <time.h>

#include "Unicode.h"
#include "Alphabet.h"
#include "DELA.h"
#include "String_hash.h"
#include "LocatePattern.h"
#include "Fst2.h"
#include "Text_tokens.h"
#include "List_int.h"
#include "CompoundWordTree.h"
#include "Text_parsing.h"
#include "LocateMatches.h"
#include "TransductionVariables.h"
#include "TransductionStack.h"
#include "ParsingInfo.h"
#include "File.h"
#include "Copyright.h"
#include "Locate.h"
#include "Error.h"
#include "UnitexGetOpt.h"
#include "ProgramInvoker.h"



#ifndef HAS_UNITEX_NAMESPACE
#define HAS_UNITEX_NAMESPACE 1
#endif

namespace unitex {

const char* usage_Locate =
         "Usage: Locate [OPTIONS] <fst2>\n"
         "\n"
         "  <fst2>: the grammar to be applied\n"
         "\n"
         "OPTIONS:\n"
         "  -t TXT/--text=TXT: the .snt text file\n"
         "  -a ALPH/--alphabet=ALPH: the language alphabet file\n"
         "  -m DIC/--morpho=DIC: specifies that DIC is a .bin dictionary\n"
         "                       to use in morphological mode. Use as many\n"
         "                       -m XXX as there are .bin to use. You can also\n"
		 "                       separate several .bin with semi-colons.\n"
         "  -s/--start_on_space: enables morphological use of space\n"
         "  -x/--dont_start_on_space: disables morphological use of space (default)\n"
         "  -c/--char_by_char: uses char by char tokenization; useful for languages like Thai\n"
         "  -w/--word_by_word: uses word by word tokenization (default)\n"
         "  -d X/--sntdir=X: uses directory X instead of the text directory; note that X must be\n"
         "                   (back)slash terminated\n"
         "  -K/--korean: tells Locate that it works on Korean\n"
         "  -u X/--arabic_rules=X: Arabic typographic rule configuration file\n"
         "  -g minus/--negation_operator=minus: uses minus as negation operator for Unitex 2.0 graphs\n"
         "  -g tilde/--negation_operator=tilde: uses tilde as negation operator (default)\n"
         "\n"
         "Search limit options:\n"
         "  -l/--all: looks for all matches (default)\n"
         "  -n N/--number_of_matches=N: stops after the first N matches\n"
         "\n"
         "Maximum iterations per token options:\n"
         "  -o N/--stop_token_count=N : stops after N iterations on a token\n"
         "  -o N,M/--stop_token_count=N,M : emits a warning after N iterations on\n"
         "                                    a token and stops after M iterations\n"
         "\n"
         "Matching mode options:\n"
         "  -S/--shortest_matches\n"
         "  -L/--longest_matches (default)\n"
         "  -A/--all_matches\n"
         "\n"
         "Output options:\n"
         "  -I/--ignore (default)\n"
         "  -M/--merge\n"
         "  -R/--replace\n"
         "  -p/--protect_dic_chars: when -M or -R mode is used, -p protects some input characters\n"
		 "                          with a backslash. This is useful when Locate is called by Dico\n"
		 "                          in order to avoid producing bad lines like \"3,14,.PI.NUM\"\n"
         "\n"
         "Ambiguous output options:\n"
         "  -b/--ambiguous_outputs: allows the production of several matches with same input\n"
         "                          but different outputs (default)\n"
         "  -z/--no_ambiguous_outputs: forbids ambiguous outputs\n"
         "\n"
         "Variable error options:\n"
         "These options have no effect if the output mode is --ignore; otherwise, they rule\n"
         "the behavior of the Locate program when an output is found that contains a reference\n"
         "to a variable that is not correctly defined.\n"
         "  -X/--exit_on_variable_error: kills the program\n"
         "  -Y/--ignore_variable_errors: acts as if the variable has an empty content (default)\n"
         "  -Z/--backtrack_on_variable_errors: stop exploring the current path of the grammar\n"
         "\n"
		 "Variable injection:\n"
		 "  -v X=Y/--variable=X=Y: sets an output variable named X with content Y. Note that\n"
		 "                         Y must be ASCII\n"
		 "\n"
		 "  -h/--help: this help\n"
         "\n"
         "Applies a grammar to a text, and saves the matching sequence index in a\n"
         "file named \"concord.ind\" stored in the text directory. A result info file\n"
         "named \"concord.n\" is also saved in the same directory.\n";


static void usage() {
u_printf("%S",COPYRIGHT);
u_printf(usage_Locate);
#ifndef REGEX_FACADE_ENGINE
   error("\nWARNING: on this system, morphological filters will not be taken into account,\n");
   error("         because wide characters are not supported\n");
#endif
}


const char* optstring_Locate=":t:a:m:SLAIMRXYZln:d:cewsxbzpKhk:q:o:u:g:Tv:";
const struct option_TS lopts_Locate[]= {
      {"text",required_argument_TS,NULL,'t'},
      {"alphabet",required_argument_TS,NULL,'a'},
      {"morpho",required_argument_TS,NULL,'m'},
      {"shortest_matches",no_argument_TS,NULL,'S'},
      {"longest_matches",no_argument_TS,NULL,'L'},
      {"all_matches",no_argument_TS,NULL,'A'},
      {"ignore",no_argument_TS,NULL,'I'},
      {"merge",no_argument_TS,NULL,'M'},
      {"replace",no_argument_TS,NULL,'R'},
      {"exit_on_variable_error",no_argument_TS,NULL,'X'},
      {"ignore_variable_errors",no_argument_TS,NULL,'Y'},
      {"backtrack_on_variable_errors",no_argument_TS,NULL,'Z'},
      {"all",no_argument_TS,NULL,'l'},
      {"number_of_matches",required_argument_TS,NULL,'n'},
      {"sntdir",required_argument_TS,NULL,'d'},
      {"char_by_char",no_argument_TS,NULL,'c'},
      {"word_by_word",no_argument_TS,NULL,'w'},
      {"start_on_space",no_argument_TS,NULL,'s'},
      {"dont_start_on_space",no_argument_TS,NULL,'x'},
      {"ambiguous_outputs",no_argument_TS,NULL,'b'},
      {"no_ambiguous_outputs",no_argument_TS,NULL,'z'},
      {"protect_dic_chars",no_argument_TS,NULL,'p'},
      {"korean",no_argument_TS,NULL,'K'},
      {"stop_token_count",required_argument_TS,NULL,'o'},
      {"input_encoding",required_argument_TS,NULL,'k'},
      {"output_encoding",required_argument_TS,NULL,'q'},
      {"arabic_rules",required_argument_TS,NULL,'u'},
      {"negation_operator",required_argument_TS,NULL,'g'},
      {"dont_use_locate_cache",no_argument_TS,NULL,'e'},
      {"dont_allow_trace",no_argument_TS,NULL,'T'},
      {"variable",required_argument_TS,NULL,'v'},
      {"help",no_argument_TS,NULL,'h'},
      {NULL,no_argument_TS,NULL,0}
};


/*
 * This function behaves in the same way that a main one, except that it does
 * not invoke the setBufferMode function.
 */
int main_Locate(int argc,char* const argv[]) {
if (argc==1) {
   usage();
   return 0;
}
int val,index=-1;
char alph[FILENAME_MAX]="";
char text[FILENAME_MAX]="";
char dynamicSntDir[FILENAME_MAX]="";
char arabic_rules[FILENAME_MAX]="";
char* morpho_dic=NULL;
MatchPolicy match_policy=LONGEST_MATCHES;
OutputPolicy output_policy=IGNORE_OUTPUTS;
int search_limit=NO_MATCH_LIMIT;
TokenizationPolicy tokenization_policy=WORD_BY_WORD_TOKENIZATION;
SpacePolicy space_policy=DONT_START_WITH_SPACE;
AmbiguousOutputPolicy ambiguous_output_policy=ALLOW_AMBIGUOUS_OUTPUTS;
VariableErrorPolicy variable_error_policy=IGNORE_VARIABLE_ERRORS;
int protect_dic_chars=0;
int is_korean=0;
int max_count_call=0;
int max_count_call_warning=0;
int tilde_negation_operator=1;
int useLocateCache=1;
int selected_negation_operator=0;
int allow_trace=1;
char foo;
vector_ptr* injected_vars=new_vector_ptr();
VersatileEncodingConfig vec=VEC_DEFAULT;
struct OptVars* vars=new_OptVars();
while (EOF!=(val=getopt_long_TS(argc,argv,optstring_Locate,lopts_Locate,&index,vars))) {
   switch(val) {
   case 't': if (vars->optarg[0]=='\0') {
                fatal_error("You must specify a non empty text file name\n");
             }
             strcpy(text,vars->optarg);
             break;
   case 'a': if (vars->optarg[0]=='\0') {
                fatal_error("You must specify a non empty alphabet name\n");
             }
             strcpy(alph,vars->optarg);
             break;
   case 'm': if (vars->optarg[0]!='\0') {
                if (morpho_dic==NULL) {
                  morpho_dic=strdup(vars->optarg);
                  if (morpho_dic==NULL) {
                     fatal_alloc_error("main_Locate");
                  }
                }
                else
                {
                    morpho_dic = (char*)realloc((void*)morpho_dic,strlen(morpho_dic)+strlen(vars->optarg)+2);
                    if (morpho_dic==NULL) {
                       fatal_alloc_error("main_Locate");
                    }
                    strcat(morpho_dic,";");
                    strcat(morpho_dic,vars->optarg);
                }
             }
             break;
   case 'g': if (vars->optarg[0]=='\0') {
                fatal_error("You must specify an argument for negation operator\n");
             }
             selected_negation_operator=1;
             if ((strcmp(vars->optarg,"minus")==0) || (strcmp(vars->optarg,"-")==0))
             {
                 tilde_negation_operator=0;
             }
             else
             if ((strcmp(vars->optarg,"tilde")!=0) && (strcmp(vars->optarg,"~")!=0))
             {
                 fatal_error("You must specify a valid argument for negation operator\n");
             }
             break;
   case 'S': match_policy=SHORTEST_MATCHES; break;
   case 'L': match_policy=LONGEST_MATCHES; break;
   case 'A': match_policy=ALL_MATCHES; break;
   case 'I': output_policy=IGNORE_OUTPUTS; break;
   case 'M': output_policy=MERGE_OUTPUTS; break;
   case 'R': output_policy=REPLACE_OUTPUTS; break;
   case 'X': variable_error_policy=EXIT_ON_VARIABLE_ERRORS; break;
   case 'Y': variable_error_policy=IGNORE_VARIABLE_ERRORS; break;
   case 'Z': variable_error_policy=BACKTRACK_ON_VARIABLE_ERRORS; break;
   case 'l': search_limit=NO_MATCH_LIMIT; break;
   case 'e': useLocateCache=0; break;
   case 'T': allow_trace=0; break;
   case 'n': if (1!=sscanf(vars->optarg,"%d%c",&search_limit,&foo) || search_limit<=0) {
                /* foo is used to check that the search limit is not like "45gjh" */
                fatal_error("Invalid search limit argument: %s\n",vars->optarg);
             }
             break;
   case 'o': {
                int param1 = 0;
                int param2 = 0;
                int ret_scan = sscanf(vars->optarg,"%d,%d%c",&param1,&param2,&foo);
                if (ret_scan == 2) {
                    max_count_call_warning = param1;
                    max_count_call = param2;
                    if (((max_count_call < -1)) || (max_count_call_warning < -1)) {
                        /* foo is used to check that the search limit is not like "45gjh" */
                        fatal_error("Invalid stop count argument: %s\n",vars->optarg);
                    }
                }
                else
                    if (1!=sscanf(vars->optarg,"%d%c",&max_count_call,&foo) || (max_count_call < -1)) {
                        /* foo is used to check that the search limit is not like "45gjh" */
                        fatal_error("Invalid stop count argument: %s\n",vars->optarg);
                    }
             }
             break;
   case 'd': if (vars->optarg[0]=='\0') {
                fatal_error("You must specify a non empty snt dir name\n");
             }
             strcpy(dynamicSntDir,vars->optarg);
             break;
   case 'c': tokenization_policy=CHAR_BY_CHAR_TOKENIZATION; break;
   case 'w': tokenization_policy=WORD_BY_WORD_TOKENIZATION; break;
   case 's': space_policy=START_WITH_SPACE; break;
   case 'x': space_policy=DONT_START_WITH_SPACE; break;
   case 'b': ambiguous_output_policy=ALLOW_AMBIGUOUS_OUTPUTS; break;
   case 'z': ambiguous_output_policy=IGNORE_AMBIGUOUS_OUTPUTS; break;
   case 'p': protect_dic_chars=1; break;
   case 'K': is_korean=1;
             break;
   case 'h': usage(); return 0;
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
   case 'u': if (vars->optarg[0]=='\0') {
                fatal_error("You must specify a non empty arabic rule configuration file name\n");
             }
             strcpy(arabic_rules,vars->optarg);
             break;
   case 'v': {
	   unichar* key=u_strdup(vars->optarg);
	   unichar* value=u_strchr(key,'=');
	   if (value==NULL) {
		   fatal_error("Invalid variable injection: %s\n",vars->optarg);
	   }
	   (*value)='\0';
	   value++;
	   value=u_strdup(value);
	   vector_ptr_add(injected_vars,key);
	   vector_ptr_add(injected_vars,value);
	   break;
   }
   case ':': if (index==-1) fatal_error("Missing argument for option -%c\n",vars->optopt);
             else fatal_error("Missing argument for option --%s\n",lopts_Locate[index].name);
   case '?': if (index==-1) fatal_error("Invalid option -%c\n",vars->optopt);
             else fatal_error("Invalid option --%s\n",vars->optarg);
             break;
   }
   index=-1;
}

if (selected_negation_operator==0)
    get_graph_compatibility_mode_by_file(&vec,&tilde_negation_operator);

if (text[0]=='\0') {
   fatal_error("You must specify a .snt text file\n");
}
if (vars->optind!=argc-1) {
   fatal_error("Invalid arguments: rerun with --help\n");
}

char staticSntDir[FILENAME_MAX];
char tokens_txt[FILENAME_MAX];
char text_cod[FILENAME_MAX];
char dlf[FILENAME_MAX];
char dlc[FILENAME_MAX];
char err[FILENAME_MAX];

get_snt_path(text,staticSntDir);
if (dynamicSntDir[0]=='\0') {
   strcpy(dynamicSntDir,staticSntDir);
}

strcpy(tokens_txt,staticSntDir);
strcat(tokens_txt,"tokens.txt");

strcpy(text_cod,staticSntDir);
strcat(text_cod,"text.cod");

strcpy(dlf,staticSntDir);
strcat(dlf,"dlf");

strcpy(dlc,staticSntDir);
strcat(dlc,"dlc");

strcpy(err,staticSntDir);
strcat(err,"err");

int OK=locate_pattern(text_cod,tokens_txt,argv[vars->optind],dlf,dlc,err,alph,match_policy,output_policy,
               &vec,
               dynamicSntDir,tokenization_policy,space_policy,search_limit,morpho_dic,
               ambiguous_output_policy,variable_error_policy,protect_dic_chars,is_korean,
               max_count_call,max_count_call_warning,arabic_rules,tilde_negation_operator,
               useLocateCache,allow_trace,injected_vars);
if (morpho_dic!=NULL) {
   free(morpho_dic);
}
free_vector_ptr(injected_vars,free);
free_OptVars(vars);
return (!OK);
}


/**
 * Launches the Locate main function with the appropriate arguments.
 * This function is used to apply a .fst2 as dictionary in the Dico
 * program.
 *
 * @author Alexis Neme
 * Modified by Sébastien Paumier
 */
int launch_locate_as_routine(const VersatileEncodingConfig* vec,
                             const char* text_snt,const char* fst2,const char* alphabet,
                             OutputPolicy output_policy,MatchPolicy match_policy,const char* morpho_dic,
                             int protect_dic_chars,int is_korean,const char* arabic_rules,
                             const char* negation_operator,
                             int n_matches_max) {
/* We test if we are working on Thai, on the basis of the alphabet file */
char path[FILENAME_MAX];
char lang[FILENAME_MAX];
get_path(alphabet,path);
path[strlen(path)-1]='\0';
remove_path(path,lang);
int thai=0;
if (!strcmp(lang,"Thai")) {
   thai=1;
}
int md=0;
if (morpho_dic!=NULL) {
   md=1;
}
ProgramInvoker* invoker=new_ProgramInvoker(main_Locate,"main_Locate");
char tmp[FILENAME_MAX];
{
    tmp[0]=0;
    get_reading_encoding_text(tmp,sizeof(tmp)-1,vec->mask_encoding_compatibility_input);
    if (tmp[0] != '\0') {
        add_argument(invoker,"-k");
        add_argument(invoker,tmp);
    }

    tmp[0]=0;
    get_writing_encoding_text(tmp,sizeof(tmp)-1,vec->encoding_output,vec->bom_output);
    if (tmp[0] != '\0') {
        add_argument(invoker,"-q");
        add_argument(invoker,tmp);
    }
}
if (negation_operator != NULL) {
    if ((*negation_operator) != 0) {
        char negation_operator_argument[0x40];
        sprintf(negation_operator_argument,"--negation_operator=%s",negation_operator);
        add_argument(invoker,negation_operator_argument);
    }
}
/* If needed: just to know that the call come from here if necessary */
sprintf(tmp,"--text=%s",text_snt);
add_argument(invoker,tmp);
if (alphabet!=NULL && alphabet[0]!='\0') {
	sprintf(tmp,"-a%s",alphabet);
	add_argument(invoker,tmp);
}
/* We set the match policy */
switch(match_policy) {
case ALL_MATCHES: add_argument(invoker,"-A"); break;
case SHORTEST_MATCHES: add_argument(invoker,"-S"); break;
case LONGEST_MATCHES: add_argument(invoker,"-L"); break;
}
/* We set the output policy */
switch (output_policy) {
   case MERGE_OUTPUTS: add_argument(invoker,"-M"); break;
   case REPLACE_OUTPUTS: add_argument(invoker,"-R"); break;
   default: add_argument(invoker,"-I"); break;
}
/* We look for all the occurrences */
if (n_matches_max==-1) {
	add_argument(invoker,"--all");
} else {
	sprintf(tmp,"-n%d",n_matches_max);
	add_argument(invoker,tmp);
}
/* If needed, we add the -thai option */
if (thai) {
   add_argument(invoker,"--thai");
}
if (md) {
	add_argument(invoker,"-m");
	add_argument(invoker,morpho_dic);
}
if (protect_dic_chars) {
	add_argument(invoker,"-p");
}
if (is_korean) {
	add_argument(invoker,"-K");
}
if (arabic_rules && arabic_rules[0]!='\0') {
   sprintf(tmp,"--arabic_rules=%s",arabic_rules);
   add_argument(invoker,tmp);
}
add_argument(invoker,fst2);
/* Finally, we call the main function of Locate */
int ret=invoke(invoker);
free_ProgramInvoker(invoker);
return ret;
}

} // namespace unitex
