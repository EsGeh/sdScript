# @configure_input@

# this is only needed for recursive Makefiles, to point to Makefiles
# in other source dirs
# SUBDIRS = src

PATH_GLOBAL = src/global
PATH_CODE = src/sdScript
PATH_PD_OBJS = pd_objs

AM_CFLAGS = -Wall -fpic -std=c99 -Winline -fgnu89-inline -I$(top_srcdir)/$(PATH_GLOBAL)

# by default, copy all to the prefix dir, no subdirs
libdir = $(prefix)
datadir = $(prefix)

# list of all products:
lib_LTLIBRARIES = sdScript.la
data_DATA = \
	$(PATH_PD_OBJS)/sdScript-help.pd

sdScript_la_SOURCES = \
	$(PATH_CODE)/sdScriptObj.c $(PATH_CODE)/sdScriptObj.h \
	$(PATH_CODE)/sdScript.c $(PATH_CODE)/sdScript.h \
	$(PATH_CODE)/SymbolTable.c $(PATH_CODE)/SymbolTable.h \
	$(PATH_CODE)/Commands.c $(PATH_CODE)/Commands.h

sdScript_la_LDFLAGS = -module
