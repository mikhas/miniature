OBJECTS_DIR = .obj
MOC_DIR = .moc

GAME_TARGET = miniature-game
GAME_LIB = $${GAME_TARGET}

GAME_DIR = $${DIR_PREFIX}/game
SRC_DIR = $${DIR_PREFIX}/src
TESTS_DIR = $${DIR_PREFIX}/tests

enable-cli{
    DEFINES += MINIATURE_CLI_ENABLED
}

INCLUDEPATH += \
    $${GAME_DIR} \

