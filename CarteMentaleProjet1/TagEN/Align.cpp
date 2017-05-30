/****************************************************
Nom:
align

Auteur:
Jean-Fran�ois Berroyer

Date de cr�ation:
09/09/2004

Date de derni�re modification:
09/09/2004

Commande:
align oldfile reffile newfile

Param�tres:
oldfile : fichier � modifier
reffile : fichier de r�f�rence
newfile : fichier modifi�

Description:
Ce programme lit en parall�le le fichier de r�f�rence et le fichier � 
modifier.
Il �crit dans un nouveau fichier (3�me param�tre)
Il remplace toute s�quences de carat�res d'espacement (blanc, 
tabulation et \n)
lue dans le fichier � modifier par la s�quence correspondante lue dans 
le fichier
de r�f�rence.

Note: une erreur se produit si l'un des fichier n'a pas le m�me nombre 
de s�quences
d'espacement. Le programme affiche alors un message d'erreur sur la 
sortie d'erreur.
Le nouveau fichier n'est pas produit.

Le programme retourne 0 si aucune erreur ne s'est produit, un code 
d'erreur sinon


ALGO:

- Le programme lit le fichier de r�f�rence (2�me para) jusqu'� trouver une s�quence de blancs
- Le programme lit le fichier � modifier (1er para) caract�re par carat�re et recopie
  chaque caract�re dans le fichier modifi� (3�me para) tant que ce n'est pas un blanc.
- Il passe la s�quence de blancs et la remplace par celle lue dans le r�f�rence
- retour au d�but de l'algo.
  Le programme s'arr�te quand on arrive � la fin de l'un des deux fichiers

*******************************************************/

#include <iostream>
#include <fstream>

using namespace std;

int main (int argc, char** argv) {
  // erreur de param�tres
  if (argc != 4) {
    cerr << "3 parameters required!\n";
  }

  // lecture des param�tres et ouverture des fichiers
  ifstream oldfile(argv[1], ios::in);
  ifstream reffile(argv[2], ios::in);
  ofstream newfile(argv[3], ios::out);

  // variables
  char oldchar;

  char refchar;

  // lecture des premiers caract�res des fichiers
  refchar=reffile.get();
  oldchar=oldfile.get();

  // on recopie les premiers blancs du fichier de ref (s'il commence par des blancs)
  while (!reffile.eof() && ((refchar == ' ') || (refchar == '\n') || (refchar == '\t'))) {
  	newfile << refchar;
  	refchar=reffile.get(); // lecture du caract�re suivant
  }

  while (!oldfile.eof() && !reffile.eof()) {
	// lecture du fichier de ref jusqu'� trouver un s�parateur
	while (!reffile.eof() && (refchar != ' ') && (refchar != '\n') && (refchar != '\t')) {
		refchar=reffile.get(); // lecture du caract�re suivant
	}

	// lecture du fichier � modifier et recopie dans le nouveau
	// tant que ce n'est pas un blanc
	while (!oldfile.eof() && (oldchar != ' ') && (oldchar != '\n') &&(oldchar != '\t')) {
		newfile << oldchar;
		oldchar=oldfile.get(); // lecture du caract�re suivant
	}

	// on recopie les blancs du fichier de ref
	while (!reffile.eof() && ((refchar == ' ') || (refchar == '\n') || (refchar == '\t'))) {
		newfile << refchar;
		refchar=reffile.get(); // lecture du caract�re suivant
	}

	// on zappe les blancs du vieux fichier
	while (!oldfile.eof() && ((oldchar == ' ') || (oldchar == '\n') || (oldchar == '\t'))) {
		oldchar=oldfile.get(); // lecture du caract�re suivant
	}
  }

  int ret=0; //valeur de retour

  if (!reffile.eof()) {
	cerr << "Error: \"" << argv[1] << "\" is too long for matching!\n";
	ret = 1;
  }

  if (!oldfile.eof()) {
  	cerr << "Error: \"" << argv[2] << "\" is too long for matching!\n";
	ret = 2;
  }

  // fermeture des fichiers
  oldfile.close();
  reffile.close();
  newfile.close();

  return 0; // Julien DERIVIERE
  //  return ret;
}
