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


#include "autalmot.h"
#include "list_aut.h"
#include "entrsort.h"
#include "String_hash.h"
#include "fst2autalmot.h"
#include "utils.h"




void determine_sortesymbole(tSymbole * symb) {


  if (symb->sorteSymbole != INDETERMINE) { return; }


  if (complet(symb->gramm)) {

    switch (*symb->canonique) {

    case 0:
      symb->sorteSymbole = CODE;
      break;

    case '!':
      symb->sorteSymbole = NEGATIF;
      break;

    default:
      symb->sorteSymbole = ATOME;
    }

  } else {
    
    if (*symb->canonique) { die("bad symbol: canonical form (%S) with incomplete code (%S)\n", symb->canonique, symb->gramm); }

    symb->sorteSymbole = INCOMPLET;
  }
}


void symbole_strip(tSymbole * symb) {

  unichar * p, * q;

  for (p = symb->gramm; *p != '+'; p++) { if (*p == 0) return; }

  for (q = p + 1; (*q != 0) && (*q != ':'); q++);

  while ((*(p++) = *(q++)));
}




int symbole_developp(tAlphMot * alphabet, tSymbole * symb) {


  bool INDET = (symb->sorteSymbole == INDETERMINE);

  if (symb->sorteSymbole == SPECIAL) { /* <def> */
    return 0;
  }

  int nbflex = compte_flex(symb->gramm);

  if (nbflex < 2) {

    if (INDET) { determine_sortesymbole(symb); }

    alphabet_add(alphabet, symb);
    return 1;
  }

  int i, k, n;
  unichar common[maxGramm];
  unichar * p;

  for (i = 0; symb->gramm[i] != ':'; i++) { common[i] = symb->gramm[i]; }
  common[i] = 0;

  tSymbole s;
  symbole_copy(&s, symb);

  for (p = symb->gramm + i, n = 0; n < nbflex; n++) {

    unichar flex[maxGramm];
    k = 0; p++;

    while (*p && *p !=  ':') { flex[k++] = *(p++); }
    flex[k] = 0;

    u_sprintf(s.gramm, "%S:%S", common, flex);

    if (INDET) { determine_sortesymbole(& s); }

    alphabet_add(alphabet, & s);
  }

  return nbflex;
}



static int check_dic_entry(unichar * label) {

  /* {__,__.__} */

  if (*label != '{') { error("label %S isn't a good DELA entry\n", label); return -1; }

  unichar * p = label + 1;

  while (*p !=  ',') { // look for ','
    if (*p == 0) { error("malformed DELA entry: '%S' (',' is missing).\n", label); return -1; }
    p++;
  }
    
  while (*p != '.') {
    if (*p == 0) { error("malformed DELA entry: '%S' ('.' is missing).\n", label); return -1; }
    p++;
  }

  while (*p != '}') {
    if (*p == 0) { error("malformed DELA entry: '%S' ('}' is missing).\n", label); return -1; }
    p++;
  }
  
  p++;
  if (*p != 0) { error("malformed label: '%S': label should end with '}'.\n", label); return -1; }

  return 0;
}



/* text label can be either a full DELA entry, either a punc symbol, either an unknow word
 */

static int check_text_label(unichar * label) {

  unichar * p;

  if ((label == NULL)) { error("check label: no label\n"); return -1; }

  if (*label == 0) { error("error: check label: label is empty\n"); return -1; }

  if (*label == '{') { return check_dic_entry(label); }

  /* no spaces */

  for (p = label; *p; p++) {
    if ((*p == ' ') || (*p == '\t') || (*p == '\n')) { error("malformed label: '%S'.\n", label); return -1; }
  }

  return 0;
}





void load_text_symbol(tSymbole * symb, unichar * lex) {

  int i, j;

  if (check_text_label(lex) == -1) { die("bad text label: \"%S\"\n", lex); }

  symb->sorteSymbole = ATOME;


  if (*lex == '{') {   // {__,__.__}

    /* inflected form */

    i = 1, j = 0;

    while (*(lex + i) != ',') {

      symb->flechie[j++] = lex[i++];

      if (j >= maxMot) { die("inflected form '%S' is too long.\n", lex); }
    }

    symb->flechie[j] = 0;
    i++;


    /* forme canonique */

    for (j = i ; *(lex + j) != '.'; j++);
    symb->canonique = (unichar *) malloc((j + 1 - i) * sizeof(unichar));

    j = 0 ;
    while (*(lex + i) != '.') { symb->canonique[j++] = lex[i++]; }

    symb->canonique[j] = 0;
    i++;


    /* traits gramma ... */

    j = 0;
    while (lex[i] && lex[i] != '}' && j < maxGramm - 1) { symb->gramm[j++] = lex[i++]; }
    symb->gramm[j] = 0;


  } else {    /* mot inconnu dans un texte ou ponctuation (pas d'accolade, pas de point, pas de virgule) */


    if (lex[0] == '\\') {

      if (lex[2]) { die("illegal label '%S'\n", lex); }

      *symb->flechie  = 0;
      symb->canonique = u_strdup(lex);

      u_strcpy_char(symb->gramm, "PONCT");


    } else if (strchr("-'\"%.,:;#!=?()/>$&*+@[]|_", *lex)) {

      if (lex[1]) { die("illegal label text: '%S'\n", symb->flechie); }

      *symb->flechie  = 0;
      symb->canonique = u_strdup(lex);

      u_strcpy_char(symb->gramm, "PONCT");


    } else if (u_is_digit(*lex)) { /* chiffre arabe */

      if (lex[1]) { die("illegal label text: '%S'\n", symb->flechie); }

      *symb->flechie  = 0;
      symb->canonique = u_strdup(lex);

      u_strcpy_char(symb->gramm, "CHFA");


    } else { // mot inconnu

      i = 0;
      while (lex[i]) {
	if (i >= maxMot) { die("inflected form too long in '%S'.\n", lex); }
	symb->flechie[i] = lex[i];
	i++;
      }
      symb->flechie[i] = 0;

      symb->canonique    = (unichar *) xmalloc(sizeof(unichar));
      symb->canonique[0] = 0;

      symb->gramm[0] = '?';
      symb->gramm[1] = 0;
    }
  }
}




void load_gramm_symbol(tSymbole * symb, unichar * _lex) {

  int i, j;
  unichar * p, * delim;

  unichar * lex = u_strdup(_lex);

  //  debug("load_gramm_symbol: \"%S\"\n", lex);

  if (*lex == '{') {   /* dictionnary entry */

    if (u_strcmp_char(lex, "{S}") == 0) {
      
      symb->sorteSymbole = ATOME;
      *symb->flechie = 0;
      symb->canonique = lex;
      u_strcpy_char(symb->gramm, "PONCT");
      return;
    }


    if (check_dic_entry(lex) == -1) { die("bad label grammar '%S'\n", lex); }

    symb->sorteSymbole = ATOME;

    /* inflected form */

    i = 1, j = 0;

    while (lex[i] != ',') {

      if (j >= maxMot) { die("inflected form too long in '%S'\n", lex); }

      symb->flechie[j++] = lex[i++];
    }

    symb->flechie[j] = 0;
    i++;


    /* forme canonique */

    for (j = i; lex[j] != '.'; j++);
    lex[j] = 0;

    symb->canonique = u_strdup(lex + i);

    i = j + 1;


    /* traits gramma ... */

    j = 0;
    while (lex[i] && lex[i] != '}' && j < maxGramm - 1) { symb->gramm[j++] = lex[i++]; }
    symb->gramm[j] = 0;

    free(lex);
    return;
  }


  if (*lex ==  '<') {

    if (u_strcmp_char(lex, "<def>") == 0) { /* special <def> label */
      symb->sorteSymbole = SPECIAL;
      symb->canonique    = lex;
      u_strcpy(symb->flechie, lex);
      symb->gramm[0] = 0;
      return;
    }


    *symb->flechie = 0;

    p = lex + 1;

    if ((delim = u_strchr(p, '.'))) { /* read canonical form */

      *delim = 0;
      symb->canonique = u_strdup(p);
      p = delim + 1;

    } else {

      symb->canonique  = (unichar *) xmalloc(sizeof(unichar));
      *symb->canonique = 0;
    }

    i = 0;

    while ((*p != '>') && (i < maxGramm - 1)) {

      if (*p == 0) { die("missing closing '>' in label: '%S'\n", _lex); }

      symb->gramm[i++] = *(p++);
    }
    symb->gramm[i] = 0;


    symb->sorteSymbole = INDETERMINE;

    free(lex);
    return;
  }



  /* label doesn't start with '{' nor '<' */


  if ((*lex == '!') || (*lex == '=')) { /* special rule's symbols */

    if (lex[1]) { die("bad grammar label: '%S'\n", lex); }

    symb->sorteSymbole = ATOME;
    symb->canonique    = (unichar *) xmalloc(sizeof(unichar));
    symb->canonique[0] = 0;
    u_strcpy(symb->flechie, lex);
    u_strcpy(symb->gramm, lex);

    free(lex);

    return;
  }



  /* mot inconnu dans un texte ou ponctuation (pas d'accolade, pas de point, pas de virgule) */


  symb->sorteSymbole = ATOME;
  *symb->flechie = 0;

  if (*lex == '\\') {

    if (! lex[1] || lex[2]) { die("illegal label '%S'\n", *lex); }

    symb->canonique = lex;
    u_strcpy_char(symb->gramm, "PONCT");    
    return;
  }


  if (strchr("-'\"%.,:;#?()/>}<$&*+@[]|_", *lex)) {     /* ponctuation */

    if (lex[1]) { die("bad grammar label: '%S'\n", lex); }

    symb->canonique = lex;
    u_strcpy_char(symb->gramm, "PONCT");
    return;
  }

 
  if (u_is_digit(*lex)) { // chiffre arabe

    if (lex[1]) { die("illegal label text: '%S'\n", lex); }

    symb->canonique = lex;
    u_strcpy_char(symb->gramm, "CHFA");
    return;
  }


  /* mot inconnu */

  i = 0;
  while (lex[i]) {
    if (i >= maxMot) { die("inflected form too long: %S.\n", lex); }
    symb->flechie[i] = lex[i];
    i++;
  }
  symb->flechie[i] = 0;


  symb->sorteSymbole = ATOME;

  symb->canonique    = (unichar *) xmalloc(sizeof(unichar));
  symb->canonique[0] = 0;

  symb->gramm[0] = '?';
  symb->gramm[1] = 0;

  free(lex);
}







tAutAlMot * fst2AutAlMot(Automate_fst2 * A, int nb) {

  nb++; /* Fst2 start at 1 */

  tAutAlMot * aut = initAutAlMot(A->nombre_etats_par_grf[nb]);

  tAlphMot * alphabet = alphabet_new();

  int base = A->debut_graphe_fst2[nb];

  for (int q = 0; q < (int) aut->nbEtats; q++) {

    int qq = base + q;

    aut->type[q] = 0;


    if (A->etat[qq]->controle & 1)  { aut->type[q] |= AUT_FINAL;   }
    if (A->etat[qq]->controle & 2)  { aut->type[q] |= AUT_INITIAL; }

    aut->etats[q] = NULL;

    for (struct transition_fst * trans = A->etat[qq]->trans; trans; trans = trans->suivant) {

      tSymbole symb;

      load_text_symbol(& symb, A->etiquette[trans->etiquette]->contenu);

      alphabet_clear(alphabet);

      int nbflex = symbole_developp(alphabet, & symb);

      if (nbflex == 0) { nouvTrans(aut, q, NULL, trans->arr - base); }

      while (nbflex--) {
	nouvTrans(aut, q, alphabet->symb + nbflex, trans->arr - base);
	free(alphabet->symb[nbflex].canonique);
      }
    }
  }

  alphabet_delete(alphabet);

  return aut;
}




list_aut * load_text_automaton(char * fname) {


  unichar buf[1024];

  debug("load_fst2 ...\n");
  Automate_fst2 * A = load_fst2(fname, 1);

  debug("done.\n");

  if (A == NULL) { die("cannot load %s\n", fname); }

  list_aut * res = (list_aut *) xmalloc(sizeof(list_aut));

  res->nb_aut    = A->nombre_graphes;
  res->les_aut   = (tAutAlMot **) xmalloc(sizeof(tAutAlMot *) * res->nb_aut);
  res->tailleLog = (double *) xmalloc(res->nb_aut * sizeof(double));

  tAlphMot * alphabet = alphabet_new();

  for (int i = 0; i < res->nb_aut; i++) {

    int nb   = i + 1;  /* automate in fst2 start at index 1 */
    int base = A->debut_graphe_fst2[nb];

    tAutAlMot * aut = initAutAlMot(A->nombre_etats_par_grf[nb]);

    aut->name = u_strdup(A->nom_graphe[nb]);

    for (int q = 0; q < (int) aut->nbEtats; q++) {

      int qq = base + q;

      aut->type[q] = 0;

      if (A->etat[qq]->controle & 1 /*TERMINAL*/) { aut->type[q] |= AUT_TERMINAL; }
      if (A->etat[qq]->controle & 2 /*INITIALL*/) { aut->type[q] |= AUT_INITIAL;  }

      aut->etats[q] = NULL;

      for (struct transition_fst * trans = A->etat[qq]->trans; trans; trans = trans->suivant) {

	tSymbole symb;

	u_strcpy(buf, A->etiquette[trans->etiquette]->contenu);
	load_text_symbol(& symb, buf);

	//load_text_symbol(& symb, A->etiquette[trans->etiquette]->contenu);

	symbole_strip(&symb);

	alphabet_clear(alphabet);

	int nbflex = symbole_developp(alphabet, & symb);

	if (nbflex == 0) { nouvTrans(aut, q, NULL, trans->arr - base); }

	while (nbflex--) {
	  nouvTrans(aut, q, alphabet->symb + nbflex, trans->arr - base);
	  free(alphabet->symb[nbflex].canonique);
	}
      }
    }
    res->les_aut[i] = aut;
  }


  alphabet_delete(alphabet);
  free_fst2(A);

  return res;
}





tAutAlMot * load_grammar_automaton(char * fname) {


  unichar buf[1024];

  Automate_fst2 * A = load_fst2(fname, 1);

  if (A == NULL) { die("cannot load %s\n", fname); }

  if (A->nombre_graphes != 1) { error("%d graphs in grammar %s\n", A->nombre_graphes, fname); }

  int nb   = 1;  /* automate in fst2 start at index 1 */
  int base = A->debut_graphe_fst2[nb];


  tAutAlMot * aut = initAutAlMot(A->nombre_etats_par_grf[1]);

  aut->name = u_strdup(A->nom_graphe[nb]);

  tAlphMot * alphabet = alphabet_new();


  for (int q = 0; q < (int) aut->nbEtats; q++) {

    int qq = base + q;

    aut->type[q] = 0;

    if (A->etat[qq]->controle & 1) { aut->type[q] |= AUT_TERMINAL; }
    if (A->etat[qq]->controle & 2) { aut->type[q] |= AUT_INITIAL;  }

    aut->etats[q] = NULL;

    for (struct transition_fst * trans = A->etat[qq]->trans; trans; trans = trans->suivant) {

      tSymbole symb;

      u_strcpy(buf, A->etiquette[trans->etiquette]->contenu);

      //      debug("before=%S\n", buf);
      load_gramm_symbol(& symb, buf);
      //      debug("after: "); Affiche_Symbole(&symb); fprintf(stderr, "\n");

      alphabet_clear(alphabet);

      int nbflex = symbole_developp(alphabet, & symb);

      if (nbflex == 0) { nouvTrans(aut, q, NULL, trans->arr - base); }

      while (nbflex--) {
	//#warning "must free canonique!!!!????"
	nouvTrans(aut, q, alphabet->symb + nbflex, trans->arr - base);
	free(alphabet->symb[nbflex].canonique);
      }

    }
  }

  alphabet_delete(alphabet);
  free_fst2(A);

  return aut;
}



void text_output_fst2(list_aut * txt, FILE * f) {

  unichar buf[1024];
  u_strcpy_char(buf, "0000000000");

  int i = 9;
  int n = txt->nb_aut;

  while (n) {
    buf[i--] = '0' + (n % 10);
    n = n / 10;
  }

  u_fprintf(f, "%S\n", buf);


  string_hash * hash = new_string_hash();


  /* add epsilon before all other labels */

  u_strcpy_char(buf, "<E>");

  get_hash_number(buf, hash);


  for (i = 0; i < txt->nb_aut; i++) {

    tAutAlMot * A = txt->les_aut[i];

    u_fprintf(f, "-%d %s\n", i + 1, A->name);

    for (etat q = 0; q < A->nbEtats; q++) {

      u_fputc((A->type[q] & AUT_TERMINAL) ? 't' : ':', f);
      u_fputc(' ', f);

      for (tTransitions * t = A->etats[q]; t; t = t->suivant) {

	if (t->etiq == NULL) {

	  error("<def> trans on a txt automaton???\n");
	  u_strcpy_char(buf, "<def>");

	} else {

	  switch (t->etiq->sorteSymbole) {
	    
	  case ATOME:

	    if (*t->etiq->flechie == 0) { // PONCT, CHFA ...

	      u_sprintf(buf, "%S", t->etiq->canonique);

	    } else if (*t->etiq->gramm == '?') { // unknow word
	      
	      u_sprintf(buf, "%S", t->etiq->flechie);

	    } else {

	      u_sprintf(buf, "{%S,%S.%S}", t->etiq->flechie, t->etiq->canonique, t->etiq->gramm);
	    }
	    break;

	  case SPECIAL:
	    error("text_output_fst2\n");
	    u_strcpy(buf, t->etiq->flechie);
	    break;

	  case UNIVERSEL:
	    error("text_output_fst2\n");
	    u_sprintf(buf, "<.>");
	    break;

	  case CODE:
	    error("text_output_fst2\n");
	    u_sprintf(buf, "<%S>", t->etiq->gramm);
	    break;

	  case NEGATIF:
	  case INCOMPLET:
	    error("text_output_fst2\n");
	    u_sprintf(buf, "<%S.%S>", t->etiq->canonique, t->etiq->gramm);
	    break;

	  case INDETERMINE:
	    error("ouptut_fst2: symbol code is INDETERMINE\n", t->etiq->sorteSymbole);
	    u_sprintf(buf, "<INDETERMINE:%S,%S.%S>", t->etiq->flechie, t->etiq->canonique, t->etiq->gramm);
	  }
	}

	u_fprintf(f, "%d %d ", get_hash_number(buf, hash), t->but);
      }

      u_fputc('\n', f);
    }

    u_fprints_char("f \n", f);
  }
  
  output_fst2_labels(hash, f);

  free_string_hash(hash);
}



int text_output_fst2_fname(list_aut * txt, char * fname) {

  FILE * f = u_fopen(fname, U_WRITE);

  if (f == NULL) { error("output_fst2: unable to open %s for writing\n", fname); return -1; }

  text_output_fst2(txt, f);

  fclose(f);

  return 0;
}
