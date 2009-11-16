QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmanager
CONFIG+=testcase

include(../../../common.pri)
INCLUDEPATH += $$SOURCE_DIR/tests/auto
SOURCES  += tst_qcontactmanager.cpp

qtAddLibrary(QtContacts)

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles
}
