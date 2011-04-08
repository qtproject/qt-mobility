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

#include "qversitproperty.h"

#include "cntversitmycardplugin.h"

const QString MYCARD_DEFINITION_NAME = "MyCard";

/*!
    \class CntVersitMyCardPlugin
    \brief Implements the QVersitContactHandler interface for custom handling of 
    X-SELF property during import\export of contacts. 
  
    IMPORT:
        Adds MyCard custom detail in imported contact if X-SELF property found
    
    EXPORT: 
        Appends versit property "X-SELF" to versit properties if contact set as MyCard
*/

CntVersitMyCardPlugin::CntVersitMyCardPlugin() : mIsMyCard(false)
{
    
}

/*!
   Called for each processed versit property from QVersitContactImporter
   during contact import. 
 */
void CntVersitMyCardPlugin::propertyProcessed(
    const QVersitDocument& document,
    const QVersitProperty& property,
    const QContact& contact,
    bool* alreadyProcessed,
    QList<QContactDetail>* updatedDetails)
{
    Q_UNUSED(document);
    Q_UNUSED(contact);
    Q_UNUSED(updatedDetails);
    Q_UNUSED(contact);
    Q_UNUSED(alreadyProcessed);    
    if (property.name() == QLatin1String("X-SELF")) {
        mIsMyCard = true;
    }
}

/*!
  Called from QVersitContactImporter as a final step during 
  contact import once all the document properties have been processed.
 */
void CntVersitMyCardPlugin::documentProcessed(const QVersitDocument& document, QContact* contact)
{
    Q_UNUSED(document);
    
    if (mIsMyCard) {
        QContactDetail myCardDetail(MYCARD_DEFINITION_NAME);
        contact->saveDetail(&myCardDetail);
    }
    mIsMyCard = false;
}

/*!
  Called for each processed detail from QVersitContactExporter during 
  contact export. 
 */
void CntVersitMyCardPlugin::detailProcessed(
    const QContact& contact,
    const QContactDetail& detail,
    const QVersitDocument& document,
    QSet<QString>* processedFields,
    QList<QVersitProperty>* toBeRemoved,
    QList<QVersitProperty>* toBeAdded)
{
    Q_UNUSED(contact);
    Q_UNUSED(processedFields);
    Q_UNUSED(document);
    Q_UNUSED(toBeRemoved);

    if (detail.definitionName() == MYCARD_DEFINITION_NAME) {
        QVersitProperty property;
        property.setName(QLatin1String("X-SELF"));
        property.setValue("0");
        toBeAdded->append(property);
    }
    // QVersitContactExporter takes care of adding the toBeAdded properties
    // to the versit document.
}

/*!
  Called from QVersitContactExporter as a final step during 
  contact export once all the contact details have been processed 
 */
void CntVersitMyCardPlugin::contactProcessed(const QContact& contact, QVersitDocument* document)
{
    Q_UNUSED(contact)
    Q_UNUSED(document)
}

// End of File
