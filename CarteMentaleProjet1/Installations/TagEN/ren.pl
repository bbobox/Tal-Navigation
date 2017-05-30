#!/usr/bin/perl

use strict;

use Annotation;
use XMLEntities;

my $doc_hash;
my $token;
my $id;
my %hash_tokens;

my $corpus;
my $ligne;

my $nb_doc;

sub sort_keys{
    my ($cle1,$cle2);
    $a=~m/^(token|word)([0-9]+)/;
    $cle1=$2;
    $b=~m/^(token|word)([0-9]+)/;
    $cle2=$2;
    return -1 if ($cle1<$cle2);
    return 0 if ($cle1==$cle2);
    return 1 if ($cle1>$cle2);
}

sub traiter_doc
{
    $corpus="";
####
    print STDERR "  Chargement des tokens...     ";
    $doc_hash=Annotation::load_xml();
    print STDERR "OK - document ".$Annotation::document_record_id." ($nb_doc)\n";
    if($doc_hash==0){
	return 0;
    }
    
####
    print STDERR "  Récupération des tokens...   ";
    foreach $token (keys %$doc_hash){
	if($token=~/^(token[0-9]+)/){
	    $id=$1;
	    $hash_tokens{$id}=$doc_hash->{$id}->{'cont'};
	}
    }
    print STDERR "OK\n";
    
####
    print STDERR "  Traitement en cours...       ";
    $corpus="";
    foreach $token(sort sort_keys keys %hash_tokens){
	$corpus.=$hash_tokens{$token};
    }
    $corpus=~s/\\n/\n/g;
    $corpus=~s/\\t/\t/g;
    XMLEntities::decode($corpus);
    
    open CORPUS,">corpus.txt";
    print CORPUS $corpus;
    close CORPUS;
    
    print `./tagen -t :bio corpus.txt`;

# dumper corpus.tag.txt en préfixant chaque ligne
    open CORPUS,"corpus.tag.txt";
    while($ligne=<CORPUS>){
	print $Annotation::document_record_id."\t".$ligne;
    }
    close CORPUS;
#
    print `rm corpus*.txt`;
    
    print STDERR "OK\n";
    return 1;
}

$nb_doc=1;
while(traiter_doc()==1){
    $nb_doc++;
}
