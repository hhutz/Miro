## -*- Makefile -*- ##########################################################
##
## This file is part of Miro (The Middleware For Robots)
##
## $Id$
##
## Miro/src/params/Makefile.am
##
##############################################################################


include $(top_srcdir)/config/Make-rules
if COND_PCH
AM_CPPFLAGS+=-include $(top_builddir)/pch/all_shared.h
endif


lib_LTLIBRARIES = libmiroParams.la


sources = \
	Type.cpp \
	Parameter.cpp \
	Parser.cpp \
	Generator.cpp \
	TextErrorHandler.cpp

libmiroParams_la_SOURCES = $(sources)
libmiroParams_la_LIBADD = \
	$(ACE_LIBS) \
	$(TAO_LIBS) \
        $(INHERITED_LIBS) \
	$(QT_LIBS)

includefiles1 = $(sources:.xml=.h)
includefiles2 = $(includefiles1:.cpp=.h)
params_include_HEADERS = $(includefiles2)

all-local: libmiroParams.la
	$(INSTALLLTLIBRARIES)

clean-local:
	$(CLEANLTLIBRARIES)
