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
*     QtMobility Versit plugin for VCard X-FAVORITE handling 
*
*/
#include <QtPlugin>

#include "cntversitfavoritepluginfactory.h"
#include "cntversitfavoriteplugin.h"

QString CntVersitFavoritePluginFactory::name() const
{
    return QLatin1String("com.nokia.symbian.contacts.cntversitfavoriteplugin");
}

QVersitContactHandler* CntVersitFavoritePluginFactory::createHandler() const
{
    return new CntVersitFavoritePlugin();
}

Q_EXPORT_PLUGIN2(cntversitfavoriteplugin, CntVersitFavoritePluginFactory);

// End of File
