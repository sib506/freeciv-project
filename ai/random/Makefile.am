## Process this file with automake to produce Makefile.in

if AI_MOD_STATIC_RANDOM
noinst_LTLIBRARIES = librandomai.la
else
aimodule_LTLIBRARIES = fc_ai_random.la
endif

AM_CPPFLAGS = -I$(top_srcdir)/utility -I$(top_srcdir)/common -I$(top_srcdir)/common/aicore -I$(top_srcdir)/server/advisors -I$(top_srcdir)/ai/default

da_sources = \
	randomai.c	\
	randomai.h

if !AI_MOD_STATIC_RANDOM
fc_ai_random_la_SOURCES = $(da_sources)

fc_ai_random_la_LDFLAGS = -module
else
librandomai_la_SOURCES = $(da_sources)
endif
