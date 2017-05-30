#
# Nom:
# justify.pl
#
# Auteur:
# Jean-François Berroyer
#
# Date de création:
# 17/06/2004
#
# Date de dernière modification:
# 17/06/2004
#
# utilisation:
# justify infile [outfile] [nb_colonnes]
#
# Description:
# Ce script justifie un texte sur un nombre de colonne spécifié (par défaut 80)
# Si une ligne fait plus que 80 caractère, elle est découpée au mot près en lignes de 80 caractères
# sauf éventuellement la dernière.
# Si aucun fichier n'est spécifié en sortie, le résultat est affiché sur la sortie standard
#

# définition des variables
my $infile = $ARGV[0];
my $output = "STDOUT";
if (exists($ARGV[1])) {
  $output = "OUTFILE";
  open(OUTFILE, ">$ARGV[1]");
}
my $nb_cols = exists($ARGV[2]) ? $ARGV[2] : 80;

$tab = "        "; # taille des tabulation (8 blancs)

open(INFILE, $infile);
my $line;
while ($line = <INFILE>) {
  $line =~ s/\t/$tab/;
  if (length($line) > $nb_cols+1) {
    # découpage de la ligne
    chomp $line;
    my @words = split / /, $line;
    $line = "";
    foreach my $w (@words) {
      if (length($line)+length($w) > $nb_cols) {
        chop $line; # on enlève le dernier blanc

        # TODO on rajoute des blancs pour avoir exactement 80 caractères (en option)

        print $output "$line\n"; # on affiche la ligne
        $line = ""; # on réinitialise la variable $line
      }
      $line .= "$w "; # on ajoute le mot
    }
  }
  chop $line; # on enlève le dernier blanc ou \n
  print $output "$line\n";
}

# fermeture des fichiers
close INFILE;
close $output if $output;