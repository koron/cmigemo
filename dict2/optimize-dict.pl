#!/usr/bin/perl -w
# vim:set ts=8 sts=4 sw=4 tw=0 et:
# 
# optimize-dict.pl - Optimize migemo-dict to load by C/Migemo.
#
# Author:  MURAOKA Taro <koron.kaoriya@gmail.com>

binmode STDOUT;
my %migemo;
my @migemo;

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

@migemo = sort {length($b) <=> length($a) or $a cmp $b} @migemo;

# Write migemo-dict file.
my $i;
for ($i = 0; $i < @migemo; ++$i)
{
    my $label = $migemo[$i];
    if (exists $migemo{$label})
    {
        print "$label\t" . join("\t", &uniq_array($migemo{$label})) . "\n";
        delete $migemo{$label};
    }
}

sub uniq_array
{
    my %array = map {$_, 1} @{$_[0]};
    return keys %array
}
