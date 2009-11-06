include $(top_srcdir)/build/autotroll.mk

ACLOCAL_AMFLAGS = -I build

src_miniature_CXXFLAGS = $(QT_CXXFLAGS)
src_miniature_CPPFLAGS = $(QT_CPPFLAGS)
src_miniature_LDFLAGS  = $(QT_LDFLAGS)
src_miniature_LDADD = $(QT_LIBS)

bin_PROGRAMS = src/miniature
# for each binary, list the sources it depends on, inlucding headers and moc files
src_miniature_SOURCES = \
                        src/miniature.cc

# list moc files here, so that autrotroll can clean up
# BUILT_SOURCES = 
