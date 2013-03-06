DIR_PREFIX=..
include(../config.pri)

TEMPLATE=app
TARGET=miniature
QT += gui network declarative

HEADERS += \

SOURCES += \
    main.cc \

RESOURCES += \
    frontend/frontend.qrc

HEADERS += \
    $${GAME_DIR}/run.h \
    $${GAME_DIR}/game.h \
    $${GAME_DIR}/side.h \
    $${GAME_DIR}/dispatcher.h \
    $${GAME_DIR}/registry.h \
    $${GAME_DIR}/commands/login.h \
    $${GAME_DIR}/commands/logout.h \
    $${GAME_DIR}/commands/loginfailed.h \
    $${GAME_DIR}/commands/advertisement.h \
    $${GAME_DIR}/commands/play.h \
    $${GAME_DIR}/commands/move.h \
    $${GAME_DIR}/commands/creategame.h \
    $${GAME_DIR}/commands/activategame.h \
    $${GAME_DIR}/commands/destroygame.h \
    $${GAME_DIR}/commands/seek.h \
    $${GAME_DIR}/commands/invalidseek.h \
    $${GAME_DIR}/commands/invalidmove.h \
    $${GAME_DIR}/commands/gameended.h \
    $${GAME_DIR}/commands/message.h \
    $${GAME_DIR}/commands/gameresolution.h \
    $${GAME_DIR}/frontend/miniature.h \
    $${GAME_DIR}/frontend/chessboard.h \
    $${GAME_DIR}/frontend/sideelement.h \
    $${GAME_DIR}/frontend/gameelement.h \
    $${GAME_DIR}/frontend/availableseeks.h \
    $${GAME_DIR}/frontend/messagelog.h \
    $${GAME_DIR}/directinputdevice.h \
    $${GAME_DIR}/linereader.h \
    $${GAME_DIR}/commandline.h \
    $${GAME_DIR}/fics/engine.h \
    $${GAME_DIR}/abstractcommand.h \
    $${GAME_DIR}/abstractengine.h \
    $${GAME_DIR}/position.h \
    $${GAME_DIR}/square.h \

SOURCES += \
    $${GAME_DIR}/run.cc \
    $${GAME_DIR}/game.cc \
    $${GAME_DIR}/side.cc \
    $${GAME_DIR}/dispatcher.cc \
    $${GAME_DIR}/registry.cc \
    $${GAME_DIR}/commands/login.cc \
    $${GAME_DIR}/commands/logout.cc \
    $${GAME_DIR}/commands/loginfailed.cc \
    $${GAME_DIR}/commands/advertisement.cc \
    $${GAME_DIR}/commands/play.cc \
    $${GAME_DIR}/commands/move.cc \
    $${GAME_DIR}/commands/creategame.cc \
    $${GAME_DIR}/commands/activategame.cc \
    $${GAME_DIR}/commands/destroygame.cc \
    $${GAME_DIR}/commands/seek.cc \
    $${GAME_DIR}/commands/invalidseek.cc \
    $${GAME_DIR}/commands/invalidmove.cc \
    $${GAME_DIR}/commands/gameended.cc \
    $${GAME_DIR}/commands/message.cc \
    $${GAME_DIR}/commands/gameresolution.cc \
    $${GAME_DIR}/frontend/miniature.cc \
    $${GAME_DIR}/frontend/chessboard.cc \
    $${GAME_DIR}/frontend/sideelement.cc \
    $${GAME_DIR}/frontend/gameelement.cc \
    $${GAME_DIR}/frontend/availableseeks.cc \
    $${GAME_DIR}/frontend/messagelog.cc \
    $${GAME_DIR}/directinputdevice.cc \
    $${GAME_DIR}/linereader.cc \
    $${GAME_DIR}/commandline.cc \
    $${GAME_DIR}/fics/engine.cc \
    $${GAME_DIR}/abstractcommand.cc \
    $${GAME_DIR}/abstractengine.cc \
    $${GAME_DIR}/position.cc \
    $${GAME_DIR}/square.cc \

OTHER_FILES += \
    frontend/MainPage.qml \
    frontend/SeekGame.qml \
    frontend/chessboard.svg \
    frontend/main.qml \
    frontend/emptysquare.png \
    frontend/miniatureBIG.png \
    frontend/OnlineBoard.qml \
    frontend/white/bishop.png \
    frontend/white/queen.png \
    frontend/white/pawn.png \
    frontend/white/knight.png \
    frontend/white/king.png \
    frontend/white/rook.png \
    frontend/black/bishop.png \
    frontend/black/queen.png \
    frontend/black/pawn.png \
    frontend/black/knight.png \
    frontend/black/king.png \
    frontend/black/rook.png \
    frontend/miniatureMedium.png \
    frontend/miniature_miniaturets_en_US.qm \
    frontend/miniature_miniaturets_es.qm \
    frontend/miniature_miniaturets_de.qm \
    frontend/miniature_miniaturets_pt.qm \
    frontend/miniature_miniaturets_nl.qm \
    frontend/miniature_miniaturets_ne_NP.qm \
    frontend/miniature_miniaturets_nb.qm \
    frontend/miniature_miniaturets_pl.qm \
    frontend/miniature_miniaturets_zh_HK.qm \
    frontend/miniature_miniaturets_zh_CN.qm \
    frontend/miniature_miniaturets_sv.qm \
    frontend/miniature_miniaturets_ru.qm \
    frontend/miniature_miniaturets_ro.qm \
    frontend/miniature_miniaturets_ja.qm \
    frontend/miniature_miniaturets_it.qm \
    frontend/miniature_miniaturets_zh_TW.qm \
    frontend/miniature_miniaturets_da_DK.qm \
    frontend/miniature_miniaturets_fr.qm \
    frontend/miniature_miniaturets_fi.qm \
    frontend/miniature_miniaturets_en_GB.qm \

target.path = /opt/sdk/bin
target.depends += \
    $${IN_PWD}/frontend/frontend.qrc \
    $${IN_PWD}/frontend/MainPage.qml \
    $${IN_PWD}/frontend/main.qml \
    $${IN_PWD}/frontend/OnlineBoard.qml \
    $${IN_PWD}/frontend/SeekGame.qml \

desktop.files = ../miniature.desktop
desktop.path = /opt/sdk/share/applications
maemo5 {
    desktop.path = /usr/share/applications/hildon
}

icon.files = $${TARGET}-n9.png
icon.path = /usr/share/themes/base/meegotouch/icons/
maemo5 {
    icon.path = /usr/share/icons/hicolor/64x64/apps/
}

contains(CONFIG, desktop) {
    DEFINES *= DESKTOP
    QT += opengl
}

INSTALLS += target desktop

DEFINES += DEPLOYMENT_PATH=\"\\\"\"$${DEPLOYMENT_PATH}/\"\\\"\"

CONFIG += link_pkgconfig
packagesExist(qdeclarative-boostable) {
    message("Building with qdeclarative-boostable support")
    DEFINES += HAS_BOOSTER
    PKGCONFIG += qdeclarative-boostable
} else {
    warning("qdeclarative-boostable not available; startup times will be slower")
}
