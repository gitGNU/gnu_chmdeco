## Process this file with automake to produce Makefile.in
## Process Makefile.in with autoconf to create a Makefile

## Options
AUTOMAKE_OPTIONS = gnits
AM_CFLAGS = -Wall

## Our one and only program
bin_PROGRAMS = chmdeco

## Sources and headers for the program
chmdeco_SOURCES = \
chmdeco.c common.c convert.c hhc.c hhk.c hhp.c misc.c sitemap.c strings.c system.c windows.c \
chmdeco.h common.h convert.h hhc.h hhk.h hhp.h misc.h sitemap.h strings_file.h system.h windows.h

## A wrapper script that will eventually go away (FIXME)
dist_bin_SCRIPTS = chmdeco-popups

## The manpage and how to create it
dist_man_MANS = chmdeco.1
CLEANFILES = $(dist_man_MANS)
EXTRA_DIST = chmdeco.docbook
chmdeco.1: chmdeco.docbook
	docbook-to-man $< > $@ || \
	xmlto man $< > $@ || \
	( docbook2man $< && mv CHMDECO.1 chmdeco.1 && rm -f manpage.links manpage.refs ) || \
	( echo ERROR: You need a docbook to man converter && touch chmdeco.1 );

## Distribute a bunch of file(1) magic numbers
## FIXME: figure out how to install this
EXTRA_DIST += magic.chmfile

## Cygwin programs dump *.stackdump's not *.core
MOSTLYCLEANFILES = *.stackdump

## +---------------------------------------------------------------------------+
## |   The nasty shite below is for people wanting to create binary packages   |
## |   Debian (deb), Red Hat et al. (rpm), MS Windows - NSIS installer (nsi)   |
## | nsi$ CFLAGS="-mno-cygwin -mconsole -s" ./configure                        |
## +---------------------------------------------------------------------------+

bindist: deb rpm nsi

## FIXME: Add files to create more binary packages
## FIXME: Use autopackage for this if possible?

## deb and rpm both rely on some aspect of the automake dist target

## Requires the debhelper debian package
## debuild automatically reconfigures, compiles, etc
EXTRA_DIST += debian/changelog debian/compat debian/control debian/copyright debian/docs debian/menu debian/rules debian/watch
deb:
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

EXTRA_DIST += chmdeco.nsi checks.bmp chmdeco.ico BSD LICENCE
nsidir = $(PACKAGE)-$(VERSION)-nsi
## DOCS are converted to .txt files with CRLF line endings
DOCS = README AUTHORS COPYING ChangeLog NEWS THANKS TODO BSD LICENCE
COPY = chmdeco.nsi checks.bmp chmdeco.exe chmdeco.ico istorage.exe

nsi: nsidir
	-chmod -R a+r $(nsidir)
	cd $(nsidir) && makensis /V4 /DPACKAGE="$(PACKAGE)" /DVERSION="$(VERSION)" /DDOCS="$(addsuffix .txt,$(DOCS))" chmdeco.nsi
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
