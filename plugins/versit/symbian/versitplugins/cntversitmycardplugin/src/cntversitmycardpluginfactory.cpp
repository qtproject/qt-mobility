/*
* Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*     QtMobility Versit plugin for VCard X-SELF property handling 
*
*/
#include <QtPlugin>

#include "cntversitmycardpluginfactory.h"
#include "cntversitmycardplugin.h"

QString CntVersitMyCardPluginFactory::name() const
{
    return QLatin1String("com.nokia.symbian.contacts.cntversitmycardplugin");
}

QVersitContactHandler* CntVersitMyCardPluginFactory::createHandler() const
{
    return new CntVersitMyCardPlugin();
}

Q_EXPORT_PLUGIN2(cntversitxselfplugin, CntVersitMyCardPluginFactory);

// End of File
