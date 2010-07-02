include $(top_srcdir)/build/autotroll.mk

ACLOCAL_AMFLAGS = -I build

gamesdir = $(prefix)/games
games_PROGRAMS = src/miniature

src_miniature_CXXFLAGS = $(MINIATURE_CFLAGS) $(MAEMOCONTACTSELECTOR_CFLAGS) $(QT_CXXFLAGS) -Wall -Werror -I./src $(AM_CXXFLAGS)
src_miniature_CPPFLAGS = $(QT_CPPFLAGS)
src_miniature_LDFLAGS  = $(QT_LDFLAGS) $(AM_LDFLAGS)
src_miniature_LDADD = $(MINIATURE_LIBS) $(MAEMOCONTACTSELECTOR_LIBS) $(QT_LIBS)

# for each binary, list the sources it depends on, inlucding headers and moc files
src_miniature_SOURCES = \
			src/main.cc \
                        src/main_window.cc \
                        src/main_window.h \
                        src/main_window.moc.cc \
                        src/dbusappactivator.cc \
                        src/dbusappactivator.h \
                        src/dbusappactivator.moc.cc \
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
			src/telepathy/tpaccountitem.moc.cc \
			src/telepathy/accountselectiondlg.h \
			src/telepathy/accountselectiondlg.cc \
			src/telepathy/accountselectiondlg.moc.cc \
			src/telepathy/tpaccountlistmodel.h \
			src/telepathy/tpaccountlistmodel.cc \
			src/telepathy/tpaccountlistmodel.moc.cc \
			src/telepathy/tpcontactslistmodel.h \
			src/telepathy/tpcontactslistmodel.cc \
			src/telepathy/tpcontactslistmodel.moc.cc \
			src/telepathy/tpgame.h \
			src/telepathy/tpgame.cc \
			src/telepathy/tpgame.moc.cc \
			src/telepathy/tptubesclienthandler.h \
			src/telepathy/tptubesclienthandler.cc \
			src/telepathy/tptubesclienthandler.moc.cc \
			src/telepathy/tpapprovermanager.h \
			src/telepathy/tpapprovermanager.cc \
			src/telepathy/tpapprovermanager.moc.cc \
			src/telepathy/tpapprover.h \
			src/telepathy/tpapprover.cc \
			src/telepathy/tpapprover.moc.cc \
			src/telepathy/tphelpers.h \
			src/telepathy/tphelpers.cc \
			src/telepathy/tpoutgoingtube.h \
			src/telepathy/tpoutgoingtube.cc \
			src/telepathy/tpoutgoingtube.moc.cc \
			src/telepathy/tpincomingtube.h \
			src/telepathy/tpincomingtube.cc \
			src/telepathy/tpincomingtube.moc.cc \
			src/telepathy/tptextclienthandler.h \
			src/telepathy/tptextclienthandler.cc \
			src/telepathy/tptextclienthandler.moc.cc \
			src/telepathy/tpchatsession.h \
			src/telepathy/tpchatsession.cc \
			src/telepathy/tpchatsession.moc.cc \
			src/telepathy/tptubeserver.h \
			src/telepathy/tptubeserver.cc \
			src/telepathy/tptubeserver.moc.cc \
			src/telepathy/tptubeclient.h \
			src/telepathy/tptubeclient.cc \
			src/telepathy/tptubeclient.moc.cc \
			$(null)

if HAVE_MAEMOCONTACTSELECTOR
src_miniature_SOURCES += \
			src/telepathy/contact-chooser.h \
			src/telepathy/contact-chooser.cc \
			src/telepathy/contact-chooser.moc.cc \
			$(null)
endif

if MINIATURE_PLATFORM_MAEMO5
src_miniature_SOURCES += \
			src/maemo5/main_window.cc \
			$(null)
else
if MINIATURE_PLATFORM_MAEMO6
src_miniature_SOURCES += \
			src/meego/main_window.cc \
			$(null)
else
src_miniature_SOURCES += \
			src/generic/main_window.cc \
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
		src/network_game.moc.cc \
		src/pieces/piece.moc.cc \
		src/pregame.moc.cc \
		src/telepathy/tpaccountmanager.moc.cc \
		src/telepathy/tpaccountitem.moc.cc \
		src/select_account.ui.h \
                src/main_window.moc.cc \
                src/dbusappactivator.moc.cc \
		$(null)

dist_desktopentry_DATA = data/miniature.desktop

dbusservicedir = $(datadir)/dbus-1/services
dist_dbusservice_DATA = \
                data/org.maemo.miniature.service \
                data/org.freedesktop.Telepathy.Client.Miniature.service \
                data/org.freedesktop.Telepathy.Client.MiniatureApprover.service

clientsdir = /usr/share/telepathy/clients
clients_DATA = \
                data/Miniature.client \
                data/MiniatureApprover.client

iconthemedir = $(datadir)/icons/hicolor
appicon48dir = $(iconthemedir)/48x48/apps
appiconscdir = $(iconthemedir)/scalable/apps

dist_appicon48_DATA = data/icons/48x48/miniature.png
dist_appiconsc_DATA = data/icons/scalable/miniature.svg

DISTCHECK_CONFIGURE_FLAGS := --with-qt=@QT_PATH@

install-data-hook:
	update-desktop-database
