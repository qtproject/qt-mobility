QT += testlib
TEMPLATE=app
TARGET=tst_qcontactfilter
CONFIG+=testcase

include(../../../common.pri)

SOURCES  += tst_qcontactfilter.cpp

qtAddLibraryFixMe(QtContacts)

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
}
