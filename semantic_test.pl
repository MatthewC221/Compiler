#!/usr/bin/perl

@array = `ls parser_tests/*`;

foreach (@array) {
    $temp = $_;
    $temp =~ s/(.*)\/(.*)/$2/g;
    system("./scanner semantic $temp");
    print "./scanner semantic $temp\n";
}
