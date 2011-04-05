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
*      Provides implementation for the QtMobility Versit plugin for vCard import\export
*
*/
#include <QtPlugin>

#include "cntversitlandlinepluginfactory.h"
#include "cntversitlandlineplugin.h"


QString CntVersitLandlinePluginFactory::name() const
{
    return QLatin1String("com.nokia.symbian.contacts.cntversitlandlineplugin");  
}

QVersitContactHandler* CntVersitLandlinePluginFactory::createHandler() const
{
    return new CntVersitLandlinePlugin();
}

Q_EXPORT_PLUGIN2(CntVersitLandlinePlugin, CntVersitLandlinePluginFactory);

// End of File
