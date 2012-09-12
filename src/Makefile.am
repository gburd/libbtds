MAINTAINERCLEANFILES	= Makefile.in

AM_CFLAGS		= -fPIC -Werror -Wall
if GCC
AM_CFLAGS		+= -W -Wwrite-strings -Wshadow
endif

AM_CPPFLAGS		= -I$(top_builddir)/include -I$(top_srcdir)/src

# Library.
lib_LTLIBRARIES		= libbtds.la
libbtds_la_SOURCES	= btree.c
#libbtds_la_LDFLAGS	= -lpthread -leio

include_HEADERS		= btree.h

libbtds_CPPFLAGS	= $(GLIB_CFLAGS)

noinst_HEADERS		= btree.h \
			  config.h

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

lint:
	-splint $(INCLUDES) $(LINT_FLAGS) $(CFLAGS) *.c
