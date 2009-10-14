QT += testlib
TEMPLATE=app
TARGET=tst_qcontactdetails
CONFIG+=testcase

include(../../../common.pri)

SOURCES  += tst_qcontactdetails.cpp

qtAddLibraryFixMe(QtContacts)

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
}
