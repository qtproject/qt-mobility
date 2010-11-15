include(../common.pri)
test = $$files(./tst*.cpp)
name = $$replace(test,.cpp,)
SOURCES += $$test
TARGET = $$name