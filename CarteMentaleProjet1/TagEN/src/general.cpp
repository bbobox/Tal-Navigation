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
/* Nom 		: general.c */
/* Date 	: juin 98 */


#include <stdio.h>
#include <stdlib.h>
#include "general.h"

//FILE * fErr ;

void erreurMem(char * fonction) {
fprintf(fErr, "\nNo more memory [%s].\n", fonction) ;
fclose(fErr) ;
exit(1) ; }

void erreurInt(char * fonction) {
fprintf(fErr, "\nInternal error [%s].\n", fonction) ;
fclose(fErr) ;
exit(1) ; }
