include $(top_srcdir)/build/autotroll.mk

ACLOCAL_AMFLAGS = -I build

src_miniature_CXXFLAGS = $(QT_CXXFLAGS) -Wall -Werror
src_miniature_CPPFLAGS = $(QT_CPPFLAGS)
src_miniature_LDFLAGS  = $(QT_LDFLAGS)
src_miniature_LDADD = $(QT_LIBS)

bin_PROGRAMS = src/miniature
# for each binary, list the sources it depends on, inlucding headers and moc files
src_miniature_SOURCES = \
                        src/miniature.h \
                        src/miniature.moc.cc \
                        src/miniature.cc \
                        src/game.h \
                        src/game.moc.cc \
                        src/game.cc \
                        src/board_view.h \
                        src/board_view.cc \
                        data/data.qrc \
                        data/data.qrc.cc \
                        src/miniature.ui \
                        src/miniature.ui.h

# list moc, uic, qrc files here, so that autrotroll can create them/clean them up
BUILT_SOURCES =  src/miniature.moc.cc src/game.moc.cc data/data.qrc.cc src/miniature.ui.h
desktopdir = $(datadir)/applications
dist_desktop_DATA = miniature.desktop
