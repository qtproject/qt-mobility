include(../../../common.pri)

TESTS = \
qgeomapcircleobject \
qgeomapgroupobject \
qgeomappolygonobject \
qgeomappolylineobject \
qgeomaprectangleobject \
qgeomaprouteobject \
qgeomaptextobject \
qgeomappixmapobject


##################################################################################
#TEST SUITE MODE
##################################################################################
suite{

message('Compiling test suite')
TEMPLATE = app
TARGET = suite_runner


QT += gui
CONFIG += qtestlib mobility
MOBILITY = location
INCLUDEPATH += ../../../src/location/maps
HEADERS = testhelper.h testsuite.h
SOURCES = testhelper.cpp main.cpp

for(test,TESTS) {
    message('Add '$$test' test case.')
    !exists($${test}/tst_$${test}.cpp):message(Missing $${test}/tst_$${test}.cpp)
    SOURCES += $${test}/tst_$${test}.cpp
}
}
###################################################################################
#STANDALONE TEST MODE
###################################################################################

!suite{
TEMPLATE=subdirs
for(test,TESTS) {
    SUBDIRS += $$test
}
}
