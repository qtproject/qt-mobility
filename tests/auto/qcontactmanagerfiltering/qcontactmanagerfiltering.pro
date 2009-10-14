QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmanagerfiltering
CONFIG+=testcase

include(../../../common.pri)

SOURCES  += tst_qcontactmanagerfiltering.cpp

qtAddLibraryFixMe(QtContacts)

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles
}
