include(../tsrc.pri)
QT += testlib
TEMPLATE=app
TARGET=tst_details
CONFIG+=testcase
INCLUDEPATH += $$SYMBIAN_PATHS

HEADERS += $$SYMBIAN_HEADERS

SOURCES  += $$SYMBIAN_SOURCES \
            tst_details.cpp

CONFIG += mobility
MOBILITY = contacts

SOURCES  += tst_details.cpp

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
}
