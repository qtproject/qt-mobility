QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmanagerfiltering
CONFIG+=testcase

include(../../../common.pri)

INCLUDEPATH += ../../../src/contacts \
               ../../../src/contacts/details \
               ../../../src/contacts/requests \
               ../../../src/contacts/filters
INCLUDEPATH += ../

qtAddLibrary(QtContacts)
SOURCES  += tst_qcontactmanagerfiltering.cpp

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles
}
