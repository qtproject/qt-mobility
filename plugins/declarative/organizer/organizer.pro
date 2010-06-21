DEPENDPATH += .
INCLUDEPATH += . \
    ../../../include \
    ../../../src/organizer \
    ../../../src/organizer/requests \
    ../../../src/organizer/details \
    ../../../src/organizer/filters \
    ../../../src/versit

TARGET = $$qtLibraryTarget(declarative_contacts)
TARGETPATH = QtMobility/organizer

TEMPLATE = lib
CONFIG += plugin
PLUGIN_TYPE = declarative

include(../../../common.pri)

QT += declarative


CONFIG += mobility
MOBILITY = organizer

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH



# Input
HEADERS += qmlorganizermodel.h \
           qmlorganizeritem.h \
           qmlorganizeritemdetail.h \
           qmlorganizer.h

SOURCES += plugin.cpp \
           qmllorganizermodel.cpp \
           qmlorganizeritem.cpp \
           qmlorganizeritemdetail.cpp \
           qmlorganizer.cpp

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

OTHER_FILES += qmldir

INSTALLS += target qmldir

