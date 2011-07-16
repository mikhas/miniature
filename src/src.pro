DIR_PREFIX = ..
include(../config.pri)

TEMPLATE=app
TARGET=miniature
QT += gui

HEADERS += \

SOURCES += \
    main.cc \

LIBS += \
    -L$${GAME_DIR} \
    -l$${GAME_LIB} \
