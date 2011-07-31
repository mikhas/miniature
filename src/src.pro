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

LIBS += \
    -L$${GAME_DIR} \
    -l$${GAME_LIB} \

target.path = /usr/games
desktop.files = ../miniature.desktop
desktop.path = /usr/share/applications

INSTALLS += \
    target \
    desktop \

