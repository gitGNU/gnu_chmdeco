## chmdeco -- extract files from ITS/CHM files and decompile CHM files
## Copyright (C) 2003 Pabs

## This file is part of chmdeco; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## any later version.

## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
## GNU General Public License for more details.

## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software Foundation,
## Inc., 59 Temple Place, Suite 330, Boston, MA, 02111-1307, USA or visit:
## http://www.gnu.org

## Process this file with automake to produce Makefile.in
## Process Makefile.in with autoconf to create a Makefile

## Options
AUTOMAKE_OPTIONS = gnits
AM_CFLAGS = -Wall

## Source code is located in src
SUBDIRS = src doc

## A wrapper script that will eventually go away (FIXME)
dist_bin_SCRIPTS = chmdeco-popups

## Distribute a bunch of file(1) magic numbers
## FIXME: figure out how to install this
EXTRA_DIST = magic.chmfile

## Cygwin programs dump *.stackdump's not *.core
MOSTLYCLEANFILES = *.stackdump

## +---------------------------------------------------------------------------+
## |   The nasty shite below is for people wanting to create binary packages   |
## |   Debian (deb), Red Hat et al. (rpm), MS Windows - NSIS installer (nsi)   |
## | nsi$ CFLAGS="-mno-cygwin -mconsole -s" ./configure                        |
## | nsi$ CFLAGS="-mno-cygwin -mconsole -s" ./cross-configure.sh               |
## +---------------------------------------------------------------------------+

bindist: deb rpm nsi

## FIXME: Add files to create more binary packages
## FIXME: Use autopackage for this if possible?

## deb and rpm both rely on some aspect of the automake dist target

## Requires the debhelper debian package
## debuild automatically reconfigures, compiles, etc
deb: debian/changelog debian/compat debian/control debian/copyright debian/docs debian/menu debian/rules debian/watch
	debuild

## Reqires the Redhat Package Manager (rpm)
## There is probably a better way, but this is easier
## WARNING: Possible automake compatiblity problem (using distdir)
EXTRA_DIST += chmdeco.spec.in chmdeco.spec
rpm: dist
	rpm --sign -tb $(distdir).tar.gz

## Requires the NullSoft Installer System (NSIS) of Winamp fame
## This is a bit more complex, cause we have to transform stuff
## For Windows we need to perform line ending & filename conversions
## The rules below were modified from automake rules for make dist
## WARNING: Probably a huge automake compatibility problem
## WARNING: Probably a huge portability problem

EXTRA_DIST += chmdeco.nsi checks.bmp chmdeco.ico
nsidir = $(PACKAGE)-$(VERSION)-nsi
## DOCS are converted to .txt files with CRLF line endings
DOCS = README AUTHORS COPYING ChangeLog NEWS THANKS TODO BSD LICENCE
COPY = chmdeco.nsi checks.bmp chmdeco.exe chmdeco.ico istorage.exe

BSD:
	cp -f ../BSD .

LICENCE:
	cp -f ../LICENCE .

istorage.exe:
	cp -f ../istorage.exe .

chmdeco.exe:
	cp -f src/chmdeco.exe .

CLEANFILES=BSD LICENCE istorage.exe chmdeco.exe 

nsi: nsidir
	-chmod -R a+r $(nsidir)
	cd $(nsidir) && makensis -V4 -DPACKAGE="$(PACKAGE)" -DVERSION="$(VERSION)" -DDOCS="$(addsuffix .txt,$(DOCS))" chmdeco.nsi || makensis /V4 /DPACKAGE="$(PACKAGE)" /DVERSION="$(VERSION)" /DDOCS="$(addsuffix .txt,$(DOCS))" chmdeco.nsi
	-rm -rf $(nsidir)

## The next few lines can be used if DOCS contains files with extensions
##	    ext=`echo -n $$file | sed -e 's/^[^.]*\(.*\)/\1/g'`;
##	    if test "x$$ext" = x; then ext=".txt"; else ext=""; fi;
##	    || awk 'BEGIN { ORS = "\r\n" } { print }' < $$d/$$file > $(nsidir)/$$file$$ext || :; \

nsidir: $(DOCS) $(COPY)
	-rm -rf $(nsidir)
	mkdir $(nsidir)
	-chmod 777 $(nsidir)
	@for file in $(DOCS); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -pr $$d/$$file $(nsidir)/$$file; \
	  else \
	    test -f $(nsidir)/$$file \
	    || unix2dos < $$d/$$file > $(nsidir)/$$file.txt || :; \
	  fi; \
	done
	@for file in $(COPY); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -pr $$d/$$file $(nsidir)/$$file; \
	  else \
	    test -f $(nsidir)/$$file \
	    || cp -p $$d/$$file $(nsidir)/$$file || :; \
	  fi; \
	done

## FIXME: Add an Microsoft Installer package (msi)
## FIXME: If only to learn about MSIs & reverse their internal formats

.PHONY: bindist deb rpm nsi nsidir
