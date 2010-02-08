TEMPLATE = lib
TARGET = QtLandmarks
QT = core

include(../../common.pri)

DEFINES += QT_BUILD_LANDMARKS_LIB QT_MAKEDLL

CONFIG += mobility
MOBILITY += location

INCLUDEPATH += . ../location
DEPENDPATH += .

PUBLIC_HEADERS += qlandmark.h \
                  qlandmarkdatabase.h \
                  qlandmarkmanager.h \
                  qsearchfilter.h

PRIVATE_HEADERS += qlandmark_p.h

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES += qlandmark.cpp

symbian {
    TARGET.CAPABILITY = ALL -TCB
#    TARGET.UID3 = ?


    deploy.path = $$EPOCROOT
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include/mw

    #export headers into EPOCROOT
    for(header, exportheaders.sources) {
        BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$exportheaders.path/$$basename(header)"
    }

    QtLandmarksDeployment.sources = QtLandmarks.dll
    QtLandmarksDeployment.path = /sys/bin
    DEPLOYMENT += QtLandmarksDeployment
}

include(../../features/deploy.pri)
