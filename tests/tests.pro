DIR_PREFIX = ../..
TESTS_DIR_PREFIX = ..
include(config.pri)

TEMPLATE = subdirs
CONFIG += ordered qt

SUBDIRS = \
    game \
    gnuchess \
    fics \
    localparser \
    position \
