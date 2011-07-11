/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
