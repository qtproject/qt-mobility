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

#include "qtcontacts.h"
#include "qversitproperty.h"
#include "cntversitprefplugin.h"

// Mapping between a string in versit specifications and Qt contact details
struct CntVersitPrefPluginContactDetailMapping {
    const char* versitPropertyName;
    const char* contactDetailDefinitionName;
    const char* contactDetailValueKey;
};

// Mappings from versit property names to Qt contact details
const CntVersitPrefPluginContactDetailMapping versitContactDetailMappings[] = {
    {"EMAIL",           QContactEmailAddress::DefinitionName.latin1(),
                        QContactEmailAddress::FieldEmailAddress.latin1()},
    {"IMPP",            QContactOnlineAccount::DefinitionName.latin1(),
                        QContactOnlineAccount::SubTypeImpp.latin1()},
    {"TEL",             QContactPhoneNumber::DefinitionName.latin1(),
                        QContactPhoneNumber::FieldNumber.latin1()},
    {"URL",             QContactUrl::DefinitionName.latin1(),
                        QContactUrl::FieldUrl.latin1()},
    {"X-ASSISTANT-TEL", QContactPhoneNumber::DefinitionName.latin1(),
                        QContactPhoneNumber::SubTypeAssistant.latin1()},
    {"X-IMPP",          QContactOnlineAccount::DefinitionName.latin1(),
                        QContactOnlineAccount::SubTypeImpp.latin1()},
    {"X-JABBER",        QContactOnlineAccount::DefinitionName.latin1(),
                        QContactOnlineAccount::SubTypeImpp.latin1()},
    {"X-SIP",           QContactOnlineAccount::DefinitionName.latin1(),
                        ""}
};

// Action name constants
const char* callActionName = "call";
const char* emailActionName = "email";
const char* imppActionName = "OnlineAccountActions";
const char* urlActionName = "url";


CntVersitPrefPlugin::CntVersitPrefPlugin()
{
    // Contact detail mappings
    int versitPropertyCount =
        sizeof(versitContactDetailMappings) / sizeof(CntVersitPrefPluginContactDetailMapping);
    for (int i = 0; i < versitPropertyCount; i++)
    {
        QString versitPropertyName =
            QLatin1String(versitContactDetailMappings[i].versitPropertyName);
        QString contactDetailDefinitionName =
            QLatin1String(versitContactDetailMappings[i].contactDetailDefinitionName);
        mDetailMappings.insert(versitPropertyName,contactDetailDefinitionName);
    }
}

void CntVersitPrefPlugin::propertyProcessed(
    const QVersitDocument& document,
    const QVersitProperty& property,
    const QContact& contact,
    bool* alreadyProcessed,
    QList<QContactDetail>* updatedDetails)
{
    Q_UNUSED(document);
    Q_UNUSED(contact);
    
    if (*alreadyProcessed && !updatedDetails->isEmpty())
    {
        QStringList typeParameters = property.parameters().values(QLatin1String("TYPE"));
        if (typeParameters.contains(QLatin1String("PREF"), Qt::CaseInsensitive))
        {
            if ((mDetailMappings.value(property.name()) == QContactPhoneNumber::DefinitionName) ||
                (mDetailMappings.value(property.name()) == QContactEmailAddress::DefinitionName) ||
                (mDetailMappings.value(property.name()) == QContactOnlineAccount::DefinitionName) ||
                (mDetailMappings.value(property.name()) == QContactUrl::DefinitionName))
            {
                // This method is called before the corresponding detail gets imported to QContact.
                // setPreferredDetail() cannot be called here -> detail is stored and will be set
                // preferred after whole versit document is processed
                mPrefDetailList.append(updatedDetails->last()); 
            }
        }
    }
}

void CntVersitPrefPlugin::documentProcessed(const QVersitDocument& document, QContact* contact)
{
    Q_UNUSED(document);
    
    // Now details are in QContact and setPreferredDetail can be called
    foreach (QContactDetail detail, mPrefDetailList)
    {
        QString actionName;
        if (detail.definitionName() == QContactPhoneNumber::DefinitionName)
        {
            actionName = callActionName;
        }
        if (detail.definitionName() == QContactEmailAddress::DefinitionName)
        {
            actionName = emailActionName; 
        }
        if (detail.definitionName() == QContactOnlineAccount::DefinitionName)
        {
            actionName = imppActionName; 
        }
        if (detail.definitionName() == QContactUrl::DefinitionName)
        {
            actionName = urlActionName; 
        }        
        contact->setPreferredDetail(actionName, detail);
    }
    mPrefDetailList.clear();
}

void CntVersitPrefPlugin::detailProcessed(
    const QContact& contact,
    const QContactDetail& detail,
    const QVersitDocument& document,
    QSet<QString>* processedFields,
    QList<QVersitProperty>* toBeRemoved,
    QList<QVersitProperty>* toBeAdded)
{
    Q_UNUSED(processedFields);
    Q_UNUSED(document);
    Q_UNUSED(toBeRemoved);

    // The QVersitContactExporter has already processed the detail, when
    //  this method gets called. The processed detail can be found from
    //  toBeAdded list.
    if ((detail.definitionName() == QContactPhoneNumber::DefinitionName &&
        contact.isPreferredDetail(callActionName, detail)) ||
        (detail.definitionName() == QContactEmailAddress::DefinitionName &&
        contact.isPreferredDetail(emailActionName, detail)) ||
        (detail.definitionName() == QContactOnlineAccount::DefinitionName &&
        contact.isPreferredDetail(imppActionName, detail)) || 
        (detail.definitionName() == QContactUrl::DefinitionName &&
        contact.isPreferredDetail(urlActionName, detail)))
    {
        // If we now find a default property, we have to add the PREF 
        // parameter to the property in the toBeAdded list. There should
        // be only a single property in the list.
        if (!toBeAdded->isEmpty())
        {
            toBeAdded->last().insertParameter(QLatin1String("TYPE"),
                QLatin1String("PREF"));
        }
    }
    // QVersitContactExporter takes care of adding the toBeAdded properties
    // to the versit document.
}


void CntVersitPrefPlugin::contactProcessed(const QContact& contact, QVersitDocument* document)
{
    Q_UNUSED(contact)
    Q_UNUSED(document)
}

// End of File

