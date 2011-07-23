include(../config.pri)

TEMPLATE = lib
VERSION = 0.0.1
TARGET = $$GAME_TARGET
QT -= gui
QT += network

HEADERS += \
    namespace.h \
    game.h \
    abstracttokenizer.h \
    linereader.h \
    directinputdevice.h \
    abstractside.h \
    localside.h \
    gnuchess.h \
    move.h \
    position.h \
    square.h \
    abstractparser.h \
    localparser.h \
    ficsparser.h \
    ficsside.h \

SOURCES += \
    game.cc \
    abstracttokenizer.cc \
    linereader.cc \
    directinputdevice.cc \
    abstractside.cc \
    localside.cc \
    gnuchess.cc \
    move.cc \
    position.cc \
    square.cc \
    abstractparser.cc \
    localparser.cc \
    ficsparser.cc \
    ficsside.cc \
