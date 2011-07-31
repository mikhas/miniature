DIR_PREFIX = ..
include(../config.pri)

TEMPLATE=app
TARGET=miniature
QT += gui

HEADERS += \
    gamemanager.h \

SOURCES += \
    main.cc \
    gamemanager.cc \

RESOURCES += \
    frontend/frontend.qrc

LIBS += \
    -L$${GAME_DIR} \
    -l$${GAME_LIB} \

target.path = /usr/games
target.depends += \
    $${IN_PWD}/frontend/frontend.qrc \
    $${IN_PWD}/frontend/MainPage.qml \
    $${IN_PWD}/frontend/main.qml \
    $${IN_PWD}/frontend/OnlineBoard.qml \
    $${IN_PWD}/frontend/SeekGame.qml \

desktop.files = ../miniature.desktop
desktop.path = /usr/share/applications

INSTALLS += \
    target \
    desktop \

