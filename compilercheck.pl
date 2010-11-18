#!/usr/bin/perl
# use module
use XML::Simple;
use Data::Dumper;

$armxmlcfgfile = $ENV{'SBS_HOME'} . "/lib/config/arm.xml";
$xml = new XML::Simple;
$data = $xml->XMLin($armxmlcfgfile);
$compileraliascall = $data->{alias}->{armv5_urel}->{meaning};
print "check sbs release armv5 default compiler...\n";
$count = length($compileraliascall);
if (($count > 0) && (index($compileraliascall, "rvct") > 0)){
    $idx = index($compileraliascall, "_");
    if(($idx < ($count - 1)) && ($idx > 0)){
        $version_hig = substr($compileraliascall, $idx-1, 1);
        $version_low = substr($compileraliascall, $idx+1, 1);
        print "- sbs default release armv5 compiler found: RVCT" . $version_hig . "." . $version_low . "\n";
        # check if RVCT in sbs config is < 4.0
        if($version_hig < 4){
            #check installed ARM compiler version
            #RVCT2.2 gives nothing back
            my $result = `armcc 2>&1`;
            if( index($result, "RVCT") > 0){
                $idx = index($result, ".");
                $rvctversion_hig = substr($result, $idx-1, 1);
                $rvctversion_low = substr($result, $idx+1, 1);
                print "- installed RVCT compiler: RVCT". $rvctversion_hig . "." . $rvctversion_low . "\n";
                #check if installed RVCT is >= 4.0
                if( $rvctversion_hig >= 4){
                    #check if the variable QT_RVCT_VERSION is set to the rvct version
                    $qtvarval = $ENV{'QT_RVCT_VERSION'};
                    $qtvarshould = $rvctversion_hig. "." .$rvctversion_low;
                    if((length($qtvarval) <= 0) || ($qtvarshould != $qtvarval)){
                        print "- Please set enviroment variable QT_RVCT_VERSION=";
                        print "$rvctversion_hig.$rvctversion_low and run configure again!!!\n";
                    }
                }
            }
            else{
                print "- OK\n";
            }
        }
    }
}
else{
    print "- OK\n";
}
