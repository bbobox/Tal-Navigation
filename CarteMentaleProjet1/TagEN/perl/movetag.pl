#
# Nom:
# movetag.pl
#
# Auteur:
# Jean-François Berroyer
#
# Date de création:
# 01/07/2004
#
# Date de dernière modification:
# 01/07/2004
#
# utilisation:
# perl movetag infile outfile
#
# Description:
# décalage des balises pour qu'elles collent aux mots qu'elles entourent.
# ex:  balise ouvrante:    blabla<a> aaa</a> blabla   devient blabla <a>aaa</a> blabla
# ex:  balise ouvrante:    blabla <a>aaa </a>blabla   devient blabla <a>aaa</a> blabla
#
# tous les blancs après une balise ouvrante sont replacés avant
# tous les blancs avant une balise fermante sont replacés après

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
