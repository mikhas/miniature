DIR_PREFIX = ../..
TESTS_DIR_PREFIX = ..
include(../config.pri)

TEMPLATE=app
TARGET=testfics

DEFINES += \
    MINIATURE_LIB_PATH=\\\"$${PWD}/$${GAME_DIR}\\\" \
    MINIATURE_BIN=\\\"$${PWD}/$${SRC_DIR}/miniature\\\" \
    MINIATURE_FICS_SESSION_LOG=\\\"$${PWD}/$${TESTS_DIR}/data/fics-session.log\\\" \
    MINIATURE_FICS_PLAY_LOG=\\\"$${PWD}/$${TESTS_DIR}/data/fics-play.log\\\" \
    MINIATURE_FICS_COMMANDS_LOG=\\\"$${PWD}/$${TESTS_DIR}/data/fics-commands.log\\\" \
    TEST_SCENARIOS_DIR=\\\"$${PWD}/$${TESTS_DIR}/data/scenarios\\\" \

HEADERS += \
    $${TESTS_DIR_PREFIX}/testutils.h \
    $${TESTS_DIR_PREFIX}/utils/scenario.h \
    $${TESTS_DIR_PREFIX}/utils/scenarioloader.h \

SOURCES += \
    $${TESTS_DIR_PREFIX}/utils/scenario.cc \
    $${TESTS_DIR_PREFIX}/utils/scenarioloader.cc \
    testfics.cc \

LIBS += \
    -L$${GAME_DIR} \
    -l$${GAME_LIB} \

