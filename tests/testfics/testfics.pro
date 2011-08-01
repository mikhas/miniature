DIR_PREFIX = ../..
TESTS_DIR_PREFIX = ..
include(../config.pri)

TEMPLATE=app
TARGET=testfics

DEFINES += MINIATURE_LIB_PATH=\\\"$${PWD}/$${GAME_DIR}\\\"
DEFINES += MINIATURE_BIN=\\\"$${PWD}/$${SRC_DIR}/miniature\\\"
DEFINES += MINIATURE_FICS_SESSION_LOG=\\\"$${PWD}/$${TESTS_DIR}/data/fics-session.log\\\"

HEADERS += \
    $${TESTS_DIR_PREFIX}/testutils.h

SOURCES += \
    testfics.cc \

LIBS += \
    -L$${GAME_DIR} \
    -l$${GAME_LIB} \
