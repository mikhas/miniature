include(../config.pri)

TEMPLATE = lib
VERSION = 0.0.1
TARGET = $$GAME_TARGET
QT -= gui

HEADERS += \
    game.h \
    abstractside.h \
    localside.h \
    move.h \
    position.h \
    square.h \

SOURCES += \
    game.cc \
    abstractside.cc \
    localside.cc \
    move.cc \
    position.cc \
    square.cc \

