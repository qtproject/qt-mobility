/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef CNTVERSITPREFPLUGIN_H
#define CNTVERSITPREFPLUGIN_H

#include "qversitcontacthandler.h"

QTM_USE_NAMESPACE

class CntVersitPrefPlugin : public QVersitContactHandler
{
public:
    CntVersitPrefPlugin();
    void propertyProcessed(const QVersitDocument& document,
        const QVersitProperty& property,
        const QContact& contact,
        bool* alreadyProcessed,
        QList<QContactDetail>* updatedDetails);
    void documentProcessed(const QVersitDocument& document, QContact* contact);
    void detailProcessed(const QContact& contact,
        const QContactDetail& detail,
        const QVersitDocument& document,
        QSet<QString>* processedFields,
        QList<QVersitProperty>* toBeRemoved,
        QList<QVersitProperty>* toBeAdded);
    void contactProcessed(const QContact& contact, QVersitDocument* document);
private:
    friend class TestCntVersitPrefPlugin;
    QMap<QString,QString> mDetailMappings;
    QList<QContactDetail> mPrefDetailList;
};

#endif

