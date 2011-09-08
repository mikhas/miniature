FRONTEND_DIR = ./frontend

HEADERS += \
    $${FRONTEND_DIR}/miniature.h \
    $${FRONTEND_DIR}/chessboard.h \
    $${FRONTEND_DIR}/sideelement.h \
    $${FRONTEND_DIR}/gameelement.h \
    $${FRONTEND_DIR}/availableseeks.h \

SOURCES += \
    $${FRONTEND_DIR}/miniature.cc \
    $${FRONTEND_DIR}/chessboard.cc \
    $${FRONTEND_DIR}/sideelement.cc \
    $${FRONTEND_DIR}/gameelement.cc \
    $${FRONTEND_DIR}/availableseeks.cc \

#INCLUDEPATH += $${FRONTEND_DIR}
DEPENDPATH += $${FRONTEND_DIR}
