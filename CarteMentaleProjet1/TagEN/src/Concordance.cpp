 /*
  * Unitex 
  *
  * Copyright (C) 2001-2003 Universit� de Marne-la-Vall�e <unitex@univ-mlv.fr>
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
#include "Concordance.h"
//---------------------------------------------------------------------------



void create_concordance(FILE* concor,FILE* text,struct text_tokens* tok,
                        int mode,int l,int r,char* font,char* fontsize,
                        char* dir,char* result_mode,char* alphabet_sort,
                        int n_enter_char,int enter_pos[]) {
FILE* out;
FILE* f;
char nom_txt[2000];
char nom_html[2000];
int RES;
sort_mode=mode;
compute_token_length(tok);
if (strcmp(result_mode,"html") && strcmp(result_mode,"text") && strcmp(result_mode,"glossanet")) {
   // if we must produces a text file
   create_new_text_file(concor,text,tok,result_mode);
   return;
}
if (!strcmp(result_mode,"html")) RES=HTML_;
else if (!strcmp(result_mode,"text")) RES=TEXT_;
else {
     RES=GLOSSANET_;
     sort_mode=TEXT_ORDER; // we force the text order mode for glossanet html pages
     unichar r[2];
     r[0]='[';
     r[1]='\0';
     open_bracket=get_token_number(r,tok);
     r[0]=']';
     close_bracket=get_token_number(r,tok);
}
strcpy(nom_txt,dir);
strcat(nom_txt,"concord_.txt");
strcpy(nom_html,dir);
if (RES==TEXT_)
   strcat(nom_html,"concord.txt");
else
   strcat(nom_html,"concord.html");
int N_MATCHES;

f=u_fopen(nom_txt,U_WRITE);
if (f==NULL) {
   fprintf(stderr,"Cannot write %s\n",nom_txt);
   return;
}
// we create a rax text concordance
N_MATCHES=create_raw_text_concordance(f,concor,text,tok,l,r,RES,n_enter_char,enter_pos);
u_fclose(f);
// if necessary, we sort it
if (sort_mode!=TEXT_ORDER) {
   char s[2000];
   strcpy(s,"SortTxt \"");
   strcat(s,nom_txt);
   strcat(s,"\" -n" );
   if (strcmp(alphabet_sort,"NULL")) {
      strcat(s," -o \"");
      strcat(s,alphabet_sort);
      strcat(s,"\"");
   }
   if (CHAR_BY_CHAR==THAI) strcat(s," -thai");
   system(s);
}
f=u_fopen(nom_txt,U_READ);
if (f==NULL) {
   fprintf(stderr,"Cannot read %s\n",nom_txt);
   return;
}
if (RES!=TEXT_) {
   out=fopen(nom_html,"w");
} else {
   out=u_fopen(nom_html,U_WRITE);
}
if (out==NULL) {
   fprintf(stderr,"Cannot write %s\n",nom_html);
   u_fclose(f);
   return;
}
if (RES!=TEXT_) write_HTML_header(out,N_MATCHES,font,fontsize);
unichar A[3000];
unichar B[3000];
unichar C[3000];
unichar href[3000];
unichar indices[100];
unichar* left;
unichar* middle;
unichar* right;
int j;
int c;
while ((c=u_fgetc(f))!=EOF) {
  j=0;
  if (RES==GLOSSANET_) {
     while (c!=0x09) {
       // reading the left context
       href[j++]=(unichar)c;
       c=u_fgetc(f);
     }
     href[j]='\0';
     c=u_fgetc(f);
     j=0;
  }
  while (c!=0x09) {
    // reading the left context
    A[j++]=(unichar)c;
    c=u_fgetc(f);
  }
  A[j]='\0';
  c=u_fgetc(f);
  j=0;
  while (c!=0x09) {
     // reading the match
     B[j++]=(unichar)c;
     c=u_fgetc(f);
  }
  B[j]='\0';
  c=u_fgetc(f);
  j=0;
  while (c!='\n' && c!='\t') {
     // reading the match
     C[j++]=(unichar)c;
     c=u_fgetc(f);
  }
  C[j]='\0';
  indices[0]='\0';
  if (c=='\t') {
     // if there are indices to be read (15 17), we read them
     c=u_fgetc(f);
     j=0;
     while (c!='\n') {
        // reading the match
        indices[j++]=(unichar)c;
        c=u_fgetc(f);
        }
     indices[j]='\0';
  }
  switch(sort_mode) {
     case TEXT_ORDER: left=A; middle=B; right=C; break;
     case LEFT_CENTER: left=A; middle=B; right=C; break;
     case LEFT_RIGHT: left=A; right=B; middle=C; break;
     case CENTER_LEFT: middle=A; left=B; right=C; break;
     case CENTER_RIGHT: middle=A; right=B; left=C; break;
     case RIGHT_LEFT: right=A; left=B; middle=C; break;
     case RIGHT_CENTER: right=A; middle=B; left=C; break;
  }
  if (sort_mode!=TEXT_ORDER) {
     if (CHAR_BY_CHAR==THAI) reverse_initial_vowels_thai(left);
     if (RES!=TEXT_)
         u_fprints_html_reverse(left,out);
     else
         u_fprints_reverse(left,out);
  }
  else {
       if (RES!=TEXT_)
          u_fprints_html(left,out);
       else u_fprints(left,out);
  }
  if (RES==HTML_) {
     char tmp[100];
     u_to_char(tmp,indices);
     fprintf(out,"<a href=\"%s\">",tmp);
     u_fprints_html(middle,out);
     fprintf(out,"</a>");
     u_fprints_html(right,out);
     fprintf(out,"<br>\n");
  }
  if (RES==GLOSSANET_) {
     fprintf(out,"<A HREF=\"http://glossa.fltr.ucl.ac.be/scripts/glossa/cgi/seek.pl?rec=");
     u_fprints_html(middle,out);
     fprintf(out,"&adr=");
     u_fprints_html(href,out);
     fprintf(out,"\" style=\"color: rgb(0,0,128)\">");
     u_fprints_html(middle,out);
     fprintf(out,"</A>");
     u_fprints_html(right,out);
     fprintf(out,"<br>\n");
  }
  if (RES==TEXT_) {
     u_fprints_char("\t",out);
     u_fprints(middle,out);
     u_fprints_char("\t",out);
     u_fprints(right,out);
     u_fprints_char("\n",out);
  }

}
if (RES!=TEXT_) write_HTML_end(out);
u_fclose(f);
remove(nom_txt);
fclose(out);
}



void compute_token_length(struct text_tokens* tok) {
int i;
for (i=0;i<tok->N;i++) {
  token_length[i]=u_strlen(tok->token[i]);
}
}



void write_HTML_header(FILE* f,int N,char* font,char* fontsize) {
fprintf(f,"<html lang=en>\n");
fprintf(f,"<head>\n");
fprintf(f,"   <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n");
fprintf(f,"   <title>%d match%s</title>\n",N,(N>1)?"es":"");
fprintf(f,"</head>\n");
fprintf(f,"<body>\n");
fprintf(f,"<font face=\"%s\" size=%s>\n",font,fontsize);
}



void write_HTML_end(FILE* f) {
fprintf(f,"</font>\n");
fprintf(f,"</body>\n");
fprintf(f,"</html>\n");
}



void blockchange(int start_pos,FILE* text,struct text_tokens* TOK) {
long int adresse=(start_pos-MAX_CONTEXT_IN_UNITS)*sizeof(int);
// on va sauter de ((start_pos-MAX_CONTEXT_IN_UNITS) - N_UNITS_ALLREADY_READ) entiers
int L=((start_pos-MAX_CONTEXT_IN_UNITS) - N_UNITS_ALLREADY_READ);
fseek(text,N_UNITS_ALLREADY_READ*sizeof(int),SEEK_SET);
while (L!=0) {
   BUFFER_LENGTH=fread(buffer,sizeof(int),(L>=BUFFER_SIZE)?BUFFER_SIZE:L,text);
   L=L-BUFFER_LENGTH;
   for (int i=0;i<BUFFER_LENGTH;i++) {
     origine_courante_char=origine_courante_char+token_length[buffer[i]];
     if (buffer[i]==TOK->SENTENCE_MARKER) phrase_courante++;
   }
}
fseek(text,adresse,SEEK_SET);
N_UNITS_ALLREADY_READ=start_pos-MAX_CONTEXT_IN_UNITS;
BUFFER_LENGTH=fread(buffer,sizeof(int),BUFFER_SIZE,text);
}



void extract_left_context(int pos,unichar* left,struct text_tokens* tok,
                          int LEFT_CONTEXT_LENGTH) {
int i=LEFT_CONTEXT_LENGTH-1;
if (CHAR_BY_CHAR==THAI) {
   // if we are in thai mode, we must ignore diacritic sign to count "real" printed chars
   i=0;
   int count=0;
   left[0]='\0';
   pos--;
   int l=token_length[buffer[pos]]-1;
   unichar* s=tok->token[buffer[pos]];
   while (pos>=0 && count<LEFT_CONTEXT_LENGTH) {
     left[i]=s[l--];
     if (!u_is_to_be_ignored_thai(left[i])) {
        // we increase the char count only we don't have a diacritic mark
        count++;
     }
     i++;
     if (l<0) {
        // if we must change of token
        pos--;
        if (pos>=0) {
           // and if we can
           l=token_length[buffer[pos]]-1;
           s=tok->token[buffer[pos]];
        }
     }
   }
   if (count!=LEFT_CONTEXT_LENGTH) {
     while (count++!=LEFT_CONTEXT_LENGTH) {
       left[i++]=' ';
     }
   }
   left[i]='\0';
   u_reverse_string(left);
   return;
}

left[LEFT_CONTEXT_LENGTH]='\0';
pos--;  // we must start on the left of the match
int l=token_length[buffer[pos]]-1;
unichar* s=tok->token[buffer[pos]];
while (pos>=0 && i>=0) {
  left[i--]=s[l--];
  if (l<0) {
     // if we must change of token
     pos--;
     if (pos>=0) {
        // and if we can
        l=token_length[buffer[pos]]-1;
        s=tok->token[buffer[pos]];
     }
  }
}
if (i>=0) {
   // if we have not completed the string because we are at the beginning of the buffer
   do {
     left[i--]=' ';
   } while (i>=0);
}
}



void extract_match(int start_pos,int end_pos,unichar* output,unichar* middle,struct text_tokens* tok) {
int i,j,k;
unichar* s;
if (output==NULL) {
   // if there is no output, we compute the match from the text
   j=0;
   for (i=start_pos;i<=end_pos;i++) {
     k=0;
     s=tok->token[buffer[i]];
     while (s[k]!='\0') {
       middle[j++]=s[k++];
     }
   }
   middle[j]='\0';
} else {
   // if there is an output, then the match is the output
   u_strcpy(middle,output);
}
}



void extract_right_context(int pos,unichar* right,struct text_tokens* tok,
                          int RIGHT_CONTEXT_LENGTH,int MATCH_LENGTH) {
right[0]='\0';
if (MATCH_LENGTH>=RIGHT_CONTEXT_LENGTH) {
   // we return if we all allready overpassed the right context length with the match
   return;
}
RIGHT_CONTEXT_LENGTH=RIGHT_CONTEXT_LENGTH-MATCH_LENGTH;
int i=0;
int count=0;
pos++;  // we must start on the right of the match
int l=0;
unichar* s=tok->token[buffer[pos]];
while (pos<BUFFER_LENGTH && count<RIGHT_CONTEXT_LENGTH) {
  right[i]=s[l++];
  if (CHAR_BY_CHAR==THAI) {
     if (!u_is_to_be_ignored_thai(right[i])) count++;
  } else count++;
  i++;
  if (s[l]=='\0') {
     // if we must change of token
     pos++;
     if (pos<BUFFER_LENGTH) {
        // and if we can
        l=0;
        s=tok->token[buffer[pos]];
     }
  }
}
if (count<RIGHT_CONTEXT_LENGTH) {
   // if we have not completed the string because we are at the beginning of the buffer
   do {
     right[i++]=' ';
     count++;
   } while (count<RIGHT_CONTEXT_LENGTH);
}
right[i]='\0';
}



int extract_href(int end_pos,unichar* href,struct text_tokens* tok) {
href[0]='\0';
if (open_bracket==-1 || close_bracket==-1) {
   // if there are no both open and close bracket, there
   // is no chance to find any href
   return 1;
}
int i=end_pos;
int op=0;
int cl=0;
while (i<BUFFER_LENGTH && op!=2 && cl!=2) {
      if (buffer[i]==open_bracket) {op++;cl=0;}
      else if (buffer[i]==close_bracket) {cl++;op=0;}
      else {op=0;cl=0;}
      i++;
}
if (cl==2) {
   // if we have found ]], it means that the matches is part of a href
   return 0;
}
if (op!=2) return 1;
while (i<BUFFER_LENGTH && buffer[i]!=close_bracket) {
   u_strcat(href,tok->token[buffer[i]]);
   i++;
}
return 1;
}



void reverse_initial_vowels_thai(unichar* s) {
int i=0;
unichar c;
while (s[i]!='\0') {
   if (u_is_vowel_thai(s[i]) && s[i+1]!='\0') {
      c=s[i+1];
      s[i+1]=s[i];
      s[i]=c;
      i++;
   }
   i++;
}
}



int create_raw_text_concordance(FILE* f,FILE* concor,FILE* text,struct text_tokens* tok,
                                int left_context_length,int right_context_length,int RES,
                                int n_enter_char,int enter_pos[]) {
struct liste_matches* l;
struct liste_matches* l_tmp;
unichar left[5000];
unichar middle[5000];
unichar right[5000];
unichar href[5000];
int TRANSDUCTION_MODE;
int N=0;
int GOOD_MATCH=1;
int start_pos,end_pos;
N_UNITS_ALLREADY_READ=0;
printf("Loading concordance index...\n%s",CR);
l=load_match_list(concor,&TRANSDUCTION_MODE);
BUFFER_LENGTH=fread(buffer,sizeof(int),BUFFER_SIZE,text);
origine_courante_char=0;
int debut_char;
int fin_char;
int locale_phrase_courante=phrase_courante;
int char_pos=0;
int int_pos=0;
printf("Constructing concordance...\n%s",CR);
while (l!=NULL) {
   if (BUFFER_LENGTH==BUFFER_SIZE && ((l->debut-N_UNITS_ALLREADY_READ)+MAX_HTML_CONTEXT)>BUFFER_LENGTH) {
      // if we must change of block...
      blockchange(l->debut,text,tok);
      // on reinitialise char_pos et int_pos
      char_pos=origine_courante_char;
      int_pos=0;
      locale_phrase_courante=phrase_courante;
   }
   // now, we are sure that the buffer contains all we want
   start_pos=l->debut-N_UNITS_ALLREADY_READ;
   end_pos=l->fin-N_UNITS_ALLREADY_READ;
   // we extract the 3 parts of the concordance
   debut_char=char_pos;
   for (int z=int_pos;z<start_pos;z++) {
      debut_char=debut_char+token_length[buffer[z]];
      if (buffer[z]==tok->SENTENCE_MARKER) locale_phrase_courante++;
   }
   char_pos=debut_char;
   int_pos=start_pos;
   fin_char=debut_char;
   for (int z=start_pos;z<=end_pos;z++) {
      fin_char=fin_char+token_length[buffer[z]];
   }
   extract_left_context(start_pos,left,tok,left_context_length);
   extract_match(start_pos,end_pos,l->output,middle,tok);
   unichar indice[100];
   char ch[100];
   // ce bloc sert � calculer le decalage du aux retours � la ligne qui compte 2 caracteres
   int decalage=get_decalage(n_enter_char,enter_pos,start_pos);
   debut_char=debut_char+decalage;
   fin_char=fin_char+decalage;
   // fin decalage
   sprintf(ch,"\t%d %d %d",debut_char,fin_char,locale_phrase_courante);
   u_strcpy_char(indice,ch);
   int MATCH_WIDTH;
   if (CHAR_BY_CHAR==THAI) MATCH_WIDTH=u_strlen_thai_without_diacritic(middle);
   else MATCH_WIDTH=u_strlen(middle);
   extract_right_context(end_pos,right,tok,right_context_length,MATCH_WIDTH);
   if (RES==GLOSSANET_) GOOD_MATCH=extract_href(end_pos,href,tok);
   if (GOOD_MATCH) {
      if (sort_mode!=TEXT_ORDER) {
         // if we must reverse the left context in thai mode,
         // we must reverse initial vowels with their following consonants
         if (CHAR_BY_CHAR==THAI) reverse_initial_vowels_thai(left);
      }
      // and we save them according to the sort mode
      switch(sort_mode) {
        case TEXT_ORDER: {if (RES==GLOSSANET_) {
                             u_fprints(href,f);u_fprints_char("\t",f);
                          }
                          u_fprints(left,f);u_fprints_char("\t",f);
                          u_fprints(middle,f);u_fprints_char("\t",f);
                          u_fprints(right,f);u_fprints(indice,f);u_fprints_char("\n",f);} break;
        case LEFT_CENTER: {u_fprints_reverse(left,f);u_fprints_char("\t",f);
                           u_fprints(middle,f);u_fprints_char("\t",f);
                           u_fprints(right,f);u_fprints(indice,f);u_fprints_char("\n",f);} break;
        case LEFT_RIGHT: {u_fprints_reverse(left,f);u_fprints_char("\t",f);
                          u_fprints(right,f);u_fprints_char("\t",f);
                          u_fprints(middle,f);u_fprints(indice,f);u_fprints_char("\n",f);} break;
        case CENTER_LEFT: {u_fprints(middle,f);u_fprints_char("\t",f);
                           u_fprints_reverse(left,f);u_fprints_char("\t",f);
                           u_fprints(right,f);u_fprints(indice,f);u_fprints_char("\n",f);} break;
        case CENTER_RIGHT: {u_fprints(middle,f);u_fprints_char("\t",f);
                            u_fprints(right,f);u_fprints_char("\t",f);
                            u_fprints_reverse(left,f);u_fprints(indice,f);u_fprints_char("\n",f);} break;
        case RIGHT_LEFT: {u_fprints(right,f);u_fprints_char("\t",f);
                          u_fprints_reverse(left,f);u_fprints_char("\t",f);
                          u_fprints(middle,f);u_fprints(indice,f);u_fprints_char("\n",f);} break;
        case RIGHT_CENTER: {u_fprints(right,f);u_fprints_char("\t",f);
                            u_fprints(middle,f);u_fprints_char("\t",f);
                            u_fprints_reverse(left,f);u_fprints(indice,f);u_fprints_char("\n",f);} break;
      }
      // counting matches
      N++;
   }
   // and skipping to the next element of the list
   l_tmp=l;
   l=l->suivant;
   free_liste_matches(l_tmp);
}
return N;
}




//
// this function saves the text from the token n� current_global_position to the n� match_start
//
void move_in_text_with_writing(int match_start,FILE* text,struct text_tokens* tok,
                               int current_global_position,FILE* res) {
long int adresse=(current_global_position)*sizeof(int);
fseek(text,adresse,SEEK_SET);
while ((match_start-current_global_position)>BUFFER_SIZE) {
   // if the distance between current position and final position is larger than
   // buffer size, we read a full buffer
   fread(buffer,sizeof(int),BUFFER_SIZE,text);
   for (adresse=0;adresse<BUFFER_SIZE;adresse++) {
      u_fprints(tok->token[buffer[adresse]],res);
   }
   current_global_position=current_global_position+BUFFER_SIZE;
}
current_global_position=fread(buffer,sizeof(int),match_start-current_global_position,text);
for (adresse=0;adresse<current_global_position;adresse++) {
   u_fprints(tok->token[buffer[adresse]],res);
}
}



//
// this function saves the text from the token n� current_global_position to the end
//
void move_to_end_of_text_with_writing(FILE* text,struct text_tokens* tok,
                               int current_global_position,FILE* res) {
long int adresse=(current_global_position)*sizeof(int);
fseek(text,adresse,SEEK_SET);
while (0!=(current_global_position=fread(buffer,sizeof(int),BUFFER_SIZE,text))) {
   for (adresse=0;adresse<current_global_position;adresse++) {
      u_fprints(tok->token[buffer[adresse]],res);
   }
}
}



void create_new_text_file(FILE* concord,FILE* text,struct text_tokens* tok,char* result_name) {
FILE* res=u_fopen(result_name,U_WRITE);
if (res==NULL) {
   fprintf(stderr,"Cannot write file %s\n",result_name);
   u_fclose(concord);
   fclose(text);
   exit(1);
}
struct liste_matches* l;
struct liste_matches* l_tmp;
N_UNITS_ALLREADY_READ=0;
int current_global_position;
l=load_match_list(concord,&current_global_position);
current_global_position=0;
printf("Merging outputs with text...\n%s",CR);
while (l!=NULL) {
   while (l!=NULL && l->debut<current_global_position) {
     // if we must ignore this match
     l_tmp=l;
     l=l->suivant;
     free_liste_matches(l_tmp);
   }
   if (l!=NULL) {
      // there, we are sure that we have a valid match to process
      move_in_text_with_writing(l->debut,text,tok,current_global_position,res);
      // now, we are sure that the buffer contains all we want
      if (l->output!=NULL) {
         u_fprints(l->output,res);
      }
      current_global_position=l->fin+1;
      // and skipping to the next element of the list
      l_tmp=l;
      l=l->suivant;
      free_liste_matches(l_tmp);
   }
}
move_to_end_of_text_with_writing(text,tok,current_global_position,res);
printf("Done.\n%s",CR);
}




int trouver_par_dichotomie(int a,int* t,int n) {
int debut,milieu;
if (n==0||t==NULL) return 0;
if (a<t[0]) return 0;
if (a>t[n-1]) return n;
n=n-1;
debut=0;
while (debut<=n) {
  milieu=(debut+n)/2;
  if (t[milieu]==a) return milieu;
  if (t[milieu]<a) {
    debut=milieu+1;
  }
  else {
    n=milieu-1;
  }
}
return n+1;
}



//
// cette fonction prend le nombre de retours � la ligne, le tableau des positions
// des retours � la ligne et une position; elle renvoie le nombre de retours � la ligne
// se trouvant avant cette position
//
int get_decalage(int n_enter_char,int enter_pos[],int start_pos) {
int res=trouver_par_dichotomie(start_pos,enter_pos,n_enter_char);
return res;
}

