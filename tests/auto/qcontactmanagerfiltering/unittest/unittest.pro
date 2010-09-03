QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmanagerfiltering
CONFIG+=testcase

include(../../../../common.pri)

INCLUDEPATH += ../../../../src/global \
               ../../../../src/contacts \
               ../../../../src/contacts/details \
               ../../../../src/contacts/requests \
               ../../../../src/contacts/filters \
               ../../../../src/serviceframework
INCLUDEPATH += ../../

CONFIG += mobility
MOBILITY = contacts serviceframework
SOURCES  += tst_qcontactmanagerfiltering.cpp
HEADERS += ../../qcontactmanagerdataholder.h

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData
}
