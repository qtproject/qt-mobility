TEMPLATE = app
TARGET = hapticsplayer
DEPENDPATH += .
INCLUDEPATH += . $$QT_MOBILITY_SOURCE_TREE/src/feedback

#
# mobility.prf should do this (since it's platform and release/debug dependent,
# it can't just be -lQtFeedback (might QtFeedback1, QtFeedbackd, QtFeedback1d etc)
#
# if it isn't doing it, mobility.prf is probably not in the right place
#
# LIBS += -lQtFeedback

# Input
HEADERS += dialog.h
SOURCES += dialog.cpp main.cpp
FORMS += dialog.ui

CONFIG += mobility
MOBILITY = feedback

include(../examples.pri)


