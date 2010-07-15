#
# Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#

my %data;

opendir(DIR, '.') or die "Can't opendir .: $!";
while (defined(my $file = readdir(DIR))) {
  if ($file =~ /log$/i) {
    ReadLogFile($file);
  }
}
closedir(DIR);

DumpData();


#
# Subs.
#

sub ReadLogFile {
  my $logName = shift;
  my $logTitle = $logName;
  $logTitle =~ s/(.*)\.log/$1/i;

  open (LOG, $logName) or die "Couldn't open $logName for reading: $!\n";

  my $testDes;

  while (<LOG>) {
    if (/^Test\s+\#\d+\s+(.*)/) {
      $testDes = $1;
    }
    elsif (/^Result\s+\#([0-9.]+)\s+(.*)\s\[([0-9.]+)\]/) {
      my $testNumber = $1;
      my $description = "$testDes - $2";
      my $result = $3;

      if (exists $data{$testNumber}) {
	unless ($description eq $data{$testNumber}->{description}) {
	  die "Non-matching result description for test #$testNumber\n";
	}

	if (exists $data{$testNumber}->{results}->{$logTitle}) {
	  die "Result already found for test #$testNumber in set $logTitle\n";
	}
	
	$data{$testNumber}->{results}->{$logTitle} = $result;
      }
      else {
	my $testData = { description => $description };
	$testData->{results}->{$logTitle} = $result;
	$data{$testNumber} = $testData;
      }
    }
  }

  close (LOG);
}

sub DumpData {
  print "Description,#";

  my $first = 1;
  foreach my $thisTestNum (sort { $a <=> $b } keys %data) {
    if ($first) {
      # Print remainder to title row.
      $first = 0;
      foreach (sort keys %{$data{$thisTestNum}->{results}}) {
	print ",$_";
      }
      print "\n";
    }

    print "$data{$thisTestNum}->{description},$thisTestNum";
    foreach (sort keys %{$data{$thisTestNum}->{results}}) {
      print ",$data{$thisTestNum}->{results}->{$_}";
    }
    print "\n";
  }
}
