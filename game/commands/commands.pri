COMMANDS_DIR = ./commands

HEADERS += \
    $${COMMANDS_DIR}/logincommand.h \
    $${COMMANDS_DIR}/logoutcommand.h \

SOURCES += \
    $${COMMANDS_DIR}/logincommand.cc \
    $${COMMANDS_DIR}/logoutcommand.cc \

#INCLUDEPATH += $${COMMANDS_DIR}
DEPENDPATH += $${COMMANDS_DIR}
