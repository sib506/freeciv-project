## Process this file with automake to produce Makefile.in
noinst_LTLIBRARIES = libmcts.la

AM_CPPFLAGS = \
	-I$(top_srcdir)/utility \
	-I$(top_srcdir)/common \
	-I$(top_srcdir)/ai/default \
	-I$(top_srcdir)/server/advisors

libmcts_la_SOURCES = \
	mcts_node.c \
	mcts_node.h	\
	mcts.c		\
	mcts.h

