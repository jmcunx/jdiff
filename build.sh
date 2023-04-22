#!/bin/sh
#
# Copyright (c) 2004 ... 2022 2023
#     John McCue <jmccue@jmcunx.com>
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
#==========================================================================
# generate a Makefile from Makefile.unx
# for a UN*X Type Systems
#

#------------------------------------------------------------------------------
# set install location and OS Info
#------------------------------------------------------------------------------
if test "$DESTDIR" = ""
then
    DESTDIR="/usr/local"
    export DESTDIR
fi

if test "$OS" = ""
then
    OS=`uname -s`
    export OS
fi

case "`uname -m`" in
    "x86_64")
	if test "$OS" = "Linux"
	then
	    g_sed_1="^#LINUX64#"
	    g_lib="lib64"
	else
	    g_sed_1="^#LINUX32#"
	    g_lib="lib"
	fi
	;;
    "amd64")
	g_sed_1="^#BSD64#"
	g_lib="lib"
	;;
    "i386")
	g_sed_1="^#BSD32#"
	g_lib="lib"
	;;
    *)
	if test "$OS" = "AIX"
	then
	    g_sed_1="^#AIX#"
	    g_lib="lib"
	else
	    echo "E01: $OS not supported"
	    exit 2
	fi
	;;
esac

type ectags > /dev/null 2>&1
if test "$?" -eq "0"
then
    g_sed_2="=ectags"
else
    type exctags > /dev/null 2>&1
    if test "$?" -eq "0"
    then
	g_sed_2="=exctags"
    else
	g_sed_2="=ctags"
    fi
fi

case "$CC" in
    "cc")
	g_sed_3="CC=cc"
	;;
    "gcc")
	g_sed_3="CC=gcc"
	;;
    "clang")
	g_sed_3="CC=clang"
	;;
    *)
	g_sed_3="CC=cc"
	;;
esac

#------------------------------------------------------------------------------
# create Makefile
#------------------------------------------------------------------------------
sed "s%$g_sed_1%%;s%=ctags%$g_sed_2%;s%CC=cc%$g_sed_3%;s%INCJLIB%$g_include%g;s%JLIBLOC%$g_libloc%g;s%LOCATION%$DESTDIR%" < Makefile.unx > Makefile

cat << EOF

Created Makefile
    OS     $OS
    Install Location:         $DESTDIR
    Compiler:                 `echo $g_sed_3 | sed 's/CC=//'`

Review and modify if necessary

EOF

exit 0
