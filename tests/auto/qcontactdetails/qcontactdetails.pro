QT += testlib
TEMPLATE=app
TARGET=tst_qcontactdetails
CONFIG+=testcase

include(../../../common.pri)
INCLUDEPATH += $$SOURCE_DIR/tests/auto
SOURCES  += tst_qcontactdetails.cpp

qtAddLibrary(QtContacts)

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
}
