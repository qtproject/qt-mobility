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
#include "qcontactmanager.h"
#include "qcontactmanager_p.h"
//#include "qcontactmanagercapabilities.h"
//#include "qcontactmanagercapabilities_p.h"

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nco.h>

#include "tracker2qcontact.h"

Live<nco::Contact> QContactTrackerEngineData::contactByContext(const QContactDetail& det, const Live<nco::PersonContact>& ncoContact) {
    // Our contact node - this can either be an OrganizationContact or PersonContact depending on
    // if we are saving a work or home phone number.
    Live<nco::Contact> contact;

    if (locationContext(det) == ContactContext::Work) {
        // For "work" properties, we need to get the affiliation relationship and the OrganizationContact from that.
        // Tracker will create new nodes for us if these don't already exist.
        Live<nco::Affiliation> aff = ncoContact->getHasAffiliation();
        return contact = aff->getOrg();
    } else {   // Assume home context.
        // Tracker will return the same contact as we are editing - we want to add "home" properties to it.
        return ncoContact;
    }
}

ContactContext::Location QContactTrackerEngineData::locationContext(const QContactDetail& det) const {
    if (det.attributes().value(QContactDetail::AttributeContext).contains(QContactDetail::AttributeContextHome)) {
        return ContactContext::Home;
    } else if(det.attributes().value(QContactDetail::AttributeContext).contains(QContactDetail::AttributeContextWork)) {
        return ContactContext::Work;
    }
    return ContactContext::Unknown;
}

template <class T>
Live<T> QContactTrackerEngineData::nodeByClasstype(QList<Live<nco::ContactMedium> > contactMediums) const {
    foreach(Live<nco::ContactMedium> media, contactMediums) {
        if(media.hasType<T>()) {
            return media;
        }
    }
    return Live<T>();
}

QContactManagerEngine* ContactTrackerFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    Q_UNUSED(error);
    return new QContactTrackerEngine(parameters);
}

QString ContactTrackerFactory::managerName() const
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

QList<QUniqueId> QContactTrackerEngine::contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    Q_UNUSED(filter)
    return contacts(sortOrders, error);
}

QList<QUniqueId> QContactTrackerEngine::contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    Q_UNUSED(sortOrders)

    QList<QUniqueId> ids;
    RDFVariable RDFContact = RDFVariable::fromType<nco::PersonContact>();
    RDFSelect query;

    query.addColumn("contact_uri", RDFContact);
    query.addColumn("contactId", RDFContact.property<nco::contactUID>());
    LiveNodes ncoContacts = ::tracker()->modelQuery(query);
    for(int i=0; i<ncoContacts->rowCount(); i++) {
        ids.append(ncoContacts->index(i, 1).data().toInt());
    }

    error = QContactManager::NoError;
    return ids;
}

QContact QContactTrackerEngine::contact(const QUniqueId& contactId, QContactManager::Error& error ) const
{
    error = QContactManager::NoError;

    // TODO: Do with LiveNodes when they support strict querying.
    RDFVariable rDFContact = RDFVariable::fromType<nco::PersonContact>();

    rDFContact.property<nco::contactUID>() = LiteralValue(QString::number(contactId));

    RDFSelect query;
    query.addColumn("contact_id", rDFContact);

    LiveNodes ncoContacts = ::tracker()->modelQuery(query);

    Q_ASSERT( ncoContacts.value().size() <= 1 );

    if(ncoContacts->rowCount() == 0) {
        error = QContactManager::DoesNotExistError;
        return QContact();
    }

    QContact contact;
    return Tracker2QContact::copyContactData( ncoContacts->liveNode(0), contact ) ? contact : QContact();
}



bool QContactTrackerEngine::saveContact(QContact* contact, QSet<QUniqueId>& contactsAdded, QSet<QUniqueId>& contactsChanged, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error)
{
    //::tracker()->setVerbosity(3);
    groupsChanged.clear(); // TODO not yet supported
    contactsAdded.clear();
    contactsChanged.clear();
    
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
        ncoContact->setContactUID(QString::number(d->m_lastUsedId));
        definitions.setValue("nextAvailableContactId", QString::number(d->m_lastUsedId));
        contactsAdded << contact->id();
    }  else {
        ncoContact = ::tracker()->liveNode(QUrl("contact:"+QString::number(contact->id())));
        contactsChanged << contact->id();
    }

    // Iterate the contact details that are set for the contact. Save them.
    foreach(const QContactDetail& det, contact->details()) {
        QString definition = det.definitionName();

        /* Save name data */
        if(definition == QContactName::DefinitionName) {
            ncoContact->setNameGiven(det.value(QContactName::FieldFirst));
            ncoContact->setNameAdditional(det.value(QContactName::FieldMiddle));
            ncoContact->setNameFamily(det.value(QContactName::FieldLast));

        /* Save address data */
        } else if(definition == QContactAddress::DefinitionName) {
            // OrganizationContact or PersonalContact depending on the context
            Live<nco::Contact> contact = d->contactByContext(det, ncoContact);
            Live<nco::PostalAddress> ncoPostalAddress = contact->getHasPostalAddress();

            // Found the correct address resource. Now update the data.
            ncoPostalAddress->setStreetAddress(det.value(QContactAddress::FieldStreet));
            ncoPostalAddress->setLocality(det.value(QContactAddress::FieldLocality));
            ncoPostalAddress->setPostalcode(det.value(QContactAddress::FieldPostcode));
            ncoPostalAddress->setRegion(det.value(QContactAddress::FieldRegion));
            ncoPostalAddress->setCountry(det.value(QContactAddress::FieldCountry));

        /* Save phone numbers. */
        } else if(definition == QContactPhoneNumber::DefinitionName) {
            // The new phone number field to save into Tracker.
            QString phoneNumber = det.value(QContactPhoneNumber::FieldNumber);

            // OrganizationContact or PersonalContact depending on the context
            Live<nco::Contact> contact = d->contactByContext(det, ncoContact);

            // Get all existing phone numbers from the Contact node.
            LiveNodes numbers = contact->getHasPhoneNumbers();

            // Save mobile phone number.
            if (det.attributes().value(QContactDetail::AttributeSubType).contains(QContactPhoneNumber::AttributeSubTypeMobile)) {
                Live<nco::CellPhoneNumber> number = d->nodeByClasstype<nco::CellPhoneNumber>(numbers);
                if(!number.isLive()) {
                    number = contact->addHasPhoneNumber();
                }
                number->setPhoneNumber(phoneNumber);
            // Or save as general voice number.
            } else {
                Live<nco::VoicePhoneNumber> number = d->nodeByClasstype<nco::VoicePhoneNumber>(numbers);
                if(!number.isLive()) {
                    number = contact->addHasPhoneNumber();
                }
                number->setPhoneNumber(phoneNumber);
            }

        /* Save emails */
        } else if(definition == QContactEmailAddress::DefinitionName) {
            QString email = det.value(QContactEmailAddress::FieldEmailAddress);
            Live<nco::Contact> contact = d->contactByContext(det, ncoContact);

            // TODO: Known issue: we support only one of each type at the moment.
            //       We should somehow get a notification from UI if we are adding
            //       a new detail field, or editing the existing one.
            Live<nco::EmailAddress> liveEmail = contact->firstHasEmailAddress();
            if(liveEmail == 0) {
                liveEmail = contact->addHasEmailAddress();
            }
            liveEmail->setEmailAddress(email);
        /* Save avatar */
        } else if(definition == QContactAvatar::DefinitionName) {
            QUrl avatar = det.value(QContactAvatar::FieldAvatar);
            Live<nco::Contact> contact = d->contactByContext(det, ncoContact);
            Live<nie::DataObject> fdo = ::tracker()->liveNode( avatar );
            contact->setPhoto(fdo);
        /* Save url */
        } else if(definition == QContactUrl::DefinitionName) {
            QUrl url = det.value(QContactUrl::FieldUrl);
            Live<nco::Contact> contact = d->contactByContext(det, ncoContact);

            if (det.attributes().value(QContactDetail::AttributeSubType).contains(QContactUrl::AttributeSubTypeHomePage)){
                Live< rdfs::Resource > liveUrl = contact->addWebsiteUrl();
                liveUrl = url;
                contact->setWebsiteUrl(liveUrl);
                // At the moment the url is a homepage or not. Waiting for tracker ontology additions for the rest.
                //            } else if (det.attributes().value(QContactDetail::AttributeSubType).contains(QContactUrl::AttributeSubTypeFavourite)){
                //qDebug() << "Favourite subtype not implemented!!!" << __FUNCTION__;
                //} else if (det.attributes().value(QContactDetail::AttributeSubType).contains(QContactUrl::AttributeSubTypeSocialNetworking)){
                // qDebug() << "SocialNetworking subtype not implemented!!!" << __FUNCTION__;
            } else {
                Live< rdfs::Resource > liveUrl = contact->addUrl();
                liveUrl = url;
                contact->setUrl(liveUrl);
            }
        }
/*
 *      
        else if (definition == QContactServiceId::DefinitionName) {
            QString account = det.value(QContactServiceId::FieldAccount);
            QString serviceName = det.value(QContactServiceId::FieldServiceName);
*/      
        // TODO replace when IMaccount is implemented by QtMobility team          
        else if (definition == "ServiceId") {
            QString account = det.value("Account");
            QString serviceName = det.value("ServiceName");
            Live<nco::Contact> contact = d->contactByContext(det, ncoContact);
            Live<nco::IMAccount> liveIMAccount = contact->firstHasIMAccount();
            if (0 == liveIMAccount) {
                liveIMAccount = contact->addHasIMAccount();
            }
            liveIMAccount->setImID(account);
            liveIMAccount->setImAccountType(serviceName);
        }
    }

/*! commented out as it is fired from QContactManager
    if(!batch) {
        QList<QUniqueId> emitList;
        emitList.append(contact->id());
        if (newContact) {
            emit contactsAdded(emitList);
        } else {
            emit contactsChanged(emitList);
        }
    }
*/
    error = QContactManager::NoError;
    return true;
}

bool QContactTrackerEngine::removeContact(const QUniqueId& contactId, QSet<QUniqueId>& contactsRemoved, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error)
{
    Q_UNUSED(groupsChanged)
    contactsRemoved.clear();
    error = QContactManager::NoError;

    // TODO: Do with LiveNodes when they support strict querying.
    RDFVariable RDFContact = RDFVariable::fromType<nco::PersonContact>();
    RDFContact.property<nco::contactUID>() = LiteralValue(QString::number(contactId));
    RDFSelect query;

    query.addColumn("contact_uri", RDFContact);
    LiveNodes ncoContacts = ::tracker()->modelQuery(query);
    if(ncoContacts->rowCount() == 0) {
        error = QContactManager::DoesNotExistError;
        return false;
    }


    Live< nco::PersonContact> ncoContact = ncoContacts->liveNode(0);
    LiveNodes contactMediums = ncoContact->getHasContactMediums();
    foreach(Live<nco::ContactMedium> media, contactMediums) {
        media->remove();
    }
    ncoContact->remove();
    
    contactsRemoved << contactId;
    return true;
}

QList<QContactManager::Error> QContactTrackerEngine::saveContacts(QList<QContact>* contacts, QSet<QUniqueId>& contactsAdded, QSet<QUniqueId>& contactsChanged, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error)
{
    QList<QContactManager::Error> errorList;
    QContactManager::Error functionError = QContactManager::NoError;
    contactsAdded.clear();
    contactsChanged.clear();

    if(contacts == 0) {
        error = QContactManager::BadArgumentError;
        return QList<QContactManager::Error>();
    }

    for(int i=0; i<contacts->count(); i++) {
        QContact contact = contacts->at(i);

        QSet<QUniqueId> added4One;
        QSet<QUniqueId> changed4One;
        QSet<QUniqueId> changedGroup4One;
        
        if(!saveContact(&contact, added4One, changed4One, changedGroup4One, error)) {
            functionError = error;
            errorList.append(functionError);
        } else {
            // No error while saving.
            errorList.append(QContactManager::NoError);
        }
        groupsChanged += changedGroup4One;
        contactsAdded += added4One;
        contactsChanged += changed4One;
    }

    error = functionError;      // Last operation error is the batch error.
/*
    // commented as it is fired from manager
    if(!addedList.isEmpty()) {
        emit contactsAdded(addedList);
    }

    if(!changedList.isEmpty()) {
        emit contactsChanged(changedList);
    }
*/
    return errorList;

}

QList<QContactManager::Error> QContactTrackerEngine::removeContacts(QList<QUniqueId>* contactIds, QSet<QUniqueId>& contactsRemoved, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error)
{
    groupsChanged.clear();
    contactsRemoved.clear();
    QList<QContactManager::Error> errors;
    error = QContactManager::NoError;

    if (!contactIds) {
        error = QContactManager::BadArgumentError;
        return errors;
    }

    for (int i = 0; i < contactIds->count(); i++) {
        QContactManager::Error lastError;
        QSet<QUniqueId> removedCs;
        QSet<QUniqueId> changedGs;
        removeContact(contactIds->at(i),removedCs, changedGs, lastError);
        errors.append(lastError);
        contactsRemoved += removedCs;
        groupsChanged += changedGs;
        if (lastError == QContactManager::NoError) {
            (*contactIds)[i] = 0;
        }
        else {
            error = lastError;
        }
    }

    // emit signals removed as they are fired from QContactManager
    return errors;
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
        QContactDetailDefinition avatarDef = d->m_definitions.value(QContactAvatar::DefinitionName);
        avatarDef.setUnique(true);
        d->m_definitions.insert(QContactAvatar::DefinitionName, avatarDef);

        // modification: url is unique.
        QContactDetailDefinition urlDef = d->m_definitions.value(QContactUrl::DefinitionName);
        urlDef.setUnique(true);
        d->m_definitions.insert(QContactUrl::DefinitionName, urlDef);
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

