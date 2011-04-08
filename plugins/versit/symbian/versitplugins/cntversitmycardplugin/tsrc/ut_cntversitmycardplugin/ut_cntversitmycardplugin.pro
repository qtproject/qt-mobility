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
    HEADERS += ut_cntversitmycardplugin.loc
    
    HEADERS += ut_cntversitmycardplugin.h
    HEADERS += ../../inc/cntversitmycardpluginfactory.h
    HEADERS += ../../inc/cntversitmycardplugin.h
    SOURCES += ut_cntversitmycardplugin.cpp
    SOURCES += ../../src/cntversitmycardpluginfactory.cpp
    SOURCES += ../../src/cntversitmycardplugin.cpp
    TARGET.CAPABILITY = ALL \
        -TCB
        
    CONFIG += symbian_test
}
