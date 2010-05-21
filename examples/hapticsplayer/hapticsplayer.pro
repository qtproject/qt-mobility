TEMPLATE = app
TARGET = hapticsplayer
DEPENDPATH += .
INCLUDEPATH += . $$QT_MOBILITY_SOURCE_TREE/src/feedback


# Input
HEADERS += dialog.h
SOURCES += dialog.cpp main.cpp
FORMS += dialog.ui

LIBS += -lQtFeedback

include(../examples.pri)


