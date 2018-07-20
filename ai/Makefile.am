## Process this file with automake to produce Makefile.in

<<<<<<< HEAD
module_dirs = random mcts
=======
module_dirs = mcts
>>>>>>> experimental-loading

if AI_MOD_DEFAULT_NEEDED
module_dirs += default
endif

if AI_MOD_STATIC_CLASSIC
module_dirs += classic
endif

if AI_MOD_STATIC_THREADED
module_dirs += threaded
endif

AM_CPPFLAGS = -I$(top_srcdir)/utility -I$(top_srcdir)/common -I$(srcdir)/mcts

if AI_MODULES
if !AI_MOD_STATIC_CLASSIC
module_dirs += classic
endif
if EXP_AI_MODULES
if !AI_MOD_STATIC_THREADED
module_dirs += threaded
endif
module_dirs += stub
else
# AI module loading supported, but experimental modules not built
endif
endif

SUBDIRS = $(module_dirs)

noinst_LTLIBRARIES = libaicommon.la

libaicommon_la_SOURCES = \
	aitraits.c	\
	aitraits.h
	

