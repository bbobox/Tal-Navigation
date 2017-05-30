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
#ifndef VerifierRecursionH
#define VerifierRecursionH
//---------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "unicode.h"
#include "AutomateFst2.h"
#include "LiberationFst2.h"

struct liste_num {
  int numero;
  struct liste_num* suivant;
};
typedef struct liste_num* Liste_num;



int pas_de_recursion(char *);
int chercher_recursion(int,Liste_num,int*,unichar**,Etat_fst*,Etiquette*,int*);

#endif
