#!/usr/bin/env perl
# vim:set ts=8 sts=4 sw=4 tw=0 et:
#
# optimize-dict.pl - Optimize migemo-dict to load by C/Migemo.
#
# Author:  MURAOKA Taro <koron.kaoriya@gmail.com>

use strict;
use warnings;

binmode STDOUT, ':utf8';
binmode STDIN,  ':utf8';

my %migemo;
my @migemo;

# Extract the midpoint from a sorted array using DFS and rearrange the
# elements.
sub balance_array {
    my (@arr) = @_;
    my @result;

    my $dfs;
    $dfs = sub {
        my ($low, $high) = @_;
        return if $low > $high;

        my $mid = int(($low + $high) / 2);
        push @result, $arr[$mid];

        $dfs->($low, $mid - 1);
        $dfs->($mid + 1, $high);
    };

    $dfs->(0, $#arr);
    return @result;
}

# Read migemo-dict file.
while (<>)
{
    chomp;
    next if /^;/;
    my ($label, @word) = split /\t/;
    $label = lc($label); # Key must be lower case
    push @migemo, $label;
    push @{$migemo{$label}}, @word;
}

@migemo = balance_array(sort {$a cmp $b} @migemo);

# Write migemo-dict file.
my $i;
for ($i = 0; $i < @migemo; ++$i)
{
    my $label = $migemo[$i];
    if (exists $migemo{$label})
    {
        my @values = uniq_array($migemo{$label});
        @values = balance_array(@values);
        print "$label\t" . join("\t", @values) . "\n";
        delete $migemo{$label};
    }
}

sub uniq_array
{
    my %array = map {$_, 1} @{$_[0]};
    return sort keys %array
}
