FICS_DIR = ./fics

HEADERS += \
    $${FICS_DIR}/side.h \
    $${FICS_DIR}/backend.h \

SOURCES += \
    $${FICS_DIR}/side.cc \
    $${FICS_DIR}/backend.cc \

#INCLUDEPATH += $${FICS_DIR}
DEPENDPATH += $${FICS_DIR}
