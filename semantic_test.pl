#!/usr/bin/perl

@array = `ls parser_tests/*`;

foreach (@array) {
    $temp = $_;
    $temp =~ s/(.*)\/(.*)/$2/g;
    $full_path = "parser_tests/$temp";
    system("cat $full_path");
    system("./scanner semantic $temp");
    print "./scanner semantic $temp\n";
}
