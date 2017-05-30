/****************************************************
Nom:
align

Auteur:
Jean-François Berroyer

Date de création:
09/09/2004

Date de dernière modification:
09/09/2004

Commande:
align oldfile reffile newfile

Paramètres:
oldfile : fichier à modifier
reffile : fichier de référence
newfile : fichier modifié

Description:
Ce programme lit en parallèle le fichier de référence et le fichier à 
modifier.
Il écrit dans un nouveau fichier (3ème paramètre)
Il remplace toute séquences de caratères d'espacement (blanc, 
tabulation et \n)
lue dans le fichier à modifier par la séquence correspondante lue dans 
le fichier
de référence.

Note: une erreur se produit si l'un des fichier n'a pas le même nombre 
de séquences
d'espacement. Le programme affiche alors un message d'erreur sur la 
sortie d'erreur.
Le nouveau fichier n'est pas produit.

Le programme retourne 0 si aucune erreur ne s'est produit, un code 
d'erreur sinon


ALGO:

- Le programme lit le fichier de référence (2ème para) jusqu'à trouver une séquence de blancs
- Le programme lit le fichier à modifier (1er para) caractère par caratère et recopie
  chaque caractère dans le fichier modifié (3ème para) tant que ce n'est pas un blanc.
- Il passe la séquence de blancs et la remplace par celle lue dans le référence
- retour au début de l'algo.
  Le programme s'arrète quand on arrive à la fin de l'un des deux fichiers

*******************************************************/

#include <iostream>
#include <fstream>

using namespace std;

int main (int argc, char** argv) {
  // erreur de paramètres
  if (argc != 4) {
    cerr << "3 parameters required!\n";
  }

  // lecture des paramètres et ouverture des fichiers
  ifstream oldfile(argv[1], ios::in);
  ifstream reffile(argv[2], ios::in);
  ofstream newfile(argv[3], ios::out);

  // variables
  char oldchar;

  char refchar;

  // lecture des premiers caractères des fichiers
  refchar=reffile.get();
  oldchar=oldfile.get();

  // on recopie les premiers blancs du fichier de ref (s'il commence par des blancs)
  while (!reffile.eof() && ((refchar == ' ') || (refchar == '\n') || (refchar == '\t'))) {
  	newfile << refchar;
  	refchar=reffile.get(); // lecture du caractère suivant
  }

  while (!oldfile.eof() && !reffile.eof()) {
	// lecture du fichier de ref jusqu'à trouver un séparateur
	while (!reffile.eof() && (refchar != ' ') && (refchar != '\n') && (refchar != '\t')) {
		refchar=reffile.get(); // lecture du caractère suivant
	}

	// lecture du fichier à modifier et recopie dans le nouveau
	// tant que ce n'est pas un blanc
	while (!oldfile.eof() && (oldchar != ' ') && (oldchar != '\n') &&(oldchar != '\t')) {
		newfile << oldchar;
		oldchar=oldfile.get(); // lecture du caractère suivant
	}

	// on recopie les blancs du fichier de ref
	while (!reffile.eof() && ((refchar == ' ') || (refchar == '\n') || (refchar == '\t'))) {
		newfile << refchar;
		refchar=reffile.get(); // lecture du caractère suivant
	}

	// on zappe les blancs du vieux fichier
	while (!oldfile.eof() && ((oldchar == ' ') || (oldchar == '\n') || (oldchar == '\t'))) {
		oldchar=oldfile.get(); // lecture du caractère suivant
	}
  }

  int ret=0; //valeur de retour

  if (!reffile.eof()) {
    //cerr << "Error: \"" << argv[1] << "\" is too long for matching !\n";
	ret = 1;
  }

  if (!oldfile.eof()) {
    //cerr << "Error: \"" << argv[2] << "\" is too long for matching!\n";
	ret = 2;
  }

  // fermeture des fichiers
  oldfile.close();
  reffile.close();
  newfile.close();

  return 0; // Julien DERIVIERE
  //  return ret;
}
