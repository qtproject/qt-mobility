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
                // prepare query to get all phone numbers
                RDFVariable rdfcontact1 = RDFVariable::fromType<nco::PersonContact>();
                applyFilterToRDFVariable(rdfcontact1,r->filter());
                // criteria - only those with phone numbers
                RDFVariable phone = rdfcontact1.property<nco::hasPhoneNumber>();
                RDFVariable type = phone.type();
                type.property<rdfs::subClassOf>() = nco::PhoneNumber::iri(); // sparql cannot handle exact type but returns all super types as junk rows

                // columns
                RDFSelect queryidsnumbers;
                queryidsnumbers.addColumn("contactId", rdfcontact1.property<nco::contactUID> ());
                queryidsnumbers.addColumn("phoneno", phone.property<nco::phoneNumber> ());
                // rdfcontact1.property<nco::hasPhoneNumber> ().isOfType( nco::PhoneNumber::iri(), true);
                queryidsnumbers.addColumn("type", type);

                queryPhoneNumbersNodes = ::tracker()->modelQuery(queryidsnumbers);
                // need to store LiveNodes in order to receive notification from model
                QObject::connect(queryPhoneNumbersNodes.model(), SIGNAL(modelUpdated()), this,
                        SLOT(phoneNumbersReady()));
            }

            if (r->definitionRestrictions().contains(QContactPresence::DefinitionName)
                    || r->definitionRestrictions().contains(QContactOnlineAccount::DefinitionName))
            {
                // prepare query to get all im accounts
                RDFVariable rdfcontact1 = RDFVariable::fromType<nco::PersonContact>();
                applyFilterToRDFVariable(rdfcontact1,r->filter());
                // criteria - only those with im accounts
                rdfcontact1.property<nco::hasIMAccount> ();
                // columns
                RDFSelect queryidsimacccounts;
                queryidsimacccounts.addColumn("contactId", rdfcontact1.property<nco::contactUID> ());

                queryidsimacccounts.addColumn("IMId", rdfcontact1.property<nco::hasIMAccount>().property<nco::imID> ());
                queryidsimacccounts.addColumn("status", rdfcontact1.property<nco::hasIMAccount>().optional().property<nco::imStatus> ());
                queryidsimacccounts.addColumn("message", rdfcontact1.property<nco::hasIMAccount> ().optional().property<nco::imStatusMessage> ());
                queryidsimacccounts.addColumn("nick", rdfcontact1.property<nco::hasIMAccount> ().optional().property<nco::imNickname> ());
                queryidsimacccounts.addColumn("type", rdfcontact1.property<nco::hasIMAccount> ().optional().property<nco::imAccountType> ());
                queryIMAccountNodes = ::tracker()->modelQuery(queryidsimacccounts);
                QObject::connect(queryIMAccountNodes.model(),SIGNAL(modelUpdated()),SLOT(iMAcountsReady()));
            }


            QList<QUniqueId> ids;
            RDFVariable RDFContact1 = RDFVariable::fromType<nco::PersonContact>();
            applyFilterToRDFVariable(RDFContact1, r->filter());
            RDFSelect quer;
            quer.addColumn("contactId",  RDFContact1.property<nco::contactUID> ());
            quer.addColumn("firstname",  RDFContact1.optional().property<nco::nameGiven>());
            quer.addColumn("secondname", RDFContact1.optional().property<nco::nameFamily> ());
            quer.addColumn("photo",      RDFContact1.optional().property<nco::photo> ());


            if (r->definitionRestrictions().contains(QContactEmailAddress::DefinitionName))
            {
                // constraints for start: reading only one of items
                quer.addColumn("email", RDFContact.optional().property<nco::hasEmailAddress>().property<nco::emailAddress>());
            }

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
        contact.saveDetail(&avatar);

        if (request->definitionRestrictions().contains(QContactEmailAddress::DefinitionName))
        {
            QContactEmailAddress mail; // constraint here for start only one email
            mail.setEmailAddress(query->index(i, column++).data().toString());
            contact.saveDetail(&mail);
        }
        result.append(contact);
    }
    if(request->definitionRestrictions().contains( QContactPhoneNumber::DefinitionName ))
    {
        processQueryPhoneNumbers(queryPhoneNumbersNodes, result);
    }
    if (request->definitionRestrictions().contains(QContactPresence::DefinitionName)
            || request->definitionRestrictions().contains(QContactOnlineAccount::DefinitionName))
    {
        processQueryIMAccounts(queryIMAccountNodes, result);
    }

    if (engine)
        engine->updateRequest(req, result, QContactManager::NoError, QList<
                QContactManager::Error> (), QContactAbstractRequest::Finished,
                true);
}

void QTrackerContactAsyncRequest::phoneNumbersReady()
{
    queryPhoneNumbersNodesReady = true;
}

void QTrackerContactAsyncRequest::iMAcountsReady()
{
    queryIMAccountNodesReady = true;
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

void QTrackerContactAsyncRequest::processQueryPhoneNumbers(SopranoLive::LiveNodes queryPhoneNumbers, QList<QContact>& contacts)
{
    Q_ASSERT_X( queryPhoneNumbersNodesReady, Q_FUNC_INFO, "Phonenumbers query was supposed to be ready and it is not." );
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
                number.setNumber(queryPhoneNumbers->index(i, 1).data().toString());
                number.setSubTypeAttribute(subtype);
                contacts[j].saveDetail(&number);
                break;
            }
        }
    }
}

void QTrackerContactAsyncRequest::processQueryIMAccounts(SopranoLive::LiveNodes queryIMAccounts, QList<QContact>& contacts)
{
    Q_ASSERT_X( queryIMAccountNodesReady, Q_FUNC_INFO, "IMAccount query was supposed to be ready and it is not." );
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
                QContactOnlineAccount account;;
                account.setValue("Account", queryIMAccounts->index(i, 1).data().toString()); // IMId
                account.setValue("ServiceName", queryIMAccounts->index(i, 5).data().toString()); // getImAccountType?
                contacts[j].saveDetail(&account);
                QContactPresence presence;
                presence.setValue(QContactPresence::FieldNickname, queryIMAccounts->index(i, 4).data().toString()); // nick
                presence.setValue(QContactPresence::FieldPresence, queryIMAccounts->index(i, 2).data().toString()); // imStatus
                presence.setValue(QContactPresence::FieldStatusMessage, queryIMAccounts->index(i, 3).data().toString()); // imStatusMessage
                contacts[j].saveDetail(&presence);
                break;
            }
        }
    }
}
