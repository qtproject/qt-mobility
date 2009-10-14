QT += testlib
TEMPLATE=app
TARGET=tst_qcontactgroup
CONFIG+=testcase

include(../../../common.pri)

SOURCES  += tst_qcontactgroup.cpp

qtAddLibraryFixMe(QtContacts)

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
}
