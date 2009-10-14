QT += testlib
TEMPLATE=app
TARGET=tst_qcontactdetaildefinition
CONFIG+=testcase

include(../../../common.pri)

SOURCES  += tst_qcontactdetaildefinition.cpp

qtAddLibraryFixMe(QtContacts)

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
}
