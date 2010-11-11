include(../common.pri)
test = $$files(./tst*.cpp)
SOURCES += $$test
TARGET = $$basename($$test)

