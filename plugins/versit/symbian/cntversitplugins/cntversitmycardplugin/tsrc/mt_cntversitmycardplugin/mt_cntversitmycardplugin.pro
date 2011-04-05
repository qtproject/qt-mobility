#
# Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

TEMPLATE = app
TARGET = 

QT += testlib sql xml
CONFIG  += qtestlib

DEPENDPATH += .
INCLUDEPATH += .

symbian:
{
	INCLUDEPATH += ../../../../../inc
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
	# Input
	HEADERS += mt_cntversitmycardplugin.h
	
	SOURCES += mt_cntversitmycardplugin.cpp
	
	TARGET.CAPABILITY = ALL -TCB
  
  LIBS += -lQtContacts -lQtVersit
  
}

