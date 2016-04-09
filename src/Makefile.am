LIBS = $(ZAKFORMINI_LIBS)

AM_CPPFLAGS = $(ZAKFORMINI_CFLAGS) \
              -DLOCALEDIR=\"$(localedir)\" \
              -DG_LOG_DOMAIN=\"ZakFormIni\"

lib_LTLIBRARIES = libzakformini.la

libzakformini_la_SOURCES = \
                           form.c

libzakformini_la_LDFLAGS = -no-undefined

libzakformini_include_HEADERS = \
                                libzakformini.h \
                                form.h

libzakformini_includedir = $(includedir)/libzakformini
