## jdiff - Sorted diff of two Text Files

**Note:** Starting October 12 2023, GitHub is forcing me to use
my Cell Phone for 2FA to allow access to my repositories.  I do
not want Microsoft to have access to my phone.  So updates to
this project will no longer occur on GitHub.  I will make a note
of where source can be retrieved from on my Gemini Capsule once
everything is moved off of GitHub.
Capsule: gemini://gem.sdf.org/jmccue/ or mirror gemini://sdf.org/jmccue/

Will perform a diff on Two Sorted Text Files.

Most cases diff(1) is better, but if you need to compare 2 **Very
Large Extracts** from say two different databases, this will work
fine.  But, the Files must be in sorted order.

This works fine under Linux, BSD(s) and AIX.

You should use
[diff(1)](https://en.wikipedia.org/wiki/Diff)
if possible.
This was created to compare very large files
extracted from
[SQL](https://en.wikipedia.org/wiki/SQL)
Databases on Systems
that have a memory constrained diff(1).

To compile:
* If "DESTDIR" is not set, will install under /usr/local
* Execute ./build.sh to create a Makefile
* Works on Linux, BSD and AIX

Will install:
* Binary jdiff
* Manual jdiff.1[.gz] depending on OS

