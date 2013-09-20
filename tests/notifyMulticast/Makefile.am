## -*- Makefile -*- ##########################################################
##
## This file is part of Miro (The Middleware For Robots)
##
## $Id$
##
## Miro/tests/notifyMulticast/Makefile.am
##
##############################################################################


include $(top_srcdir)/config/Make-rules
if COND_PCH
AM_CPPFLAGS+=-include $(top_builddir)/pch/all_static.h
endif


bin_PROGRAMS = NotifyMulticastTest MulticastAdapter


NotifyMulticastTest_SOURCES = \
	NotifyMulticastTest.cpp \
	NotifyMulticastTest.h

NotifyMulticastTest_LDADD = \
	$(top_builddir)/lib/libmiroSvc.a \
	-lmiro

MulticastAdapter_SOURCES = \
	MulticastAdapter.cpp

MulticastAdapter_LDADD = \
	$(top_builddir)/lib/libmiroSvc.a \
	-lmiro

all-local: NotifyMulticastTest MulticastAdapter
	$(INSTALLPROGRAMS)

clean-local:
	$(CLEANPROGRAMS)
