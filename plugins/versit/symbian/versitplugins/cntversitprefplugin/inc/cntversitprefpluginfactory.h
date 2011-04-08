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

#ifndef CNTVERSITPREFPLUGINFACTORY_H
#define CNTVERSITPREFPLUGINFACTORY_H

#include <QObject>
#include "qversitcontacthandler.h"

QTM_USE_NAMESPACE

class CntVersitPrefPluginFactory : public QObject, public QVersitContactHandlerFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QVersitContactHandlerFactory)

public:
    QString name() const;
    int index() const;
    QVersitContactHandler* createHandler() const;
};

#endif // CNTVERSITPREFPLUGINFACTORY_H

