include $(top_srcdir)/build/autotroll.mk

ACLOCAL_AMFLAGS = -I build

gamesdir = $(prefix)/games
games_PROGRAMS = src/miniature

src_miniature_CXXFLAGS = $(QT_CXXFLAGS) -Wall -Werror -I./src $(AM_CXXFLAGS)
src_miniature_CPPFLAGS = $(QT_CPPFLAGS)
src_miniature_LDFLAGS  = $(QT_LDFLAGS) $(AM_LDFLAGS)
src_miniature_LDADD = $(QT_LIBS)

# for each binary, list the sources it depends on, inlucding headers and moc files
src_miniature_SOURCES = \
			src/main.cc \
			src/main.h \
			src/main.moc.cc \
			data/data.qrc \
			data/data.qrc.cc \
			src/board_view.cc \
			src/board_view.h \
			src/board_view.moc.cc \
			src/scene.cc \
			src/scene.h \
			src/scene.moc.cc \
			src/dashboard_button.cc \
			src/dashboard_button.h \
			src/dashboard_button.moc.cc \
			src/dashboard_item.cc \
			src/dashboard_item.h \
			src/dashboard_item.moc.cc \
			src/game.cc \
			src/game.h \
			src/game.moc.cc \
			src/game_log.cc \
			src/game_log.h \
			src/game_log.moc.cc \
			src/game_store.cc \
			src/game_store.h \
			src/game_store.moc.cc \
			src/iconic_button.cc \
			src/iconic_button.h \
			src/iconic_button.moc.cc \
			src/local_game.cc \
			src/local_game.h \
			src/local_game.moc.cc \
			src/logic_analyzer.cc \
			src/logic_analyzer.h \
			src/position.cc \
			src/position.h \
			src/half_move.cc \
			src/half_move.h \
			src/graphics_board_item.cc \
			src/graphics_board_item.h \
			src/graphics_board_item.moc.cc \
			src/network_game.cc \
			src/network_game.h \
			src/network_game.moc.cc \
			src/pieces.h \
			src/pieces/piece.h \
			src/pieces/piece.cc \
			src/pieces/piece.moc.cc \
			src/pieces/pawn.h \
			src/pieces/pawn.cc \
			src/pieces/rook.h \
			src/pieces/rook.cc \
			src/pieces/knight.h \
			src/pieces/knight.cc \
			src/pieces/bishop.h \
			src/pieces/bishop.cc \
			src/pieces/queen.h \
			src/pieces/queen.cc \
			src/pieces/king.h \
			src/pieces/king.cc \
			src/pregame.cc \
			src/pregame.h \
			src/pregame.moc.cc \
			src/telepathy/tpaccountmanager.h \
			src/telepathy/tpaccountmanager.cc \
			src/telepathy/tpaccountmanager.moc.cc \
			src/telepathy/tpaccountitem.h \
			src/telepathy/tpaccountitem.cc \
			$(null)

if MINIATURE_PLATFORM_MAEMO5
src_miniature_SOURCES += \
			src/maemo5/main.cc \
			$(null)
else
if MINIATURE_PLATFORM_MAEMO6
src_miniature_SOURCES += \
			src/maemo5/main.cc \
			$(null)
else
src_miniature_SOURCES += \
			src/generic/main.cc \
			$(null)
endif
endif

# list moc, uic, qrc files here, so that autrotroll can create them/clean them up
BUILT_SOURCES = \
		data/data.qrc.cc \
		src/board_view.moc.cc \
		src/scene.moc.cc \
		src/dashboard_button.moc.cc \
		src/dashboard_item.moc.cc \
		src/game.moc.cc \
		src/game_log.moc.cc \
		src/game_store.moc.cc \
		src/graphics_board_item.moc.cc \
		src/iconic_button.moc.cc \
		src/local_game.moc.cc \
		src/main.moc.cc \
		src/network_game.moc.cc \
		src/pieces/piece.moc.cc \
<<<<<<< HEAD:Makefile.am
		src/pregame.moc.cc \
		src/telepathy/tpaccountmanager.moc.cc \
		src/telepathy/tpaccountitem.moc.cc \
=======
		src/select_account.ui.h \
>>>>>>> Add account selection dialog:Makefile.am
		$(null)

dist_desktopentry_DATA = data/miniature.desktop
dist_dbusservice_DATA = data/org.maemo.miniature.service

iconthemedir = $(datadir)/icons/hicolor
appicon48dir = $(iconthemedir)/48x48/apps
appiconscdir = $(iconthemedir)/scalable/apps

dist_appicon48_DATA = data/icons/48x48/miniature.png
dist_appiconsc_DATA = data/icons/scalable/miniature.svg

DISTCHECK_CONFIGURE_FLAGS := --with-qt=@QT_PATH@

install-data-hook:
	update-desktop-database
