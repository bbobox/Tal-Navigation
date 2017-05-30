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
//-----------------------------------------------

#include "TransductionStack.h"

void push_char(unichar c) {
stack[StackPointer++]=c;
}



//
// this function return true is c can be a part of a variable name
//
int is_variable_char(unichar c) {
return ((c>='A' && c<='Z') || (c>='a' && c<='z') || (c>='0' && c<='9') || c=='_');
}



void push_string(unichar* s) {
int i=0;
if (s==NULL || !u_strcmp_char(s,"<E>")) {
  // we do nothing if the transduction is <E>
  return;
}
while (s[i]!='\0') {
      if (s[i]=='$') {
         // case of a variable name
         unichar name[100];
         int L=0;
         i++;
         while (is_variable_char(s[i])) {
           name[L++]=s[i++];
         }
         name[L]='\0';
         struct transduction_variable* v=get_transduction_variable(name);
         if (v==NULL) {
            if (DISPLAY_VARIABLE_ERRORS) {
               char NAME[100];
               u_to_char(NAME,name);
               fprintf(stderr,"Error: undefined variable $%s\n",NAME);
            }
         }
         else if (v->start==-1) {
            if (DISPLAY_VARIABLE_ERRORS) {
               char NAME[100];
               u_to_char(NAME,name);
               fprintf(stderr,"Error: starting position of variable $%s undefined\n",NAME);
            }
         }
         else if (v->end==-1) {
            if (DISPLAY_VARIABLE_ERRORS) {
               char NAME[100];
               u_to_char(NAME,name);
               fprintf(stderr,"Error: end position of variable $%s undefined\n",NAME);
            }
         }
         else if (v->start > v->end) {
            if (DISPLAY_VARIABLE_ERRORS) {
               char NAME[100];
               u_to_char(NAME,name);
               fprintf(stderr,"Error: end position before starting position for variable $%s\n",NAME);
            }
         }
         else {
            // if the variable definition is correct
            for (int k=v->start;k<=v->end;k++)
              push_string(TOKENS->tab[texte[k+origine_courante]]);
         }
      }
      else {
         push_char(s[i]);
         i++;
      }
}
}



void process_transduction(unichar* s) {
push_string(s);
}
