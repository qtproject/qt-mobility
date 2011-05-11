TEMPLATE = app
TARGET = publishsubscribe_cenrep_mt
symbian:TARGET.UID3 = 0xE056F50B

CONFIG += mobility
MOBILITY = publishsubscribe
 
QT += core

CONFIG += qtestlib console

symbian: {
    CONFIG += symbian_test
    
    HEADERS += moduletest_configurability.h
        
    SOURCES += moduletest_configurability.cpp
        
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    
    testcenrep.sources = e056f50b.cre
    testcenrep.path = c:/private/10202be9/persists/
    DEPLOYMENT += testcenrep
} 
