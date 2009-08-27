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
    if (filter.type() == QContactFilter::IdList)
    {
        QContactIdListFilter filt = filter;
        // for now only works for one ID, TODO ask iridian how to do it in one query

        if( !filt.ids().isEmpty() )
            variable.property<nco::contactUID>() = LiteralValue(filt.ids()[0]);
        else
            qWarning()<<Q_FUNC_INFO<<"QContactIdListFilter idlist is empty";
    }
}

void addSortingPartToRDFSelectQuery(RDFSelect &query, RDFVariable &contact, QList<QContactSortOrder> sorting)
{
    foreach(QContactSortOrder sort, sorting)
    {
        if( sort.detailDefinitionName() == QContactName::DefinitionName)
        {
            if( sort.detailFieldName() == QContactName::FieldFirst)
            {
                query.orderBy(contact.optional().property<nco::nameGiven>());
            }
            if( sort.detailFieldName() == QContactName::FieldLast)
            {
                query.orderBy(contact.optional().property<nco::nameFamily>());
            }
            else
                qWarning()<<"QTrackerContactAsyncRequest"<<"sorting by"<<sort.detailDefinitionName()<<sort.detailFieldName()<<"is not yet supported";

        }else
            qWarning()<<"QTrackerContactAsyncRequest"<<"sorting by"<<sort.detailDefinitionName()<<"is not yet supported";
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
    type.property<rdfs::subClassOf>() = nco::PhoneNumber::iri(); // sparql cannot handle exact type but returns all super types as junk rows

    // columns
    RDFSelect queryidsnumbers;
    queryidsnumbers.addColumn("contactId", rdfcontact1.property<nco::contactUID> ());
    queryidsnumbers.addColumn("phoneno", phone.property<nco::phoneNumber> ());
    // rdfcontact1.property<nco::hasPhoneNumber> ().isOfType( nco::PhoneNumber::iri(), true);
    queryidsnumbers.addColumn("type", type);
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
    return queryidsimacccounts;
}




QTrackerContactAsyncRequest::QTrackerContactAsyncRequest(
        QContactAbstractRequest* request, QContactManagerEngine* parent) :
    QObject(parent), queryPhoneNumbersNodesReady(false)
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
                queryPhoneNumbersNodes.clear(); queryIMAccountNodesReady = 0;
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
            quer.addColumn("contactId",  RDFContact1.property<nco::contactUID>());
            quer.addColumn("firstname",  RDFContact1.optional().property<nco::nameGiven>());
            quer.addColumn("secondname", RDFContact1.optional().property<nco::nameFamily>());
            quer.addColumn("photo",      RDFContact1.optional().property<nco::photo>());


            // for now adding columns to main query. later separate queries
            if (r->definitionRestrictions().contains(QContactEmailAddress::DefinitionName))
            {
                quer.addColumn("email", RDFContact.optional().property<nco::hasEmailAddress>().property<nco::emailAddress>());
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
            if (r->definitionRestrictions().contains(QContactAddress::DefinitionName))
            {
                RDFVariable address = RDFContact.optional().property<nco::addressLocation>();
                quer.addColumn("street", address.optional().property<nco::streetAddress>());
                quer.addColumn("city", address.optional().property<nco::locality>());
                quer.addColumn("country", address.optional().property<nco::country>());
                quer.addColumn("pcode", address.optional().property<nco::postalcode>());
                quer.addColumn("reg", address.optional().property<nco::region>());
            }

            // QContactAnniversary - no such thing in tracker
            // QContactGeolocation - nco:hasLocation is not having class defined in nco yet. no properties. maybe rdfs:Resource:label


            // supporting sorting only here, difficult and no requirements in UI for sorting in multivalue details (phones, emails)
            addSortingPartToRDFSelectQuery( quer, RDFContact1, r->sorting());

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
        result.append(query->index(i, 1).data().toUInt());
    }

    if (engine)
        engine->updateRequest(req, result, QContactManager::NoError, QList<
                QContactManager::Error> (), QContactAbstractRequest::Finished,
                false);
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
    for(int i = 0; i < query->rowCount(); i++)
    {
        int column = 0;
        QContact contact;
        contact.setId(query->index(i, column++).data().toUInt());
        QContactName name;
        name.setFirst(query->index(i, column++).data().toString());
        name.setLast(query->index(i, column++).data().toString());
        contact.saveDetail(&name);

        QContactAvatar avatar;
        avatar.setAvatar(query->index(i, column++).data().toString());
        if( avatar.avatar().isEmpty() )
            contact.saveDetail(&avatar);

        // TODO extract generic from bellow ... mapping field names
        if (request->definitionRestrictions().contains(QContactEmailAddress::DefinitionName))
        {
            // no office mails yet
            QContactEmailAddress mail; // constraint here for start only one email
            mail.setEmailAddress(query->index(i, column++).data().toString());
            if( mail.emailAddress().isEmpty() )
                contact.saveDetail(&mail);
        }
        if (request->definitionRestrictions().contains(QContactUrl::DefinitionName))
        {
            QContactUrl url;
            url.setUrl(query->index(i, column++).data().toString());
            if(!url.url().isEmpty())
                contact.saveDetail(&url);
        }
        if (request->definitionRestrictions().contains(QContactBirthday::DefinitionName))
        {
            QVariant var = query->index(i, column++).data();
            if( !var.toString().isEmpty() /* enable reading wrong && var.toDate().isValid()*/)
            {
                QContactBirthday birth;
                birth.setDate(var.toDate());
                contact.saveDetail(&birth);
            }
        }
        if (request->definitionRestrictions().contains(QContactGender::DefinitionName))
        {
            QString var = query->index(i, column++).data().toString();
            if( !var.isEmpty() )
            {
                QContactGender g;
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
                contact.saveDetail(&o);
            }
        }
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
                QContactAddress a;
                a.setStreet(street);
                a.setLocality(loc);
                a.setCountry(country);
                a.setPostcode(pcode);
                a.setRegion(region);
            }
        }
        result.append(contact);
    }
    if(request->definitionRestrictions().contains( QContactPhoneNumber::DefinitionName ))
    {
        Q_ASSERT(queryPhoneNumbersNodes.size() == 2);
        for( int cnt = 0; cnt < queryPhoneNumbersNodes.size(); cnt++)
            processQueryPhoneNumbers(queryPhoneNumbersNodes[cnt], result, cnt);
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
        return QContactPhoneNumber::AttributeSubTypeVoice;
    else if ( rdfPhoneType.endsWith("CarPhoneNumber") )
        return QContactPhoneNumber::AttributeSubTypeCar;
    else if ( rdfPhoneType.endsWith("CellPhoneNumber") )
        return QContactPhoneNumber::AttributeSubTypeMobile;
    else if ( rdfPhoneType.endsWith("BbsPhoneNumber") )
        return QContactPhoneNumber::AttributeSubTypeBulletinBoardSystem;
    else if ( rdfPhoneType.endsWith("FaxNumber") )
        return QContactPhoneNumber::AttributeSubTypeFacsimile;
    else if ( rdfPhoneType.endsWith("ModemNumber") )
        return QContactPhoneNumber::AttributeSubTypeModem;
    else if ( rdfPhoneType.endsWith("PagerNumber") )
        return QContactPhoneNumber::AttributeSubTypePager;
    else if ( rdfPhoneType.endsWith("MessagingNumber") )
        return QContactPhoneNumber::AttributeSubTypeMessagingCapable;
    else
        qWarning()<<Q_FUNC_INFO<<"Not handled phone number type:"<<rdfPhoneType;
    return "";
}

void QTrackerContactAsyncRequest::processQueryPhoneNumbers(SopranoLive::LiveNodes queryPhoneNumbers, QList<QContact>& contacts, bool officeStuff)
{
    Q_ASSERT_X( queryPhoneNumbersNodesReady==2, Q_FUNC_INFO, "Phonenumbers query was supposed to be ready and it is not." );
    for(int i = 0; i < queryPhoneNumbers->rowCount(); i++)
    {
        qDebug()<<Q_FUNC_INFO<<i<<queryPhoneNumbers->rowCount()<<queryPhoneNumbers->columnCount()<<queryPhoneNumbers->index(i, 0).data().toString()<<queryPhoneNumbers->index(i, 1).data().toString()<<queryPhoneNumbers->index(i, 2).data().toString();
        // ignore if next one is the same - asked iridian about making query to ignore supertypes
        // TODO remove after his answer
        if( i+1 < queryPhoneNumbers->rowCount()
            && queryPhoneNumbers->index(i, 0).data().toString() == queryPhoneNumbers->index(i+1, 0).data().toString()
            && queryPhoneNumbers->index(i, 1).data().toString() == queryPhoneNumbers->index(i+1, 1).data().toString())
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
                if( officeStuff )
                    number.setContextAttribute(QContactPhoneNumber::AttributeContextWork);
                number.setNumber(queryPhoneNumbers->index(i, 1).data().toString());
                number.setSubTypeAttribute(subtype);
                contacts[j].saveDetail(&number);
                break;
            }
        }
    }
}

void QTrackerContactAsyncRequest::processQueryIMAccounts(SopranoLive::LiveNodes queryIMAccounts, QList<QContact>& contacts, bool officeStuff)
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
                if( officeStuff )
                    account.setContextAttribute(QContactOnlineAccount::AttributeContextWork);
                account.setValue("Account", queryIMAccounts->index(i, 1).data().toString()); // IMId
                account.setValue("ServiceName", queryIMAccounts->index(i, 5).data().toString()); // getImAccountType?
                contacts[j].saveDetail(&account);
                QContactPresence presence;
                if( officeStuff )
                    presence.setContextAttribute(QContactPresence::AttributeContextWork);
                presence.setValue(QContactPresence::FieldNickname, queryIMAccounts->index(i, 4).data().toString()); // nick
                presence.setValue(QContactPresence::FieldPresence, queryIMAccounts->index(i, 2).data().toString()); // imStatus
                presence.setValue(QContactPresence::FieldStatusMessage, queryIMAccounts->index(i, 3).data().toString()); // imStatusMessage
                contacts[j].saveDetail(&presence);
                break;
            }
        }
    }
}
