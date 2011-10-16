DIR_PREFIX = ../..
TESTS_DIR_PREFIX = ..
include(../config.pri)

TEMPLATE=app
TARGET=fics

DEFINES += \
    MINIATURE_LIB_PATH=\\\"$${PWD}/$${GAME_DIR}\\\" \
    MINIATURE_BIN=\\\"$${PWD}/$${SRC_DIR}/miniature\\\" \
    TEST_SCENARIOS_DIR=\\\"$${PWD}/$${TESTS_DIR}/data/scenarios\\\" \

HEADERS += \
    $${TESTS_DIR_PREFIX}/testutils.h \
    $${TESTS_DIR_PREFIX}/utils/scenario.h \
    $${TESTS_DIR_PREFIX}/utils/scenarioloader.h \

SOURCES += \
    $${TESTS_DIR_PREFIX}/utils/scenario.cc \
    $${TESTS_DIR_PREFIX}/utils/scenarioloader.cc \
    fics.cc \

LIBS += \
    -L$${GAME_DIR} \
    -l$${GAME_LIB} \

