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




#include <QTimer>
#include <qtcontacts.h>
#include <QtTracker/ontologies/nie.h>
#include <QtTracker/ontologies/nco.h>
#include <qtrackercontactasyncrequest.h>

using namespace SopranoLive;

void matchPhoneNumber(RDFVariable &variable, QContactDetailFilter &filter)
{
    // This here is the first implementation of filtering that takes into account also affiliations.
    // needs to be applied for other filters too - TODO
    RDFVariable officeContact;
    RDFVariable homeContact;

    RDFVariable rdfPhoneNumber;
    rdfPhoneNumber = homeContact.property<nco::hasPhoneNumber>().property<nco::phoneNumber>();

    RDFVariable rdfOfficePhoneNumber;
    rdfOfficePhoneNumber = officeContact.property<nco::hasAffiliation>().property<nco::hasPhoneNumber>().property<nco::phoneNumber>();

    QString filterValue = filter.value().toString();
    if (filter.matchFlags() == Qt::MatchEndsWith)
    {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Nokia","Trackerplugin");
        int matchDigitCount = settings.value("phoneNumberMatchDigitCount", "7").toInt();
        filterValue = filterValue.right(matchDigitCount);
        qDebug() << "match with:" << matchDigitCount << ":" << filterValue;
        rdfPhoneNumber.hasSuffix(filterValue);
        rdfOfficePhoneNumber.hasSuffix(filterValue);
    }
    else
    {   // default to exact match
        rdfOfficePhoneNumber.matchesRegexp(filterValue);
        rdfPhoneNumber.matchesRegexp(filterValue);
    }
    // This is the key part, including both contacts and affiliations
    variable.isMemberOf(RDFVariableList()<<homeContact);// TODO report bug doesnt work in tracker <<officeContact);
}

/*
 * RDFVariable describes all contacts in tracker before filter is applied.
 * This method translates QContactFilter to tracker rdf filter. When query is made
 * after this method, it would return only contacts that fit the filter.
 */
void QTrackerContactFetchRequest::applyFilterToContact(RDFVariable &variable,
        const QContactFilter &filter)
{
    if (filter.type() == QContactFilter::LocalIdFilter) {
        QContactLocalIdFilter filt = filter;
        if (!filt.ids().isEmpty()) {
            variable.property<nco::contactUID>().isMemberOf(filt.ids());
        } else {
            qWarning() << Q_FUNC_INFO << "QContactLocalIdFilter idlist is empty";
        }
    } else if (filter.type() == QContactFilter::ContactDetailFilter) {
        // this one is tricky as we need to match in contacts or in affiliations

        QContactDetailFilter filt = filter;
        if ( QContactPhoneNumber::DefinitionName == filt.detailDefinitionName()
             && QContactPhoneNumber::FieldNumber == filt.detailFieldName()) {
            matchPhoneNumber(variable, filt);
        }
        else if (filt.matchFlags() == Qt::MatchExactly) {
            if (QContactEmailAddress::DefinitionName == filt.detailDefinitionName()
                       && QContactEmailAddress::FieldEmailAddress == filt.detailFieldName()) {
                RDFVariable rdfEmailAddress;
                rdfEmailAddress = variable.property<nco::hasEmailAddress>();
                rdfEmailAddress.property<nco::emailAddress>() = LiteralValue(filt.value().toString());
            } else if (QContactOnlineAccount::DefinitionName == filt.detailDefinitionName()
                       && filt.detailFieldName() == "Account") {
                RDFVariable imaccount = variable.property<nco::hasIMAccount>();
                imaccount.property<nco::imID>().isMemberOf(QStringList()<<filt.value().toString());
            } else {
                qWarning() << "QContactTrackerEngine: Unsupported QContactFilter::ContactDetail"
                    << filt.detailDefinitionName();
            }
        }
    } else if (filter.type() == QContactFilter::ChangeLogFilter) {
        const QContactChangeLogFilter& clFilter = static_cast<const QContactChangeLogFilter&>(filter);
        // do not return facebook and telepathy contacts here
        // it is a temp implementation for the what to offer to synchronization constraint
        variable.property<nao::hasTag>().property<nao::prefLabel>() = LiteralValue("addressbook");

        if (clFilter.eventType() == QContactChangeLogFilter::EventRemoved) { // Removed since
            // did not find how to set errror to async request
            // error = QContactManager::NotSupportedError;
            qWarning() << "QContactTrackerEngine: Unsupported QContactChangeLogFilter::Removed (contacts removed since)";
        } else if (clFilter.eventType() == QContactChangeLogFilter::EventAdded) { // Added since
            variable.property<nie::contentCreated>() >= LiteralValue(clFilter.since().toString(Qt::ISODate));
        } else if (clFilter.eventType() == QContactChangeLogFilter::EventChanged) { // Changed since
            variable.property<nie::contentLastModified>() >= LiteralValue(clFilter.since().toString(Qt::ISODate));
        }
    }
}

/*
 * To understand why all the following methods have for affiliation param, check nco ontology:
 * every contact has all these properties and also linked to affiliations (also contacts - nco:Role)
 * that again have the same properties. So it was needed to make the same query 2-ce - once for contact
 * and once for affiliations
 */
RDFSelect preparePhoneNumbersQuery(RDFVariable &rdfcontact1, bool forAffiliations)
{
    RDFVariable phone;
    if (!forAffiliations)
        phone = rdfcontact1.property<nco::hasPhoneNumber>();
    else
        phone = rdfcontact1.property<nco::hasAffiliation>().property<nco::hasPhoneNumber>();
    RDFVariable type = phone.type();
    type.property<rdfs::subClassOf>().notEqual(nco::ContactMedium::iri()); // sparql cannot handle exact type but returns all super types as junk rows
    type.property<rdfs::subClassOf>().notEqual(rdfs::Resource::iri()); // sparql cannot handle exact type but returns all super types as junk rows
    // therefore we eliminate those rows that are not of interest
    // columns
    RDFSelect queryidsnumbers;
    queryidsnumbers.addColumn("contactId", rdfcontact1.property<nco::contactUID> ());
    queryidsnumbers.addColumn("phoneno", phone.property<nco::phoneNumber> ());
    queryidsnumbers.addColumn("type", type);
    queryidsnumbers.distinct();
    return queryidsnumbers;
}

RDFSelect prepareEmailAddressesQuery(RDFVariable &rdfcontact1, bool forAffiliations)
{
    RDFVariable email;
    if (!forAffiliations)
        email = rdfcontact1.property<nco::hasEmailAddress>();
    else
        email = rdfcontact1.property<nco::hasAffiliation>().property<nco::hasEmailAddress>();
    const RDFVariable& type = email.type();
    type.property<rdfs::subClassOf>().notEqual(nco::Resource::iri()); // sparql cannot handle exact type but returns all super types as junk rows
    // therefore we eliminate those rows that are not of interest
    // columns
    RDFSelect queryidsnumbers;
    queryidsnumbers.addColumn("contactId", rdfcontact1.property<nco::contactUID> ());
    queryidsnumbers.addColumn("emailaddress", email.property<nco::emailAddress> ());
    rdfcontact1.property<nco::hasEmailAddress> ().isOfType( nco::EmailAddress::iri(), true);
    queryidsnumbers.addColumn("type", type);
    queryidsnumbers.distinct();
    return queryidsnumbers;
}

RDFSelect prepareIMAccountsQuery(RDFVariable &rdfcontact1, bool forAffiliations)
{
    RDFVariable imaccount;
    if (!forAffiliations)
        imaccount = rdfcontact1.property<nco::hasIMAccount> ();
    else
        imaccount = rdfcontact1.property<nco::hasAffiliation> ().property<nco::hasIMAccount> ();
    // columns
    RDFSelect queryidsimacccounts;
    queryidsimacccounts.addColumn("contactId", rdfcontact1.property<nco::contactUID> ());

    queryidsimacccounts.addColumn("IMId", imaccount.property<nco::imID> ());
    queryidsimacccounts.addColumn("status", imaccount.optional().property<nco::imStatus> ());
    queryidsimacccounts.addColumn("message", imaccount.optional().property<nco::imStatusMessage> ());
    queryidsimacccounts.addColumn("nick", imaccount.optional().property<nco::imNickname> ());
    queryidsimacccounts.addColumn("type", imaccount.optional().property<nco::imAccountType> ());
    queryidsimacccounts.addColumn("comment", imaccount.optional().property<nco::contactMediumComment>());
    return queryidsimacccounts;
}


QTrackerContactAsyncRequest::QTrackerContactAsyncRequest(QContactAbstractRequest* request)
: req(request)
{
}

QTrackerContactAsyncRequest::~QTrackerContactAsyncRequest()
{

}

/*!
 * The method was initially created to add default fields in case client did not supply
 * fields constraint - in that case the constraint is that default contact fields (ones
 * being edited in contact card and synchronized) are queried.
 */
void QTrackerContactFetchRequest::validateRequest()
{
    Q_ASSERT(req);
    Q_ASSERT(req->type() == QContactAbstractRequest::ContactFetchRequest);
    QContactFetchRequest* r = qobject_cast<QContactFetchRequest*> (req);
    if (r && r->definitionRestrictions().isEmpty()) {
        QStringList fields;
        fields << QContactAvatar::DefinitionName
            << QContactBirthday::DefinitionName
            << QContactAddress::DefinitionName
            << QContactEmailAddress::DefinitionName
            << QContactGender::DefinitionName
            << QContactAnniversary::DefinitionName
            << QContactName::DefinitionName
            << QContactOnlineAccount::DefinitionName
            << QContactOrganization::DefinitionName
            << QContactPhoneNumber::DefinitionName
            << QContactUrl::DefinitionName;
        r->setDefinitionRestrictions(fields);
    }
}

QTrackerContactFetchRequest::QTrackerContactFetchRequest(QContactAbstractRequest* request,
                                                         QContactManagerEngine* parent) :
    QObject(parent),QTrackerContactAsyncRequest(request),
    queryPhoneNumbersNodesReady(false),
    queryEmailAddressNodesReady(false)
{
    Q_ASSERT(parent);
    Q_ASSERT(request);
    QList<QContactManager::Error> dummy;
    QContactManagerEngine::updateRequestStatus(req, QContactManager::NoError, dummy,
                                QContactAbstractRequest::Active);

    QTimer::singleShot(0, this, SLOT(run()));
}

void QTrackerContactFetchRequest::run()
{
    validateRequest();
    QContactFetchRequest* r = qobject_cast<QContactFetchRequest*> (req);

    RDFVariable RDFContact = RDFVariable::fromType<nco::PersonContact>();
    applyFilterToContact(RDFContact, r->filter());
    if (r->definitionRestrictions().contains(QContactPhoneNumber::DefinitionName)) {
        queryPhoneNumbersNodes.clear();
        queryPhoneNumbersNodesReady = 0;
        for(int forAffiliations = 0; forAffiliations <= 1; forAffiliations++) {
            // prepare query to get all phone numbers
            RDFVariable rdfcontact1 = RDFVariable::fromType<nco::PersonContact>();
            applyFilterToContact(rdfcontact1, r->filter());
            // criteria - only those with phone numbers
            RDFSelect queryidsnumbers = preparePhoneNumbersQuery(rdfcontact1, forAffiliations);
            queryPhoneNumbersNodes << ::tracker()->modelQuery(queryidsnumbers);
            // need to store LiveNodes in order to receive notification from model
            QObject::connect(queryPhoneNumbersNodes[forAffiliations].model(),
                             SIGNAL(modelUpdated()), this, SLOT(phoneNumbersReady()));
        }
    }

    if (r->definitionRestrictions().contains(QContactEmailAddress::DefinitionName)) {
        queryEmailAddressNodes.clear();
        queryEmailAddressNodesReady = 0;
        for(int forAffiliations = 0; forAffiliations <= 1; forAffiliations++) {
            // prepare query to get all email addresses
            RDFVariable rdfcontact1 = RDFVariable::fromType<nco::PersonContact>();
            applyFilterToContact(rdfcontact1, r->filter());
            // criteria - only those with email addresses
            RDFSelect queryidsnumbers = prepareEmailAddressesQuery(rdfcontact1,forAffiliations);
            queryEmailAddressNodes << ::tracker()->modelQuery(queryidsnumbers);
            // need to store LiveNodes in order to receive notification from model
            QObject::connect(queryEmailAddressNodes[forAffiliations].model(),
                             SIGNAL(modelUpdated()), this, SLOT(emailAddressesReady()));
        }
    }

    if ( r->definitionRestrictions().contains( QContactOnlineAccount::DefinitionName) ) {
        queryIMAccountNodes.clear();
        queryIMAccountNodesReady = 0;
        for(int forAffiliations = 0; forAffiliations <= 1; forAffiliations++) {
            // prepare query to get all im accounts
            RDFVariable rdfcontact1 = RDFVariable::fromType<nco::PersonContact>();
            applyFilterToContact(rdfcontact1, r->filter());
            // criteria - only those with im accounts
            RDFSelect queryidsimacccounts = prepareIMAccountsQuery(rdfcontact1,forAffiliations);
            queryIMAccountNodes << ::tracker()->modelQuery(queryidsimacccounts);
            QObject::connect(queryIMAccountNodes[forAffiliations].model(),
                             SIGNAL(modelUpdated()),SLOT(iMAcountsReady()));
        }
    }

    QList<QContactLocalId> ids;
    RDFVariable RDFContact1 = RDFVariable::fromType<nco::PersonContact>();
    applyFilterToContact(RDFContact1, r->filter());
    RDFSelect quer;
    RDFVariable prefix = RDFContact1.optional().property<nco::nameHonorificPrefix> ();
    RDFVariable lastname = RDFContact1.optional().property<nco::nameFamily> ();
    RDFVariable middlename = RDFContact1.optional().property<nco::nameAdditional> ();
    RDFVariable firstname = RDFContact1.optional().property<nco::nameGiven> ();
    RDFVariable nickname = RDFContact1.optional().property<nco::nickname> ();
    quer.addColumn("contactId", RDFContact1.property<nco::contactUID> ());
    quer.addColumn("prefix", prefix);
    quer.addColumn("firstname", firstname);
    quer.addColumn("middlename", middlename);
    quer.addColumn("secondname", lastname);
    quer.addColumn("photo", RDFContact1.optional().property<nco::photo> ());
    quer.addColumn("nickname", nickname);

    // for now adding columns to main query. later separate queries
    if (r->definitionRestrictions().contains(QContactAddress::DefinitionName)) {
        RDFVariable address = RDFContact.optional().property< nco::hasPostalAddress> ();
        quer.addColumn("street",address.optional().property<nco::streetAddress> ());
        quer.addColumn("city", address.optional().property<nco::locality> ());
        quer.addColumn("country", address.optional().property<nco::country> ());
        quer.addColumn("pcode", address.optional().property<nco::postalcode> ());
        quer.addColumn("reg", address.optional().property<nco::region> ());
    }
    if (r->definitionRestrictions().contains(QContactUrl::DefinitionName)) {
        quer.addColumn("homepage", RDFContact.optional().property<nco::websiteUrl> ());
        quer.addColumn("url", RDFContact.optional().property<nco::url> ());
        quer.addColumn("work_homepage", RDFContact.optional().property<nco::hasAffiliation> ().property<nco::websiteUrl> ());
        quer.addColumn("work_url", RDFContact.optional().property<nco::hasAffiliation> ().property<nco::url> ());
    }
    if (r->definitionRestrictions().contains(QContactBirthday::DefinitionName)) {
        quer.addColumn("birth",RDFContact.optional().property<nco::birthDate> ());
    }
    if (r->definitionRestrictions().contains(QContactGender::DefinitionName)) {
        quer.addColumn("gender", RDFContact.optional().property<nco::gender> ());
    }
    if (r->definitionRestrictions().contains(QContactOrganization::DefinitionName)) {
        RDFVariable rdforg = RDFContact.optional().property<nco::hasAffiliation> ().optional().property<nco::org> ();
        quer.addColumn("org", rdforg.optional().property<nco::fullname> ());
        quer.addColumn("logo", rdforg.optional().property<nco::logo> ());
    }

    // QContactAnniversary - no such thing in tracker
    // QContactGeolocation - nco:hasLocation is not having class defined in nco yet. no properties. maybe rdfs:Resource:label

    // supporting sorting only here, difficult and no requirements in UI for sorting in multivalue details (phones, emails)
    foreach(QContactSortOrder sort, r->sorting()) {
        if (sort.detailDefinitionName() == QContactName::DefinitionName) {
            if (sort.detailFieldName() == QContactName::FieldFirst)
                quer.orderBy(firstname);
            else if (sort.detailFieldName() == QContactName::FieldLast)
                quer.orderBy(lastname);
            else
                qWarning() << "QTrackerContactFetchRequest" << "sorting by"
                    << sort.detailDefinitionName()
                    << sort.detailFieldName() << "is not yet supported";
        } else {
            qWarning() << "QTrackerContactFetchRequest" << "sorting by"
                << sort.detailDefinitionName()
                << "is not yet supported";
        }
    }
    query = ::tracker()->modelQuery(quer);
    // need to store LiveNodes in order to receive notification from model
    QObject::connect(query.model(), SIGNAL(modelUpdated()), this, SLOT(contactsReady()));
}

bool detailExisting(const QString &definitionName, const QContact &contact, const QContactDetail &adetail)
{
    QList<QContactDetail> details = contact.details(definitionName);
    foreach(const QContactDetail &detail, details) {
        if (detail == adetail) {
            return true;
        }
    }
    return false;
}

void QTrackerContactFetchRequest::contactsReady()
{
    QContactFetchRequest* request = qobject_cast<QContactFetchRequest*> (req);
    Q_ASSERT( request ); // signal is supposed to be used only for contact fetch
    // fastest way to get this working. refactor
    if (!request) {
        QList<QContactManager::Error> dummy;
        QContactManagerEngine::updateRequestStatus(req, QContactManager::UnspecifiedError, dummy,
                                    QContactAbstractRequest::Finished);
        return;
    }
    QList<QContact> result;
    // access existing contacts in result list, contactid to array index (result) lookup
    QHash<quint32, int> resultLookup;
    for(int i = 0; i < query->rowCount(); i++) {
        QContact contact; // one we will be filling with this row
        int column = 0;

        QContactId id; id.setLocalId(query->index(i, column++).data().toUInt());
        contact.setId(id);
        bool ok;
        QContactLocalId contactid = query->index(i, 0).data().toUInt(&ok);
        if (!ok) {
            // Got an invalid contact id, skip the whole result
            qWarning()<< Q_FUNC_INFO <<"Invalid contact ID: "<< query->index(i, 0).data().toString();
            continue;
        }
        QHash<quint32, int>::const_iterator it = resultLookup.find(contactid);
        int index = -1;
        if (resultLookup.end() != it) {
            if (it.value() < result.size() && it.value() >= 0) {
                index = it.value();
                contact = result[index];
            }
            Q_ASSERT(query->index(i, 0).data().toUInt() == contact.localId());
        }


        // using redundancy to get less queries to tracker - it is possible
        // that rows are repeating: information for one contact is in several rows
        // that's why we update existing contact and append details to it if they
        // are not already in QContact
        QContactName name = contact.detail(QContactName::DefinitionName);
        name.setPrefix(query->index(i, column++).data().toString());
        name.setFirst(query->index(i, column++).data().toString());
        name.setMiddle(query->index(i, column++).data().toString());
        name.setLast(query->index(i, column++).data().toString());
        contact.saveDetail(&name);

        QContactAvatar avatar = contact.detail(QContactAvatar::DefinitionName);
        avatar.setAvatar(query->index(i, column++).data().toString());
        if (!avatar.avatar().isEmpty()) {
            contact.saveDetail(&avatar);
        }
        QContactNickname nick = contact.detail(QContactNickname::DefinitionName);
        nick.setNickname(query->index(i, column++).data().toString());
        contact.saveDetail(&nick);

        // TODO extract generic from bellow ... mapping field names
        if (request->definitionRestrictions().contains(QContactAddress::DefinitionName)) {
            QString street = query->index(i, column++).data().toString();
            QString loc = query->index(i, column++).data().toString();
            QString country = query->index(i, column++).data().toString();
            QString pcode = query->index(i, column++).data().toString();
            QString region = query->index(i, column++).data().toString();
            if (!(country.isEmpty() && pcode.isEmpty() && region.isEmpty()
                  && street.isEmpty() && loc.isEmpty())) {
                // for multivalue fields is a bit tricky - try to find existing an
                QContactAddress a;
                a.setStreet(street);
                a.setLocality(loc);
                a.setCountry(country);
                a.setPostcode(pcode);
                a.setRegion(region);
                if (!detailExisting(QContactAddress::DefinitionName, contact, a)) {
                    contact.saveDetail(&a);
                }
            }
        }
        if (request->definitionRestrictions().contains(QContactUrl::DefinitionName)) {
            // check query preparation (at the moment in constructor TODO refactor)
            // home website
            // if it is websiteUrl then interpret as homepage, if it is nco:url then fovourite url
            QContactUrl url;
            url.setSubType(QContactUrl::SubTypeHomePage);
            url.setContexts(QContactUrl::ContextHome);
            url.setUrl(query->index(i, column++).data().toString());
            if (url.url().isEmpty()) {
                // website url is at the same time url, so we handle duplication here
                // if only url then set it as favourite
                url.setUrl(query->index(i, column++).data().toString());
                url.setSubType(QContactUrl::SubTypeFavourite);
            }

            if (!url.url().isEmpty() && !detailExisting(QContactUrl::DefinitionName, contact, url)) {
                contact.saveDetail(&url);
            }
            // office website
            QContactUrl workurl;
            workurl.setContexts(QContactUrl::ContextWork);
            workurl.setSubType(QContactUrl::SubTypeHomePage);
            workurl.setUrl(query->index(i, column++).data().toString());
            if (workurl.url().isEmpty()) {
                workurl.setUrl(query->index(i, column++).data().toString());
                workurl.setSubType(QContactUrl::SubTypeFavourite);
            }
            if (!workurl.url().isEmpty() && !detailExisting(QContactUrl::DefinitionName, contact, workurl)) {
                contact.saveDetail(&workurl);
            }
        }
        if (request->definitionRestrictions().contains(QContactBirthday::DefinitionName)) {
            QVariant var = query->index(i, column++).data();
            if (!var.toString().isEmpty() /* enable reading wrong && var.toDate().isValid()*/) {
                QContactBirthday birth = contact.detail(QContactBirthday::DefinitionName);
                birth.setDate(var.toDate());
                contact.saveDetail(&birth);
            }
        }
        if (request->definitionRestrictions().contains(QContactGender::DefinitionName)) {
            QString var = query->index(i, column++).data().toString();
            if (!var.isEmpty()) {
                QContactGender g = contact.detail(QContactGender::DefinitionName);
                g.setGender(var);
                contact.saveDetail(&g);
            }
        }
        if (request->definitionRestrictions().contains(QContactOrganization::DefinitionName)) {
            QString org = query->index(i, column++).data().toString();
            QString logo = query->index(i, column++).data().toString();
            if (!( org.isEmpty() && logo.isEmpty())) {
                QContactOrganization o;
                o.setName(org);
                o.setLogo(logo);
                if (!detailExisting(QContactOrganization::DefinitionName, contact, o)) {
                    contact.saveDetail(&o);
                }
            }
        }
        if (index < result.size() && index >= 0) {
            result[index] = contact;
        } else {
            resultLookup[contact.localId()] = result.size();
            result.append(contact);
        }
    }
    if (request->definitionRestrictions().contains(QContactPhoneNumber::DefinitionName)) {
        Q_ASSERT(queryPhoneNumbersNodes.size() == 2);
        for( int cnt = 0; cnt < queryPhoneNumbersNodes.size(); cnt++) {
            processQueryPhoneNumbers(queryPhoneNumbersNodes[cnt], result, cnt);
        }
    }
    if (request->definitionRestrictions().contains(QContactEmailAddress::DefinitionName)) {
        qDebug() << "processQueryEmailAddresses";
        Q_ASSERT(queryEmailAddressNodes.size() == 2);
        for (int cnt = 0; cnt < queryEmailAddressNodes.size(); cnt++) {
            processQueryEmailAddresses(queryEmailAddressNodes[cnt], result, cnt);
        }
    }
    if ( request->definitionRestrictions().contains(QContactOnlineAccount::DefinitionName)) {
        Q_ASSERT(queryIMAccountNodes.size() == 2);
        for (int cnt = 0; cnt < queryIMAccountNodes.size(); cnt++) {
            processQueryIMAccounts(queryIMAccountNodes[cnt], result, cnt);
        }
    }
    // update display labels
    QContactManagerEngine *engine = dynamic_cast<QContactManagerEngine*>(parent());
    Q_ASSERT(engine);
    for(int i = 0; i < result.count(); i++)
    {
        QContact &cont(result[i]);
        QContactDisplayLabel dl = cont.detail(QContactDisplayLabel::DefinitionName);
        if (dl.label().isEmpty()) {
            QContactManager::Error synthError;
            result[i] = engine->setContactDisplayLabel(engine->synthesizeDisplayLabel(cont, synthError), cont);
        }
    }

    QContactManagerEngine::updateRequest(req, result, QContactManager::NoError,
                              QList<QContactManager::Error> (),
                              QContactAbstractRequest::Finished, true);
}

void QTrackerContactFetchRequest::phoneNumbersReady()
{
    queryPhoneNumbersNodesReady++;
}

void QTrackerContactFetchRequest::emailAddressesReady()
{
    queryEmailAddressNodesReady++;
}

void QTrackerContactFetchRequest::iMAcountsReady()
{
    queryIMAccountNodesReady++;
    // now we know that the query is ready before get all contacts, check how it works with transactions
}

/*!
 * An internal helper method for converting nco:PhoneNumber subtype to
 * QContactPhoneNumber:: subtype attribute
 */
const QString rdfPhoneType2QContactSubtype(const QString rdfPhoneType)
{
    if( rdfPhoneType.endsWith("VoicePhoneNumber") )
        return QContactPhoneNumber::SubTypeVoice;
    else if ( rdfPhoneType.endsWith("CarPhoneNumber") )
        return QContactPhoneNumber::SubTypeCar;
    else if ( rdfPhoneType.endsWith("CellPhoneNumber") )
        return QContactPhoneNumber::SubTypeMobile;
    else if ( rdfPhoneType.endsWith("BbsPhoneNumber") )
        return QContactPhoneNumber::SubTypeBulletinBoardSystem;
    else if ( rdfPhoneType.endsWith("FaxNumber") )
        return QContactPhoneNumber::SubTypeFacsimile;
    else if ( rdfPhoneType.endsWith("ModemNumber") )
        return QContactPhoneNumber::SubTypeModem;
    else if ( rdfPhoneType.endsWith("PagerNumber") )
        return QContactPhoneNumber::SubTypePager;
    else if ( rdfPhoneType.endsWith("MessagingNumber") )
        return QContactPhoneNumber::SubTypeMessagingCapable;
    else
        qWarning() << Q_FUNC_INFO << "Not handled phone number type:" << rdfPhoneType;
    return "";
}

void QTrackerContactFetchRequest::processQueryPhoneNumbers(SopranoLive::LiveNodes queryPhoneNumbers,
                                                           QList<QContact>& contacts,
                                                           bool affiliationNumbers )
{
    Q_ASSERT_X( queryPhoneNumbersNodesReady==2, Q_FUNC_INFO, "Phonenumbers query was supposed to be ready and it is not." );
    for (int i = 0; i < queryPhoneNumbers->rowCount(); i++) {
        qDebug() << Q_FUNC_INFO << i << queryPhoneNumbers->rowCount() << queryPhoneNumbers->columnCount()
            << queryPhoneNumbers->index(i, 0).data().toString()
            << queryPhoneNumbers->index(i, 1).data().toString()
            << queryPhoneNumbers->index(i, 2).data().toString();
        // ignore if next one is the same - asked iridian about making query to ignore supertypes
        // TODO remove after his answer
        if ( i-1 >= 0
             && (queryPhoneNumbers->index(i, 0).data().toString()
                 == queryPhoneNumbers->index(i-1, 0).data().toString())
             && (queryPhoneNumbers->index(i, 1).data().toString()
                 == queryPhoneNumbers->index(i-1, 1).data().toString())) {
            // this is for ignoring duplicates. bad approach, asked iridian about
            // how to eliminate super types in query results
            continue;
        }

        QString subtype = rdfPhoneType2QContactSubtype(queryPhoneNumbers->index(i, 2).data().toString());
        QContactLocalId contactid = queryPhoneNumbers->index(i, 0).data().toUInt();
        // TODO replace this lookup with something faster
        for (int j = 0; j < contacts.size(); j++) {
            if (contacts[j].localId() == contactid ) {
                QContactPhoneNumber number;
                if( affiliationNumbers )
                    number.setContexts(QContactPhoneNumber::ContextWork);
                else
                    number.setContexts(QContactPhoneNumber::ContextHome);
                number.setNumber(queryPhoneNumbers->index(i, 1).data().toString());
                number.setSubTypes(subtype);
                contacts[j].saveDetail(&number);
                break;
            }
        }
    }
}

void QTrackerContactFetchRequest::processQueryEmailAddresses( SopranoLive::LiveNodes queryEmailAddresses,
                                                              QList<QContact>& contacts,
                                                              bool affiliationEmails)
{
    Q_ASSERT_X(queryEmailAddressNodesReady == 2, Q_FUNC_INFO, "Email query was supposed to be ready and it is not." );
    for (int i = 0; i < queryEmailAddresses->rowCount(); i++) {
        qDebug() << Q_FUNC_INFO << i
            << queryEmailAddresses->rowCount()
            << queryEmailAddresses->columnCount()
            << queryEmailAddresses->index(i, 0).data().toString()
            << queryEmailAddresses->index(i, 1).data().toString()
            << queryEmailAddresses->index(i, 2).data().toString();
        // ignore if next one is the same - asked iridian about making query to ignore supertypes
        // TODO remove after his answer
        if ( i-1 >= 0
             && (queryEmailAddresses->index(i, 0).data().toString()
                 == queryEmailAddresses->index(i-1, 0).data().toString())
             && (queryEmailAddresses->index(i, 1).data().toString()
                 == queryEmailAddresses->index(i-1, 1).data().toString())) {
            // this is for ignoring duplicates. bad approach, asked iridian
            // about how to eliminate super types in query results
            continue;
        }

        //QString subtype = rdfPhoneType2QContactSubtype(queryEmailAddresses->index(i, 2).data().toString());
        QContactLocalId contactid = queryEmailAddresses->index(i, 0).data().toUInt();
        // TODO replace this lookup with something faster
        for (int j = 0; j < contacts.size(); j++) {
            if( contacts[j].localId() == contactid ) {
                QContactEmailAddress email;
                if( affiliationEmails )
                    email.setContexts(QContactEmailAddress::ContextWork);
                else
                    email.setContexts(QContactEmailAddress::ContextHome);
                email.setEmailAddress(queryEmailAddresses->index(i, 1).data().toString());
                //email.setSubTypes(subtype);
                contacts[j].saveDetail(&email);
                break;
            }
        }
    }
}

void QTrackerContactFetchRequest::processQueryIMAccounts(SopranoLive::LiveNodes queryIMAccounts,
                                                         QList<QContact>& contacts,
                                                         bool affiliationAccounts )
{
    Q_ASSERT_X(queryIMAccountNodesReady == 2, Q_FUNC_INFO, "IMAccount query was supposed to be ready and it is not." );
    for (int i = 0; i < queryIMAccounts->rowCount(); i++) {
        //contactid, imid, status, message, nick, type
        qDebug() << Q_FUNC_INFO
            << queryIMAccounts->index(i, 0).data().toString()
            << queryIMAccounts->index(i, 1).data().toString()
            << queryIMAccounts->index(i, 2).data().toString()
            << queryIMAccounts->index(i, 3).data().toString();

        QContactLocalId contactid = queryIMAccounts->index(i, 0).data().toUInt();
        // TODO replace this lookup with something faster
        for (int j = 0; j < contacts.size(); j++) {
            if (contacts[j].localId() == contactid ) {
                // TODo replace when QtMobility guys implement support for IMAccount
                QContactOnlineAccount account;
                if (affiliationAccounts)
                    account.setContexts(QContactOnlineAccount::ContextWork);
                account.setValue("Account", queryIMAccounts->index(i, 1).data().toString()); // IMId
                if (!queryIMAccounts->index(i, 5).data().toString().isEmpty())
                    account.setValue("AccountPath", queryIMAccounts->index(i, 5).data().toString()); // getImAccountType?
                account.setValue("Capabilities", queryIMAccounts->index(i, 6).data().toString()); // getImAccountType?
                account.setNickname(queryIMAccounts->index(i, 4).data().toString()); // nick
                account.setPresence(queryIMAccounts->index(i, 2).data().toString()); // imStatus
                qDebug() << "the status from **tracker**" << account.presence();
                account.setStatusMessage(queryIMAccounts->index(i, 3).data().toString()); // imStatusMessage
                contacts[j].saveDetail(&account);
                contacts[j].saveDetail(&account);
                break;
            }
        }
    }
}
