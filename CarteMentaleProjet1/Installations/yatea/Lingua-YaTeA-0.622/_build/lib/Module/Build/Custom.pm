package Module::Build::Custom;
use Module::Build;
@ISA = qw(Module::Build);

sub ACTION_install {
    my $self = shift;
    $self->SUPER::ACTION_install;
#     warn "\nDon't forget to set the variables CONFIG_DIR and LOCALE_DIR in the configuration file yatea.rc to the right path if necessary.\n\n";
}

sub ACTION_fakeinstall {
    my $self = shift;
    $self->SUPER::ACTION_fakeinstall;

#     warn "\nDon't forget to set the variables CONFIG_DIR and LOCALE_DIR in the configuration file yatea.rc to the right path if necessary.\n\n";
}

sub ACTION_build {
    my $self = shift;

    require Config::General;

    warn "Setting configuration variables in etc/yatea/yatea.rc\n";
    my $rcfile = $self->base_dir() . "/etc/yatea/yatea.rc";
    # warn "RCFILE: $rcfile\n";
    chmod(0644, $rcfile);
    if (!( -f "$rcfile.orig") ) {
	File::Copy::copy($rcfile, "$rcfile.orig");
    }

    my $conf = new Config::General('-ConfigFile' => $rcfile . ".orig",
				   '-InterPolateVars' => 0,
				   '-InterPolateEnv' => 0,
				   '-StoreDelimiter' => " = ",
				   );
    my %config = $conf->getall;
    # warn "SHARE: " .  $self->install_destination("share") . "\n";
    $config{'DefaultConfig'}->{'CONFIG_DIR'} = $self->install_destination("share") . "/YaTeA/config";
    $config{'DefaultConfig'}->{'LOCALE_DIR'} = $self->install_destination("share") . "/YaTeA/locale";

    # warn $config{'DefaultConfig'}->{'CONFIG_DIR'} . "\n";

    $conf->save_file($rcfile,\%config);
    warn "Done\n";

    warn "Setting the default location of yatea.rc in lib/Lingua/YaTeA.pm\n";
    my $mainpmfile = $self->base_dir() . "/lib/Lingua/YaTeA.pm";
    
    require File::Copy;

    my $n = FileHandle->new($mainpmfile,"r");
#     File::Copy->cp($n,"$mainpmfile.orig");

    chmod(0644, $mainpmfile);
    if (!( -f "$mainpmfile.orig") ) {
	File::Copy::copy($mainpmfile, "$mainpmfile.orig");
    }
    my $line;
    open MAINPMFILE, "$mainpmfile.orig" or die "No such file ($mainpmfile.orig)\n";
    open MAINPMFILEDEST, ">$mainpmfile" or die "Can not open file $mainpmfile\n";
    while($line = <MAINPMFILE>) {
	if ($line =~ /^(\s\$rcfile\s=\s\")\/etc\/yatea\/yatea.rc(\".*\n)/) {
	    $line = $1 . $self->install_destination("conf") . "/yatea/yatea.rc" . $2;
	}
	print MAINPMFILEDEST $line;
    }
    close MAINPMFILE;
    close MAINPMFILEDEST;
    warn "Done\n";

    $self->SUPER::ACTION_build;
}

sub ACTION_clean {
    my $self = shift;
    $self->SUPER::ACTION_clean;

    require File::Copy;
    my $mainpmfile = $self->base_dir() . "/lib/Lingua/YaTeA.pm";
    File::Copy::move("$mainpmfile.orig", $mainpmfile);

    my $rcfile = $self->base_dir() . "/etc/yatea/yatea.rc";
    File::Copy::move("$rcfile.orig", $rcfile);
}


1;
