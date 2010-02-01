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
CONFIG  += qtestlib

DEPENDPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += inc
INCLUDEPATH += ../../
INCLUDEPATH += ../../inc
INCLUDEPATH += ../../../inc

SOURCES += src/ut_cntsqlsearch.cpp\
        ../../src/cntsqlsearch.cpp


# Input
HEADERS += inc/ut_cntsqlsearch.h\
        ../../inc/cntsqlsearch.h

