#ifndef _INTER_H_
#define _INTER_H_

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

/*************************************************************************
 *                                  inter.h                        v0.01 *
 *                                                                       *
 *   interAut : renvoie un nouvel automate resultant de                  *
 *                           l'intersection des deux premiers            * 
 *                                                                       *
 *                      (c) 1997 S. SAUPIQUE - MagiX                     *
 *************************************************************************/

tAutAlMot * interAut(tAutAlMot * a, tAutAlMot * b) ;
tAutAlMot * interAutAtome(tAutAlMot * a, tAutAlMot * b) ;

void alphabet_dump(alphabet * a, FILE * f=stderr);
void affBiAlph(tBiAlph * b, FILE * f=stderr);

#endif

