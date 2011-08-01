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

icon.files = $${TARGET}.png
icon.path = /usr/share/themes/base/meegotouch/icons/

INSTALLS += \
    target \
    desktop \
    icon \

# enable booster
CONFIG += qdeclarative-boostable
QMAKE_CXXFLAGS += -fPIC -fvisibility=hidden -fvisibility-inlines-hidden
QMAKE_LFLAGS += -pie -rdynamic
