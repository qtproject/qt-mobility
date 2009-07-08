/* * This file is part of qtcontacts-tracker
 * Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Contact: Aleksandar Stojiljkovic <aleksandar.stojiljkovic@nokia.com>
 * This software, including documentation, is protected by copyright controlled by
 * Nokia Corporation. All rights are reserved. Copying, including reproducing, storing,
 * adapting or translating, any or all of this material requires the prior written consent
 * of Nokia Corporation. This material also contains confidential information which may
 * not be disclosed to others without the prior written consent of Nokia.
 */

#include "qcontacttrackerbackend_p.h"

#include "qcontact_p.h"
#include "qcontactgroup_p.h"
#include "qcontactmanagercapabilities.h"
#include "qcontactmanagercapabilities_p.h"

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nco.h>
#include <QtTracker/ontologies/nie.h>

#include "tracker2qcontact.h"

QContactManagerEngine* ContactTrackerFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    Q_UNUSED(error);
    return new QContactTrackerEngine(parameters);
}

QString ContactTrackerFactory::managerId() const
{
    return QString("tracker");
}
Q_EXPORT_PLUGIN2(qtcontacts_tracker, ContactTrackerFactory);

QContactTrackerEngine::QContactTrackerEngine(const QMap<QString, QString>& parameters)
    : d(new QContactTrackerEngineData)
{
    Q_UNUSED(parameters);
    // Let's load the settings. Here we store out detail definitions
    // last used id, ...
    bool ok;
    QSettings definitions(QSettings::IniFormat, QSettings::UserScope, "Nokia", "Trackerplugin");
    d->m_lastUsedId = definitions.value("nextAvailableContactId", "1").toUInt(&ok);
}

QContactTrackerEngine::QContactTrackerEngine(const QContactTrackerEngine& other)
    : QContactManagerEngine(), d(other.d)
{
    Q_UNUSED(other);
}

QContactTrackerEngine& QContactTrackerEngine::operator=(const QContactTrackerEngine& other)
{
    d = other.d;
    return *this;
}

QContactTrackerEngine::~QContactTrackerEngine()
{
}

QContactManagerEngine* QContactTrackerEngine::clone()
{
    // this engine allows sharing - so we increase the reference count.
    d->m_refCount.ref();
    return this;
}

void QContactTrackerEngine::deref()
{
    if (!d->m_refCount.deref())
        delete this;
}

QList<QUniqueId> QContactTrackerEngine::contacts() const
{
    return QList<QUniqueId>();
}

QList<QUniqueId> QContactTrackerEngine::contactsWithDetail(const QString& definitionId, const QVariant& value) const
{
    Q_UNUSED(definitionId)
    Q_UNUSED(value)
    return QList<QUniqueId>();
}

QContact QContactTrackerEngine::contact(const QUniqueId& contactId, QContactManager::Error& error ) const
{
    error = QContactManager::NoError;
    SopranoLive::Live<SopranoLive::nco::PersonContact> ncoContact = ::tracker()->liveNode(QUrl("contact:"+QString::number(contactId)));

    if( ncoContact->getContactUID() == 0 ) {
        error = QContactManager::DoesNotExistError;
        return QContact();
    }

    QContact contact;
    Tracker2QContact::copyContactData( ncoContact, contact );

    return contact;
}

bool QContactTrackerEngine::saveContact(QContact* contact, bool batch, QContactManager::Error& error)
{
    Q_UNUSED(batch);

    if(contact == 0) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    // Ensure that the contact data is ok. This comes from QContactModelEngine
    if(!validateContact(*contact, error)) {
        error = QContactManager::InvalidDetailError;
        return false;
    }

    Live<nco::PersonContact> ncoContact;
    bool newContact = false;
    if(contact->id() == 0) {
        // Save new contact
        newContact = true;
        d->m_lastUsedId += 1;
        ncoContact = ::tracker()->liveNode(QUrl("contact:"+(QString::number(d->m_lastUsedId))));
        QSettings definitions(QSettings::IniFormat, QSettings::UserScope, "Nokia", "Trackerplugin");
        contact->setId(d->m_lastUsedId);
        definitions.setValue("nextAvailableContactId", QString::number(d->m_lastUsedId));
    }  else {
        ncoContact = ::tracker()->liveNode(QUrl("contact:"+QString::number(contact->id())));
    }

    ncoContact->setContactUID(QString::number(d->m_lastUsedId));

    // Iterate the contact details that are set for the contact. Save them.
    foreach(const QContactDetail& det, contact->details()) {
        QString definition = det.definitionId();

        /* Save name data */
        if(definition == QContactName::DefinitionId) {
            ncoContact->setNameGiven(det.value(QContactName::FieldFirst));
            ncoContact->setNameAdditional(det.value(QContactName::FieldMiddle));
            ncoContact->setNameFamily(det.value(QContactName::FieldLast));

        /* Save address data */
        } else if(definition == QContactAddress::DefinitionId) {
            Live<nco::PostalAddress> ncoPostalAddress;

            // Get the correct live nodes for address resources - depending on if
            // this is home or work address.
            if(d->locationContext(det) == ContactContext::Home) {
                if(newContact) {
                    ncoPostalAddress    = ncoContact->addHasPostalAddress();
                } else {
                    ncoPostalAddress    = ncoContact->getHasPostalAddress();
                }

            } else if (d->locationContext(det) == ContactContext::Work) {
                Live<nco::OrganizationContact> org;
                Live<nco::Affiliation> aff;
                if(newContact) {
                    aff                 = ncoContact->addHasAffiliation();
                    org                 = aff->addOrg();
                    ncoPostalAddress    = org->addHasPostalAddress();
                } else {
                    aff                 = ncoContact->getHasAffiliation();
                    org                 = aff->getOrg();
                    ncoPostalAddress    = org->getHasPostalAddress();
                }
            }

            // Found the correct address resource. Now update the data.
            ncoPostalAddress->setStreetAddress(det.value(QContactAddress::FieldStreet));
            ncoPostalAddress->setLocality(det.value(QContactAddress::FieldLocality));
            ncoPostalAddress->setPostalcode(det.value(QContactAddress::FieldPostcode));
            ncoPostalAddress->setRegion(det.value(QContactAddress::FieldRegion));
            ncoPostalAddress->setCountry(det.value(QContactAddress::FieldCountry));

        /* Save phone numbers. */
        } else if(definition == QContactPhoneNumber::DefinitionId) {
            // The new phone number field to save into Tracker.
            QString phoneNumber = det.value(QContactPhoneNumber::FieldNumber);

            Live<nco::Contact> contact = d->getContactByContext(det, ncoContact);
            // Get all existing phone numbers from the Contact node.
            LiveNodes numbers = contact->getHasPhoneNumbers();

            // Save mobile phone number.
            // TODO: Find a better way to edit and manipulate subclass types of LiveNodes.
            if (det.attributes().value(QContactDetail::AttributeSubType).contains(QContactPhoneNumber::AttributeSubTypeMobile)) {
                bool found = false;
                foreach( Live<nco::PhoneNumber> number, numbers) {
                    if(number.hasType<nco::CellPhoneNumber>()) {
                        number->setPhoneNumber(phoneNumber);
                        found = true;
                    }
                }
                if(!found) {
                    Live<nco::CellPhoneNumber> number = ::tracker()->createLiveNode();
                    number->setPhoneNumber(phoneNumber);
                    contact->addHasPhoneNumber(number);
                }
            // Save voice number.
            } else if(det.attributes().value(QContactDetail::AttributeSubType).contains(QContactPhoneNumber::AttributeSubTypeVoice)) {
                bool found = false;
                foreach( Live<nco::VoicePhoneNumber> number, numbers) {
                    if(number.hasType<nco::VoicePhoneNumber>()) {
                        number->setPhoneNumber(phoneNumber);
                        found = true;
                    }
                }
                if(!found) {
                    Live<nco::VoicePhoneNumber> number = ::tracker()->createLiveNode();
                    number->setPhoneNumber(phoneNumber);
                    contact->addHasPhoneNumber(number);
                }
            // Or save it as a general phone number
            } else {
                bool found = false;
                foreach( Live<nco::PhoneNumber> number, numbers) {
                    if(number.hasType<nco::PhoneNumber>()) {
                        number->setPhoneNumber(phoneNumber);
                        found = true;
                    }
                }
                if(!found) {
                    Live<nco::PhoneNumber> number = ::tracker()->createLiveNode();
                    number->setPhoneNumber(phoneNumber);
                    contact->addHasPhoneNumber(number);
                }
            }
        /* Save emails */
        } else if(definition == QContactEmailAddress::DefinitionId) {
            QString email = det.value(QContactEmailAddress::FieldEmailAddress);
            Live<nco::Contact> contact = d->getContactByContext(det, ncoContact);

            // TODO: Known issue: we support only on of each type at the moment.
            //       We should somehow get a notification from UI if we are adding
            //       a new detail field, or editing the existing one.
            LiveNodes liveEmails = contact->getHasEmailAddresss();
            QList<Live<nco::EmailAddress> > emailList = liveEmails;
            if(emailList.isEmpty()) {
                Live<nco::EmailAddress> newEmail = contact->addHasEmailAddress();
                newEmail->setEmailAddress(email);
            } else {
                emailList.first()->setEmailAddress(email);
            }
        }
    }

    QList<QUniqueId> emitList;
    emitList.append(contact->id());
    if (newContact) {
        emit contactsAdded(emitList);
    } else {
        emit contactsChanged(emitList);
    }

    error = QContactManager::NoError;
    return true;
}

bool QContactTrackerEngine::removeContact(const QUniqueId& contactId, bool batch)
{
    Q_UNUSED(contactId)
    Q_UNUSED(batch)

    return false;
}

QList<QContactManager::Error> QContactTrackerEngine::saveContacts(QList<QContact>* contacts)
{
    Q_UNUSED(contacts)
    return QList<QContactManager::Error>();
}

QList<QContactManager::Error> QContactTrackerEngine::removeContacts(QList<QUniqueId>* contactIds)
{
    Q_UNUSED(contactIds)
    return QList<QContactManager::Error>();
}

QList<QUniqueId> QContactTrackerEngine::groups() const
{
    return QList<QUniqueId>();
}

QContactGroup QContactTrackerEngine::group(const QUniqueId& groupId) const
{
    Q_UNUSED(groupId)
    return QContactGroup();
}

bool QContactTrackerEngine::saveGroup(QContactGroup* group)
{
    Q_UNUSED(group)
    return false;
}

bool QContactTrackerEngine::removeGroup(const QUniqueId& groupId)
{
    Q_UNUSED(groupId)

    return false;
}

QMap<QString, QContactDetailDefinition> QContactTrackerEngine::detailDefinitions(QContactManager::Error& error) const
{
    // lazy initialisation of schema definitions.
    if (d->m_definitions.isEmpty()) {
        // none in the list?  get the schema definitions, and modify them to match our capabilities.
        d->m_definitions = QContactManagerEngine::schemaDefinitions();

        // modification: avatar is unique.
        QContactDetailDefinition avatarDef = d->m_definitions.value(QContactAvatar::DefinitionId);
        avatarDef.setUnique(true);
        d->m_definitions.insert(QContactAvatar::DefinitionId, avatarDef);

        // modification: url is unique.
        QContactDetailDefinition urlDef = d->m_definitions.value(QContactUrl::DefinitionId);
        urlDef.setUnique(true);
        d->m_definitions.insert(QContactUrl::DefinitionId, urlDef);
    }

    error = QContactManager::NoError;
    return d->m_definitions;
}

QContactDetailDefinition QContactTrackerEngine::detailDefinition(const QString& definitionId, QContactManager::Error& error) const
{
    detailDefinitions(error); // just to populate the definitions if we haven't already.
    error = QContactManager::DoesNotExistError;
    if (d->m_definitions.contains(definitionId))
        error = QContactManager::NoError;
    return d->m_definitions.value(definitionId);
}

bool QContactTrackerEngine::saveDetailDefinition(const QContactDetailDefinition& def, QContactManager::Error& error)
{
    Q_UNUSED(def)
    error = QContactManager::UnspecifiedError; // Not implemented yet;

    return false;
}

bool QContactTrackerEngine::removeDetailDefinition(const QContactDetailDefinition& def, QContactManager::Error& error)
{
    Q_UNUSED(def)
    error = QContactManager::UnspecifiedError; // Not implemented yet;

    return false;
}


/*! Returns the capabilities of the in-memory engine. */
QStringList QContactTrackerEngine::capabilities() const
{
    // TODO: Check capabilities for Tracker backend.
    QStringList caplist;
    caplist << "Groups" << "Locking" << "Batch" << "ReadOnly" << "Filtering" << "Sorting" << "Preferences";
    // ie, doesn't support: Changelog, Volatile, Asynchronous.
    return caplist;
}

/*!
 * Returns a list of definition identifiers which are natively (fast) filterable
 * on the default backend store managed by the manager from which the capabilities object was accessed
 */
QStringList QContactTrackerEngine::fastFilterableDefinitions() const
{
    // TODO: Check definitions for Tracker backend.
    QStringList fastlist;
    fastlist << "Name::First" << "Name::Last" << "PhoneNumber::PhoneNumber" << "EmailAddress::EmailAddress";
    return fastlist;
}

/*!
 * Returns the list of data types supported by the vCard engine
 */
QList<QVariant::Type> QContactTrackerEngine::supportedDataTypes() const
{
    // TODO: Check supported datatypes for Tracker backend. 
    QList<QVariant::Type> st;
    st.append(QVariant::String);
    st.append(QVariant::Date);
    st.append(QVariant::DateTime);

    return st;
}

#if 0
// Might not be needed but leaving here just in case.
QString QContactTrackerEngine::escaped(const QString& input) const
{
    QString retn = "";
    for (int i = 0; i < input.length(); i++) {
        QChar currChar = input.at(i);
        if (currChar == '\\' ||
                currChar == '=' ||
                currChar == ',' ||
                currChar == ';') {
            // we need to escape this character.
            retn += '\\'; // escape with a single backslash.
        }
        retn += currChar;
    }

    return retn;
}
#endif

Live<nco::Contact> QContactTrackerEngineData::getContactByContext(const QContactDetail& det, const Live<nco::PersonContact>& ncoContact) {
    // Our contact node - this can either be an OrganizationContact or PersonContact depending on
    // if we are saving a work or home phone number.
    Live<nco::Contact> contact;

    if (locationContext(det) == ContactContext::Home) {
        // Tracker will return the same contact as we are editing - we want to add "home" properties to it.
        contact = ::tracker()->liveNode(QUrl("contact:"+ncoContact->getContactUID()));
    } else if (locationContext(det) == ContactContext::Work) {
        // For "work" properties, we need to get the affiliation relationship and the OrganizationContact from that.
        // Tracker will create new nodes for us if these don't already exist.
        Live<nco::Affiliation> aff = ncoContact->getHasAffiliation();
        contact = aff->getOrg();
    }
    return contact;
}

ContactContext::Location QContactTrackerEngineData::locationContext(const QContactDetail& det) const {
    if (det.attributes().value(QContactDetail::AttributeContext).contains(QContactDetail::AttributeContextHome)) {
        return ContactContext::Home;
    } else if(det.attributes().value(QContactDetail::AttributeContext).contains(QContactDetail::AttributeContextWork)) {
        return ContactContext::Work;
    }
    return ContactContext::Unknown;
}
