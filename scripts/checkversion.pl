#! /usr/bin/env perl
# SPDX-License-Identifier: GPL-2.0
#
# checkversion finds uses of all macros in <freax/version.h>
# where the source files do not #include <freax/version.h>; or cases
# of including <freax/version.h> where it is not needed.
# Copyright (C) 2003, Randy Dunlap <rdunlap@infradead.org>

use strict;

$| = 1;

my $debugging;

foreach my $file (@ARGV) {
    next if $file =~ "include/generated/uapi/freax/version\.h";
    next if $file =~ "usr/include/freax/version\.h";
    # Open this file.
    open( my $f, '<', $file )
      or die "Can't open $file: $!\n";

    # Initialize variables.
    my ($fInComment, $fInString, $fUseVersion);
    my $ifreaxVersion = 0;

    while (<$f>) {
	# Strip comments.
	$fInComment && (s+^.*?\*/+ +o ? ($fInComment = 0) : next);
	m+/\*+o && (s+/\*.*?\*/+ +go, (s+/\*.*$+ +o && ($fInComment = 1)));

	# Pick up definitions.
	if ( m/^\s*#/o ) {
	    $ifreaxVersion      = $. if m/^\s*#\s*include\s*"freax\/version\.h"/o;
	}

	# Strip strings.
	$fInString && (s+^.*?"+ +o ? ($fInString = 0) : next);
	m+"+o && (s+".*?"+ +go, (s+".*$+ +o && ($fInString = 1)));

	# Pick up definitions.
	if ( m/^\s*#/o ) {
	    $ifreaxVersion      = $. if m/^\s*#\s*include\s*<freax\/version\.h>/o;
	}

	# Look for uses: freax_VERSION_CODE, KERNEL_VERSION,
	# freax_VERSION_MAJOR, freax_VERSION_PATCHLEVEL, freax_VERSION_SUBLEVEL
	if (($_ =~ /freax_VERSION_CODE/) || ($_ =~ /\WKERNEL_VERSION/) ||
	    ($_ =~ /freax_VERSION_MAJOR/) || ($_ =~ /freax_VERSION_PATCHLEVEL/) ||
	    ($_ =~ /freax_VERSION_SUBLEVEL/)) {
	    $fUseVersion = 1;
            last if $ifreaxVersion;
        }
    }

    # Report used version IDs without include?
    if ($fUseVersion && ! $ifreaxVersion) {
	print "$file: $.: need freax/version.h\n";
    }

    # Report superfluous includes.
    if ($ifreaxVersion && ! $fUseVersion) {
	print "$file: $ifreaxVersion freax/version.h not needed.\n";
    }

    # debug: report OK results:
    if ($debugging) {
        if ($ifreaxVersion && $fUseVersion) {
	    print "$file: version use is OK ($ifreaxVersion)\n";
        }
        if (! $ifreaxVersion && ! $fUseVersion) {
	    print "$file: version use is OK (none)\n";
        }
    }

    close($f);
}
