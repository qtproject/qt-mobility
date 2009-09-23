QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmanagerfiltering
CONFIG+=testcase

include(../../../common.pri)

INCLUDEPATH += ../../../contacts \
               ../../../contacts/details \
               ../../../contacts/requests \
               ../../../contacts/filters

qtAddLibrary(QtContacts)
SOURCES  += tst_qcontactmanagerfiltering.cpp

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles
}
