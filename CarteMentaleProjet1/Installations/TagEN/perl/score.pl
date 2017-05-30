# Nom:
# score.pl
#
# Auteur:
# Jean-François Berroyer
#
# Date de création:
# 05/08/2004
#
# Date de dernière modification:
# 25/08/2004
#
# utilisation: score file reference
#
# Calcule le score de concordance du fichier file avec le fichier reference
# Les fichiers comparé sont du même format que les fichiers de sorties de extag.pl
# chaque ligne est de la forme:
# nom_de_la_balise	begin	end
#

use strict;

# liste des compteurs
# Chaque tableau a comme clés la liste des balises et comme valeur les score associés
my %good;	# concordance parfaite avec la référence
my %spur;	# balise dans le fichier mais pas dans la reference (spurious)
my %miss;	# balise dans la reference mais oublié dans le fichier
my %near;	# concordance mais les bornes ne sont pas bien placées
my %badSpur;	# erreur de classification (concordance parfaite mais balises différentes) - type reconnu à tord
my %badMiss;	# idem mais type non reconnu
my %badNearSpur;	# erreur de classification + erreur de bornes - type reconnu à tord
my %badNearMiss;	# idem mais type non reconnu
# my %bad;	# erreur de classification (concordance parfaite mais balises différentes) - type reconnu à tord
# my %badNear;	# erreur de classification + erreur de bornes

my $file = $ARGV[0];
my $reference = $ARGV[1];

open (REFERENCE, $reference);
open (FILE, $file);

# lignes lues
my $ref_line;
my $file_line;

# lecture de la 1ere ligne dans le fichier de référence
$ref_line = <REFERENCE>;
# lecture de la 1ere ligne dans la fichier à évaluer
$file_line = <FILE>;
# variables pour chaque lignes lues
chomp($ref_line);
chomp($file_line);
my @ref_tab = split(/\t/, $ref_line);
my @file_tab = split(/\t/, $file_line);
my $ref_name = $ref_tab[0];
my $ref_begin = $ref_tab[1];
my $ref_end = $ref_tab[2];
my $file_name = $file_tab[0];
my $file_begin = $file_tab[1];
my $file_end = $file_tab[2];

while (($ref_line ne '') or ($file_line ne '')) {
	# affichage de chaque étape de la boucle
# 	print "\nRef : $ref_name\t[$ref_begin, $ref_end]\n";
# 	print "File: $file_name\t[$file_begin, $file_end]\n";

	if ($ref_begin > $file_end) {
		$spur{$file_name}++;
		$file_line = <FILE>;
	}
	elsif ($file_begin > $ref_end) {
		$miss{$ref_name}++;
		$ref_line = <REFERENCE>;
	}
	else {
		if (($ref_begin == $file_begin) && ($ref_end == $file_end)) {
			if ($file_name eq $ref_name) {
				$good{$file_name}++;
			}
			else {
				${$badSpur{$file_name}}{$ref_name}++;
				${$badMiss{$ref_name}}{$file_name}++;
			}
		}
		else {
			if ($file_name eq $ref_name) {
				$near{$file_name}++;
			}
			else {
				${$badNearSpur{$file_name}}{$ref_name}++;
				${$badNearMiss{$ref_name}}{$file_name}++;
			}
		}
		$file_line = <FILE>;
		$ref_line = <REFERENCE>;
	}
	chomp($ref_line);
	chomp($file_line);
	@ref_tab = split(/\t/, $ref_line);
	@file_tab = split(/\t/, $file_line);
	$ref_name = $ref_tab[0];
	$ref_begin = $ref_tab[1];
	$ref_end = $ref_tab[2];
	$file_name = $file_tab[0];
	$file_begin = $file_tab[1];
	$file_end = $file_tab[2];
}

close FILE;
close REFERENCE;

# Affichage des résultats

my $type; # Variable de parcourt des tables de hashage
my $key; # idem pour les tableaux bad et badNear
# pour les totaux
my $total_good = 0;
my $total_spur = 0;
my $total_miss = 0;
my $total_near = 0;
my $total_bad = 0;
my $total_badNear = 0;

my %alltags; # tableau de toutes les clés de tous les tableaux sans redondance
@alltags{(keys %good, keys %spur, keys %miss, keys %near, keys %badSpur, keys %badMiss, keys %badNearSpur, keys %badNearMiss)} = ();
#@alltags{(keys %good, keys %spur, keys %miss, keys %near, keys %bad, keys (values %bad), keys %badNear, keys (values %badNear)} = ();

#######################################################################################

print "Results by types of errors\n";
print "--------------------------\n";
if (%good > 0) {
	print "Good\n";
	foreach $type (sort keys %alltags) {
		$total_good += $good{$type};
		print "\t$type\t".int($good{$type})."\n";
	}
	print "\t---------------------------\n";
	print "\tTotal\t$total_good\n";
} else { print "\n- No Good -\n";}
if (%spur > 0) {
	print "\nSpurious\n";
	foreach $type (sort keys %alltags) {
		$total_spur += $spur{$type};
		print "\t$type\t".int($spur{$type})."\n";
	}
	print "\t---------------------------\n";
	print "\tTotal\t$total_spur\n";
} else { print "\n- No Spurious -\n";}
if (%miss > 0) {
	print "\nMissed\n";
	foreach $type (sort keys %alltags) {
		$total_miss += $miss{$type};
		print "\t$type\t".int($miss{$type})."\n";
	}
	print "\t---------------------------\n";
	print "\tTotal\t$total_miss\n";
} else { print "\n- No Missed -\n";}
if (%near > 0) {
	print "\nNear\n";
	foreach $type (sort keys %alltags) {
		$total_near += $near{$type};
		print "\t$type\t".int($near{$type})."\n";
	}
	print "\t---------------------------\n";
	print "\tTotal\t$total_near\n";
} else { print "\n- No Near -\n";}

# tableau compteur de toutes les erreurs de classement
# les clés sont de la forme "a for b"
my %bad;
foreach $type (keys %badMiss) {
	foreach $key (keys %{$badMiss{$type}}) {
		$bad{"$key\tfor\t$type"}++;
	}
}

if (%bad > 0) {
	print "\nBad\n";
	foreach $type (sort keys %bad) {
		$total_bad += $bad{$type};
		print "\t$type\t$bad{$type}\n";
	}
	print "\t---------------------------\n";
	print "\tTotal\t$total_bad\n";
} else { print "\n- No Bad -\n";}

my %badNear;
foreach $type (keys %badNearMiss) {
	foreach $key (keys %{$badNearMiss{$type}}) {
		$badNear{"$key\tfor\t$type"}++;
	}
}

if (%badNear > 0) {
	print "\nBad & Near\n";
	foreach $type (sort keys %badNear) {
		$total_badNear += $badNear{$type};
		print "\t$type\t$badNear{$type}\n";
	}
	print "\t---------------------------\n";
	print "\tTotal\t$total_badNear\n";
} else { print "\n- No Bad & Near -\n";}

# affichage des totaux et des scores de précision et de rappel
my $total_required = $total_good+$total_bad+$total_miss+$total_near+$total_badNear;
my $total_found = $total_good+$total_bad+$total_spur+$total_near+$total_badNear;
print "\nTotal required :	$total_required\n";
print "Total found :		$total_found\n\n";

my $min_precision = ($total_good/$total_found)*100;
my $near_precision = (($total_good+$total_near)/$total_found)*100;
my $cool_precision = (($total_good+0.5*$total_bad)/$total_found)*100;
my $max_precision = (($total_good+0.5*$total_bad+$total_near+0.5*$total_badNear)/$total_found)*100;
print "Precision:\n";
print "\tmin	: $min_precision %\n";
print "\t	: $cool_precision %\n";
print "\t	: $near_precision %\n";
print "	max	: $max_precision %\n";
my $min_recall = ($total_good/$total_required)*100;
my $near_recall = (($total_good+$total_near)/$total_required)*100; # en comptant bon les entité dont la borne n'est pas bonne
my $cool_recall = (($total_good+0.5*$total_bad)/$total_required)*100;
my $max_recall = (($total_good+0.5*$total_bad+$total_near+0.5*$total_badNear)/$total_required)*100;
print "Recall:\n";
print "\tmin	: $min_recall %\n";
print "\t	: $cool_recall %\n";
print "\t	: $near_recall %\n";
print "\tmax	: $max_recall %\n";

#########################################################################################

print "\nResults by types of entities\n";
print "----------------------------\n";

# tableaux de totalisation des sortes de bad (mal classé)
# les clé seront les type d'entité
my %total_badSpur; # nombre total d'entité classées à tord du type de la clé
my %total_badMiss; # nombre total d'entité de type de la clé classée à tord d'un autre type
my %total_badNearSpur;
my %total_badNearMiss;

# affichage par type d'entité
foreach $type (sort keys %alltags) {
	print "\n$type\n";
	if (%good > 0) {
		print "\tGood\t\t".int($good{$type})."\n";
	} else { print "\t- No Good -\n";}
	if (%spur > 0) {
		print "\tSpurious\t".int($spur{$type})."\n";
	} else { print "\t- No Spurious -\n";}
	if (%miss > 0) {
		print "\tMissed\t\t".int($miss{$type})."\n";
	} else { print "\t- No Missed -\n";}
	if (%near > 0) {
		print "\tNear\t\t".int($near{$type})."\n";
	} else { print "\t- No Near -\n";}

	if (keys %{$badSpur{$type}} > 0) {
 		print "\tBad spurious, $type replacing:\n";
 		foreach $key (keys %{$badSpur{$type}}) {
			$total_badSpur{$type} += ${$badSpur{$type}}{$key};
			print "\t\t$key\t${$badSpur{$type}}{$key}\n";
		}
	} else { print "\t- No Bad spurious -\n";}

	if (keys (%{$badMiss{$type}}) > 0) {
		print "\tBad missed, $type replaced by:\n";
	 	foreach $key (keys %{$badMiss{$type}}) {
			$total_badMiss{$type} += ${$badMiss{$type}}{$key};
			print "\t\t$key\t${$badMiss{$type}}{$key}\n";
		}
	} else { print "\t- No Bad missed -\n";}

	if (keys %{$badNearSpur{$type}} > 0) {
 		print "\tBad & Near spurious, $type replacing:\n";
 		foreach $key (keys %{$badNearSpur{$type}}) {
			$total_badNearSpur{$type} += ${$badNearSpur{$type}}{$key};
			print "\t\t$key\t${$badNearSpur{$type}}{$key}\n";
		}
	} else { print "\t- No Bad & Near spurious -\n";}

	if (keys (%{$badNearMiss{$type}}) > 0) {
		print "\tBad & Near missed, $type replaced by:\n";
	 	foreach $key (keys %{$badNearMiss{$type}}) {
			$total_badNearMiss{$type} += ${$badNearMiss{$type}}{$key};
			print "\t\t$key\t${$badNearMiss{$type}}{$key}\n";
		}
	} else { print "\t- No Bad & Near missed -\n";}
	print "\t---------------------------\n";
	$total_required = $good{$type}+$miss{$type}+$total_badMiss{$type}+$near{$type}+$total_badNearMiss{$type};
	$total_found = $good{$type}+$spur{$type}+$total_badSpur{$type}+$near{$type}+$total_badNearSpur{$type};
	print "\tTotal required	$total_required\n";
	print "\tTotal found	$total_found\n";
	print "\t---------------------------\n";
	if ($total_found > 0) {
		$min_precision = ($good{$type}/$total_found)*100;
		$near_precision = (($good{$type}+$near{$type})/$total_found)*100;
		$cool_precision = (($good{$type}+0.5*($total_badMiss{$type}+$total_badSpur{$type}))/$total_found)*100;
		$max_precision = (($good{$type}+0.5*($total_badMiss{$type}+$total_badSpur{$type})+$near{$type}
					+0.5*($total_badNearMiss{$type}+$total_badNearSpur{$type}))/$total_found)*100;
		print "\tPrecision:\n";
		print "\t\tmin	: $min_precision %\n";
		print "\t\t	: $cool_precision %\n";
		print "\t\t	: $near_precision %\n";
		print "\t\tmax	: $max_precision %\n";
	} else { print "\t- No precision -\n"; }
	if ($total_required > 0) {
		$min_recall = ($good{$type}/$total_required)*100;
		$near_recall = (($good{$type}+$near{$type})/$total_required)*100; # en comptant bon les entité dont la borne n'est pas bonne
		$cool_recall = (($good{$type}+0.5*($total_badMiss{$type}+$total_badSpur{$type}))/$total_required)*100;
		$max_recall = (($good{$type}+0.5*($total_badMiss{$type}+$total_badSpur{$type})+$near{$type}
					+0.5*($total_badNearMiss{$type}+$total_badNearSpur{$type}))/$total_required)*100;
		print "\tRecall:\n";
		print "\t\tmin	$min_recall %\n";
		print "\t\t	$cool_recall %\n";
		print "\t\t	$near_recall %\n";
		print "\t\tmax	$max_recall %\n";
	} else { print "\t- No recall -\n"; }
}



