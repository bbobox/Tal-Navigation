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

#include "Liste_num.h"
//---------------------------------------------------------------------------

struct liste_num* new_liste_num(int n,int sommet,unichar pile[]) {
struct liste_num* l;
l=(struct liste_num*)malloc(sizeof(struct liste_num));
l->n=n;
l->suivant=NULL;
l->sommet=sommet;
u_strcpy(l->pile,pile);
return l;
}


struct liste_num* inserer_si_absent(int n,struct liste_num* l,int sommet,unichar pile[]) {
if (l==NULL) return new_liste_num(n,sommet,pile);
if (l->n==n) {
   l->sommet=sommet;
   u_strcpy(l->pile,pile);
   return l;
}
l->suivant=inserer_si_absent(n,l->suivant,sommet,pile);
return l;
}
