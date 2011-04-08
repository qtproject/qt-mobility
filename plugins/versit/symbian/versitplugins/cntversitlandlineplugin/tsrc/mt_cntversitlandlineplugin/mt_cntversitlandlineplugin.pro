# Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:
TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
QT += testlib \
    sql \
    xml
CONFIG += qtestlib

# Input
HEADERS += mt_cntversitlandlineplugin.h 
SOURCES += mt_cntversitlandlineplugin.cpp 

symbian:
 { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lqtcontacts \
        -lqtversit
        
    CONFIG += symbian_test
}
