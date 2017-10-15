## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES = librandomai.la

AM_CPPFLAGS = -I$(top_srcdir)/utility -I$(top_srcdir)/common -I$(top_srcdir)/common/aicore -I$(top_srcdir)/server -I$(top_srcdir)/server/advisors

da_sources = \
	aiRandom.c	\
	aiRandom.h
	
librandomai_la_SOURCES = $(da_sources)

