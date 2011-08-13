DIR_PREFIX = ..
include(../config.pri)

TEMPLATE=app
TARGET=miniature
QT += gui

HEADERS += \
    gamemanager.h \

SOURCES += \
    main.cc \
    gamemanager.cc \

RESOURCES += \
    frontend/frontend.qrc

# Madde, another madly broken tool, cannot link against libraries from the same
# project correctly, horray! That is why we just pull everything in so that
# this puny attempt of an SDK shuts up.
!enable-madde-support {

LIBS += \
    -L$${GAME_DIR} \
    -l$${GAME_LIB} \

} else {
    HEADERS += \
        $${GAME_DIR}/game.h \
        $${GAME_DIR}/dispatcher.h \
        $${GAME_DIR}/frontend.h \
        $${GAME_DIR}/commands/login.h \
        $${GAME_DIR}/commands/logout.h \
        $${GAME_DIR}/commands/advertisement.h \
        $${GAME_DIR}/commands/recordcommand.h \
        $${GAME_DIR}/directinputdevice.h \
        $${GAME_DIR}/linereader.h \
        $${GAME_DIR}/commandline.h \
        $${GAME_DIR}/move.h \
        $${GAME_DIR}/fics/backend.h \
        $${GAME_DIR}/abstractcommand.h \
        $${GAME_DIR}/abstractbackend.h \
        $${GAME_DIR}/position.h \
        $${GAME_DIR}/square.h \

    SOURCES += \
        $${GAME_DIR}/game.cc \
        $${GAME_DIR}/dispatcher.cc \
        $${GAME_DIR}/frontend.cc \
        $${GAME_DIR}/commands/login.cc \
        $${GAME_DIR}/commands/logout.cc \
        $${GAME_DIR}/commands/advertisement.cc \
        $${GAME_DIR}/commands/recordcommand.cc \
        $${GAME_DIR}/directinputdevice.cc \
        $${GAME_DIR}/linereader.cc \
        $${GAME_DIR}/commandline.cc \
        $${GAME_DIR}/move.cc \
        $${GAME_DIR}/fics/backend.cc \
        $${GAME_DIR}/abstractcommand.cc \
        $${GAME_DIR}/abstractbackend.cc \
        $${GAME_DIR}/position.cc \
        $${GAME_DIR}/square.cc \

}

target.path = /usr/games
target.depends += \
    $${IN_PWD}/frontend/frontend.qrc \
    $${IN_PWD}/frontend/MainPage.qml \
    $${IN_PWD}/frontend/main.qml \
    $${IN_PWD}/frontend/OnlineBoard.qml \
    $${IN_PWD}/frontend/SeekGame.qml \

desktop.files = ../miniature.desktop
desktop.path = /usr/share/applications

icon.files = $${TARGET}.png
icon.path = /usr/share/themes/base/meegotouch/icons/

INSTALLS += \
    target \
    desktop \
    icon \

# enable booster
CONFIG += qdeclarative-boostable
QMAKE_CXXFLAGS += -fPIC -fvisibility=hidden -fvisibility-inlines-hidden
QMAKE_LFLAGS += -pie -rdynamic

OTHER_FILES += \
    frontend/MainPage.qml \
    frontend/SeekGame.qml \
    frontend/chessboard.svg \
    frontend/main.qml \
    frontend/emptysquare.png \
    frontend/BoardModel.qml \
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
    frontend/black/rook.png
