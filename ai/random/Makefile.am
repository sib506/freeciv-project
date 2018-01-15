## Process this file with automake to produce Makefile.in

aimodule_LTLIBRARIES = fc_ai_random.la

AM_CPPFLAGS = -I$(top_srcdir)/utility -I$(top_srcdir)/common -I$(top_srcdir)/common/aicore -I$(top_srcdir)/server -I$(top_srcdir)/server/advisors -I$(top_srcdir)/ai/default -I$(top_srcdir)/ai/mcts

fc_ai_random_la_SOURCES = \
	raisettler.c	\
	raisettler.h	\
	raiunit.c	\
	raiunit.h	\
	randomai.c	\
	randomai.h	

fc_ai_random_la_LDFLAGS = -module
fc_ai_random_la_LIBADD = -lm
