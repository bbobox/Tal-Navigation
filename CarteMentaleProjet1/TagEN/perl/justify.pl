#
# Nom:
# justify.pl
#
# Auteur:
# Jean-Fran�ois Berroyer
#
# Date de cr�ation:
# 17/06/2004
#
# Date de derni�re modification:
# 17/06/2004
#
# utilisation:
# justify infile [outfile] [nb_colonnes]
#
# Description:
# Ce script justifie un texte sur un nombre de colonne sp�cifi� (par d�faut 80)
# Si une ligne fait plus que 80 caract�re, elle est d�coup�e au mot pr�s en lignes de 80 caract�res
# sauf �ventuellement la derni�re.
# Si aucun fichier n'est sp�cifi� en sortie, le r�sultat est affich� sur la sortie standard
#

# d�finition des variables
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
    # d�coupage de la ligne
    chomp $line;
    my @words = split / /, $line;
    $line = "";
    foreach my $w (@words) {
      if (length($line)+length($w) > $nb_cols) {
        chop $line; # on enl�ve le dernier blanc

        # TODO on rajoute des blancs pour avoir exactement 80 caract�res (en option)

        print $output "$line\n"; # on affiche la ligne
        $line = ""; # on r�initialise la variable $line
      }
      $line .= "$w "; # on ajoute le mot
    }
  }
  chop $line; # on enl�ve le dernier blanc ou \n
  print $output "$line\n";
}

# fermeture des fichiers
close INFILE;
close $output if $output;