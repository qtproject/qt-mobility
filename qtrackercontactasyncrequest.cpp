/* * This file is part of qtcontacts-tracker
 * Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Contact: Aleksandar Stojiljkovic <aleksandar.stojiljkovic@nokia.com>
 * This software, including documentation, is protected by copyright controlled by
 * Nokia Corporation. All rights are reserved. Copying, including reproducing, storing,
 * adapting or translating, any or all of this material requires the prior written consent
 * of Nokia Corporation. This material also contains confidential information which may
 * not be disclosed to others without the prior written consent of Nokia.
 */

#include "qcontactabstractrequest.h"
#include "qcontactmanagerengine.h"


#include <QtTracker/ontologies/nie.h>
#include <QtTracker/ontologies/nco.h>

#include "qcontactdetails.h"
#include "qcontactfilters.h"
#include "qcontactrequests.h"
#include "qtrackercontactasyncrequest.h"
using namespace SopranoLive;


void applyFilterToRDFVariable(RDFVariable &variable,
        const QContactFilter &filter)
{
    if (filter.type() == QContactFilter::IdList){
        QContactIdListFilter filt = filter;
        if( !filt.ids().isEmpty() )
            variable.property<nco::contactUID>().isMemberOf(filt.ids());// = LiteralValue(filt.ids()[0]);
        else
            qWarning()<<Q_FUNC_INFO<<"QContactIdListFilter idlist is empty";
    }
    else if (filter.type() == QContactFilter::ContactDetail){
        // right now implementing this for phone numbers
        // later for the rest of fields
        QContactDetailFilter filt = filter;
        // TODO doesnt pass at the moment, check the reason 
        //if( filt.matchFlags() &  Qt::MatchExactly)// | Qt::MatchEndsWith | Qt::MatchStartsWith | Qt::MatchContains ) )
        {
            if( QContactPhoneNumber::DefinitionName == filt.detailDefinitionName()
                    && QContactPhoneNumber::FieldNumber == filt.detailFieldName() )
            {
                RDFVariable rdfPhoneNumber;
                rdfPhoneNumber = variable.property<nco::hasPhoneNumber>();
                // TODO figure out how to use filter, now only exact match
                rdfPhoneNumber.property<nco::phoneNumber>() = LiteralValue(filt.value().toString());
            }
            else if( QContactEmailAddress::DefinitionName == filt.detailDefinitionName()
                    && QContactEmailAddress::FieldEmailAddress == filt.detailFieldName() )
            {
                RDFVariable rdfEmailAddress;
                rdfEmailAddress = variable.property<nco::hasEmailAddress>();
                // TODO figure out how to use filter, now only exact match
                rdfEmailAddress.property<nco::emailAddress>() = LiteralValue(filt.value().toString());
            }
            else
                qWarning()<<"QContactTrackerEngine: Unsupported QContactFilter::ContactDetail"<<filt.detailDefinitionName();
        }
    } 
    else if(filter.type() == QContactFilter::ChangeLog)
    {
        const QContactChangeLogFilter& clFilter = static_cast<const QContactChangeLogFilter&>(filter);
        // do not return facebook and telepathy contacts here
        // it is a temp implementation for the what to offer to synchronization constraint 
        variable.property<nao::hasTag>().property<nao::prefLabel>() = LiteralValue("addressbook");

        // Removed since
        if (clFilter.changeType() == QContactChangeLogFilter::Removed) {
            // did not find how to set errror to async request
            // error = QContactManager::NotSupportedError;
            qWarning()<<"QContactTrackerEngine: Unsupported QContactChangeLogFilter::Removed (contacts removed since)";
        }
        // Added since
        else if (clFilter.changeType() == QContactChangeLogFilter::Added) {
            variable.property<nie::contentCreated>() >= LiteralValue(clFilter.since().toString(Qt::ISODate));
        }
        // Changed since
        else if (clFilter.changeType() == QContactChangeLogFilter::Changed) {
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
    if( !forAffiliations )
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
    rdfcontact1.property<nco::hasPhoneNumber> ().isOfType( nco::PhoneNumber::iri(), true);
    queryidsnumbers.addColumn("type", type);
    queryidsnumbers.distinct();
    return queryidsnumbers;
}

RDFSelect prepareEmailAddressesQuery(RDFVariable &rdfcontact1, bool forAffiliations)
{
    RDFVariable email;
    if( !forAffiliations )
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




QTrackerContactAsyncRequest::QTrackerContactAsyncRequest(
        QContactAbstractRequest* request, QContactManagerEngine* parent) :
    QObject(parent),
    queryPhoneNumbersNodesReady(false),
    queryEmailAddressNodesReady(false)
{
    req = request;
    switch (req->type())
    {
        case QContactAbstractRequest::ContactIdFetch:
        {
            QContactIdFetchRequest* r =
                    static_cast<QContactIdFetchRequest*> (req);
            QContactFilter filter = r->filter();
            QList<QContactSortOrder> sorting = r->sorting();
            Q_UNUSED(filter)
            Q_UNUSED(sorting)

            RDFVariable RDFContact =
                    RDFVariable::fromType<nco::PersonContact>();
            RDFSelect quer;

            quer.addColumn("contact_uri", RDFContact);
            quer.addColumn("contactId",
                    RDFContact.property<nco::contactUID> ());
            query = ::tracker()->modelQuery(quer);
            // need to store LiveNodes in order to receive notification from model
            QObject::connect(query.model(), SIGNAL(modelUpdated()), this,
                    SLOT(modelUpdated()));

            break;
        }
        case QContactAbstractRequest::ContactFetch:
        {
            QContactFetchRequest* r =
                    static_cast<QContactFetchRequest*> (req);

            RDFVariable RDFContact = RDFVariable::fromType<nco::PersonContact>();
            applyFilterToRDFVariable(RDFContact,r->filter());
            if( r->definitionRestrictions().contains( QContactPhoneNumber::DefinitionName ) )
            {
                queryPhoneNumbersNodes.clear(); queryPhoneNumbersNodesReady = 0;
                for (int forAffiliations = 0; forAffiliations <= 1; forAffiliations++ )
                {
                    // prepare query to get all phone numbers
                    RDFVariable rdfcontact1 = RDFVariable::fromType<nco::PersonContact>();
                    applyFilterToRDFVariable(rdfcontact1,r->filter());
                    // criteria - only those with phone numbers
                    RDFSelect queryidsnumbers = preparePhoneNumbersQuery(rdfcontact1, forAffiliations);
                    queryPhoneNumbersNodes<< ::tracker()->modelQuery(queryidsnumbers);
                    // need to store LiveNodes in order to receive notification from model
                    QObject::connect(queryPhoneNumbersNodes[forAffiliations].model(), SIGNAL(modelUpdated()), this,
                            SLOT(phoneNumbersReady()));
                }
            }

            if (r->definitionRestrictions().contains(QContactEmailAddress::DefinitionName))
            {
                queryEmailAddressNodes.clear();
                queryEmailAddressNodesReady = 0;
                for (int forAffiliations = 0; forAffiliations <= 1; forAffiliations++ )
                {
                    // prepare query to get all email addresses
                    RDFVariable rdfcontact1 = RDFVariable::fromType<nco::PersonContact>();
                    applyFilterToRDFVariable(rdfcontact1,r->filter());
                    // criteria - only those with email addresses
                    RDFSelect queryidsnumbers = prepareEmailAddressesQuery(rdfcontact1, forAffiliations);
                    queryEmailAddressNodes<< ::tracker()->modelQuery(queryidsnumbers);
                    // need to store LiveNodes in order to receive notification from model
                    QObject::connect(queryEmailAddressNodes[forAffiliations].model(), SIGNAL(modelUpdated()), this,
                            SLOT(emailAddressesReady()));
                }
            }

            if (r->definitionRestrictions().contains(QContactPresence::DefinitionName)
                    || r->definitionRestrictions().contains(QContactOnlineAccount::DefinitionName))
            {
                queryIMAccountNodes.clear(); queryIMAccountNodesReady = 0;
                for (int forAffiliations = 0; forAffiliations <= 1; forAffiliations++ )
                {
                    // prepare query to get all im accounts
                    RDFVariable rdfcontact1 = RDFVariable::fromType<nco::PersonContact>();
                    applyFilterToRDFVariable(rdfcontact1,r->filter());
                    // criteria - only those with im accounts
                    RDFSelect queryidsimacccounts = prepareIMAccountsQuery(rdfcontact1, forAffiliations);
                    queryIMAccountNodes << ::tracker()->modelQuery(queryidsimacccounts);
                    QObject::connect(queryIMAccountNodes[forAffiliations].model(),SIGNAL(modelUpdated()),SLOT(iMAcountsReady()));
                }
            }
            
            QList<QUniqueId> ids;
            RDFVariable RDFContact1 = RDFVariable::fromType<nco::PersonContact>();
            applyFilterToRDFVariable(RDFContact1, r->filter());
            RDFSelect quer;
            RDFVariable lastname = RDFContact1.optional().property<nco::nameFamily>();
            RDFVariable middlename = RDFContact1.optional().property<nco::nameAdditional>();
            RDFVariable firstname = RDFContact1.optional().property<nco::nameGiven>();
            quer.addColumn("contactId",  RDFContact1.property<nco::contactUID>());
            quer.addColumn("firstname",  firstname);
            quer.addColumn("middlename",  middlename);
            quer.addColumn("secondname", lastname);
            quer.addColumn("photo",      RDFContact1.optional().property<nco::photo>());


            // for now adding columns to main query. later separate queries
            if (r->definitionRestrictions().contains(QContactAddress::DefinitionName))
            {
                RDFVariable address = RDFContact.optional().property<nco::hasPostalAddress>();
                quer.addColumn("street", address.optional().property<nco::streetAddress>());
                quer.addColumn("city", address.optional().property<nco::locality>());
                quer.addColumn("country", address.optional().property<nco::country>());
                quer.addColumn("pcode", address.optional().property<nco::postalcode>());
                quer.addColumn("reg", address.optional().property<nco::region>());
            }
            if (r->definitionRestrictions().contains(QContactUrl::DefinitionName))
            {
                quer.addColumn("homepage", RDFContact.optional().property<nco::websiteUrl>());
            }
            if (r->definitionRestrictions().contains(QContactBirthday::DefinitionName))
            {
                quer.addColumn("birth", RDFContact.optional().property<nco::birthDate>());
            }
            if (r->definitionRestrictions().contains(QContactGender::DefinitionName))
            {
                quer.addColumn("gender", RDFContact.optional().property<nco::gender>());
            }
            if (r->definitionRestrictions().contains(QContactOrganisation::DefinitionName))
            {
                RDFVariable rdforg = RDFContact.optional().property<nco::hasAffiliation>().optional().property<nco::org>();
                quer.addColumn("org", rdforg.optional().property<nco::fullname>());
                quer.addColumn("logo", rdforg.optional().property<nco::logo>());
            }

            // QContactAnniversary - no such thing in tracker
            // QContactGeolocation - nco:hasLocation is not having class defined in nco yet. no properties. maybe rdfs:Resource:label

            // supporting sorting only here, difficult and no requirements in UI for sorting in multivalue details (phones, emails)
            foreach(QContactSortOrder sort, r->sorting())
            {
                if( sort.detailDefinitionName() == QContactName::DefinitionName)
                {
                    if( sort.detailFieldName() == QContactName::FieldFirst)
                        quer.orderBy(firstname);
                    else if( sort.detailFieldName() == QContactName::FieldLast)
                        quer.orderBy(lastname);
                    else
                        qWarning()<<"QTrackerContactAsyncRequest"<<"sorting by"<<sort.detailDefinitionName()<<sort.detailFieldName()<<"is not yet supported";
                }else
                    qWarning()<<"QTrackerContactAsyncRequest"<<"sorting by"<<sort.detailDefinitionName()<<"is not yet supported";
            }

            query = ::tracker()->modelQuery(quer);
            // need to store LiveNodes in order to receive notification from model
            QObject::connect(query.model(), SIGNAL(modelUpdated()), this,
                    SLOT(contactsReady()));



            break;
        }

            // implement the rest
        default:
            break;
    }
}

QTrackerContactAsyncRequest::~QTrackerContactAsyncRequest()
{

}

void QTrackerContactAsyncRequest::modelUpdated()
{
    // fastest way to get this working. refactor
    QContactManagerEngine *engine = qobject_cast<QContactManagerEngine *> (
            parent());
    QList<QUniqueId> result;
    // for now this only serves get all contacts
    for(int i = 0; i < query->rowCount(); i++)
    {
        bool ok;
        QUniqueId id = query->index(i, 1).data().toUInt(&ok);
        if (ok) {
            // Append only, if we have a valid contact id
            result.append(id);
        }
    }

    if (engine)
        engine->updateRequest(req, result, QContactManager::NoError, QList<
                QContactManager::Error> (), QContactAbstractRequest::Finished,
                false);
}

bool detailExisting(const QString &definitionName, const QContact &contact, const QContactDetail &adetail)
{
    QList<QContactDetail> details = contact.details(definitionName);
    foreach(const QContactDetail &detail, details)
    {
        if( detail == adetail )
        {
            return true;
        }
    }
    return false;
}

void QTrackerContactAsyncRequest::contactsReady()
{
    QContactFetchRequest* request = (req->type() == QContactAbstractRequest::ContactFetch)?
            static_cast<QContactFetchRequest*> (req):0;
    Q_ASSERT( request ); // signal is supposed to be used only for contact fetch
    // fastest way to get this working. refactor
    QContactManagerEngine *engine = qobject_cast<QContactManagerEngine *> (
            parent());
    QList<QContact> result;
    // access existing contacts in result list, contactid to array index (result) lookup
    QHash<quint32, int> resultLookup;
    for(int i = 0; i < query->rowCount(); i++)
    {
        QContact contact; // one we will be filling with this row
        int column = 0;

        contact.setId(query->index(i, column++).data().toUInt());
        bool ok;
        QUniqueId contactid = query->index(i, 0).data().toUInt(&ok);
        if (!ok) {
            // Got an invalid contact id, skip the whole result
            continue;
        }
        QHash<quint32, int>::const_iterator it = resultLookup.find(contactid);
        int index = -1;
        if (resultLookup.end() != it)
        {
            if (it.value() < result.size() && it.value() >= 0)
            {
                index = it.value();
                contact = result[index];
            }
            Q_ASSERT(query->index(i, 0).data().toUInt() == contact.id());
        }
        

        // using redundancy to get less queries to tracker - it is possible
        // that rows are repeating: information for one contact is in several rows
        // that's why we update existing contact and append details to it if they
        // are not already in QContact
        QContactName name = contact.detail(QContactName::DefinitionName);
        name.setFirst(query->index(i, column++).data().toString());
        name.setMiddle(query->index(i, column++).data().toString());
        name.setLast(query->index(i, column++).data().toString());
        contact.saveDetail(&name);

        QContactAvatar avatar = contact.detail(QContactAvatar::DefinitionName);
        avatar.setAvatar(query->index(i, column++).data().toString());
        if( !avatar.avatar().isEmpty() )
            contact.saveDetail(&avatar);
        // TODO extract generic from bellow ... mapping field names
        if (request->definitionRestrictions().contains(QContactAddress::DefinitionName))
        {
            QString street = query->index(i, column++).data().toString();
            QString loc = query->index(i, column++).data().toString();
            QString country = query->index(i, column++).data().toString();
            QString pcode = query->index(i, column++).data().toString();
            QString region = query->index(i, column++).data().toString();
            if (!(country.isEmpty() && pcode.isEmpty() && region.isEmpty()
                    && street.isEmpty() && loc.isEmpty()))
            {
                // for multivalue fields is a bit tricky - try to find existing an
                QContactAddress a;
                a.setStreet(street);
                a.setLocality(loc);
                a.setCountry(country);
                a.setPostcode(pcode);
                a.setRegion(region);
                if( !detailExisting(QContactAddress::DefinitionName, contact, a) )
                    contact.saveDetail(&a);
            }
        }
        /*
        if (request->definitionRestrictions().contains(QContactEmailAddress::DefinitionName))
        {
            // no office mails yet
            QContactEmailAddress mail; // constraint here for start only one email
            mail.setEmailAddress(query->index(i, column++).data().toString());
            if( !mail.emailAddress().isEmpty() )
            {
                if( !detailExisting(QContactEmailAddress::DefinitionName, contact, mail) )
                {
                    contact.saveDetail(&mail);
                }
            }
            }*/
        if (request->definitionRestrictions().contains(QContactUrl::DefinitionName))
        {
            QContactUrl url;
            url.setUrl(query->index(i, column++).data().toString());
            if(!url.url().isEmpty())
                if( !detailExisting(QContactUrl::DefinitionName, contact, url) )
                    contact.saveDetail(&url);
        }
        if (request->definitionRestrictions().contains(QContactBirthday::DefinitionName))
        {
            QVariant var = query->index(i, column++).data();
            if( !var.toString().isEmpty() /* enable reading wrong && var.toDate().isValid()*/)
            {
                QContactBirthday birth = contact.detail(QContactBirthday::DefinitionName);
                birth.setDate(var.toDate());
                contact.saveDetail(&birth);
            }
        }
        if (request->definitionRestrictions().contains(QContactGender::DefinitionName))
        {
            QString var = query->index(i, column++).data().toString();
            if( !var.isEmpty() )
            {
                QContactGender g = contact.detail(QContactGender::DefinitionName);
                g.setGender(var);
                contact.saveDetail(&g);
            }
        }
        if (request->definitionRestrictions().contains(QContactOrganisation::DefinitionName))
        {
            QString org = query->index(i, column++).data().toString();
            QString logo = query->index(i, column++).data().toString();
            if( !( org.isEmpty() && logo.isEmpty()) )
            {
                QContactOrganisation o;
                o.setDisplayLabel(org);
                o.setLogo(logo);
                if( !detailExisting(QContactOrganisation::DefinitionName, contact, o) )
                    contact.saveDetail(&o);
            }
        }
        if (index < result.size() && index >= 0)
        {
            result[index] = contact;
        }
        else{
            resultLookup[contact.id()] = result.size();
            result.append(contact);
        }
    }
    if(request->definitionRestrictions().contains( QContactPhoneNumber::DefinitionName ))
    {
        Q_ASSERT(queryPhoneNumbersNodes.size() == 2);
        for( int cnt = 0; cnt < queryPhoneNumbersNodes.size(); cnt++)
            processQueryPhoneNumbers(queryPhoneNumbersNodes[cnt], result, cnt);
    }
    if(request->definitionRestrictions().contains( QContactEmailAddress::DefinitionName ))
    {
        qDebug() << "processQueryEmailAddresses";
        Q_ASSERT(queryEmailAddressNodes.size() == 2);
        for( int cnt = 0; cnt < queryEmailAddressNodes.size(); cnt++)
            processQueryEmailAddresses(queryEmailAddressNodes[cnt], result, cnt);
    }
    if (request->definitionRestrictions().contains(QContactPresence::DefinitionName)
            || request->definitionRestrictions().contains(QContactOnlineAccount::DefinitionName))
    {
        Q_ASSERT(queryIMAccountNodes.size() == 2);
        for( int cnt = 0; cnt < queryIMAccountNodes.size(); cnt++)
            processQueryIMAccounts(queryIMAccountNodes[cnt], result, cnt);
    }

    if (engine)
        engine->updateRequest(req, result, QContactManager::NoError, QList<
                QContactManager::Error> (), QContactAbstractRequest::Finished,
                true);
}

void QTrackerContactAsyncRequest::phoneNumbersReady()
{
    queryPhoneNumbersNodesReady++;
}

void QTrackerContactAsyncRequest::emailAddressesReady()
{
    queryEmailAddressNodesReady++;
}

void QTrackerContactAsyncRequest::iMAcountsReady()
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
        qWarning()<<Q_FUNC_INFO<<"Not handled phone number type:"<<rdfPhoneType;
    return "";
}

void QTrackerContactAsyncRequest::processQueryPhoneNumbers(SopranoLive::LiveNodes queryPhoneNumbers,
                                                           QList<QContact>& contacts,
                                                           bool affiliationNumbers )
{
    Q_ASSERT_X( queryPhoneNumbersNodesReady==2, Q_FUNC_INFO, "Phonenumbers query was supposed to be ready and it is not." );
    for(int i = 0; i < queryPhoneNumbers->rowCount(); i++)
    {
        qDebug()<<Q_FUNC_INFO<<i<<queryPhoneNumbers->rowCount()<<queryPhoneNumbers->columnCount()<<queryPhoneNumbers->index(i, 0).data().toString()<<queryPhoneNumbers->index(i, 1).data().toString()<<queryPhoneNumbers->index(i, 2).data().toString();
        // ignore if next one is the same - asked iridian about making query to ignore supertypes
        // TODO remove after his answer
        if( i-1 >= 0
            && queryPhoneNumbers->index(i, 0).data().toString() == queryPhoneNumbers->index(i-1, 0).data().toString()
            && queryPhoneNumbers->index(i, 1).data().toString() == queryPhoneNumbers->index(i-1, 1).data().toString())
        {
            continue; // this is for ignoring duplicates. bad approach, asked iridian about how to eliminate super types in query results
        }

        QString subtype = rdfPhoneType2QContactSubtype(queryPhoneNumbers->index(i, 2).data().toString());
        QUniqueId contactid = queryPhoneNumbers->index(i, 0).data().toUInt();
        // TODO replace this lookup with something faster
        for( int j = 0; j < contacts.size(); j++)
        {
            if( contacts[j].id() == contactid )
            {
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

void QTrackerContactAsyncRequest::processQueryEmailAddresses( SopranoLive::LiveNodes queryEmailAddresses,
                                                              QList<QContact>& contacts,
                                                              bool affiliationEmails)
{
    Q_ASSERT_X( queryEmailAddressNodesReady==2, Q_FUNC_INFO, "Email query was supposed to be ready and it is not." );
    for(int i = 0; i < queryEmailAddresses->rowCount(); i++)
    {
        qDebug()<< Q_FUNC_INFO << i
                << queryEmailAddresses->rowCount()
                << queryEmailAddresses->columnCount()
                << queryEmailAddresses->index(i, 0).data().toString()
                << queryEmailAddresses->index(i, 1).data().toString()
                << queryEmailAddresses->index(i, 2).data().toString();
        // ignore if next one is the same - asked iridian about making query to ignore supertypes
        // TODO remove after his answer
        if( i-1 >= 0
            && queryEmailAddresses->index(i, 0).data().toString() == queryEmailAddresses->index(i-1, 0).data().toString()
            && queryEmailAddresses->index(i, 1).data().toString() == queryEmailAddresses->index(i-1, 1).data().toString())
        {
            continue; // this is for ignoring duplicates. bad approach, asked iridian about how to eliminate super types in query results
        }

        //QString subtype = rdfPhoneType2QContactSubtype(queryEmailAddresses->index(i, 2).data().toString());
        QUniqueId contactid = queryEmailAddresses->index(i, 0).data().toUInt();
        // TODO replace this lookup with something faster
        for( int j = 0; j < contacts.size(); j++)
        {
            if( contacts[j].id() == contactid )
            {
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

void QTrackerContactAsyncRequest::processQueryIMAccounts(SopranoLive::LiveNodes queryIMAccounts,
                                                         QList<QContact>& contacts,
                                                         bool affiliationAccounts )
{
    Q_ASSERT_X( queryIMAccountNodesReady==2, Q_FUNC_INFO, "IMAccount query was supposed to be ready and it is not." );
    for(int i = 0; i < queryIMAccounts->rowCount(); i++)
    {   //contactid, imid, status, message, nick, type

        qDebug()<<Q_FUNC_INFO<<queryIMAccounts->index(i, 0).data().toString()<<queryIMAccounts->index(i, 1).data().toString()<<queryIMAccounts->index(i, 2).data().toString()<<queryIMAccounts->index(i, 3).data().toString();

        QUniqueId contactid = queryIMAccounts->index(i, 0).data().toUInt();
        // TODO replace this lookup with something faster
        for( int j = 0; j < contacts.size(); j++)
        {
            if( contacts[j].id() == contactid )
            {
                // TODo replace when QtMobility guys implement support for IMAccount
                QContactOnlineAccount account;
                if( affiliationAccounts )
                    account.setContexts(QContactOnlineAccount::ContextWork);
                account.setValue("Account", queryIMAccounts->index(i, 1).data().toString()); // IMId
                account.setValue("AccountPath", queryIMAccounts->index(i, 5).data().toString()); // getImAccountType?
                account.setValue("Capabilities", queryIMAccounts->index(i, 6).data().toString()); // getImAccountType?
                contacts[j].saveDetail(&account);
                contacts[j].saveDetail(&account);
                QContactPresence presence;
                if( affiliationAccounts )
                    presence.setContexts(QContactPresence::ContextWork);
                presence.setValue(QContactPresence::FieldNickname, queryIMAccounts->index(i, 4).data().toString()); // nick
                presence.setValue(QContactPresence::FieldPresence, queryIMAccounts->index(i, 2).data().toString()); // imStatus
                presence.setValue(QContactPresence::FieldStatusMessage, queryIMAccounts->index(i, 3).data().toString()); // imStatusMessage
                contacts[j].saveDetail(&presence);
                break;
            }
        }
    }
}
