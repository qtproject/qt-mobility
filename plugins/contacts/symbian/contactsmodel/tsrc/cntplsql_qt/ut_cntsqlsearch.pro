TEMPLATE = app
TARGET = 
QT += testlib
CONFIG += hb


DEPENDPATH += .
INCLUDEPATH += ./
INCLUDEPATH += ./inc
INCLUDEPATH += ../../inc
INCLUDEPATH += ../../../cntplsql/inc/
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += $$SYMBIAN_PATHS
INCLUDEPATH += /orbit/include

SOURCES += 		$$SYMBIAN_HEADERS \
				ut_cntsqlsearch.cpp\			
        		../../cntplsql/src/cntsqlsearch.cpp\
				../../cntplsql/src/cpcskeymap.cpp\
				../../cntplsql/src/cqwertykeymap.cpp\
				../../cntplsql/src/c12keykeymap.cpp

# Input
HEADERS += 		$$SYMBIAN_SOURCES \
				ut_cntsqlsearch.h\
        		../../cntplsql/inc/cntsqlsearch.h\
        		../../cntplsql/inc/cpcskeymap.h\
        		../../cntplsql/inc/cqwertykeymap.h\
        		../../cntplsql/inc/c12keykeymap.h
        		
symbian: {
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0xEb768fff
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    LIBS += -lxqservice  -lxqserviceutil
}

