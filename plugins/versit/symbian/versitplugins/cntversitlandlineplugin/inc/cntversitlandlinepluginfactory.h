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
*     Provides implementation for the QtMobility Versit plugin for vCard import\export
*/

#ifndef CNTVERSITLANDLINEPLUGINFACTORY_H
#define CNTVERSITLANDLINEPLUGINFACTORY_H

#include <QObject>
#include "qversitcontacthandler.h"

QTM_USE_NAMESPACE

class CntVersitLandlinePluginFactory : public QObject, public QVersitContactHandlerFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QVersitContactHandlerFactory)

public:
    /* From QVersitContactHandlerFactory */
    QString name() const;
    QVersitContactHandler* createHandler() const;
};

#endif // CNTVERSITLANDLINEPLUGINFACTORY_H
