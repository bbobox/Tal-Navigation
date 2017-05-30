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

#include "VerifierRecursion.h"



//
// si l'etiquette reconnait <E>, on met son champ controle a 1, a 0 sinon
//
void controler_epsilon_etiquette(Etiquette e) {
if ((e->contenu!=NULL) && (u_strlen(e->contenu)>=3) &&(e->contenu[0]=='<') && (e->contenu[1]=='E') && (e->contenu[2]=='>'))
  e->controle=1;
else e->controle=0;
}


//
// affiche la suite de graphes qui aboutit a la recursion par le graphe e
//
void afficher_liste_graphes_inverse(Liste_num l,int e,unichar **nom_graphe) {
char temp[2000];
u_to_char(temp,nom_graphe[l->numero]);
if (l->numero==e) {
  fprintf(stderr,"ERROR: %s",temp);
  return;
}
afficher_liste_graphes_inverse(l->suivant,e,nom_graphe);
fprintf(stderr," calls %s that",temp);
}


//
// insere le numero n en tete de la liste l
//
void inserer_en_tete(Liste_num *l,int n) {
Liste_num tmp;
tmp=(Liste_num)malloc(sizeof(struct liste_num));
if (tmp==NULL) {
  fprintf(stderr,"Probleme d'allocation memoire dans la fonction inserer_en_tete\n");
  exit(1);
}
tmp->numero=n;
tmp->suivant=*l;
*l=tmp;
}


int dernier(Liste_num l) {
while (l->suivant!=NULL)
  l=l->suivant;
return l->numero;
}


//
// supprime le numero en tete de la liste l
//
void supprimer_tete(Liste_num *l) {
Liste_num tmp;
tmp=(*l)->suivant;
free(*l);
*l=tmp;
}



//
// verifie si de l'etat e on peut atteindre par <E> une transition vers le graphe g
//
int peut_aller_par_E(Etat_fst e,int g,Etat_fst* graphe,Etiquette* etiquette,int *verifie){
liste_transition l;
if (e==NULL) return 0;
l=e->trans;
while (l!=NULL) {
  // cas positif
  if (l->etiquette==-g) {
    return 1;
  }
  // si on a <E> ou un sous-graphe qui reconnait <E>
  // on explore la suite
  if (l->etiquette<0 && verifie[-(l->etiquette)]==2) {
    if (peut_aller_par_E(graphe[l->arr],g,graphe,etiquette,verifie))
      return 1;
  }
  if (l->etiquette>=0 && etiquette[l->etiquette]->controle)
    if (peut_aller_par_E(graphe[l->arr],g,graphe,etiquette,verifie))
      return 1;
  l=l->suivant;
}
return 0;
}



//
// parcours les transitions partant de l'etat e
//
int explorer_etat(Etat_fst e,Liste_num l,int *debut_graphe,
                  unichar** nom_graphe,Etat_fst* graphe,Etiquette* etiquette,
                  int* verifie) {
liste_transition liste;

int ret=0;
int retour;
if (e==NULL) return 0;
if ((e->controle&8)||(e->controle&64)) {
  return 0; // on a deja traite cet etat
}
e->controle=(unsigned char)(e->controle+8);
e->controle=(unsigned char)(e->controle+64);
liste=e->trans;
while (liste!=NULL) {
  if (liste->etiquette<0) {
    // cas d'un sous-graphe
    retour=chercher_recursion(-(liste->etiquette),l,debut_graphe,nom_graphe,graphe,etiquette,verifie);
    if (retour==1) {
      // si il y a une recursion
      return 1;
    }
    if (verifie[-liste->etiquette] && explorer_etat(graphe[liste->arr],l,debut_graphe,nom_graphe,graphe,etiquette,verifie)) {
      // si le graphe reconnait <E>
      return 1;
    }
  }
  else
  if (etiquette[liste->etiquette]->controle) {
    // on est dans le cas <E>
    if (explorer_etat(graphe[liste->arr],l,debut_graphe,nom_graphe,graphe,etiquette,verifie)) {
      return 1;
    }
  }
  liste=liste->suivant;
}
if (e->controle&8) e->controle=(unsigned char)(e->controle-8);
return ret;
}



//
// renvoie 1 si n appartient a la liste l
//
int appartient_a_liste(int n,Liste_num l) {
if (l==NULL) return 0;
while (l!=NULL) {
  if (l->numero==n)
    return 1;
  l=l->suivant;
}
return 0;
}




//
// renvoie 1 si une recursion est trouvee dans le graphe numero e, 0 sinon
//
int chercher_recursion(int e,Liste_num l,int* debut_graphe,unichar** nom_graphe,Etat_fst* graphe,Etiquette* etiquette,int* verifie) {
int ret;

if (appartient_a_liste(e,l)) {
    // on retombe sur un graphe deja visite
    afficher_liste_graphes_inverse(l,e,nom_graphe);
    char temp[2000];
    u_to_char(temp,nom_graphe[e]);
    fprintf(stderr," recalls the graph %s\n",temp);
    return 1;
  }
inserer_en_tete(&l,e);
ret=explorer_etat(graphe[debut_graphe[e]],l,debut_graphe,nom_graphe,graphe,etiquette,verifie);
supprimer_tete(&l);
return ret;
}


//
// renvoie 1 si on retombe par <E> sur un etat deja explore, 0 sinon
//
int chercher_boucle_par_E_dans_etat(Etat_fst e,Etat_fst* graphe,
                                    Etiquette* etiquette,int verifie[]) {
liste_transition l;
if (e->controle&8) {
  e->controle=(unsigned char)(e->controle-8);
  return 1; // on est deja passe par la...
}

e->controle=(unsigned char)(e->controle+8);
l=e->trans;
while (l!=NULL) {
  if (l->etiquette<0) {
    if (verifie[-l->etiquette]) {
      if (chercher_boucle_par_E_dans_etat(graphe[l->arr],graphe,etiquette,verifie)) {
        e->controle=(unsigned char)(e->controle-8);
        return 1;
      }
    }
  }
  else
  if (etiquette[l->etiquette]->controle) {
    // on est dans le cas <E>
    if (chercher_boucle_par_E_dans_etat(graphe[l->arr],graphe,etiquette,verifie)) {
      e->controle=(unsigned char)(e->controle-8);
      return 1;
    }
  }
  l=l->suivant;
}
e->controle=(unsigned char)(e->controle-8);
return 0;
}



//
// renvoie 1 si une boucle par <E> est trouvee dans le graphe numero e, 0 sinon
//
int chercher_boucle_par_E(int e,int* debut_graphe,unichar** nom_graphe,Etat_fst* graphe,Etiquette* etiquette,int* verifie,int *nombre_etats_par_grf) {
int i,debut;
debut=debut_graphe[e];
if (debut<0) {
  fprintf(stderr,"Problem in the function chercher_boucle_par_E: debut_graphe[%d] not refreshed\n",e);
  return 0;
}
// on remet les controles comme apres le chargement
for (i=0;i<nombre_etats_par_grf[e];i++) {
  if (graphe[debut+i]->controle&8)
    graphe[debut+i]->controle=(unsigned char)(graphe[debut+i]->controle-8);
  if (graphe[debut+i]->controle&16)
    graphe[debut+i]->controle=(unsigned char)(graphe[debut+i]->controle-16);
}

//--------------------
for (i=0;i<nombre_etats_par_grf[e];i++)
  if (chercher_boucle_par_E_dans_etat(graphe[debut+i],graphe,etiquette,verifie)) {
    char temp[2000];
    u_to_char(temp,nom_graphe[e]);
    fprintf(stderr,"ERROR: <E> loop in the graph %s\n",temp);
    return 1;
  }
return 0;
}


//
// renvoie 1 si aucune boucle infinie n'est trouve dans le fst2 nom_fst2, 0 sinon
//
int pas_de_recursion_old(char *nom_fst2) {
FILE *f;
Etat_fst* graphe;
Etiquette* etiquette;
int* debut_graphe;
int* nombre_etats_par_grf;
int* verifie;
unichar** nom_graphe;
int nombre_graphes;
int nombre_etats;
int nombre_etiquettes;
int i,ret,retour;

f=u_fopen(nom_fst2,U_READ);
if (f==NULL) {
  fprintf(stderr,"Cannot open the graph %s\n",nom_fst2);
  return 0;
}
graphe=(Etat_fst*)malloc(sizeof(Etat_fst)*NBRE_ETATS);
etiquette=(Etiquette*)malloc(sizeof(Etiquette)*NBRE_ETIQUETTES);
if (graphe==NULL || etiquette==NULL) {
  fprintf(stderr,"Probleme d'allocation memoire dans la fonction pas_de_recursion\n");
  exit(1);
}
charger_graphe_fst2(f,graphe,etiquette,&nombre_graphes,&nombre_etats,
                         &nombre_etiquettes,&debut_graphe,&nom_graphe,1,&nombre_etats_par_grf);
u_fclose(f);
//---------------------------------
verifie=(int*)malloc(sizeof(int)*(nombre_graphes+1));
for (i=0;i<nombre_graphes+1;i++)
  verifie[i]=0;
for (i=0;i<nombre_etiquettes;i++)
  controler_epsilon_etiquette(etiquette[i]);

retour=0;

for (i=1;i<nombre_graphes+1;i++) {
  if (verifie[i]==0) {
    ret=chercher_recursion(i,NULL,debut_graphe,nom_graphe,graphe,etiquette,verifie);
    retour=ret||retour;
  }
}

for (i=1;i<nombre_graphes+1;i++) {
  ret=chercher_boucle_par_E(i,debut_graphe,nom_graphe,graphe,etiquette,verifie,nombre_etats_par_grf);
  retour=ret||retour;
}
//--------------------------------------------------

liberer_graphe_fst2(nombre_etats,nombre_etiquettes,debut_graphe,graphe,etiquette);
for (i=1;i<=nombre_graphes;i++)
  free(nom_graphe[i]);
free(nom_graphe);
free(verifie);
free(nombre_etats_par_grf);
free(graphe);
free(etiquette);
return !retour;
}


/////////////////////////////////////////////////////////////////////////////
// NOUVELLE VERSION DE LA DETECTION D'ERREURS
/////////////////////////////////////////////////////////////////////////////

#define OK 0
#define RECONNAIT_E 1
#define BOUCLE_PAR_E 2
#define AUCUNE_RECURSION 1
#define RECURSION 0

struct condition {
  int numero;
  struct condition* suivant;
};
typedef struct condition* Condition;

struct liste_conditions {
  Condition cond;
  struct liste_conditions* suivant;
};
typedef struct liste_conditions* Liste_conditions;


void liberer_condition(Condition c) {
Condition tmp;
while (c!=NULL) {
  tmp=c;
  c=c->suivant;
  free(tmp);
}
}


void liberer_liste_conditions(Liste_conditions l) {
Liste_conditions tmp;
while (l!=NULL) {
  liberer_condition(l->cond);
  tmp=l;
  l=l->suivant;
  free(tmp);
}
}


//
// renvoie 1 si l'etat e est terminal, 0 sinon
//
int est_terminal(Etat_fst e) {
return (e->controle & 1);
}



//
// supprime le dernier element de la condition
//
void supprimer_dernier_element_condition(Condition* c) {
Condition precedent,tmp;
if ((*c)==NULL) return;
if ((*c)->suivant==NULL) {
  free(*c);
  *c=NULL;
  return;
}
precedent=*c;
tmp=(*c)->suivant;
while (tmp!=NULL) {
  precedent=tmp;
  tmp=tmp->suivant;
}
free(tmp);
precedent->suivant=NULL;
}


//
// affiche la condition c
//
void afficher_condition(Condition c,unichar** nom_graphe) {
if (c==NULL) {
  printf("\n%s",CR);
  return;
}
while (c!=NULL) {
  char temp[2000];
  u_to_char(temp,nom_graphe[c->numero]);
  printf("(%d,%s) ",c->numero,temp);
  c=c->suivant;
}
printf("\n%s",CR);
}


//
// affiche la liste de conditions l
//
void afficher_liste_conditions(Liste_conditions l,unichar** nom_graphe) {
int i=0;
while (l!=NULL) {
  printf("%d) ",i++);
  afficher_condition(l->cond,nom_graphe);
  l=l->suivant;
}
}


//
// renvoie une copie de la liste c
//
Condition copie_condition(Condition c) {
Condition tmp;
if (c==NULL) return NULL;
tmp=(Condition)malloc(sizeof(struct condition));
tmp->numero=c->numero;
tmp->suivant=copie_condition(c->suivant);
return tmp;
}



//
// renvoie une copie de la liste l
//
Liste_conditions copie_liste_conditions(Liste_conditions l) {
Liste_conditions tmp;
if (l==NULL) return NULL;
tmp=(Liste_conditions)malloc(sizeof(struct liste_conditions));
tmp->cond=copie_condition(l->cond);
tmp->suivant=copie_liste_conditions(l->suivant);
return tmp;
}


Condition nouvelle_condition(int n) {
Condition tmp;
tmp=(Condition)malloc(sizeof(struct condition));
tmp->numero=n;
tmp->suivant=NULL;
return tmp;
}



//
// insere n dans la liste c si il n'y est pas deja
//
Condition inserer_dans_condition(int n,Condition c) {
if (c==NULL) return nouvelle_condition(n);
if (c->numero==n) return c;
c->suivant=inserer_dans_condition(n,c->suivant);
return c;
}


//
// insere le graphe numero n dans la liste de conditions l
//
void inserer_en_debut_de_conditions(int n,Liste_conditions* l) {
Liste_conditions tmp;
if (*l==NULL) {
  // si la liste est vide, on en cree une
  tmp=(Liste_conditions)malloc(sizeof(struct liste_conditions));
  tmp->suivant=NULL;
  tmp->cond=nouvelle_condition(n);
  *l=tmp;
  return;
}
tmp=*l;
while (tmp!=NULL) {
  tmp->cond=inserer_dans_condition(n,tmp->cond);
  tmp=tmp->suivant;
}
}



//
// fusionne les conditions d dans les conditions c
//
void fusionner(Liste_conditions *c,Liste_conditions d) {
Liste_conditions tmp;
if (*c==NULL) {
  *c=d;
  return;
}
if ((*c)->suivant==NULL) {
  (*c)->suivant=d;
  return;
}
tmp=*c;
while (tmp->suivant!=NULL) tmp=tmp->suivant;
tmp->suivant=d;
}



//
// teste si on peut reconnaitre <E> depuis l'etat courant
//
int explorer_graphe_E(int premier_etat,int indice,Etat_fst* graphe,Etiquette *etiquette,
                      int n_graphe,unichar** nom_graphe,
                      Liste_conditions conditions_pour_etat[],
                      Liste_conditions *cond_tmp) {
liste_transition l;
Etat_fst e;
int retour=OK;
int ret;

*cond_tmp=NULL;
e=graphe[indice];

if (est_terminal(e)) {
  // si on est arrive dans un etat terminal, c'est que le graphe a reconnu <E>
  e->controle=(char)(e->controle|32);
  // le 32 indique que de cet etat on peut reconnaitre <E> sans condition
  return RECONNAIT_E;
}
if (e->controle & 8) {
  // si on a fait une boucle, on en sort sans rien faire (on les traitera
  // plus tard)
  return OK;
}
if (e->controle & 16) {
  // si on est dans un etat deja visite
  if (e->controle & 32) {
    // si on peut atteindre <E> sans condition, on ne regarde rien
    // d'autre
    return RECONNAIT_E;
  }
  if (e->controle & 64) {
    // si on peut reconnaitre <E> avec des conditions, alors on retourne
    // une copie de ces conditions dans cond_tmp
    *cond_tmp=copie_liste_conditions(conditions_pour_etat[indice-premier_etat]);
    return RECONNAIT_E;
  }
  return OK;
}

l=e->trans;

e->controle=(char)(e->controle | 16);
// la valeur 16 indique que l'on est deja passe par cet etat. Cela
// permet d'eviter d'explorer plusieurs fois les memes chemins
e->controle=(char)(e->controle | 8);
// la valeur 8 n'est que temporaire. Elle sert a detecter les boucles

while (l!=NULL) {
    if (l->etiquette < 0) {
      // cas d'un sous-graphe
      *cond_tmp=NULL;
      ret=explorer_graphe_E(premier_etat,l->arr,graphe,etiquette,n_graphe,nom_graphe,conditions_pour_etat,cond_tmp);
      if (ret==RECONNAIT_E) {
        // si l'exploration a permis de reconnaitre <E>
        e->controle=(char)(e->controle|64);
        // on insere la nouvelle condition en tete des conditions
        inserer_en_debut_de_conditions(-(l->etiquette),cond_tmp);
        // on fusionne les nouvelles conditions avec celles deja existantes
        // pour cet etat
        fusionner(&conditions_pour_etat[indice-premier_etat],*cond_tmp);
        *cond_tmp=NULL;
      }
      retour=retour|ret;
    }
    else
    if (etiquette[l->etiquette]->controle&1) {
      // cas d'une transition par <E>
      *cond_tmp=NULL;
      ret=explorer_graphe_E(premier_etat,l->arr,graphe,etiquette,n_graphe,nom_graphe,conditions_pour_etat,cond_tmp);
      if (ret==RECONNAIT_E) {
        // si l'exploration a permis de reconnaitre <E>
        if (*cond_tmp==NULL) {
          // 1er cas: pas de condition
          e->controle=(char)(e->controle|32);
        }
        else {
        e->controle=(char)(e->controle|64);
        // on insere la nouvelle condition en tete des conditions
        // on fusionne les nouvelles conditions avec celles deja existantes
        // pour cet etat
        fusionner(&conditions_pour_etat[indice-premier_etat],*cond_tmp);
        *cond_tmp=NULL;
        }
      }
      retour=retour|ret;
    }
  l=l->suivant;
}
e->controle=(char)(e->controle-8);
*cond_tmp=copie_liste_conditions(conditions_pour_etat[indice-premier_etat]);
return retour;
}




//
// cherche dans la condition c si tous les membres sont marques 32 (reconnaissent
// <E>. Si c'est le cas, on reconnait_E est mis � 1
// Si un des membres est marque 128, on supprime la condition
//
Condition resoudre_simple_condition(Condition c,Etat_fst graphe[],
                                    int debut_graphe[],int *modification,
                                    int *reconnait_E) {
// reconnait_E = 1 si on reconnait <E>
//               2 si on ne reconnait pas <E>
//               3 si on ne sait pas
Condition tmp;
*reconnait_E=1;
if (c==NULL) return NULL;
tmp=resoudre_simple_condition(c->suivant,graphe,debut_graphe,modification,reconnait_E);
if ((*reconnait_E)==2) {
  // si un element de la liste ne reconnait pas <E>, il faut detruire toute
  // la liste (la suite pointee par tmp a deja ete liberee)
  free(c);
  (*modification)++;
  return NULL;
}
if ((*reconnait_E)==1) {
  // si tous les elements suivant reconnaissent <E>
  c->suivant=tmp;
  if (graphe[debut_graphe[c->numero]]->controle&32) {
    // si cet element aussi reconnait <E>
    return c;
  }
  if (graphe[debut_graphe[c->numero]]->controle&64 && !(graphe[debut_graphe[c->numero]]->controle&128)) {
    // si on ne sait pas
    *reconnait_E=3;
    return c;
  }

  // si on ne reconnait pas <E>, on vide la suite
  liberer_condition(c->suivant);
  free(c);
  *reconnait_E=2;
  (*modification)++;
  return NULL;
}
// si on n'est pas fixe sur la suite de la liste
c->suivant=tmp;
if (graphe[debut_graphe[c->numero]]->controle&32) {
  // si l'element reconnait <E>, on ne peut toujours pas decider
  *reconnait_E=3;
  return c;
}
if (graphe[debut_graphe[c->numero]]->controle&128) {
  // si l'element ne reconnait pas <E> alors on est fixe: la condition
  // ne peut etre verifie, on la supprime
  liberer_condition(c->suivant);
  free(c);
  *reconnait_E=2;
  (*modification)++;
  return NULL;
}
// si on ne sait toujours pas on ne fait rien
*reconnait_E=3;
return c;
}



//
// nettoye recursivement la liste de conditions l
//
Liste_conditions resoudre_liste_conditions(Liste_conditions l,Etat_fst graphe[],
                               int debut_graphe[],int *modification,int *reconnait_E) {
Liste_conditions tmp;
if (l==NULL) return NULL;
if (l->cond==NULL) {
  fprintf(stderr,"Erreur interne dans resoudre_liste_conditions\n");
  return NULL;
}
l->cond=resoudre_simple_condition(l->cond,graphe,debut_graphe,modification,reconnait_E);
if (*reconnait_E==1) {
  // si une condition reconnait <E> alors le graphe entier reconnait <E>
  return l;
}
if (l->cond==NULL) {
  // si on a vire toute la condition
  tmp=l->suivant;
  free(l);
  return resoudre_liste_conditions(tmp,graphe,debut_graphe,modification,reconnait_E);
}
l->suivant=resoudre_liste_conditions(l->suivant,graphe,debut_graphe,modification,reconnait_E);
return l;
}


//
// resoud les conditions du graphe indice
//
void resoudre_conditions_du_graphe(int indice,Liste_conditions conditions[],
                                   Etat_fst graphe[],int debut_graphe[],
                                   int *modification,unichar** nom_graphe) {
int reconnait_E;
if (conditions[indice]==NULL) {
  fprintf(stderr,"Probleme interne avec le graphe %d\n",indice);
  return;
}

// on nettoie recursivement toutes les conditions
reconnait_E=0;

conditions[indice]=resoudre_liste_conditions(conditions[indice],graphe,debut_graphe,modification,&reconnait_E);

if (reconnait_E==1) {
  // si une des conditions a ete verifiee
  graphe[debut_graphe[indice]]->controle=(char)(graphe[debut_graphe[indice]]->controle|32);
  liberer_liste_conditions(conditions[indice]);
  conditions[indice]=NULL;
  (*modification)++;
  return;
}

if (conditions[indice]==NULL) {
  // si le graphe n'a plus de condition
  // on le marque comme 128 (ne reconnait pas <E>)
  liberer_liste_conditions(conditions[indice]);
  conditions[indice]=NULL;
  graphe[debut_graphe[indice]]->controle=(char)(graphe[debut_graphe[indice]]->controle|128);
  (*modification)++;
}
}



//
// parcourt les conditions de chaque graphe en essayant d'en resoudre
// modification vaut 1 si au moins une op�ration a �t� faite, 0 sinon
//
void resoudre_conditions(Liste_conditions conditions[],int nombre_graphes,
                         Etat_fst graphe[],int debut_graphe[],int *modification,
                         unichar** nom_graphe) {
int i;
*modification=0;

// 32 = le graphe reconnait <E> sans condition
// 64 = le graphe peut reconnaitre <E> sous certaines conditions
// 128 = le graphe ne reconnait pas <E>
for (i=1;i<nombre_graphes+1;i++)
  if (!(graphe[debut_graphe[i]]->controle&32)) {
    if (!(graphe[debut_graphe[i]]->controle&64) ) {
      if (!(graphe[debut_graphe[i]]->controle&128)) {
        graphe[debut_graphe[i]]->controle=(char)(graphe[debut_graphe[i]]->controle|128);
        (*modification)++;
      }
    }
    // si le graphe ne reconnait pas deja <E> sans condition, et qu'il n'est
    // pas marque comme ne reconnaissant pas <E>, et qu'il peut reconnaitre <E>
    // sous conditions
    else if (!(graphe[debut_graphe[i]]->controle&128)) {
      // on ne traite le graphe que si on ne sait pas encore s'il peut
      // reconnaitre <E> ou non
      resoudre_conditions_du_graphe(i,conditions,graphe,debut_graphe,modification,nom_graphe);
    }
  }
}



//
// verifie si le graphe indice a une recursion infinie dans ses conditions
// renvoie 1 si oui, 0 sinon
//
int check_problemes_du_graphe(int indice,Etat_fst graphe[],int debut_graphe[],
                              Liste_conditions conditions[],Liste_num liste_deja_vus,
                              unichar** nom_graphe) {
Liste_conditions l;
Condition c;
Liste_num liste;
int ERROR=0;
char temp[2000];
 u_to_char(temp,nom_graphe[indice]);

if (graphe[debut_graphe[indice]]->controle&128)
  // si le graphe n'a pas de probleme
  return 0;
if (graphe[debut_graphe[indice]]->controle&32) {
  fprintf(stderr,"WARNING: the graph %s recognizes <E>\n",temp);
  return 0;
}

if (appartient_a_liste(indice,liste_deja_vus)) {
  afficher_liste_graphes_inverse(liste_deja_vus,indice,nom_graphe);
  fprintf(stderr," calls again %s\n",temp);
  return 1;
}
l=conditions[indice];
liste=(Liste_num)malloc(sizeof(struct liste_num));
liste->numero=indice;
liste->suivant=liste_deja_vus;
while (!ERROR && l!=NULL) {
  c=l->cond;
  while (!ERROR && c!=NULL) {
    ERROR=check_problemes_du_graphe(c->numero,graphe,debut_graphe,conditions,liste,nom_graphe);
    c=c->suivant;
  }
  l=l->suivant;
}
free(liste);
return ERROR;
}



//
// nettoye les marqueurs mis sur les champ controle des etats et met verifie[i]
// a 1 si le grpahe i reconnait <E>, 0 sinon
//
void nettoyer_controle(Etat_fst graphe[],int debut_graphe[],int verifie[],int nombre_graphes,int nombre_etats) {
int i;
if (verifie!=NULL) {
  for (i=1;i<nombre_graphes+1;i++)
    if (graphe[debut_graphe[i]]->controle&32)
      verifie[i]=1;
    else verifie[i]=0;
}
for (i=0;i<nombre_etats;i++) {
  if (graphe[i]->controle&8) graphe[i]->controle=(char)(graphe[i]->controle-8);
  if (graphe[i]->controle&16) graphe[i]->controle=(char)(graphe[i]->controle-16);
  if (graphe[i]->controle&32) graphe[i]->controle=(char)(graphe[i]->controle-32);
  if (graphe[i]->controle&64) graphe[i]->controle=(char)(graphe[i]->controle-64);
  if (graphe[i]->controle&128) graphe[i]->controle=(char)(graphe[i]->controle-128);
}
}

//
// renvoie 1 si aucune boucle infinie n'est trouve dans le fst2 nom_fst2, 0 sinon
//
int pas_de_recursion(char *nom_fst2) {
FILE *f;
Etat_fst* graphe;
Etiquette* etiquette;
Liste_conditions* conditions;
Liste_conditions* conditions_pour_etat;
int* debut_graphe;
int* nombre_etats_par_grf;
int* verifie;
unichar** nom_graphe;
int nombre_graphes;
int nombre_etats;
int nombre_etiquettes;
int i,j;
int ERROR=0;


printf("Recursion detection started\n%s",CR);
f=u_fopen(nom_fst2,U_READ);
if (f==NULL) {
  fprintf(stderr,"Cannot open the graph %s\n",nom_fst2);
  return 0;
}
graphe=(Etat_fst*)malloc(sizeof(Etat_fst)*NBRE_ETATS);
etiquette=(Etiquette*)malloc(sizeof(Etiquette)*NBRE_ETIQUETTES);
if (graphe==NULL || etiquette==NULL) {
  fprintf(stderr,"Probleme d'allocation memoire dans la fonction pas_de_recursion\n");
  exit(1);
}
charger_graphe_fst2(f,graphe,etiquette,&nombre_graphes,&nombre_etats,
                         &nombre_etiquettes,&debut_graphe,&nom_graphe,1,&nombre_etats_par_grf);

u_fclose(f);
//---------------------------------
verifie=(int*)malloc(sizeof(int)*(nombre_graphes+1));
conditions=(Liste_conditions*)malloc(sizeof(Liste_conditions)*(nombre_graphes+1));
if (verifie==NULL || conditions==NULL) {
  fprintf(stderr,"Probleme d'allocation memoire dans la fonction pas_de_recursion\n");
  exit(1);
}

for (i=0;i<nombre_graphes+1;i++) {
  verifie[i]=OK;
  conditions[i]=NULL;
}
for (i=0;i<nombre_etiquettes;i++)
  controler_epsilon_etiquette(etiquette[i]);


// on cherche les graphes qui reconnaissent <E> avec ou sans condition
for (i=1;i<nombre_graphes+1;i++) {
  conditions_pour_etat=(Liste_conditions*)malloc(sizeof(Liste_conditions)*nombre_etats_par_grf[i]);
  if (conditions_pour_etat==NULL) {
    fprintf(stderr,"Probleme d'allocation memoire\n");
    exit(1);
  }
  for (j=0;j<nombre_etats_par_grf[i];j++)
    conditions_pour_etat[j]=NULL;
  verifie[i]=explorer_graphe_E(debut_graphe[i],debut_graphe[i],graphe,etiquette,i,nom_graphe,conditions_pour_etat,&conditions[i]);
  if (conditions[i]!=NULL)
    liberer_liste_conditions(conditions[i]);
  conditions[i]=conditions_pour_etat[0];
  conditions_pour_etat[0]=NULL;
  for (j=1;j<nombre_etats_par_grf[i];j++)
    liberer_liste_conditions(conditions_pour_etat[j]);
  free(conditions_pour_etat);
}


printf("Resolving <E> conditions\n%s",CR);
i=1;
while (i) resoudre_conditions(conditions,nombre_graphes,graphe,debut_graphe,&i,nom_graphe);

if (graphe[debut_graphe[1]]->controle&32) {
  // si le graphe principal reconnait <E>
  char temp[2000];
  u_to_char(temp,nom_graphe[1]);
  fprintf(stderr,"ERROR: the main graph %s recognizes <E>\n",temp);
  ERROR=1;
}

if (!ERROR) {
  printf("Checking <E> dependancies\n%s",CR);
  for (i=1;(!ERROR) && i<nombre_graphes+1;i++) {
    ERROR=ERROR|check_problemes_du_graphe(i,graphe,debut_graphe,conditions,NULL,nom_graphe);
  }
}
nettoyer_controle(graphe,debut_graphe,verifie,nombre_graphes,nombre_etats);
if (!ERROR) {
  printf("Looking for <E> loops\n%s",CR);
  for (i=1;!ERROR && i<nombre_graphes+1;i++) {
    ERROR=chercher_boucle_par_E(i,debut_graphe,nom_graphe,graphe,etiquette,verifie,nombre_etats_par_grf);
  }
}
nettoyer_controle(graphe,debut_graphe,NULL,nombre_graphes,nombre_etats);
if (!ERROR) {
  printf("Looking for infinite recursions\n%s",CR);
  for (i=1;!ERROR && i<nombre_graphes+1;i++) {
    ERROR=chercher_recursion(i,NULL,debut_graphe,nom_graphe,graphe,etiquette,verifie);
  }
}


liberer_graphe_fst2(nombre_etats,nombre_etiquettes,debut_graphe,graphe,etiquette);
for (i=1;i<=nombre_graphes;i++) {
  free(nom_graphe[i]);
  liberer_liste_conditions(conditions[i]);
}
free(nom_graphe);
free(verifie);
free(conditions);
free(nombre_etats_par_grf);
free(graphe);
free(etiquette);
printf("Recursion detection completed\n%s",CR);
if (ERROR) return RECURSION;
else return AUCUNE_RECURSION;
}








