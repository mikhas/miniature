include(../config.pri)

TEMPLATE = lib
VERSION = 0.2.0
TARGET = $$GAME_TARGET
QT -= gui
QT += network

enable-gui {
    QT += declarative
}

include(commands/commands.pri)

HEADERS += \
    namespace.h \
    game.h \
    linereader.h \
    directinputdevice.h \
    abstractside.h \
    localside.h \
    gnuchess.h \
    move.h \
    position.h \
    square.h \
    ficsside.h \
    dispatcher.h \
    abstractcommand.h \
    commandline.h \
    abstractbackend.h \
    frontend.h \

SOURCES += \
    game.cc \
    linereader.cc \
    directinputdevice.cc \
    abstractside.cc \
    gnuchess.cc \
    move.cc \
    position.cc \
    square.cc \
    ficsside.cc \
    dispatcher.cc \
    abstractcommand.cc \
    localside.cc \
    commandline.cc \
    abstractbackend.cc \
    frontend.cc \

target.path = /usr/lib

headers.files = $${HEADERS}
headers.path = /usr/include/miniature

prf.files = ../miniature-game.prf
prf.path = $$[QT_INSTALL_DATA]/mkspecs/features

INSTALLS += \
    target \
    headers \
    prf \

