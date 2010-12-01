TEMPLATE = app
TARGET = qmlorganizer

include(../examples.pri)

QT += declarative

HEADERS = qmlapplicationviewer.h
SOURCES = \
    main.cpp  \
    qmlapplicationviewer.cpp

OTHER_FILES += \
    organizer.qml \
    contents/StatusBar.qml \
    contents/MenuBar.qml \
    contents/TimelineView.qml \
    contents/timeline.js \
    contents/WeekView.qml \
    contents/DayView.qml \
    contents/AgenderView.qml \
    contents/ScrollBar.qml \
    contents/MediaButton.qml \
    contents/DetailsView.qml \
    contents/Button.qml \
    contents/images/toolbutton.sci \
    contents/images/toolbutton.png \
    contents/images/titlebar.sci \
    contents/images/titlebar.png \
    contents/images/stripes.png \
    contents/images/quit.png \
    contents/images/lineedit.sci \
    contents/images/lineedit.png \
    contents/images/gloss.png \
    contents/images/default.svg \
    contents/images/button-pressed.png \
    contents/images/button.png \
    contents/month.js \
    contents/MonthView.qml \
    contents/2010-FIFA-WorldCup.ics

symbian: {
    load(data_caging_paths)
    TARGET.CAPABILITY = ReadDeviceData WriteDeviceData ReadUserData WriteUserData
    TARGET.UID3 = 0xE1407FC3
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
    contains(DEFINES, ORIENTATIONLOCK):LIBS += -lavkon -leikcore -leiksrv -lcone
    contains(DEFINES, NETWORKACCESS):TARGET.CAPABILITY += NetworkServices
}

RESOURCES += \
    qmlorganizer.qrc
