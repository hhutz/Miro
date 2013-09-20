## -*- Makefile -*- ##########################################################
##
## This file is part of Miro (The Middleware For Robots)
##
## $Id$
##
## Miro/doc/html/Makefile.am
##
##############################################################################


include $(top_srcdir)/config/Make-rules


cpp/*.html:
cpp/*.png:
cpp/MiroCpp.tag:
cpp/doxygen.css:

idl/*.html:
idl/*.png:
idl/MiroIDL.tag:
idl/doxygen.css:
idl/formula.repository:

all-local:
	@(ls @abs_top_srcdir@/idl/*.idl > dependencies.idl)
	@(for i in `cat dependencies.idl`; do hit=0; if test $$i -nt idl/index.html; then hit=1; break; fi; done; if test $$hit -gt 0; then doxygen doxygen.idl; idl/installdox -l ACE.tag@http://www.dre.vanderbilt.edu/Doxygen/Current/html/ace/ -l TAO.tag@http://www.dre.vanderbilt.edu/Doxygen/Current/html/tao/ idl/*html; fi)
	@(for i in @abs_top_srcdir@ @abs_top_builddir@; do for j in src/miro src/video utils/widgets utils/policyEditor utils/configEditor; do ls $$i/$$j/*.h $$i/$$j/*.cpp; done; done 2>&1 | grep -v "No such file or directory" > dependencies.cpp)
	@(for i in `cat dependencies.cpp`; do hit=0; if test $$i -nt cpp/index.html; then hit=1; break; fi; done; if test $$hit -gt 0; then doxygen doxygen.cpp; cpp/installdox -l ACE.tag@http://www.dre.vanderbilt.edu/Doxygen/Current/html/ace/ -l TAO.tag@http://www.dre.vanderbilt.edu/Doxygen/Current/html/tao/ -l MiroIDL.tag@../idl/ cpp/*html; fi)

docidldir = $(prefix)/share/doc/$(PACKAGE)-$(VERSION)/html/idl
docidl_DATA = idl/*.html idl/*.png idl/MiroIDL.tag idl/doxygen.css idl/formula.repository
doccppdir = $(prefix)/share/doc/$(PACKAGE)-$(VERSION)/html/cpp
doccpp_DATA = cpp/*.html cpp/*.png cpp/MiroCpp.tag cpp/doxygen.css

CLEANFILES = \
	dependencies.cpp \
	dependencies.idl \
	cpp/*.html \
	cpp/*.png \
	cpp/MiroCpp.tag \
	cpp/doxygen.css \
	cpp/installdox \
	idl/*.html \
	idl/*.png \
	idl/MiroIDL.tag \
	idl/doxygen.css \
	idl/formula.repository \
	idl/installdox

DISTCLEANFILES = \
	doxygen.idl \
	doxygen.cpp
