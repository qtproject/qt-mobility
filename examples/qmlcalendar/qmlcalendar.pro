TEMPLATE = app
TARGET = qmlcalendar

CONFIG += mobility
MOBILITY = calendar

DEPENDPATH += .
INCLUDEPATH += . \
               ../../include \
               ../../src/calendar \
               ../../src/calendar/requests \
               ../../src/calendar/details \
               ../../src/calendar/filters

QT += declarative
QT += script


# Input
SOURCES += main.cpp \
    qmlorgnizer.cpp
HEADERS += qmlorgnizer.h
    
RESOURCES += qmlcalendar.qrc

OTHER_FILES += calendar.qml \
    contents/MediaButton.qml \
    pics/calendar.png \
    pics/calendar_small.png \
    pics/edit.png \
    pics/month.png \
    pics/settings.png \
    pics/timeline.png \
    pics/timeline.png \
    pics/settings.png \
    pics/note.png \
    pics/month.png \
    pics/meeting.png \
    pics/journal.png \
    pics/info.png \
    pics/forward.png \
    pics/event.png \
    pics/edit.png \
    pics/edit2.png \
    pics/close.png \
    pics/calendar_small.png \
    pics/calendar.png \
    pics/back.png \
    pics/alert.png \
    pics/alarm.png \
    pics/add.png \
    pics/add2.png


symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        SwEvent
}
include(../examples.pri)
