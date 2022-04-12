## jdiff - Sorted diff of two Text Files

Will perform a diff on Two Sorted Text Files.

Most cases diff(1) is better, but if you need to compare 2 **Very
Large Extracts** from say two different databases, this will work
fine.  But, the Files must be in sorted order.

This works fine under Linux, BSD(s) and AIX

To compile:
* If "DESTDIR" is not set, will install under /usr/local
* Execute ./build.sh to create a Makefile
* Works on Linux, BSD and AIX

Will install:
* Binary jdiff
* Manual jdiff.1[.gz] depending on OS

