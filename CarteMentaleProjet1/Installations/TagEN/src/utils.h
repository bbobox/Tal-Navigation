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
#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>
#include <stdarg.h>
#include "unicode.h"


extern int dobreak;

#define BREAK { if (dobreak) { fprintf(stderr, "\nBREAK:%s at line %d [%s] [type enter]\n", __FILE__, __LINE__, __FUNCTION__); getchar(); } }


static inline int is_absolute_path(const char * path) { return ((*path == '/') || strchr(path, ':')); }

static inline char * dirname(char * path) {

  if (path == NULL) { return "."; }

  char * p    = path;
  char * last = NULL;

  while (*p) {

    if ((*p == '/' || *p == '\\')) { last = p; }

    p++;
  }

  if (last == NULL) { return "."; }
  if (last == path) { return "/"; }

  *last = 0;
  return path;
}



static inline char * basename(char * path) {

  if (path == NULL) { return "."; }

  char * p    = path;
  char * last = path;

  while (*p) {
    if (*p == '/' || *p == '\\') { last = p + 1; }
    p++;
  }

  if (*last == 0) { return "."; }
  return last;
}




static inline void warning(char * fmt, ...) {

  va_list plist;
  va_start(plist, fmt);
  
  fprintf(stderr, "warning: ");
  i_vfprintf(stderr, fmt, plist);

  va_end(plist);
}


static inline void debug(char * fmt, ...) {

  va_list plist;
  va_start(plist, fmt);
  
  fprintf(stderr, "debug: ");
  i_vfprintf(stderr, fmt, plist);

  va_end(plist);
}



static inline void error(char * fmt, ...) {

  va_list plist;
  va_start(plist, fmt);
  
  fprintf(stderr, "error: ");
  i_vfprintf(stderr, fmt, plist);

  va_end(plist);
}


static inline void die(char * fmt, ...) {

  va_list plist;
  va_start(plist, fmt);
  
  fprintf(stderr, "fatal error: ");
  i_vfprintf(stderr, fmt, plist);

  va_end(plist);
  exit(1);
}



static inline void * xmalloc(size_t size) {
  void * res = malloc(size);
  if (res == NULL) { die("Not Enough Memory\n"); }
  return res;
}


static inline void * xrealloc(void * ptr, size_t size) {
  void * res = realloc(ptr, size);
  if (res == NULL) { die("Not Enough Memory\n"); }
  return res;
}


static inline void * xcalloc(size_t nb, size_t size) {
  void * res = calloc(nb, size);
  if (res == NULL) { die("Not Enough Memory\n"); }
  return res;
}

#endif
