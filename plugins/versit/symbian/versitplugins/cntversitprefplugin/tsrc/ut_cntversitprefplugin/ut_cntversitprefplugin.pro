# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
QT += testlib \
    sql \
    xml
CONFIG += qtestlib
CONFIG += mobility
MOBILITY = versit \
    contacts
DEPENDPATH += .
DEPENDPATH += ../..
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += .

symbian:
 { 
    MMP_RULES += "USERINCLUDE ../.."
    
    # Input
    HEADERS += ut_cntversitprefplugin.loc
    
    HEADERS += ut_cntversitprefplugin.h
    HEADERS += ../../inc/cntversitprefpluginfactory.h
    HEADERS += ../../inc/cntversitprefplugin.h
    SOURCES += ut_cntversitprefplugin.cpp
    SOURCES += ../../src/cntversitprefpluginfactory.cpp
    SOURCES += ../../src/cntversitprefplugin.cpp
    TARGET.CAPABILITY = ALL \
        -TCB
        
    CONFIG += symbian_test
}


