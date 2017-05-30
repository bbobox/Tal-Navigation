#
# Nom:
# deletex.pl
#
# Auteur:
# Jean-Fran�ois Berroyer
#
# Date de cr�ation:
# 27/05/2004
#
# Date de derni�re modification:
# 27/05/2004
#
# Param�tres:
# deletex [options] infile [outfile]
#
# Options: // non impl�ment�es
# -v verbose	affichage des informations sur la sortie standard
# -d delete	efface toujours le fichiers source (infile)
#
# Description:
# Ce script �limine les balises comportant des exceptions
# Une balise d'esception est de la forme <!...>
# ex:
# aze<b1><b2><!ex></b2></b1>rty devient azerty
#
# Si outfile n'est pas pr�cis�, c'est le texte d'entr�e qui sera modifi�
#

use strict;

# variables
my %options;  # tableau des options activ�es
my $infile;  #fichier d'entr�e
my $outfile; #fichier de sortie

# lecture des parm�tres
foreach my $arg (@ARGV) {
  # lecture d'une option
  if ($arg =~ /^-(.)$/) {
    if ($1 eq "v") {
      $options{"verbose"} = 1;
    }
    elsif ($1 eq "d") {
      $options{"delete"} = 1;
    }
  }
  # lecture du fichier d'entr�e
  elsif (!defined $infile) {
    $infile = $arg;
  }
  elsif (!defined $outfile) {
    $outfile = $arg;
  }
}

# si le fichier de sortie n'apas pas �t� d�finit, on en d�finit un
# on met aussi un flag � 1
my $flag_outfile = 0;
if (!defined $outfile) {
  $outfile = $infile.".modified";
  $flag_outfile = 1;
}


#traitement
#lecture du texte en entr�e
open (INFILE, $infile);
open (OUTFILE, ">$outfile");
my $line;
my $flagex=0; #drapeau de d�tection d'exception
my $buffer; #buffer
my $countTag = 0; #compteur de balises ouvertes
while ($line = <INFILE>) {
  my $pos = 0; #position d'une balise dans la ligne
  my $oldpos; #ancienne position
  #lecture des balises de la ligne
  while ($line =~ m/(<([!\/]?)\w*>)/g) {
    # bufferisation de la balise et de tout ce qu'il y a avant la balise
    $oldpos = $pos;
    $pos = pos $line;
    $buffer .= substr ($line, $oldpos, $pos-$oldpos);
    # analyse du type de balise
    if ($2 eq "/") { # balise fermante
      $countTag--;
    }
    elsif ($2 eq "!") { # balise d'exception
      $flagex = 1;
    }
    else { # balise ouvrante
      $countTag++;
    }
    if ($countTag == 0) { # balise externe
      # en cas d'exception, on modifie le buffer en supprimant les balises
      if ($flagex) {
        $buffer =~ s%<[!\/]?\w*>%%g;
        $flagex = 0;
      }
      # on vide le buffer
      print OUTFILE $buffer;
      $buffer = "";
    }
  }
  if ($countTag) { #si on a encore une balise ouverte � la fin de la ligne
    # bufferisation de la fin de la ligne
    $buffer .= substr ($line, $pos);
  }
  else { #sinon, print normal
    print OUTFILE substr ($line, $pos)
  }
}
#vidage �ventuel du buffer
print OUTFILE $buffer;
close OUTFILE;
close INFILE;
