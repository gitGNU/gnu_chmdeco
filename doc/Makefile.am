## Process this file with automake to produce Makefile.in
## Process Makefile.in with autoconf to create a Makefile

## The manpage and how to create it
dist_man_MANS = chmdeco.1
CLEANFILES = $(dist_man_MANS)
EXTRA_DIST = chmdeco.docbook
chmdeco.1: chmdeco.docbook
	docbook-to-man $< > $@ || \
	xmlto man $< > $@ || \
	( docbook2man $< && mv CHMDECO.1 chmdeco.1 && rm -f manpage.links manpage.refs ) || \
	( echo ERROR: You need a docbook to man converter && touch chmdeco.1 );
