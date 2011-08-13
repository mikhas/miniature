COMMANDS_DIR = ./commands

HEADERS += \
    $${COMMANDS_DIR}/login.h \
    $${COMMANDS_DIR}/logout.h \
    $${COMMANDS_DIR}/record.h \
    $${COMMANDS_DIR}/advertisement.h \

SOURCES += \
    $${COMMANDS_DIR}/login.cc \
    $${COMMANDS_DIR}/logout.cc \
    $${COMMANDS_DIR}/record.cc \
    $${COMMANDS_DIR}/advertisement.cc \

#INCLUDEPATH += $${COMMANDS_DIR}
DEPENDPATH += $${COMMANDS_DIR}
