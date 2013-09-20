## -*- Makefile -*- ##########################################################
##
## This file is part of Miro (The Middleware For Robots)
##
## $Id$
##
## Miro/doc/tex/Makefile.am
##
##############################################################################


include $(top_srcdir)/config/Make-rules

texsources = \
	$(srcdir)/behaviourEngine.tex \
	$(srcdir)/blank.tex \
	$(srcdir)/clients.tex \
	$(srcdir)/definitions.tex \
	$(srcdir)/event_channel_logging.tex \
	$(srcdir)/group_communication.tex \
	$(srcdir)/install.tex \
	$(srcdir)/logging.tex \
	$(srcdir)/miro_manual.tex \
	$(srcdir)/miro_reference.tex \
	$(srcdir)/new_project.tex \
	$(srcdir)/parameters.tex \
	$(srcdir)/servers.tex \
	$(srcdir)/styleguide.tex \
	$(srcdir)/tex_definitions.tex \
	$(srcdir)/using.tex \
	$(srcdir)/video.tex \
	$(srcdir)/writing_a_client.tex \
	$(srcdir)/writing_a_server.tex

miro_manual.pdf: $(texsources)
	(TEXINPUTS=$(srcdir):$(srcdir)/../../examples/sonar:$(srcdir)/../../examples/video:$(srcdir)/../../botsvcs/examples/BAP/b21:@builddir@:${TEXINPUTS:-:}; export TEXINPUTS; \
	pdflatex -interaction=nonstopmode $(srcdir)/miro_manual.tex; \
	bibtex $(srcdir)/miro_manual; \
	pdflatex -interaction=nonstopmode $(srcdir)/miro_manual.tex; \
	makeindex $(srcdir)/miro_manual; \
	pdflatex -interaction=nonstopmode $(srcdir)/miro_manual.tex)

miro_manual.ps: $(texsources)
	(TEXINPUTS=$(srcdir):$(srcdir)/../../examples/sonar:$(srcdir)/../../examples/video:$(srcdir)/../../botsvcs/examples/BAP/b21:@builddir@:${TEXINPUTS:-:}; export TEXINPUTS; \
	latex -interaction=nonstopmode $(srcdir)/miro_manual.tex; \
	bibtex $(srcdir)/miro_manual; \
	latex -interaction=nonstopmode $(srcdir)/miro_manual.tex; \
	makeindex $(srcdir)/miro_manual; \
	latex -interaction=nonstopmode $(srcdir)/miro_manual.tex; \
	dvips $(<:.tex=))

docdir = $(prefix)/share/doc/$(PACKAGE)-$(VERSION)
if COND_PDF
doc_DATA = miro_manual.pdf
else
doc_DATA = miro_manual.ps
endif

CLEANFILES = \
	miro_manual.pdf \
	miro_manual.ps \
	miro_manual.aux \
	miro_manual.idx \
	miro_manual.log \
	miro_manual.out \
	miro_manual.toc
