COMMANDS_DIR = ./commands

HEADERS += \
    $${COMMANDS_DIR}/logincommand.h \
    $${COMMANDS_DIR}/logoutcommand.h \
    $${COMMANDS_DIR}/recordcommand.h \

SOURCES += \
    $${COMMANDS_DIR}/logincommand.cc \
    $${COMMANDS_DIR}/logoutcommand.cc \
    $${COMMANDS_DIR}/recordcommand.cc \

#INCLUDEPATH += $${COMMANDS_DIR}
DEPENDPATH += $${COMMANDS_DIR}
