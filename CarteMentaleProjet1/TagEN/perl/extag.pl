# Nom:
# extag.pl
#
# Auteur:
# Jean-Fran�ois Berroyer
#
# Date de cr�ation:
# 02/08/2004
#
# Date de derni�re modification:
# 24/08/2004
#
# utilisation: extag infile tagfile
#
# descprition:
# lit le fichier infile qui contient des balises XML
# et produit un fichier de tag de la forme:
# nom_de_la_balise	d�but	fin
# par exemple:
# html	2	10
# head	3	5
# body	5	10
#
# notons que les offset indiqu� font r�f�rence au texte auquel on a retir� toutes les balises
use utf8;
use strict;

my $infile = $ARGV[0];
my $tagfile = $ARGV[1];

my $line; # variable pour lire chaque ligne du fichier d'entr�e
my $offset = 0; # enregistre la taille du texte.
# la valeur initiale est le num�ro du premier �l�ment (g�n�ralement, 0 ou 1)

# trois piles:
# - une pour les noms de balise
# - une autre pour les offset de d�but des balises
# - une autre pour les offsets de fin de balises
my @stack_name;
my @stack_begin;
my @stack_end;
# Variables de r�cup�ration lors du pop
my $name;
my $begin;
my $end;

# compteurs de balises ouvertes

# compteur des balises qui ont �t� ouvertes puis referm�es
# revient � z�ro quand la pile se vide, c'est � dire quand $nb_open == 0
#my $nb_close = 0;

# pile des balises ouvertes
# my @stack_open;
# my $open; # r�cup�re la valeur de stack_open lors du d�pilement

# pointeur sur les tableaux @stack
# index de la case correspondant � la balise en court d'ouverture
my $index = -1;
open (INFILE, $infile);
open (OUTFILE, ">$tagfile");

# variables de sauvegarde de $1 etc.
my $tag;
my $increment; # valeur d'incr�ment de l'offset

while ($line = <INFILE>) {
	if ($line =~ m/</) { # si la ligne contient une balise
		while ($line =~ m/([^<]*)(<([^>]*)>)([^<]*)/g) {
		        
		        	my $var1 = $1;
                        utf8::decode($var1);
                        $offset += length($var1);
                        #print $1;
			# sauvegarde des variables $1, $2 ...
				$tag = $3;
                        my $var2 = $4;
                        utf8::decode($var2);
				$increment = length($var2);
			if ($tag =~ m/^\/([^>]*)/) { # balise fermante
				if ($index < 0) {
					die "Error: too much closing tags.";
				}
				if ($stack_name[$index] eq $1) {
					# on empile l'offset de la balise de fin
					$stack_end[$index] = $offset-1;
					# on recherche l'index de la derni�re balise ouverte
					# c'est la derni�re du tableau qui a une valeur ind�finie #
					while (($index > -1) && ($stack_end[$index] ne '#')) {
						$index--;
					}
				}
				else {
					# erreur: balise imbriqu�es
					die "Error: embedded tags.";
				}
				if ($index == -1) {
					# on vide les pile
					# Attention: les piles de noms et de begin sont des files
					# mais la pile de end est bien une pile
					while (@stack_name) {
						$name = shift(@stack_name);
						$begin = shift(@stack_begin);
						$end = shift(@stack_end);
						print OUTFILE "$name\t$begin\t$end\n";
					}
				}
			}
			else { # balise ouvrante
				# on empile la balise
#push(@stack_open, $tag);
				push(@stack_name, $tag);
				push(@stack_begin, $offset);
				push(@stack_end, '#'); # valeur ind�finie
				$index = @stack_name-1; # on pointe sur le dernier �l�ment
			}
			$offset += $increment; # longueur de la fin de la ligne
		}
	}
	else {
            my $varl = $line;
            utf8::decode($varl);
		$offset += length($varl); #longueur de toute la ligne
	}
}
if ($index > -1) {
	die "Error: missing closing tags.";
}

close OUTFILE;
close INFILE;

