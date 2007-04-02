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
deb: pkg/deb/changelog pkg/deb/compat pkg/deb/control pkg/deb/copyright pkg/deb/docs pkg/deb/rules pkg/deb/watch
	-rm -r debian
	mkdir debian
	cp $^ debian
	debuild --no-tgz-check -sd
	rm -r debian

## Reqires the Redhat Package Manager (rpm)
## There is probably a better way, but this is easier
## WARNING: Possible automake compatiblity problem (using distdir)
EXTRA_DIST += pkg/rpm/chmdeco.spec.in pkg/rpm/chmdeco.spec
rpm: dist
	rpmbuild --sign -tb $(distdir).tar.gz

## Requires the NullSoft Installer System (NSIS) of Winamp fame
## This is a bit more complex, cause we have to transform stuff
## For Windows we need to perform line ending & filename conversions
## The rules below were modified from automake rules for make dist
## WARNING: Probably a huge automake compatibility problem
## WARNING: Probably a huge portability problem

EXTRA_DIST += pkg/nsi/chmdeco.nsi pkg/nsi/checks.bmp pkg/nsi/chmdeco.ico
nsidir = $(PACKAGE)-$(VERSION)-nsi
## DOCS are converted to .txt files with CRLF line endings
DOCS = README AUTHORS COPYING ChangeLog NEWS THANKS TODO BSD LICENCE
DOCSTXT = README.txt AUTHORS.txt COPYING.txt ChangeLog.txt NEWS.txt THANKS.txt TODO.txt BSD.txt LICENCE.txt
EXT_COPY = ../BSD ../LICENCE ../istorage.exe
SRC_COPY = pkg/nsi/chmdeco.nsi pkg/nsi/checks.bmp pkg/nsi/chmdeco.ico
BLD_COPY = src/chmdeco.exe istorage.exe
CRUNCH = chmdeco.exe istorage.exe
CLEANFILES=BSD LICENCE istorage.exe chmdeco.exe 

nsi: nsidir
	makensis -V4 -DPACKAGE="$(PACKAGE)" -DVERSION="$(VERSION)" -DDOCS="$(DOCSTXT)" $(nsidir)/chmdeco.nsi
	-rm -rf $(nsidir)

nsidir: build-win32
	-rm -rf $(nsidir)
	mkdir $(nsidir)
	@for file in $(EXT_COPY); do \
	  cp -p $(srcdir)/$$file .; \
	done
	@for file in $(DOCS); do \
	  test -e $(srcdir)/$$file && \
	  unix2dos < $(srcdir)/$$file > $(nsidir)/$$file.txt ; \
	  test -e $$file && \
	  unix2dos < $$file > $(nsidir)/$$file.txt ; \
	done
	@for file in $(SRC_COPY); do \
	  cp -p $(srcdir)/$$file $(nsidir)/; \
	done
	@for file in $(BLD_COPY); do \
	  cp -p $$file $(nsidir)/; \
	done
	@for file in $(CRUNCH); do \
	  d=$(nsidir); \
	  strip $$d/$$file; \
	  upx --best $$d/$$file; \
	done


build-win32: $(srcdir)/configure
	$(srcdir)/configure --host=i586-mingw32msvc
	make

## FIXME: Add an Microsoft Installer package (msi)
## FIXME: If only to learn about MSIs & reverse their internal formats

.PHONY: bindist deb rpm nsi nsidir
