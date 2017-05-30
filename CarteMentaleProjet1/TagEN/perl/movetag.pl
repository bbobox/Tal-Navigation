#
# Nom:
# movetag.pl
#
# Auteur:
# Jean-Fran�ois Berroyer
#
# Date de cr�ation:
# 01/07/2004
#
# Date de derni�re modification:
# 01/07/2004
#
# utilisation:
# perl movetag infile outfile
#
# Description:
# d�calage des balises pour qu'elles collent aux mots qu'elles entourent.
# ex:  balise ouvrante:    blabla<a> aaa</a> blabla   devient blabla <a>aaa</a> blabla
# ex:  balise ouvrante:    blabla <a>aaa </a>blabla   devient blabla <a>aaa</a> blabla
#
# tous les blancs apr�s une balise ouvrante sont replac�s avant
# tous les blancs avant une balise fermante sont replac�s apr�s

open (INFILE, $ARGV[0]);
open (OUTFILE, ">$ARGV[1]");

my $line;
while ($line = <INFILE>) {
  $line =~ s/(<[^\/>][^>]*>)([ \t]+)/$2$1/g; # balise ouvrante
  $line =~ s/([ \t]+)(<\/[^>]*>)/$2$1/g; #balise fermante
  print OUTFILE $line;
}

close OUTFILE;
close INFILE;
