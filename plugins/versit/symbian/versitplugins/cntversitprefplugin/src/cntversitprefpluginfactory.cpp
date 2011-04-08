/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     QtMobility Versit plugin for VCard PREF handling 
*
*/
#include <QtPlugin>

#include "cntversitprefpluginfactory.h"
#include "cntversitprefplugin.h"

QString CntVersitPrefPluginFactory::name() const
{
    return QLatin1String("com.nokia.symbian.contacts.cntversitprefplugin");
}

int CntVersitPrefPluginFactory::index() const
{
    // Prefer to run this plugin last.
    return -1;
}

QVersitContactHandler* CntVersitPrefPluginFactory::createHandler() const
{
    return new CntVersitPrefPlugin();
}

Q_EXPORT_PLUGIN2(cntversitprefplugin, CntVersitPrefPluginFactory);

// End of File

