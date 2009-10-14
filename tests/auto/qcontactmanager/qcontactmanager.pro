QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmanager
CONFIG+=testcase

include(../../../common.pri)

SOURCES  += tst_qcontactmanager.cpp

qtAddLibraryFixMe(QtContacts)

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
}
