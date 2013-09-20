## -*- Makefile -*- ##########################################################
##
## This file is part of Miro (The Middleware For Robots)
##
## $Id$
##
## Miro/doc/fig/Makefile.am
##
##############################################################################


include $(top_srcdir)/config/Make-rules

SUFFIXES += .fig .ps .eps .png

.fig.eps:
	fig2dev -L eps $< $@

.fig.png:
	fig2dev -L png $< $@

.ps.png:
	convert $< $@

.ps.eps:
	$(LN_S) $< $@

figures = \
	client_server.fig \
	client_server_low_level.fig \
	grid_coords.fig \
	Miro_behaviours.fig \
	filterchain.fig \
	filtertree.fig \
	filtergraph.fig \
        filtergraphlegal.fig \
        filtergraphillegal.fig \
	federatedchannel.fig \
	signature.ps \
	pincolla.ps

if COND_PDF
buildsources1 = $(figures:.fig=.png)
buildsources = $(buildsources1:.ps=.png)
else
buildsources1 = $(figures:.fig=.eps)
buildsources = $(buildsources1:.ps=.eps)
endif

BUILT_SOURCES = $(buildsources)

CLEANFILES = $(BUILT_SOURCES)

all-local: $(buildsources)

EXTRA_DIST = $(figures)
