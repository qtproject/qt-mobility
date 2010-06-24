TEMPLATE = app
TARGET = hapticsplayer
DEPENDPATH += .
INCLUDEPATH += . $$QT_MOBILITY_SOURCE_TREE/src/feedback
LIBS += -lQtFeedback

# Input
HEADERS += dialog.h
SOURCES += dialog.cpp main.cpp
FORMS += dialog.ui

CONFIG += mobility
MOBILITY = feedback

include(../examples.pri)


