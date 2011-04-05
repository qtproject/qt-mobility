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
#ifndef CNTVERSITMYCARDPLUGIN_H
#define CNTVERSITMYCARDPLUGIN_H

#include <qversitcontacthandler.h>

QTM_USE_NAMESPACE

class CntVersitMyCardPlugin : public QVersitContactHandler
{
public:
    CntVersitMyCardPlugin();
    
    /* From QVersitContactImporterPropertyHandlerV2 */
    void propertyProcessed(const QVersitDocument& document,
        const QVersitProperty& property,
        const QContact& contact,
        bool* alreadyProcessed,
        QList<QContactDetail>* updatedDetails);
    void documentProcessed(const QVersitDocument& document, QContact* contact);
    
    /* From QVersitContactExporterDetailHandlerV2 */
    void detailProcessed(const QContact& contact,
        const QContactDetail& detail,
        const QVersitDocument& document,
        QSet<QString>* processedFields,
        QList<QVersitProperty>* toBeRemoved,
        QList<QVersitProperty>* toBeAdded);
    void contactProcessed(const QContact& contact, QVersitDocument* document);
    
#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    bool mIsMyCard;
};

#endif
