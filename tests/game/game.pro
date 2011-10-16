DIR_PREFIX = ../..
TESTS_DIR_PREFIX = ..
include(../config.pri)

TEMPLATE=app
TARGET=game

DEFINES += MINIATURE_LIB_PATH=\\\"$${PWD}/$${GAME_DIR}\\\"
DEFINES += MINIATURE_BIN=\\\"$${PWD}/$${SRC_DIR}/miniature\\\"

HEADERS += \
    $${TESTS_DIR_PREFIX}/testutils.h

SOURCES += \
    game.cc \

LIBS += \
    -L$${GAME_DIR} \
    -l$${GAME_LIB} \
