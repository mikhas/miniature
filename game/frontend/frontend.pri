FRONTEND_DIR = ./frontend

HEADERS += \
    $${FRONTEND_DIR}/miniature.h \
    $${FRONTEND_DIR}/chessboard.h \
    $${FRONTEND_DIR}/chessboardelement.h \
    $${FRONTEND_DIR}/sideelement.h \

SOURCES += \
    $${FRONTEND_DIR}/miniature.cc \
    $${FRONTEND_DIR}/chessboard.cc \
    $${FRONTEND_DIR}/chessboardelement.cc \
    $${FRONTEND_DIR}/sideelement.cc \

#INCLUDEPATH += $${FRONTEND_DIR}
DEPENDPATH += $${FRONTEND_DIR}
