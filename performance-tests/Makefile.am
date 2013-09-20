## -*- Makefile -*- ##########################################################
##
## This file is part of Miro (The Middleware For Robots)
##
## $Id$
##
## Miro/performance-tests/Makefile.am
##
##############################################################################


globaldirs = \
	video \
	panTilt \
	logging \
	nmc

if COND_B21
b21dirs = \
	mcp
endif

if COND_SPARROW99
sparrowdirs = \
	can \
	response 
endif

SUBDIRS = $(globaldirs) $(b21dirs) $(sparrowdirs)
