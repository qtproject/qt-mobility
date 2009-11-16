QT += testlib
TEMPLATE=app
TARGET=tst_qcontactdetail
CONFIG+=testcase

include(../../../common.pri)
INCLUDEPATH += $$SOURCE_DIR/tests/auto
SOURCES  += tst_qcontactdetail.cpp

qtAddLibrary(QtContacts)

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
}
