COMMANDS_DIR = ./commands

HEADERS += \
    $${COMMANDS_DIR}/logincommand.h \
    $${COMMANDS_DIR}/logoutcommand.h \
    $${COMMANDS_DIR}/recordcommand.h \
    $${COMMANDS_DIR}/advertisement.h \

SOURCES += \
    $${COMMANDS_DIR}/logincommand.cc \
    $${COMMANDS_DIR}/logoutcommand.cc \
    $${COMMANDS_DIR}/recordcommand.cc \
    $${COMMANDS_DIR}/advertisement.cc \

#INCLUDEPATH += $${COMMANDS_DIR}
DEPENDPATH += $${COMMANDS_DIR}
