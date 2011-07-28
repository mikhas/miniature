include(../config.pri)

TEMPLATE = lib
VERSION = 0.0.1
TARGET = $$GAME_TARGET
QT -= gui
QT += network

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
    abstractlink.h \
    commandline.h \

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
    abstractlink.cc \
    localside.cc \
    commandline.cc \

