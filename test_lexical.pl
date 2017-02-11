#!/usr/bin/perl -w

#the syntax for tests, scanner is ./test_lexical.pl 5
#parser is ./test_lexical parse --- PARSe


@array = `ls tests/`;

$i = 0;

if ($ARGV[0] eq "parse") {
    $number = $ARGV[1];
    system("./scanner parse -1");
} else {
    $number = $ARGV[0];
    foreach my $temp (@array) {
	    if ($i >= $number) {
		    last;
	    }
        $string = "parse $temp";
        system("./scanner $string");
        print ("$string\n");
	    $i++;
    }
}

