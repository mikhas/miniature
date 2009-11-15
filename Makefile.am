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
                        src/position.h \
                        src/position.cc \
                        src/pieces_pool.h \
                        src/pieces_pool.cc \
                        src/pieces_pool_manager.h \
                        src/pieces_pool_manager.cc \
                        src/chess_piece_item.h \
                        src/chess_piece_item.cc \
                        src/graphics_board_item.h \
                        src/graphics_board_item.moc.cc \
                        src/graphics_board_item.cc \
                        src/board_view.h \
                        src/board_view.moc.cc \
                        src/board_view.cc \
                        src/player_info.h \
                        data/data.qrc \
                        data/data.qrc.cc \
                        src/miniature.ui \
                        src/miniature.ui.h

# list moc, uic, qrc files here, so that autrotroll can create them/clean them up
BUILT_SOURCES =  src/miniature.moc.cc src/game.moc.cc src/board_view.moc.cc src/graphics_board_item.moc.cc data/data.qrc.cc src/miniature.ui.h
desktopdir = $(datadir)/applications
dist_desktop_DATA = miniature.desktop
