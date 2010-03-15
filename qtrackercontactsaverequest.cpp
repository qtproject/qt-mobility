/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qtrackercontactsaverequest.h"
#include "trackerchangelistener.h"

#include <QtTracker/Tracker>
using namespace SopranoLive;

#include "qtrackercontactslive.h"

// TODO better error handling when saving
QTrackerContactSaveRequest::QTrackerContactSaveRequest(QContactAbstractRequest* req, QContactManagerEngine* parent)
: QObject(parent), QTrackerContactAsyncRequest(req), errorCount(0)
{
    Q_ASSERT(req);
    Q_ASSERT(req->type() == QContactAbstractRequest::ContactSaveRequest);
    Q_ASSERT(parent);

    QContactSaveRequest* r = qobject_cast<QContactSaveRequest*>(req);
    if (!r) {
        QContactManagerEngine::updateRequestState(req, QContactAbstractRequest::FinishedState);
        return;
    }

    QList<QContact> contacts = r->contacts();

    if(contacts.isEmpty()) {
        QMap<int, QContactManager::Error> errors; 
        errors[0] = QContactManager::BadArgumentError;
        QContactSaveRequest* saveRequest = qobject_cast<QContactSaveRequest*>(req);
        QContactManagerEngine::updateContactSaveRequest(saveRequest, contacts, QContactManager::BadArgumentError,
                                             errors);
        return;
    }

    QContactManagerEngine::updateRequestState(req, QContactAbstractRequest::ActiveState);

    TrackerChangeListener *changeListener = new TrackerChangeListener(parent, this);
    connect(changeListener, SIGNAL(contactsChanged(const QList<QContactLocalId> &)),SLOT(onTrackerSignal(const QList<QContactLocalId> &)));
    connect(changeListener, SIGNAL(contactsAdded(const QList<QContactLocalId> &)),SLOT(onTrackerSignal(const QList<QContactLocalId> &)));

    // Save contacts with batch size
    /// @todo where to get reasonable batch size
    int batchSize = 100;
    for (int i = 0; i < contacts.size(); i+=batchSize) {
        saveContacts(contacts.mid(i, batchSize));
    }
}

void QTrackerContactSaveRequest::onTrackerSignal(const QList<QContactLocalId> &ids)
{
    computeProgress(ids);
}

void QTrackerContactSaveRequest::computeProgress(const QList<QContactLocalId> &addedIds)
{
    Q_ASSERT(req->type() == QContactAbstractRequest::ContactSaveRequest);
    QContactSaveRequest* r = qobject_cast<QContactSaveRequest*>(req);
    if (!r) {
        QContactManagerEngine::updateRequestState(req, QContactAbstractRequest::FinishedState);
        return;
    }

    foreach (QContactLocalId id, addedIds) {
        pendingContactIds.remove(id);
        // since if was OK, remove entry for error
        errorsOfContactsFinished.remove(id2Index[id]);
    }

    if (pendingContactIds.count() == 0) {
        // compute master error - part of qtcontacts api
        QContactManager::Error error = QContactManager::NoError;
        
        foreach(QContactManager::Error err, errorsOfContactsFinished.values()) {
            if( QContactManager::NoError != err )
            {
                error = err;
                break;
            }
        }

        QContactManagerEngine::updateContactSaveRequest(r, contactsFinished, error, errorsOfContactsFinished);
        QContactManagerEngine::updateRequestState(req, QContactAbstractRequest::FinishedState);
    }
}

void QTrackerContactSaveRequest::saveContacts(const QList<QContact> &contacts)
{
    QContactManagerEngine *engine = qobject_cast<QContactManagerEngine *> (parent());
    Q_ASSERT(engine);

    QSettings definitions(QSettings::IniFormat, QSettings::UserScope, "Nokia", "Trackerplugin");
    QTrackerContactsLive cLive;
    RDFServicePtr service = cLive.service();

    foreach(QContact contact, contacts) {
/*
        Validation is disabled because it blocks saving contacts parsed from vcards
        TODO left the commented code while opaque (custom) details are under discussion as remainder
        QContactManager::Error error;
        if(!engine->validateContact(contact, error)) {
            contactsFinished << contact;
            errorsOfContactsFinished[errorCount++] =  error;
            computeProgress(QList<QContactLocalId>());
            continue;
        }
*/
        Live<nco::PersonContact> ncoContact;

        if(contact.localId() == 0) {
            // Save new contact. compute ID
            bool ok;
            // what if both processes read in the same time and write at the same time, no increment
            unsigned int m_lastUsedId = definitions.value("nextAvailableContactId", "1").toUInt(&ok);
            definitions.setValue("nextAvailableContactId", QString::number(++m_lastUsedId));

            ncoContact = service->liveNode(QUrl("contact:"+(QString::number(m_lastUsedId))));
            QContactId id;
            id.setLocalId(m_lastUsedId);
            id.setManagerUri(engine->managerUri());
            contact.setId(id);
            ncoContact->setContactUID(QString::number(m_lastUsedId));
            ncoContact->setContentCreated(QDateTime::currentDateTime());
        }  else {
            ncoContact = service->liveNode(QUrl("contact:"+QString::number(contact.localId())));
            /// @note Following needed in case we save new contact with given localId
            ncoContact->setContactUID(QString::number(contact.localId()));
            ncoContact->setContentLastModified(QDateTime::currentDateTime());
        }
        pendingContactIds.insert(contact.localId());

        // if there are work related details, need to be saved to Affiliation.
        if( QTrackerContactSaveRequest::contactHasWorkRelatedDetails(contact)) {
            addAffiliation(service, contact.localId());
        }

        // Add a special tag for contact added from addressbook, not from fb, telepathy etc.
        // this is important when returning contacts to sync team
        RDFVariable rdfContact = RDFVariable::fromType<nco::PersonContact>();
        rdfContact.property<nco::contactUID>() = LiteralValue(QString::number(contact.localId()));
        addTag(service, rdfContact, "addressbook");

        saveContactDetails( service, ncoContact, contact);

        // name & nickname - different way from other details
        cLive.setLiveContact(ncoContact);
        cLive.setQContact(contact);
        if( !contact.detail<QContactName>().isEmpty() || !contact.detail<QContactNickname>().isEmpty() ) {
            cLive.saveName();
        }

        contactsFinished << contact;
        id2Index[contact.localId()] = errorCount;
        // we fill error here - once response come that everything is OK, remove entry for this contact
        errorsOfContactsFinished[errorCount++] =  QContactManager::BadArgumentError;
    }
    // remember to commit the transaction, otherwise all changes will be rolled back.
    cLive.commit();
}


QTrackerContactSaveRequest::~QTrackerContactSaveRequest()
{
    // TODO Auto-generated destructor stub
}

/*!
* Saving has to go in such way that all names are saved at once, all phone numbers together
* filled to rdfupdate query etc.
* This method goes through the contact and collect which contact detail definitions are there
*/
QStringList QTrackerContactSaveRequest::detailsDefinitionsInContact(const QContact &c)
{
    QStringList definitions;
    foreach(const QContactDetail& det, c.details())
        {
            definitions << det.definitionName();
        }
    definitions.removeDuplicates();
    return definitions;
}

//! Just moving this code out of saveContact to make it shorter
bool QTrackerContactSaveRequest::contactHasWorkRelatedDetails(const QContact &c)
{
    foreach(const QContactDetail& det, c.details())
    {
        if( det.contexts().contains(QContactDetail::ContextWork))
           return true;
    }
    return false;
}

// create nco::Affiliation if there is not one already in tracker
void QTrackerContactSaveRequest::addAffiliation(RDFServicePtr service, QContactLocalId contactId)
{
    Live<nco::PersonContact> ncoContact = service->liveNode(QUrl("contact:"+(QString::number(contactId))));
    Live<nco::Affiliation> ncoAffiliation = service->liveNode(QUrl("affiliation:"+(QString::number(contactId))));
    ncoContact->setHasAffiliation(ncoAffiliation);
}

void QTrackerContactSaveRequest::saveContactDetails( RDFServicePtr service,
                                                Live<nco::PersonContact>& ncoContact,
                                                const QContact& contact)
{
    QStringList detailDefinitionsToSave = detailsDefinitionsInContact(contact);

    // all the rest might need to save to PersonContact and to Affiliation contact
    RDFVariable rdfPerson = RDFVariable::fromType<nco::PersonContact>();
    rdfPerson.property<nco::contactUID>() = LiteralValue(QString::number(contact.localId()));

    // Delete all existing phone numbers - office and home
    deletePhoneNumbers(service, rdfPerson);

    foreach(QString definition, detailDefinitionsToSave)
    {
        QList<QContactDetail> details = contact.details(definition);
        Q_ASSERT(!details.isEmpty());

        RDFVariable rdfAffiliation;
        RDFVariable rdfPerson1;
        rdfPerson1.property<nco::hasAffiliation>() = rdfAffiliation;
        rdfPerson1.property<nco::contactUID>() = LiteralValue(QString::number(contact.localId()));

        QList<QContactDetail> workDetails;
        QList<QContactDetail> homeDetails;
        foreach(const QContactDetail& det, details) {
            // details can be for both contexts, so check for both seperately
            if( det.contexts().contains(QContactDetail::ContextWork) ) {
                workDetails << det;
            }
            if( det.contexts().contains(QContactDetail::ContextHome)) {
                homeDetails << det;
            }
        }

        /* Save details */
        if(definition == QContactPhoneNumber::DefinitionName) {
            if (!homeDetails.isEmpty()) {
                savePhoneNumbers(service, rdfPerson, homeDetails);
            }
            if( !workDetails.isEmpty()) {
                savePhoneNumbers(service, rdfAffiliation, workDetails);
            }
        }
        else if(definition == QContactEmailAddress::DefinitionName) {
            if (!homeDetails.isEmpty())
                saveEmails(service, rdfPerson, homeDetails);
            if( !workDetails.isEmpty())
                saveEmails(service, rdfAffiliation, workDetails);
        }
        else if(definition == QContactAddress::DefinitionName) {
            if (!homeDetails.isEmpty())
                saveAddresses(service, rdfPerson, homeDetails);
            if( !workDetails.isEmpty())
                saveAddresses(service, rdfAffiliation, workDetails);
        }
        else if(definition == QContactUrl::DefinitionName) {
            if (!homeDetails.isEmpty())
                saveUrls(service, rdfPerson, homeDetails);
            if( !workDetails.isEmpty())
                saveUrls(service, rdfAffiliation, workDetails);
        }
        else {
            // TODO refactor (bug: editing photo doesn't work)
            foreach(const QContactDetail &det, details )
            {
                definition = det.definitionName();
                if(definition == QContactAvatar::DefinitionName) {
                    QUrl avatar = det.value(QContactAvatar::FieldAvatar);
                    Live<nie::DataObject> fdo = service->liveNode( avatar );
                    ncoContact->setPhoto(fdo);
                }
                if(definition == QContactBirthday::DefinitionName) {
                    ncoContact->setBirthDate(QDateTime(det.variantValue(QContactBirthday::FieldBirthday).toDate(), QTime(), Qt::UTC));
                }
            } // end foreach detail
        }
    }
}

// Remove all existing references to phone numbers from the contact so that edits are
// reflected to Tracker correctly.
// Delete the references to phone numbers - not the numbers themselves as they remain in tracker
// with their canonical URI form - might be linked to history.
void QTrackerContactSaveRequest::deletePhoneNumbers(RDFServicePtr service, const RDFVariable& rdfContactIn)
{
    {
        RDFUpdate up;
        RDFVariable rdfContact = rdfContactIn.deepCopy();
        up.addDeletion(rdfContact, nco::hasPhoneNumber::iri(), rdfContact.property<nco::hasPhoneNumber>());
        service->executeQuery(up);
    }

    // affiliation
    {
        RDFUpdate up;
        RDFVariable rdfContact = rdfContactIn.deepCopy().property<nco::hasAffiliation>();
        up.addDeletion(rdfContact, nco::hasPhoneNumber::iri(), rdfContact.property<nco::hasPhoneNumber>());
        service->executeQuery(up);
    }
}

/*!
 * write all phone numbers on one query to tracker
 * TODO this is temporary code for creating new, saving contacts need to handle only what was
 * changed.
 */
void QTrackerContactSaveRequest::savePhoneNumbers(RDFServicePtr service, RDFVariable &var, const QList<QContactDetail> &details )
{
    RDFUpdate up;
    RDFVariable varForInsert = var.deepCopy();
    foreach(const QContactDetail& det, details)
    {
        QString formattedValue = det.value(QContactPhoneNumber::FieldNumber);
        // Strip RFC 3966 visual-separators reg exp "[(|-|.|)| ]"
        QString value = formattedValue.replace( QRegExp("[\\(|" \
                                                        "\\-|" \
                                                        "\\.|" \
                                                        "\\)|" \
                                                        " ]"), "");
        // Temporary, because affiliation is still used - to be refactored next week to use Live nodes
        // using RFC 3966 canonical URI form
        QUrl newPhone = QString("tel:%1").arg(value);
        Live<nco::PhoneNumber> ncoPhone = service->liveNode(newPhone);
        ncoPhone->remove();

        QStringList subtypes = det.value<QStringList>(QContactPhoneNumber::FieldSubTypes);

        if( subtypes.contains(QContactPhoneNumber::SubTypeMobile))
            up.addInsertion(newPhone, rdf::type::iri(), nco::CellPhoneNumber::iri());
        else if( subtypes.contains(QContactPhoneNumber::SubTypeCar))
            up.addInsertion(newPhone, rdf::type::iri(), nco::CarPhoneNumber::iri());
        else if( subtypes.contains(QContactPhoneNumber::SubTypeBulletinBoardSystem))
            up.addInsertion(newPhone, rdf::type::iri(), nco::BbsNumber::iri());
        else if( subtypes.contains(QContactPhoneNumber::SubTypeFacsimile))
            up.addInsertion(newPhone, rdf::type::iri(), nco::FaxNumber::iri());
        else if( subtypes.contains(QContactPhoneNumber::SubTypeModem))
            up.addInsertion(newPhone, rdf::type::iri(), nco::ModemNumber::iri());
        else if( subtypes.contains(QContactPhoneNumber::SubTypePager))
            up.addInsertion(newPhone, rdf::type::iri(), nco::PagerNumber::iri());
        else if( subtypes.contains(QContactPhoneNumber::SubTypeMessagingCapable))
            up.addInsertion(newPhone, rdf::type::iri(), nco::MessagingNumber::iri());
        else
            up.addInsertion(newPhone, rdf::type::iri(), nco::VoicePhoneNumber::iri());

        up.addInsertion(newPhone, nco::phoneNumber::iri(), LiteralValue(value));
        up.addInsertion(varForInsert, nco::hasPhoneNumber::iri(), newPhone);
    }
    service->executeQuery(up);
}

/*!
 * write all phone numbers on one query to tracker
 * TODO this is temporary code for creating new, saving contacts need to handle only what was
 * changed.
 */
void QTrackerContactSaveRequest::saveEmails(RDFServicePtr service, RDFVariable &var, const QList<QContactDetail> &details )
{
    RDFUpdate up;
    RDFVariable varForInsert = var.deepCopy();
    RDFVariable emails = var.property<nco::hasEmailAddress>();
    // delete previous references - keep email IRIs
    up.addDeletion(RDFVariableStatement(var, nco::hasEmailAddress::iri(), emails));

    foreach(const QContactDetail& det, details)
    {
        QString value = det.value(QContactEmailAddress::FieldEmailAddress);
        // Temporary, because affiliation is still used - to be refactored next week to use only Live nodes
        QUrl newEmail = QString("mailto:%1").arg(value);
        Live<nco::EmailAddress> ncoEmail = service->liveNode(newEmail);
        up.addInsertion(newEmail, rdf::type::iri(), nco::EmailAddress::iri());
        up.addInsertion(newEmail, nco::emailAddress::iri(), LiteralValue(value));
        up.addInsertion(RDFVariableStatement(varForInsert, nco::hasEmailAddress::iri(), newEmail));
    }
    service->executeQuery(up);
}

/*!
 * write all Urls
 * TODO this is temporary code for creating new, saving contacts need to handle only what was
 * changed.
 */
void QTrackerContactSaveRequest::saveUrls(RDFServicePtr service, RDFVariable &rdfContact, const QList<QContactDetail> &details )
{
    RDFUpdate up;
    RDFVariable varForInsert = rdfContact.deepCopy();
    RDFVariable urls = rdfContact.property<nco::url>();
    RDFVariable urltypes = urls.property<rdf::type>();

    RDFVariable websiteUrls = rdfContact.property<nco::websiteUrl>();
    RDFVariable websiteUrlTypes = websiteUrls.property<rdf::type>();

    up.addDeletion(RDFVariableStatement(rdfContact, nco::url::iri(), urls));
    up.addDeletion(RDFVariableStatement(rdfContact, nco::websiteUrl::iri(), websiteUrls));
    // first part - deleting previous before adding new again is to be removed

    // second part, write all urls
    foreach(const QContactDetail& det, details)
    {
        QUrl newUrl(det.value(QContactUrl::FieldUrl));//::tracker()->createLiveNode().uri();
        if(det.value(QContactUrl::FieldSubType) == QContactUrl::SubTypeFavourite)
        {
            up.addInsertion(varForInsert, nco::url::iri(), newUrl);
        }
        else // if not favourite, then homepage. don't support other
        {
            up.addInsertion(varForInsert, nco::websiteUrl::iri(), newUrl); // add it to contact
        }
    }
    service->executeQuery(up);
}

/*!
 * write all phone numbers on one query to tracker
 * TODO this is temporary code for creating new, saving contacts need to handle only what was
 * changed.
 */
void QTrackerContactSaveRequest::saveAddresses(RDFServicePtr service, RDFVariable &var, const QList<QContactDetail> &details )
{
    RDFUpdate up;
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
    service->executeQuery(up);
}

/*!
 * Not very good solution, but we add "addressbook" tag to identify which contacts
 * are added but addressbook ( in order to separate them from facebook and telepathy
 * contacts
 */
void QTrackerContactSaveRequest::createTagIfItDoesntExistAlready(SopranoLive::RDFServicePtr service, const QString &tag)
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
        service->executeQuery(up);
    }
}

void QTrackerContactSaveRequest::addTag(RDFServicePtr service, RDFVariable &var, const QString &tag)
{
    // TODO do all in one RDF query: create tag if not existing
    createTagIfItDoesntExistAlready(service, tag);
    RDFUpdate up;
    RDFVariable rdftag;
    rdftag.property<nao::prefLabel>() = LiteralValue(tag);
    up.addInsertion(var, nao::hasTag::iri(), rdftag);
    service->executeQuery(up);
}
