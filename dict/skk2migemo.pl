#!/usr/bin/env perl
# vim:set ts=8 sts=4 sw=4 tw=0 et:
#
# conv.pl - Convert SKK-JISYO to migemo-dict
#
# Author:  MURAOKA Taro <koron.kaoriya@gmail.com>

use strict;
use warnings;

binmode STDOUT, ':utf8';
binmode STDIN,  ':utf8';

while (<>)
{
    chomp;
    if (m/^;/)
    {
        print "$_\n";
        next;
    }
    m/^([^ ]+) +(.*)$/;
    my ($key, $value) = ($1, $2);

    # Skip special key
    next if $key =~ m/^[<>?]/ or $key =~ m/[<>?]$/;
    # key have number expansion
    my $have_number = ($key =~ m/#/) ? 1 : 0;
    # Remove "okuri"
    if ($key =~ m/[a-z]$/ and $key !~ m/^[ -~]+$/)
    {
        $key =~ s/.$//;
    }

    $value =~ s{^/}{};
    $value =~ s{/$}{};
    my @values = grep {
        # Remove lisp expressions.
        $_ !~ m/^\([a-zA-Z].*\)$/;
    } grep {
        # Remove values have number expression
        !$have_number || $_ !~ m/#/;
    } map {
        # Remove annotation
        s/;.*$//; $_;
    } split(/\s*\/\s*/, $value);

    # Output
    if ($key ne '' and @values) {
        print "$key\t" . join("\t", sort @values) . "\n";
    }
}
