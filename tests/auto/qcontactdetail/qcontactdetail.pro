QT += testlib
TEMPLATE=app
TARGET=tst_qcontactdetail
CONFIG+=testcase

include(../../../common.pri)

SOURCES  += tst_qcontactdetail.cpp
LIBS += -lQtContacts

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
}
