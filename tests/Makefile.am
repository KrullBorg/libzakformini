AM_CPPFLAGS = $(WARN_CFLAGS) \
              $(DISABLE_DEPRECATED_CFLAGS) \
              $(ZAKFORMGDAEX_CFLAGS) \
              -I$(top_srcdir)/src \
              -DGUIDIR="\"@abs_builddir@\""

LIBS = $(ZAKFORMGDAEX_LIBS) \
       -export-dynamic

LDADD = $(top_builddir)/src/libzakformgdaex.la

noinst_PROGRAMS =

EXTRA_DIST =
