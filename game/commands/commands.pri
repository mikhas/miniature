COMMANDS_DIR = ./commands

HEADERS += \
    $${COMMANDS_DIR}/login.h \
    $${COMMANDS_DIR}/logout.h \
    $${COMMANDS_DIR}/loginfailed.h \
    $${COMMANDS_DIR}/advertisement.h \
    $${COMMANDS_DIR}/play.h \
    $${COMMANDS_DIR}/move.h \
    $${COMMANDS_DIR}/creategame.h \
    $${COMMANDS_DIR}/activategame.h \
    $${COMMANDS_DIR}/destroygame.h \
    $${COMMANDS_DIR}/seek.h \
    $${COMMANDS_DIR}/invalidseek.h \
    $${COMMANDS_DIR}/invalidmove.h \
    $${COMMANDS_DIR}/gameended.h \
    $${COMMANDS_DIR}/message.h \
    $${COMMANDS_DIR}/gameresolution.h \

SOURCES += \
    $${COMMANDS_DIR}/login.cc \
    $${COMMANDS_DIR}/logout.cc \
    $${COMMANDS_DIR}/loginfailed.cc \
    $${COMMANDS_DIR}/record.cc \
    $${COMMANDS_DIR}/advertisement.cc \
    $${COMMANDS_DIR}/play.cc \
    $${COMMANDS_DIR}/move.cc \
    $${COMMANDS_DIR}/creategame.cc \
    $${COMMANDS_DIR}/activategame.cc \
    $${COMMANDS_DIR}/destroygame.cc \
    $${COMMANDS_DIR}/seek.cc \
    $${COMMANDS_DIR}/invalidseek.cc \
    $${COMMANDS_DIR}/invalidmove.cc \
    $${COMMANDS_DIR}/gameended.cc \
    $${COMMANDS_DIR}/message.cc \
    $${COMMANDS_DIR}/gameresolution.cc \

#INCLUDEPATH += $${COMMANDS_DIR}
DEPENDPATH += $${COMMANDS_DIR}
