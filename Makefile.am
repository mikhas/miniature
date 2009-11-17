include $(top_srcdir)/build/autotroll.mk

ACLOCAL_AMFLAGS = -I build

gamesdir = $(prefix)/games
games_PROGRAMS = src/miniature

src_miniature_CXXFLAGS = $(QT_CXXFLAGS) -Wall -Werror
src_miniature_CPPFLAGS = $(QT_CPPFLAGS)
src_miniature_LDFLAGS  = $(QT_LDFLAGS)
src_miniature_LDADD = $(QT_LIBS)

# for each binary, list the sources it depends on, inlucding headers and moc files
src_miniature_SOURCES = \
			data/data.qrc \
			data/data.qrc.cc \
			src/board_view.cc \
			src/board_view.h \
			src/board_view.moc.cc \
			src/chess_piece_item.cc \
			src/chess_piece_item.h \
			src/game.cc \
			src/game.h \
			src/game.moc.cc \
			src/graphics_board_item.cc \
			src/graphics_board_item.h \
			src/graphics_board_item.moc.cc \
			src/miniature.cc \
			src/miniature.h \
			src/miniature.moc.cc \
			src/miniature.ui \
			src/miniature.ui.h \
			src/pieces_pool.cc \
			src/pieces_pool.h \
			src/pieces_pool_manager.cc \
			src/pieces_pool_manager.h \
			src/player_info.h \
			src/position.cc \
			src/position.h \
			$(null)

# list moc, uic, qrc files here, so that autrotroll can create them/clean them up
BUILT_SOURCES = \
		data/data.qrc.cc \
		src/board_view.moc.cc \
		src/game.moc.cc \
		src/graphics_board_item.moc.cc \
		src/miniature.moc.cc \
		src/miniature.ui.h \
		$(null)

dist_desktopentry_DATA = data/miniature.desktop
dist_dbusservice_DATA = data/org.maemo.miniature.service

iconthemedir = $(datadir)/icons/hicolor
appicon48dir = $(iconthemedir)/48x48/apps
appiconscdir = $(iconthemedir)/scalable/apps

dist_appicon48_DATA = data/icons/48x48/miniature.png
dist_appiconsc_DATA = data/icons/scalable/miniature.svg

DISTCHECK_CONFIGURE_FLAGS := --with-qt=@QT_PATH@
