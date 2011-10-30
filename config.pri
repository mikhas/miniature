OBJECTS_DIR = .obj
MOC_DIR = .moc

GAME_TARGET = miniature-game
GAME_LIB = $${GAME_TARGET}

GAME_DIR = $${DIR_PREFIX}/game
SRC_DIR = $${DIR_PREFIX}/src
TESTS_DIR = $${DIR_PREFIX}/tests

isEmpty(PREFIX) {
    PREFIX = /opt
}

enable-gui {
    DEFINES += MINIATURE_GUI_ENABLED
}

enable-coverage {
    LIBS += -lgcov
    QMAKE_CXXFLAGS += -g -fprofile-arcs -ftest-coverage -O0
    QMAKE_LDFLAGS += -g -fprofile-arcs -ftest-coverage  -O0
}

INCLUDEPATH += \
    $${GAME_DIR} \

