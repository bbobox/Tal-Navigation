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
#include "String_hash.cpp"
#include "FileName.cpp"
#include "Copyright.h"
#include "DELA.cpp"
//---------------------------------------------------------------------------

#define NORMAL 0
#define CHAR_BY_CHAR 1
#define MAX_TOKENS 1000000
#define MAX_ENTER_CHAR 1000000
#define MAX_TAG_LENGTH 4000


int n_occur[MAX_TOKENS];
int enter_pos[MAX_ENTER_CHAR];
int n_enter_char=0;
int MODE;
int SENTENCES=0;
int TOKENS_TOTAL=0;
int WORDS_TOTAL=0;
int DIFFERENT_WORDS=0;
int DIGITS_TOTAL=0;
int DIFFERENT_DIGITS=0;


//
// "c:\unitex-visiteur\french\corpus\80jours.snt" "c:\unitex-visiteur\french\alphabet.txt"
//


void usage() {
printf("%s",COPYRIGHT);
printf("Usage: Tokenize <text> <alphabet> [-char_by_char]\n%s",CR);
printf("     <text> : any unicode text file\n%s",CR);
printf("     <alphabet> : the alphabet file\n%s",CR);
printf("     [-char_by_char] : with this option, the program do a char by char\n%s",CR);
printf("     tokenization (except for the tags {S}, or like {today,.ADV}). This\n%s",CR);
printf("     mode may be used for languages like Thai.\n\n%s",CR);
printf("Tokenizes the text. The token list is stored into a TOKENS.TXT file and\n%s",CR);
printf("the coded text is stored into a TEXT.COD file.\n%s",CR);
printf("The program also produces 4 files named tok_by_freq.txt, tok_by_alph.txt,\n%s",CR);
printf("stats.n and enter.pos. They contain the token list sorted by frequence and by\n%s",CR);
printf("alphabetical order and stats.n contains some statistics. The file enter.pos\n%s",CR);
printf("contains the position in tokens of all the carridge return sequences. All\n%s",CR);
printf("files are saved in the text_snt directory.\n%s",CR);
}



void ecrire_nombre_lignes(char name[],int n)
{
  FILE *f;
  int i;
  i=2+9*2; // *2 because of unicode +2 because of FF FE at file start
  f=fopen((char*)name,"r+b");
  do
    {
      fseek(f,i,0);
      i=i-2;
      u_fputc((unichar)((n%10)+'0'),f);
      n=n/10;
    }
  while (n);
  fclose(f);
}




void init_n_occur();
void sort_and_save_by_frequence(FILE*,struct string_hash*);
void sort_and_save_by_alph_order(FILE*,struct string_hash*);
void compute_statistics(FILE*,struct string_hash*,Alphabet*);
void normal_tokenization(FILE*,FILE*,FILE*,Alphabet*,struct string_hash*);
void char_by_char_tokenization(FILE*,FILE*,FILE*,Alphabet*,struct string_hash*);
void sauver_enter_pos(FILE*);



int main(int argc, char **argv) {
if (argc<3 || argc>4) {
   usage();
   return 0;
}
FILE* text;
FILE* out;
FILE* tokens;
FILE* enter;
char tokens_txt[2000];
char text_cod[2000];
char enter_pos[2000];
Alphabet* alph;

get_snt_path(argv[1],text_cod);
strcat(text_cod,"text.cod");
get_snt_path(argv[1],tokens_txt);
strcat(tokens_txt,"tokens.txt");
get_snt_path(argv[1],enter_pos);
strcat(enter_pos,"enter.pos");
text=u_fopen(argv[1],U_READ);
if (text==NULL) {
   fprintf(stderr,"Cannot open text file %s\n",argv[1]);
   return 1;
}
MODE=NORMAL;
if (argc==4) {
   if (!strcmp(argv[3],"-char_by_char"))
      MODE=CHAR_BY_CHAR;
   else {
      fprintf(stderr,"Invalid parameter %s\n",argv[3]);
   }
}
alph=load_alphabet(argv[2]);
if (alph==NULL) {
   fprintf(stderr,"Cannot load alphabet file %s\n",argv[2]);
   u_fclose(text);
   return 1;
}
out=fopen(text_cod,"wb");
if (out==NULL) {
   fprintf(stderr,"Cannot create file %s\n",text_cod);
   u_fclose(text);
   if (alph!=NULL) {
      free_alphabet(alph);
   }
   return 1;
}
enter=fopen(enter_pos,"wb");
if (enter==NULL) {
   fprintf(stderr,"Cannot create file %s\n",enter_pos);
   u_fclose(text);
   fclose(out);
   if (alph!=NULL) {
      free_alphabet(alph);
   }
   return 1;
}
tokens=u_fopen(tokens_txt,U_WRITE);
if (tokens==NULL) {
   fprintf(stderr,"Cannot create file %s\n",tokens_txt);
   u_fclose(text);
   fclose(out);
   fclose(enter);
   if (alph!=NULL) {
      free_alphabet(alph);
   }
   return 1;
}
u_fprints_char("0000000000\n",tokens);
struct string_hash* hash;
hash=new_string_hash();
init_n_occur();
printf("Tokenizing text...\n%s",CR);
if (MODE==NORMAL) {
   normal_tokenization(text,out,tokens,alph,hash);
}
else {
   char_by_char_tokenization(text,out,tokens,alph,hash);
}
printf("\nDone.\n%s",CR);
sauver_enter_pos(enter);
fclose(enter);
u_fclose(text);
fclose(out);
u_fclose(tokens);
ecrire_nombre_lignes(tokens_txt,hash->N);
// we compute some statistics
get_snt_path(argv[1],tokens_txt);
strcat(tokens_txt,"stats.n");
tokens=u_fopen(tokens_txt,U_WRITE);
if (tokens==NULL) {
   fprintf(stderr,"Cannot write %s\n",tokens_txt);
}
else {
   compute_statistics(tokens,hash,alph);
   u_fclose(tokens);
}
// we save the tokens by frequence
get_snt_path(argv[1],tokens_txt);
strcat(tokens_txt,"tok_by_freq.txt");
tokens=u_fopen(tokens_txt,U_WRITE);
if (tokens==NULL) {
   fprintf(stderr,"Cannot write %s\n",tokens_txt);
}
else {
   sort_and_save_by_frequence(tokens,hash);
   u_fclose(tokens);
}
// we save the tokens by alphabetical order
get_snt_path(argv[1],tokens_txt);
strcat(tokens_txt,"tok_by_alph.txt");
tokens=u_fopen(tokens_txt,U_WRITE);
if (tokens==NULL) {
   fprintf(stderr,"Cannot write %s\n",tokens_txt);
}
else {
   sort_and_save_by_alph_order(tokens,hash);
   u_fclose(tokens);
}

free_string_hash(hash);
if (alph!=NULL) {
   free_alphabet(alph);
}
return 0;
}
//---------------------------------------------------------------------------



void init_n_occur() {
for (int i=0;i<MAX_TOKENS;i++) {
   n_occur[i]=0;
}
}



void normal_tokenization(FILE* f,FILE* coded_text,FILE* tokens,Alphabet* alph,struct string_hash* hash) {
int c;
unichar s[MAX_TAG_LENGTH];
int n;
char ENTER;
int COUNT=0;
int current_megabyte=0;
c=u_fgetc(f);
while (c!=EOF) {
   COUNT++;
   if ((COUNT/(1024*512))!=current_megabyte) {
      current_megabyte++;
      int z=(COUNT/(1024*512));
      printf("%d megabyte%s read...%s",z,(z>1)?"s":"",CR);
   }
   if (c==' ' || c==0x0d || c==0x0a) {
      ENTER=0;
      if (c=='\n') ENTER=1;
      // if the char is a separator, we jump all the separators
      while ((c=u_fgetc(f))==' ' || c==0x0d || c==0x0a) {
        if (c=='\n') ENTER=1;
        COUNT++;
      }
      s[0]=' ';
      s[1]='\0';
      n=get_hash_number(s,hash);
      if (n==1000000) {
         fprintf(stderr,"Array overflow\n");
         exit(1);
      }
      n_occur[n]++;
      if (ENTER==1) {
         if (n_enter_char<MAX_ENTER_CHAR) {
            enter_pos[n_enter_char++]=TOKENS_TOTAL;
         }
      }
      TOKENS_TOTAL++;
      fwrite(&n,4,1,coded_text);
   }
   else if (c=='{') {
     s[0]='{';
     int z=1;
     while (z<(MAX_TAG_LENGTH-1) && (c=u_fgetc(f))!='}' && c!='{' && c!='\n') {
        s[z++]=(unichar)c;
        COUNT++;
     }
     if (z==(MAX_TAG_LENGTH-1) || c!='}') {
        // if the tag has no ending }
        fprintf(stderr,"Error: a tag without ending } has been found\n");
        exit(1);
     }
     if (c=='\n') {
        // if the tag contains a return
        fprintf(stderr,"Error: a tag containing a new-line sequence has been found\n");
        exit(1);
     }
     s[z]='}';
     s[z+1]='\0';
     if (!u_strcmp_char(s,"{S}")) {
        // if we have found a sentence delimiter
        SENTENCES++;
     } else {
        if (!check_tag_token(s)) {
           // if a tag is incorrect, we exit
           fprintf(stderr,"The text contains an invalid tag. Unitex cannot process it.");
           exit(1);
        }
     }
     n=get_hash_number(s,hash);
     if (n==1000000) {
        fprintf(stderr,"Array overflow\n");
        exit(1);
     }
     n_occur[n]++;
     TOKENS_TOTAL++;
     fwrite(&n,4,1,coded_text);
     c=u_fgetc(f);

   /* old version that does not take into account the tags like {aujourd'hui,.ADV}
     // case of the special tag {S}
     s[0]='{';
     s[1]=(unichar)u_fgetc(f);
     s[2]=(unichar)u_fgetc(f);
     s[3]='\0';
     c=u_fgetc(f);
     COUNT++; COUNT++;
     if (!u_strcmp_char(s,"{S}")) {
        // if we have read {S}
        n=get_hash_number(s,hash);
        if (n==1000000) {
           fprintf(stderr,"Array overflow\n");
           exit(1);
        }
        SENTENCES++;
        TOKENS_TOTAL++;
        n_occur[n]++;
        fwrite(&n,4,1,coded_text);
     }
     else {
        unichar b=s[1];
        unichar c=s[2];
        s[1]='\0';
        n=get_hash_number(s,hash);
        if (n==1000000) {
           fprintf(stderr,"Array overflow\n");
           exit(1);
        }
        n_occur[n]++;
        TOKENS_TOTAL++;
        fwrite(&n,4,1,coded_text);
        s[0]=b;
        n=get_hash_number(s,hash);
        if (n==1000000) {
           fprintf(stderr,"Array overflow\n");
           exit(1);
        }
        n_occur[n]++;
        TOKENS_TOTAL++;
        fwrite(&n,4,1,coded_text);
        s[0]=c;
        n=get_hash_number(s,hash);
        if (n==1000000) {
           fprintf(stderr,"Array overflow\n");
           exit(1);
        }
        n_occur[n]++;
        TOKENS_TOTAL++;
        fwrite(&n,4,1,coded_text);
     }
   */
   }
   else {
      s[0]=(unichar)c;
      n=1;
      if (!is_letter(s[0],alph)) {
         s[1]='\0';
         n=get_hash_number(s,hash);
         if (n==1000000) {
            fprintf(stderr,"Array overflow\n");
            exit(1);
         }
         n_occur[n]++;
         TOKENS_TOTAL++;
         if (c>='0' && c<='9') DIGITS_TOTAL++;
         fwrite(&n,4,1,coded_text);
         c=u_fgetc(f);
      }
      else {
         while (is_letter((unichar)(c=u_fgetc(f)),alph)) {
           s[n++]=(unichar)c;
           COUNT++;
         }
         s[n]='\0';
         n=get_hash_number(s,hash);
         if (n==1000000) {
            fprintf(stderr,"Array overflow\n");
            exit(1);
         }
         n_occur[n]++;
         TOKENS_TOTAL++;
         WORDS_TOTAL++;
         fwrite(&n,4,1,coded_text);
      }
   }
}
for (n=0;n<hash->N;n++) {
  u_fprints(hash->tab[n],tokens);
  u_fprints_char("\n",tokens);
}
}



void char_by_char_tokenization(FILE* f,FILE* coded_text,FILE* tokens,Alphabet* alph,struct string_hash* hash) {
int c;
unichar s[MAX_TAG_LENGTH];
int n;
char ENTER;
int COUNT=0;
int current_megabyte=0;
c=u_fgetc(f);
while (c!=EOF) {
   COUNT++;
   if ((COUNT/(1024*512))!=current_megabyte) {
      current_megabyte++;
      printf("%d megabytes read...%s",(COUNT/(1024*512)),CR);
   }
   if (c==' ' || c==0x0d || c==0x0a) {
      ENTER=0;
      if (c==0x0d) ENTER=1;
      // if the char is a separator, we jump all the separators
      while ((c=u_fgetc(f))==' ' || c==0x0d || c==0x0a) {
         if (c==0x0d) ENTER=1;
         COUNT++;
      }
      s[0]=' ';
      s[1]='\0';
      n=get_hash_number(s,hash);
      if (n==1000000) {
            fprintf(stderr,"Array overflow\n");
            exit(1);
         }
      if (ENTER==1) {
         if (n_enter_char<MAX_ENTER_CHAR) {
            enter_pos[n_enter_char++]=TOKENS_TOTAL;
         }
      }
      n_occur[n]++;
      TOKENS_TOTAL++;
      fwrite(&n,4,1,coded_text);
   }
   else if (c=='{') {
     s[0]='{';
     int z=1;
     while (z<(MAX_TAG_LENGTH-1) && (c=u_fgetc(f))!='}' && c!='{' && c!='\n') {
        s[z++]=(unichar)c;
        COUNT++;
     }
     if (z==(MAX_TAG_LENGTH-1) || c!='}') {
        // if the tag has no ending }
        fprintf(stderr,"Error: a tag without ending } has been found\n");
        exit(1);
     }
     if (c=='\n') {
        // if the tag contains a return
        fprintf(stderr,"Error: a tag containing a new-line sequence has been found\n");
        exit(1);
     }
     s[z]='}';
     s[z+1]='\0';
     if (!u_strcmp_char(s,"{S}")) {
        // if we have found a sentence delimiter
        SENTENCES++;
     } else {
        if (!check_tag_token(s)) {
           // if a tag is incorrect, we exit
           fprintf(stderr,"The text contains an invalid tag. Unitex cannot process it.");
           exit(1);
        }
     }
     n=get_hash_number(s,hash);
     if (n==1000000) {
        fprintf(stderr,"Array overflow\n");
        exit(1);
     }
     n_occur[n]++;
     TOKENS_TOTAL++;
     fwrite(&n,4,1,coded_text);
     c=u_fgetc(f);

   /* old version that does not take into account the tags like {aujourd'hui,.ADV}
     // case of the special tag {S}
     s[0]='{';
     s[1]=(unichar)u_fgetc(f);
     s[2]=(unichar)u_fgetc(f);
     s[3]='\0';
     COUNT++;COUNT++;
     c=u_fgetc(f);
     if (!u_strcmp_char(s,"{S}")) {
        // if we have read {S}
        n=get_hash_number(s,hash);
        if (n==1000000) {
           fprintf(stderr,"Array overflow\n");
           exit(1);
        }
        SENTENCES++;
        n_occur[n]++;
        TOKENS_TOTAL++;
        fwrite(&n,4,1,coded_text);
     }
     else {
        unichar b=s[1];
        unichar c=s[2];
        s[1]='\0';
        n=get_hash_number(s,hash);
        if (n==1000000) {
           fprintf(stderr,"Array overflow\n");
           exit(1);
        }
        n_occur[n]++;
        TOKENS_TOTAL++;
        fwrite(&n,4,1,coded_text);
        s[0]=b;
        n=get_hash_number(s,hash);
        if (n==1000000) {
           fprintf(stderr,"Array overflow\n");
           exit(1);
        }
        n_occur[n]++;
        TOKENS_TOTAL++;
        fwrite(&n,4,1,coded_text);
        s[0]=c;
        n=get_hash_number(s,hash);
        if (n==1000000) {
           fprintf(stderr,"Array overflow\n");
           exit(1);
        }
        n_occur[n]++;
        TOKENS_TOTAL++;
        fwrite(&n,4,1,coded_text);
     } */
   }
   else {
      s[0]=(unichar)c;
      s[1]='\0';
      n=get_hash_number(s,hash);
      if (n==1000000) {
           fprintf(stderr,"Array overflow\n");
           exit(1);
        }
        n_occur[n]++;
      TOKENS_TOTAL++;
      if (is_letter((unichar)c,alph)) WORDS_TOTAL++;
      else if (c>='0' && c<='9') DIGITS_TOTAL++;
      fwrite(&n,4,1,coded_text);
      c=u_fgetc(f);
   }
}
for (n=0;n<hash->N;n++) {
  u_fprints(hash->tab[n],tokens);
  u_fprints_char("\n",tokens);
}
}




int partition_pour_quicksort_by_frequence(int m, int n,struct string_hash* hash) {
int pivot;
int tmp;
unichar* tmp_char;
int i = m-1;
int j = n+1;         // indice final du pivot
pivot=n_occur[(m+n)/2];
while (true) {
  do j--;
  while ((j>(m-1))&&(pivot>n_occur[j]));
  do i++;
  while ((i<n+1)&&(n_occur[i]>pivot));
  if (i<j) {
    tmp=n_occur[i];
    n_occur[i]=n_occur[j];
    n_occur[j]=tmp;

    tmp_char=hash->tab[i];
    hash->tab[i]=hash->tab[j];
    hash->tab[j]=tmp_char;
  } else return j;
}
}



void quicksort_by_frequence(int debut,int fin,struct string_hash* hash) {
int p;
if (debut<fin) {
  p=partition_pour_quicksort_by_frequence(debut,fin,hash);
  quicksort_by_frequence(debut,p,hash);
  quicksort_by_frequence(p+1,fin,hash);
}
}



int partition_pour_quicksort_by_alph_order(int m, int n,struct string_hash* hash) {
unichar* pivot;
unichar* tmp;
int tmp_int;
int i = m-1;
int j = n+1;         // indice final du pivot
pivot=hash->tab[(m+n)/2];
while (true) {
  do j--;
  while ((j>(m-1))&&(u_strcmp(pivot,hash->tab[j])<0));
  do i++;
  while ((i<n+1)&&(u_strcmp(hash->tab[i],pivot)<0));
  if (i<j) {
    tmp_int=n_occur[i];
    n_occur[i]=n_occur[j];
    n_occur[j]=tmp_int;

    tmp=hash->tab[i];
    hash->tab[i]=hash->tab[j];
    hash->tab[j]=tmp;
  } else return j;
}
}



void quicksort_by_alph_order(int debut,int fin,struct string_hash* hash) {
int p;
if (debut<fin) {
  p=partition_pour_quicksort_by_alph_order(debut,fin,hash);
  quicksort_by_alph_order(debut,p,hash);
  quicksort_by_alph_order(p+1,fin,hash);
}
}




void sort_and_save_by_frequence(FILE *f,struct string_hash* hash) {
unichar tmp[100];
quicksort_by_frequence(0,hash->N-1,hash);
for (int i=0;i<hash->N;i++) {
   u_int_to_string(n_occur[i],tmp);
   u_fprints(tmp,f);
   u_fprints_char("\t",f);
   u_fprints(hash->tab[i],f);
   u_fprints_char("\n",f);
}
}



void sort_and_save_by_alph_order(FILE *f,struct string_hash* hash) {
unichar tmp[100];
quicksort_by_alph_order(0,hash->N-1,hash);
for (int i=0;i<hash->N;i++) {
   u_int_to_string(n_occur[i],tmp);
   u_fprints(hash->tab[i],f);
   u_fprints_char("\t",f);
   u_fprints(tmp,f);
   u_fprints_char("\n",f);
}
}



void compute_statistics(FILE *f,struct string_hash* hash,Alphabet* alph) {
for (int i=0;i<hash->N;i++) {
   if (u_strlen(hash->tab[i])==1) {
      if (hash->tab[i][0]>='0' && hash->tab[i][0]<='9') DIFFERENT_DIGITS++;
   }
   if (is_letter(hash->tab[i][0],alph)) DIFFERENT_WORDS++;
}
char tmp[3000];
unichar unitmp[3000];
sprintf(tmp,"%d sentence delimiter%s, %d (%d diff) token%s, %d (%d) simple form%s, %d (%d) digit%s",SENTENCES,(SENTENCES>1)?"s":"",TOKENS_TOTAL,hash->N,(TOKENS_TOTAL>1)?"s":"",WORDS_TOTAL,DIFFERENT_WORDS,(WORDS_TOTAL>1)?"s":"",DIGITS_TOTAL,DIFFERENT_DIGITS,(DIGITS_TOTAL>1)?"s":"");
u_strcpy_char(unitmp,tmp);
u_strcat_char(unitmp,"\n");
u_fprints(unitmp,f);
}



void sauver_enter_pos(FILE* f) {
fwrite(&enter_pos,4,n_enter_char,f);
}
