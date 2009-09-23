QT += testlib
TEMPLATE=app
TARGET=tst_details
CONFIG+=testcase

SOURCES  += tst_details.cpp
LIBS += -lQtContacts

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
}
