#                                                                    
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).   
# All rights reserved.                                                
# This component and the accompanying materials are made available    
# under the terms of "Eclipse Public License v1.0"    
# which accompanies this distribution, and is available               
# at the URL "http://www.eclipse.org/legal/epl-v10.html".   
#                                                                     
# Initial Contributors:                                               
# Nokia Corporation - initial contribution.                           
#                                                                     
# Contributors:                                                       
#                                                                     
# Description:                                                        
#                                                                     
#
TEMPLATE = app
TARGET = 
QT += testlib
CONFIG += hb


DEPENDPATH += .
INCLUDEPATH += ./
INCLUDEPATH += ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += $$SYMBIAN_PATHS

SOURCES += 		$$SYMBIAN_HEADERS \
				ut_cntsqlsearch.cpp\
				ut_cntsqlfactory.cpp\
				ut_cntsqlkoreanitut.cpp\
				ut_cntsqlsearchbase.cpp\
				main.cpp\			
        		../../cntplsql/src/cntsqlsearch.cpp\
				../../cntplsql/src/cpcskeymap.cpp\
				../../cntplsql/src/ckoreankeymap.cpp\
				../../cntplsql/src/cqwertykeymap.cpp\
				../../cntplsql/src/c12keykeymap.cpp\
				../../cntplsql/src/cntsqlkoreanitut.cpp\
				../../cntplsql/src/cntsqlsearchbase.cpp\
				../../cntplsql/src/cntsqlfactory.cpp\
				../../cntplsql/src/koreaninput.cpp

# Input
HEADERS += 		$$SYMBIAN_SOURCES \
				ut_cntsqlsearch.h\
				ut_cntsqlfactory.h\
				ut_cntsqlkoreanitut.h\
				ut_cntsqlsearchbase.h\
        		../../cntplsql/inc/cntsqlsearch.h\
        		../../cntplsql/inc/cpcskeymap.h\
        		../../cntplsql/inc/cqwertykeymap.h\
        		../../cntplsql/inc/c12keykeymap.h\
        		../../ckoreankeymap.h\
        		../../cntplsql/inc/cntsqlsearchinterface.h\
        		../../cntplsql/inc/cntsqlkoreanitut.h\
				../../cntplsql/inc/cntsqlsearchbase.h\
				../../cntplsql/inc/cntsqlfactory.h\
				../../cntplsql/inc/koreaninput.h
				
        		
symbian: {
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0xEb768fff
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    LIBS += -lxqservice  -lxqserviceutil
}

