include $(top_srcdir)/build/autotroll.mk

ACLOCAL_AMFLAGS = -I build

src_miniature_CXXFLAGS = $(QT_CXXFLAGS)
src_miniature_CPPFLAGS = $(QT_CPPFLAGS)
src_miniature_LDFLAGS  = $(QT_LDFLAGS)
src_miniature_LDADD = $(QT_LIBS)

bin_PROGRAMS = src/miniature
# for each binary, list the sources it depends on, inlucding headers and moc files
src_miniature_SOURCES = \
                        src/miniature.h \
                        src/miniature.moc.cc \
                        src/miniature.cc \
                        src/board.h \
                        src/board.cc \
                        data.qrc \
                        data.qrc.cc

# list moc, uic, qrc files here, so that autrotroll can create them/clean them up
BUILT_SOURCES =  src/miniature.moc.cc data.qrc.cc

desktopdir = $(datadir)/applications
dist_desktop_DATA = miniature.desktop
