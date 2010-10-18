TEMPLATE = app
TARGET = hapticsquare
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
HEADERS += hapticsquare.h \
    hapticbutton.h
SOURCES += hapticsquare.cpp main.cpp \
    hapticbutton.cpp

CONFIG += mobility
MOBILITY = feedback

symbian: {
     TARGET.CAPABILITY = WriteDeviceData ReadDeviceData
}

include(../examples.pri)
