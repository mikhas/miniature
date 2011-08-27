include(../config.pri)

TEMPLATE = lib
VERSION = 0.2.0
TARGET = $$GAME_TARGET
# gui is needed for QColor ...
QT += network gui

enable-gui {
    QT += gui declarative
}

include(commands/commands.pri)
include(frontend/frontend.pri)
include(fics/fics.pri)

HEADERS += \
    namespace.h \
    run.h \
    game.h \
    linereader.h \
    directinputdevice.h \
    gnuchess.h \
    position.h \
    square.h \
    dispatcher.h \
    registry.h \
    abstractcommand.h \
    commands.h \
    commandline.h \
    abstractengine.h \
    side.h

SOURCES += \
    run.cc \
    game.cc \
    linereader.cc \
    directinputdevice.cc \
    gnuchess.cc \
    position.cc \
    square.cc \
    dispatcher.cc \
    registry.cc \
    abstractcommand.cc \
    commandline.cc \
    abstractengine.cc \
    side.cc

target.path = /usr/lib

headers.files = $${HEADERS}
headers.path = /usr/include/miniature

prf.files = ../miniature-game.prf
prf.path = $$[QT_INSTALL_DATA]/mkspecs/features

INSTALLS += \
    target \
    headers \
    prf \
