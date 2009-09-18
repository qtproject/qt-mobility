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
#include "commonfilters.h"

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nco.h>
#include <QtTracker/ontologies/nie.h>
#include <QtTracker/ontologies/nao.h>
#include <QRegExp>
#include <QDir>
#include <QFile>


Live<nco::Role> QContactTrackerEngineData::contactByContext(const QContactDetail& det, const Live<nco::PersonContact>& ncoContact) {
    if (locationContext(det) == ContactContext::Work) {
        // For "work" properties, we need to get the affiliation containing job related contact data
        // #135682 bug this doesnt work - always return like there something existing  and then not saving anything
        return ncoContact->getHasAffiliation();
    } else {   // Assume home context.
        // Tracker will return the same contact as we are editing - we want to add "home" properties to it.
        return ncoContact;
    }
}

ContactContext::Location QContactTrackerEngineData::locationContext(const QContactDetail& det) const {
    if (det.contexts().contains(QContactDetail::ContextHome)) {
        return ContactContext::Home;
    } else if(det.contexts().contains(QContactDetail::ContextWork)) {
        return ContactContext::Work;
    }
    return ContactContext::Unknown;
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
    : d(new QContactTrackerEngineData),
    contactArchiveFile("removed"),
    contactArchiveDir(QDir::homePath()+"/.contacts")
{
    Q_UNUSED(parameters);
    // Let's load the settings. Here we store out detail definitions
    // last used id, ...
    bool ok;
    QSettings definitions(QSettings::IniFormat, QSettings::UserScope, "Nokia", "Trackerplugin");
    d->m_lastUsedId = definitions.value("nextAvailableContactId", "1").toUInt(&ok);
    connectToSignals();
}

QContactTrackerEngine::QContactTrackerEngine(const QContactTrackerEngine& other)
    : QContactManagerEngine(), d(other.d)
{
    Q_UNUSED(other);
    connectToSignals();
}

void QContactTrackerEngine::connectToSignals()
{
    // TODO: Is this correct? Why "nco::Audio::iri()"?
    SopranoLive::BackEnds::Tracker::ClassUpdateSignaler *signaler =
            SopranoLive::BackEnds::Tracker::ClassUpdateSignaler::get(
                    nfo::Audio::iri());
    // Note here that we are not using
    // QAbstractItemModel signals from LiveNodes::model() because
    // node list for which notification comes is fixed. Those are used for
    // async implementation
    if (signaler)
    {
        QObject::connect(signaler, SIGNAL(subjectsAdded(const QStringList &)),
                this, SLOT(subjectsAdded(const QStringList &)));
        QObject::connect(signaler,
                SIGNAL(baseRemoveSubjectsd(const QStringList &)), this,
                SLOT(subjectsRemoved(const QStringList &)));
        QObject::connect(signaler,
                SIGNAL(subjectsChanged(const QStringList &)), this,
                SLOT(subjectsChanged(const QStringList &)));
    }

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
    
    // TODO Implement sorting
    QList<QUniqueId> ids;
    RDFVariable rdfContact = RDFVariable::fromType<nco::PersonContact>();
    if (filter.type() == QContactFilter::ChangeLog) {
        const QContactChangeLogFilter& clFilter = static_cast<const QContactChangeLogFilter&>(filter);
        // Removed since
        if (clFilter.changeType() == QContactChangeLogFilter::Removed) {
            error = QContactManager::NotSupportedError;
            return ids;
        }
        // Added since
        if (clFilter.changeType() == QContactChangeLogFilter::Added) {
            rdfContact.property<nao::hasTag>().property<nao::prefLabel>() = LiteralValue("addressbook");
            rdfContact.property<nie::contentCreated>() >= LiteralValue(clFilter.since().toString(Qt::ISODate));
        }
        // Changed since
        else if (clFilter.changeType() == QContactChangeLogFilter::Changed) {
            rdfContact.property<nao::hasTag>().property<nao::prefLabel>() = LiteralValue("addressbook");
            rdfContact.property<nie::contentLastModified>() >= LiteralValue(clFilter.since().toString(Qt::ISODate));
        }
    }
    RDFSelect query;
    query.addColumn("contact_uri", rdfContact);
    query.addColumn("contactId", rdfContact.property<nco::contactUID>());
    foreach (QContactSortOrder sort, sortOrders) {
        query.orderBy(contactDetail2Rdf(rdfContact, sort.detailDefinitionName(), sort.detailFieldName()),
                      sort.direction() == Qt::AscendingOrder);
    }
    LiveNodes ncoContacts = ::tracker()->modelQuery(query);
    for (int i = 0; i < ncoContacts->rowCount(); i++) {
        ids.append(ncoContacts->index(i, 1).data().toUInt());
    }

    error = QContactManager::NoError;
    return ids;
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
        ids.append(ncoContacts->index(i, 1).data().toUInt());
    }

    error = QContactManager::NoError;
    return ids;
}

QContact QContactTrackerEngine::contact(const QUniqueId& contactId, QContactManager::Error& error ) const
{
    
    qWarning()<<"QContactManager::contact()"<<"api is not supported for tracker plugin. Please use asynchronous API QContactFetchRequest.";
    
    // the rest of the code is for internal usage, unit tests etc.
    QContactIdListFilter idlist;
    QList<QUniqueId> ids; ids << contactId;
    idlist.setIds(ids);
    QContactFetchRequest request;
    QStringList fields;

    fields << QContactAvatar::DefinitionName
            << QContactBirthday::DefinitionName
            << QContactAddress::DefinitionName
            << QContactEmailAddress::DefinitionName
            << QContactDisplayLabel::DefinitionName
            << QContactGender::DefinitionName
            << QContactAnniversary::DefinitionName
            << QContactName::DefinitionName
            << QContactOnlineAccount::DefinitionName
            << QContactOrganisation::DefinitionName
            << QContactPhoneNumber::DefinitionName
            << QContactPresence::DefinitionName;
    request.setDefinitionRestrictions(fields);
    request.setFilter(idlist);

    QContactTrackerEngine engine(*this);
    engine.startRequest(&request);
    // 10 seconds should be enough
    for(int i = 0; i < 100; i++)
    {
        usleep(100000);
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        if(request.isFinished())
            break;
    }
    if( request.isFinished())
        error = QContactManager::UnspecifiedError;
    // leave the code for now while not all other code is fixed
    error = QContactManager::NoError;
    if( request.contacts().size()>0)
        return request.contacts()[0];
    return QContact();
}

/*!
 * Not very good solution, but we add "addressbook" tag to identify which contacts
 * are added but addressbook ( in order to separate them from facebook and telepathy
 * contacts
 */
void createTagIfItDoesntExistAlready(const QString &tag)
{
    static bool checked = false;
    // only once, if someone remove tag we are in problems (lost contacts)
    if( !checked )
    {
        checked = true;
        RDFVariable rdfTag = RDFVariable::fromType<nao::Tag>();
        RDFVariable labelVar = rdfTag.optional().property<nao::prefLabel>();
        labelVar = LiteralValue(tag);
        RDFFilter doesntExist = labelVar.isBound().not_();// do not create if it already exist

        RDFUpdate up;

        QUrl newTag = ::tracker()->createLiveNode().uri();
        rdfTag = newTag;
        QList<RDFVariableStatement> insertions;
        insertions << RDFVariableStatement(rdfTag, rdf::type::iri(), nao::Tag::iri())
        << RDFVariableStatement(newTag, nao::prefLabel::iri(), labelVar);
        up.addInsertion(insertions); // this way we apply filter doesntExist to both insertions
        ::tracker()->executeQuery(up);
    }
}

void addTag(RDFServicePtr service, RDFVariable &var, const QString &tag)
{
    // TODO do all in one RDF query: create tag if not existing
    createTagIfItDoesntExistAlready(tag);
    RDFUpdate up;
    RDFVariable rdftag;
    rdftag.property<nao::prefLabel>() = LiteralValue(tag);
    up.addInsertion(var, nao::hasTag::iri(), rdftag);
    service->executeQuery(up);
}

/*!
 * Saving has to go in such way that all names are saved at once, all phone numbers together
 * filled to rdfupdate query etc.
 * This method goes through the contact and collect which contact detail definitions are there
 */
QStringList detailsDefinitionsInContact(const QContact &c)
{
    QStringList definitions;
    foreach(const QContactDetail& det, c.details())
    {
        definitions<<det.definitionName();
    }
    definitions.removeDuplicates();
    return definitions;
}

// just moving this code out of saveContact to make it shorter
bool contactHasWorkRelatedDetails(const QContact &c)
{
    foreach(const QContactDetail& det, c.details())
    {
        if( det.contexts().contains(QContactDetail::ContextWork))
           return true;
    }
    return false;
}

/*!
 * write all phone numbers on one query to tracker
 * TODO this is temporary code for creating new, saving contacts need to handle only what was
 * changed.
 */
void appendPhoneNumbersUpdate(RDFUpdate &up, RDFVariable &var, const QList<QContactPhoneNumber> &details )
{

    // not perfect way, supposed to delete all and add all in one rdf update,
    // fix after learning how to use sparql modify
    RDFVariable varForInsert = var.deepCopy();
    RDFVariable phones = var.property<nco::hasPhoneNumber>();
    RDFVariable types = phones.property<rdf::type>();
    up.addDeletion(RDFVariableStatement(var, nco::hasPhoneNumber::iri(), phones));
    up.addDeletion(phones, rdf::type::iri(), types);
    foreach(const QContactPhoneNumber& det, details)
    {
        QUrl newPhone = ::tracker()->createLiveNode().uri();
        up.addInsertion(varForInsert, nco::hasPhoneNumber::iri(), newPhone);
        //QString type = det.attribute(QContactPhoneNumber::AttributeSubType);

        if( det.subTypes().contains(QContactPhoneNumber::SubTypeMobile))
            up.addInsertion(newPhone, rdf::type::iri(), nco::CellPhoneNumber::iri());
        else if( det.subTypes().contains(QContactPhoneNumber::SubTypeCar))
            up.addInsertion(newPhone, rdf::type::iri(), nco::CarPhoneNumber::iri());
        else if( det.subTypes().contains(QContactPhoneNumber::SubTypeBulletinBoardSystem))
            up.addInsertion(newPhone, rdf::type::iri(), nco::BbsNumber::iri());
        else if( det.subTypes().contains(QContactPhoneNumber::SubTypeFacsimile))
            up.addInsertion(newPhone, rdf::type::iri(), nco::FaxNumber::iri());
        else if( det.subTypes().contains(QContactPhoneNumber::SubTypeModem))
            up.addInsertion(newPhone, rdf::type::iri(), nco::ModemNumber::iri());
        else if( det.subTypes().contains(QContactPhoneNumber::SubTypePager))
            up.addInsertion(newPhone, rdf::type::iri(), nco::PagerNumber::iri());
        else if( det.subTypes().contains(QContactPhoneNumber::SubTypeMessagingCapable))
            up.addInsertion(newPhone, rdf::type::iri(), nco::MessagingNumber::iri());
        else
            up.addInsertion(newPhone, rdf::type::iri(), nco::VoicePhoneNumber::iri());

        up.addInsertion(newPhone, nco::phoneNumber::iri(), LiteralValue(det.value(QContactPhoneNumber::FieldNumber)));

    }
}

/*!
 * write all phone numbers on one query to tracker
 * TODO this is temporary code for creating new, saving contacts need to handle only what was
 * changed.
 */
void appendEmailsUpdate(RDFUpdate &up, RDFVariable &var, const QList<QContactDetail> &details )
{

    RDFVariable varForInsert = var.deepCopy();
    RDFVariable emails = var.property<nco::hasEmailAddress>();
    RDFVariable types = emails.property<rdf::type>();
    up.addDeletion(RDFVariableStatement(var, nco::hasEmailAddress::iri(), emails));
    up.addDeletion(emails, rdf::type::iri(), types);
    foreach(const QContactDetail& det, details)
    {
        QUrl newEmail = ::tracker()->createLiveNode().uri();
        up.addInsertion(newEmail, rdf::type::iri(), nco::EmailAddress::iri());
        up.addInsertion(newEmail, nco::emailAddress::iri(), LiteralValue(det.value(QContactEmailAddress::FieldEmailAddress)));
        up.addInsertion(RDFVariableStatement(varForInsert, nco::hasEmailAddress::iri(), newEmail));
    }
}

/*!
 * write all phone numbers on one query to tracker
 * TODO this is temporary code for creating new, saving contacts need to handle only what was
 * changed.
 */
void appendAddressUpdate(RDFUpdate &up, RDFVariable &var, const QList<QContactDetail> &details )
{

    RDFVariable varForInsert = var.deepCopy();
    RDFVariable addresses = var.property<nco::hasPostalAddress>();
    RDFVariable types = addresses.property<rdf::type>();
    up.addDeletion(RDFVariableStatement(var, nco::hasPostalAddress::iri(), addresses));
    up.addDeletion(addresses, rdf::type::iri(), types);
    foreach(const QContactDetail& det, details)
    {
        QUrl newPostalAddress = ::tracker()->createLiveNode().uri();
        // TODO     nco:DomesticDeliveryAddress, nco:InternationalDeliveryAddress, nco:ParcelDeliveryAddress
        up.addInsertion(newPostalAddress, rdf::type::iri(), nco::PostalAddress::iri());
        if( det.hasValue(QContactAddress::FieldStreet))
            up.addInsertion(newPostalAddress, nco::streetAddress::iri(), LiteralValue(det.value(QContactAddress::FieldStreet)));
        if( det.hasValue(QContactAddress::FieldLocality))
            up.addInsertion(newPostalAddress, nco::locality::iri(), LiteralValue(det.value(QContactAddress::FieldLocality)));
        if( det.hasValue(QContactAddress::FieldCountry))
            up.addInsertion(newPostalAddress, nco::country::iri(), LiteralValue(det.value(QContactAddress::FieldCountry)));
        if( det.hasValue(QContactAddress::FieldPostcode))
            up.addInsertion(newPostalAddress, nco::postalcode::iri(), LiteralValue(det.value(QContactAddress::FieldPostcode)));
        if( det.hasValue(QContactAddress::FieldRegion))
            up.addInsertion(newPostalAddress, nco::region::iri(), LiteralValue(det.value(QContactAddress::FieldRegion)));

        up.addInsertion(RDFVariableStatement(varForInsert, nco::hasPostalAddress::iri(), newPostalAddress));
    }
}

// create nco::Affiliation if there is not one already in tracker
void createAffiliationIfItDoesntExist(QUniqueId contactId)
{
    RDFVariable contact = RDFVariable::fromType<nco::PersonContact>();
    contact.property<nco::contactUID> () = LiteralValue(QString::number(contactId));
    RDFVariable contact1 = contact.deepCopy();
    RDFUpdate up;

    // here we will specify to add new node for affiliation if it doesnt exist already
    RDFVariable affiliation = contact.optional().property<nco::hasAffiliation> ();
    RDFFilter doesntExist = affiliation.isBound().not_();// do not create if it already exist
    QUrl newAffiliation = ::tracker()->createLiveNode().uri();
    QList<RDFVariableStatement> insertions;
    insertions << RDFVariableStatement(contact, nco::hasAffiliation::iri(), newAffiliation)
    << RDFVariableStatement(newAffiliation, rdf::type::iri(), nco::Affiliation::iri());

    // this means that filter applies to both insertions
    up.addInsertion(insertions);

    ::tracker()->executeQuery(up);
}

bool QContactTrackerEngine::saveContact( QContact* contact,
                                         QSet<QUniqueId>& contactsAdded,
                                         QSet<QUniqueId>& contactsChanged,
                                         QSet<QUniqueId>& groupsChanged,
                                         QContactManager::Error& error)
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

    RDFTransactionPtr transaction = RDFTransactionPtr();// doesnt work with 1.6.0.2 ::tracker()->initiateTransaction();

    RDFServicePtr service;
    if(transaction)
        // if transaction was obtained, grab the service from inside it and use it
        service = transaction->service();
    else
        // otherwise, use tracker directly, with no transactions.
        service = ::tracker();

    Live<nco::PersonContact> ncoContact;
    bool newContact = false;
    if(contact->id() == 0) {
        // Save new contact
        newContact = true;
        d->m_lastUsedId += 1;
        ncoContact = service->liveNode(QUrl("contact:"+(QString::number(d->m_lastUsedId))));
        QSettings definitions(QSettings::IniFormat, QSettings::UserScope, "Nokia", "Trackerplugin");
        contact->setId(d->m_lastUsedId);
        ncoContact->setContactUID(QString::number(d->m_lastUsedId));
        ncoContact->setContentCreated(QDateTime::currentDateTime());
        definitions.setValue("nextAvailableContactId", QString::number(d->m_lastUsedId));
        contactsAdded << contact->id();
    }  else {
        ncoContact = service->liveNode(QUrl("contact:"+QString::number(contact->id())));
        ncoContact->setContentLastModified(QDateTime::currentDateTime());
        contactsChanged << contact->id();
    }
    // if there are work related details, need to be saved to Affiliation.
    if( contactHasWorkRelatedDetails(*contact))
        createAffiliationIfItDoesntExist(contact->id());

    // Add a special tag for contact added from addressbook, not from fb, telepathy etc.
    RDFVariable rdfContact = RDFVariable::fromType<nco::PersonContact>();
    rdfContact.property<nco::contactUID>() = LiteralValue(QString::number(contact->id()));
    addTag(service, rdfContact, "addressbook");

    saveContactDetails( service, ncoContact, contact, error );

    // remember to commit the transaction, otherwise all changes will be rolled back.
    if(transaction)
        transaction->commit();
    error = QContactManager::NoError;
    return true;
}

void QContactTrackerEngine::saveContactDetails( RDFServicePtr service,
                                                Live<nco::PersonContact>& ncoContact,
                                                QContact* contact,
                                                QContactManager::Error& error )
{
    // TODO There is no error handling nor reporting
    error = QContactManager::NoError;
    QStringList detailDefinitionsToSave = detailsDefinitionsInContact(*contact);

    foreach(QString definition, detailDefinitionsToSave)
    {
        QList<QContactDetail> details = contact->details(definition);
        Q_ASSERT(!details.isEmpty());

        if(definition == QContactName::DefinitionName) {
            QContactDetail detail;
            detail = details[0];
            ncoContact->setNameGiven(detail.value(QContactName::FieldFirst));
            ncoContact->setNameAdditional(detail.value(QContactName::FieldMiddle));
            ncoContact->setNameFamily(detail.value(QContactName::FieldLast));
            continue;
        } 

        // all the rest might need to save to PersonContact and to Affiliation contact
        RDFVariable rdfPerson = RDFVariable::fromType<nco::PersonContact>();
        rdfPerson.property<nco::contactUID>() = LiteralValue(QString::number(contact->id()));

        RDFVariable rdfAffiliation;
        RDFVariable rdfPerson1;
        rdfPerson1.property<nco::hasAffiliation>() = rdfAffiliation;
        rdfPerson1.property<nco::contactUID>() = LiteralValue(QString::number(contact->id()));
        RDFUpdate updateQuery;

        QList<QContactDetail> toAffiliation;
        QList<QContactDetail> toPerson;
        foreach(const QContactDetail& det, details)
            {
                if( det.contexts().contains(QContactDetail::ContextWork) )
                    toAffiliation << det;
                else
                    toPerson << det;
            }
        /* Save all phone numbers at once */
        if(definition == QContactPhoneNumber::DefinitionName) {
            if (!toPerson.isEmpty()) {
                appendPhoneNumbersUpdate(updateQuery, rdfPerson, filterByDetailType<QContactPhoneNumber>(toPerson));
            }
            if( !toAffiliation.isEmpty()) {
                appendPhoneNumbersUpdate(updateQuery, rdfAffiliation, filterByDetailType<QContactPhoneNumber>(toAffiliation));
            }
            service->executeQuery(updateQuery);
        }
        else if(definition == QContactEmailAddress::DefinitionName) {
            if (!toPerson.isEmpty())
                appendEmailsUpdate(updateQuery, rdfPerson, toPerson);
            if( !toAffiliation.isEmpty())
                appendEmailsUpdate(updateQuery, rdfAffiliation, toAffiliation);
            service->executeQuery(updateQuery);
        }
        else if(definition == QContactAddress::DefinitionName) {
            if (!toPerson.isEmpty())
                appendAddressUpdate(updateQuery, rdfPerson, toPerson);
            if( !toAffiliation.isEmpty())
                appendAddressUpdate(updateQuery, rdfAffiliation, toAffiliation);
            service->executeQuery(updateQuery);
        }
        else {
            // Work in progress. soon writing them in single rdf update too

            foreach(const QContactUrl& contacturl, filterByDetailType<QContactUrl>(details))
            {
                QUrl url = contacturl.value(QContactUrl::FieldUrl);
                Live<nco::Role> contact = d->contactByContext(contacturl, ncoContact);

                if (contacturl.subType() == QContactUrl::SubTypeHomePage ){
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

            foreach(const QContactDetail &det, details )
            {
                definition = det.definitionName();
                if(definition == QContactAvatar::DefinitionName) {
                    QUrl avatar = det.value(QContactAvatar::FieldAvatar);
                    Live<nie::DataObject> fdo = service->liveNode( avatar );
                    ncoContact->setPhoto(fdo);
                }
                else if (definition == QContactOnlineAccount::DefinitionName){
                    // TODO parse URI, once it is defined
                    QString account = det.value("Account");
                    QString serviceName = det.value("ServiceName");
                    Live<nco::Role> contact = d->contactByContext(det, ncoContact);
                    Live<nco::IMAccount> liveIMAccount = contact->firstHasIMAccount();
                    if (0 == liveIMAccount)
                        {
                            liveIMAccount = contact->addHasIMAccount();
                        }
                    liveIMAccount->setImID(account);
                    liveIMAccount->setImAccountType(serviceName);
                }
            } // end foreach detail
        }
    }
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
            (*contacts)[i].setId(contact.id());
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
 * Returns the list of data types supported by the Tracker engine
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

RDFVariable QContactTrackerEngine::contactDetail2Rdf(const RDFVariable& rdfContact, const QString& definitionName,
                                                      const QString& fieldName) const
{
    if (definitionName == QContactName::DefinitionName) {
        if (fieldName == QContactName::FieldFirst) {
            return rdfContact.property<nco::nameGiven>();
        }
        else if (fieldName == QContactName::FieldLast) {
            return rdfContact.property<nco::nameFamily>();
        }
        else if (fieldName == QContactName::FieldMiddle) {
            return rdfContact.property<nco::nameAdditional>();
        }
        else if (fieldName == QContactName::FieldPrefix) {
            return rdfContact.property<nco::nameHonorificPrefix>();
        }
        else if (fieldName == QContactName::FieldSuffix) {
            return rdfContact.property<nco::nameHonorificSuffix>();
        }
    }
    return RDFVariable();
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

// TEMPORARY here we'll for now extract ids from tracker contact URI.
// In future need nonblocking async way to get contact ids from tracker contact urls
// let's see which signals will be used from libqttracker
QUniqueId url2UniqueId(const QString &contactUrl)
{
    QRegExp rx("(\\d+)");
    bool conversion = false;
    QUniqueId id = 0;
    if( rx.lastIndexIn(contactUrl) != -1 )
    {
        id = rx.cap(1).toUInt(&conversion, 10);
    }
    if( !conversion )
        qWarning()<<Q_FUNC_INFO<<"unparsed uri to uniqueI:"<<contactUrl;
    return id;

}

void QContactTrackerEngine::subjectsAdded(const QStringList &subjects)
{
    QList<QUniqueId> added;
    foreach(const QString &uri, subjects)
    {
        added << url2UniqueId(uri);
    }
    qDebug()<<Q_FUNC_INFO<<"added contactids:"<<added;
    emit contactsAdded(added);
}

void QContactTrackerEngine::subjectsRemoved(const QStringList &subjects)
{
    QList<QUniqueId> added;
    foreach(const QString &uri, subjects)
    {
        added << url2UniqueId(uri);
    }
    qDebug()<<Q_FUNC_INFO<<"added contactids:"<<added;
    emit contactsRemoved(added);
}

// TODO data changed for full query
void QContactTrackerEngine::subjectsChanged(const QStringList &subjects)
{
    QList<QUniqueId> added;
    foreach(const QString &uri, subjects)
    {
        added << url2UniqueId(uri);
    }
    qDebug()<<Q_FUNC_INFO<<"added contactids:"<<added;
    emit contactsChanged(added);
}

/*! \reimp */
void QContactTrackerEngine::requestDestroyed(QContactAbstractRequest* req)
{
    if( d->m_requests.contains(req) )
    {
        QTrackerContactAsyncRequest *request = d->m_requests.take(req);
        delete request;
    }
}

/*! \reimp */
bool QContactTrackerEngine::startRequest(QContactAbstractRequest* req)
{
    QTrackerContactAsyncRequest *request = new QTrackerContactAsyncRequest(req, this);
    d->m_requests[req] = request;
    return true;
}
