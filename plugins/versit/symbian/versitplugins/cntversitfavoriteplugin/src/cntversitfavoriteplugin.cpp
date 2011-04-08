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

#include "qversitproperty.h"
#include "cntversitfavoriteplugin.h"


CntVersitFavoritePlugin::CntVersitFavoritePlugin()
{
    
}

void CntVersitFavoritePlugin::propertyProcessed(
    const QVersitDocument& document,
    const QVersitProperty& property,
    const QContact& contact,
    bool* alreadyProcessed,
    QList<QContactDetail>* updatedDetails)
{
    Q_UNUSED(document);
    Q_UNUSED(contact);
    Q_UNUSED(alreadyProcessed);
    Q_UNUSED(updatedDetails);
    
    if (property.name().contains(QLatin1String("X-FAVORITE"), Qt::CaseInsensitive)) {
        // This method is called before the corresponding detail gets imported to QContact.
        // Detail is saved after whole versit document is processed.
        QContactFavorite favorite;
        favorite.setFavorite(true);
        favorite.setIndex(property.value().toInt());
        mFavoriteDetailList.append(favorite);
    }
}

void CntVersitFavoritePlugin::documentProcessed(const QVersitDocument& document, QContact* contact)
{
    Q_UNUSED(document);
    
    // Save favorite details to contact
    foreach (QContactDetail detail, mFavoriteDetailList) {
        contact->saveDetail(&detail);
    }
    mFavoriteDetailList.clear();
}

void CntVersitFavoritePlugin::detailProcessed(
    const QContact& contact,
    const QContactDetail& detail,
    const QVersitDocument& document,
    QSet<QString>* processedFields,
    QList<QVersitProperty>* toBeRemoved,
    QList<QVersitProperty>* toBeAdded)
{
    Q_UNUSED(detail);
    Q_UNUSED(contact);
    Q_UNUSED(processedFields);
    Q_UNUSED(document);
    Q_UNUSED(toBeRemoved);
    
    if (detail.definitionName() == QContactFavorite::DefinitionName) {
        const QContactFavorite &favorite(static_cast<const QContactFavorite&>(detail));
        QVersitProperty property;
        property.setName(QLatin1String("X-FAVORITE"));
        property.setValue(favorite.index());
        toBeAdded->append(property);
    }
}

void CntVersitFavoritePlugin::contactProcessed(const QContact& contact, QVersitDocument* document)
{
    Q_UNUSED(contact)
    Q_UNUSED(document)
}

// End of File
