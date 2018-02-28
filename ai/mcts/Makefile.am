## Process this file with automake to produce Makefile.in
noinst_LTLIBRARIES = libmctsai.la

AM_CPPFLAGS = \
	-I$(top_srcdir)/utility \
	-I$(top_srcdir)/common \
	-I$(top_srcdir)/ai/default \
	-I$(top_srcdir)/server \
	-I$(top_srcdir)/server/advisors

da_sources = \
	mcts.c \
	mcts.h \
	mcts_node.c \
	mcts_node.h	\
	mcts_pruning.c \
	mcts_pruning.h \
	mcts_config.h

libmctsai_la_SOURCES = $(da_sources)