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

QT       += testlib
QT       -= gui

TARGET =

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../src
INCLUDEPATH += ../../inc
SOURCES += tst_ut_easydial_koreatest.cpp
SOURCES += ../../cntplsql/src/koreaninput.cpp

HEADERS += ../../cntplsql/inc/koreaninput.h

#DEFINES += SRCDIR=\\\"$$PWD/\\\"
