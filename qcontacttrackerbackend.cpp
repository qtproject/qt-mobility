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

// NOTE: Removed from Qt Mobility API.
QContactManager::Error QContactTrackerEngine::error() const
{
    return m_error;
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

QContact QContactTrackerEngine::contact(const QUniqueId& contactId) const
{
    Q_UNUSED(contactId)
    return QContact();
}

bool QContactTrackerEngine::saveContact(QContact* contact, bool batch, QContactManager::Error& error)
{
    Q_UNUSED(batch);

    // Ensure that the contact data is ok. This comes from QContactModelEngine
    if(!validateContact(*contact, error)) {
        m_error = QContactManager::InvalidDetailError;
        return false;
    }

    Live<nco::PersonContact> ncoContact;
    bool newContact = false;
    if(contact->id() == 0) {
        // Save new contact
        newContact = true;
        d->m_lastUsedId += 1;
        ncoContact = ::tracker()->liveNode(QUrl("contact:"+(d->m_lastUsedId)));
        QSettings definitions(QSettings::IniFormat, QSettings::UserScope, "Nokia", "Trackerplugin");
        contact->setId(d->m_lastUsedId);
        definitions.setValue("nextAvailableContactId", QString::number(d->m_lastUsedId));
    }  else {
        ncoContact = ::tracker()->liveNode(QUrl("contact:"+contact->id()));
    }

    ncoContact->setContactUID(QString(d->m_lastUsedId));

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
            if (det.attributes().value(QContactDetail::AttributeContext).contains(QContactDetail::AttributeContextHome)) {
                if(newContact) {
                    ncoPostalAddress    = ncoContact->addHasPostalAddress();
                } else {
                    ncoPostalAddress    = ncoContact->getHasPostalAddress();
                }

            } else if (det.attributes().value(QContactDetail::AttributeContext).contains(QContactDetail::AttributeContextWork)) {
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
            Live<nco::Contact> contact;
            if (det.attributes().value(QContactDetail::AttributeContext).contains(QContactDetail::AttributeContextHome)) {
                contact = ncoContact;
            } else
            if (det.attributes().value(QContactDetail::AttributeContext).contains(QContactDetail::AttributeContextWork)) {
                Live<nco::Affiliation> aff = ncoContact->getHasAffiliation();
                contact = aff->getOrg();
            }

            if (det.attributes().value(QContactDetail::AttributeSubType).contains(QContactPhoneNumber::AttributeSubTypeMobile)) {
                QString phoneNumber = det.value(QContactPhoneNumber::FieldNumber);
                if(newContact) {
                    Live<nco::CellPhoneNumber> cell = contact->addHasPhoneNumber();
                    cell->setPhoneNumber(phoneNumber);
                } else {
                    /* TODO: This is totally a horrible way to handle Tracker data
                     *       i.e. finding sub class types from a set of by libQtTracker... :-/
                     */
                    LiveNodes numbers = contact->getHasPhoneNumbers();
                    foreach( Live<nco::PhoneNumber> number, numbers) {
                        if(number.hasType<nco::CellPhoneNumber>()) {
                            number->setPhoneNumber(phoneNumber);
                        }
                    }
                }

            /* Save emails */
            } else if(definition == QContactEmailAddress::DefinitionId) {
                // TODO.
            }
        }
    }

    return false;
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

QStringList QContactTrackerEngine::detailDefinitions() const
{
    return QStringList();
}

QContactDetailDefinition QContactTrackerEngine::detailDefinition(const QString& definitionId) const
{
    Q_UNUSED(definitionId)
    return QContactDetailDefinition();
}

bool QContactTrackerEngine::saveDetailDefinition(const QContactDetailDefinition& def)
{
    Q_UNUSED(def)

    return false;
}

bool QContactTrackerEngine::removeDetailDefinition(const QContactDetailDefinition& def)
{
    Q_UNUSED(def)

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
