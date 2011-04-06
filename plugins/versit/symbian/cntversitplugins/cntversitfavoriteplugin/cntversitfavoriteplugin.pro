#
# Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
#
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
#    QtMobility Versit plugin for VCard X-FAVORITE handling
#

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(cntversitfavoriteplugin)
PLUGIN_TYPE = versit

include(../../../../../common.pri)

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE		   

INCLUDEPATH += inc
INCLUDEPATH += $$SOURCE_DIR/contacts
INCLUDEPATH += $$SOURCE_DIR/contacts/details
INCLUDEPATH += $$SOURCE_DIR/contacts/filters
INCLUDEPATH += $$SOURCE_DIR/contacts/requests

HEADERS += inc/cntversitfavoriteplugin.h \
           inc/cntversitfavoritepluginfactory.h
SOURCES += src/cntversitfavoriteplugin.cpp \
           src/cntversitfavoritepluginfactory.cpp

CONFIG += mobility
MOBILITY = versit contacts

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x20034590
    TARGET.CAPABILITY = CAP_GENERAL_DLL

    pluginDep.sources = $${TARGET}.dll
    pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += pluginDep
    MMP_RULES += SMPSAFE
}

