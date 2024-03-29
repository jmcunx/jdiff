## jdiff - Sorted diff of two Text Files

**Note:** Starting October 12 2023, Microsoft GitHub
is forcing me to use 2FA to access to my repositories.
Microsoft wants me to use my Cell Phone or some proprietary
application for 2FA, email is not allowed.  So updates to
this project will no longer occur on GitHub.  Updates and
full Source can be downloaded from my Gemini Capsule:

* gemini://gem.sdf.org/jmccue/computer/repository.gmi
* gemini://sdf.org/jmccue/computer/repository.gmi (mirror)

[See gemini wikipedia](https://en.wikipedia.org/wiki/Gemini_(protocol)#Software) for clients.

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

