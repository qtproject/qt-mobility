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

#include "qcontact.h"
#include "qcontactdetails.h"
#include "qcontactrequests.h"
#include "qtrackercontactasyncrequest.h"
using namespace SopranoLive;

QTrackerContactAsyncRequest::QTrackerContactAsyncRequest(
        QContactAbstractRequest* request, QContactManagerEngine* parent) :
    QObject(parent)
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
            QContactFilter filter = r->filter();
            QList<QContactSortOrder> sorting = r->sorting();
            QStringList fetchOnlyDetails = r->definitionRestrictions();
            Q_UNUSED(filter)
            Q_UNUSED(sorting)
            Q_UNUSED(fetchOnlyDetails)

            QList<QUniqueId> ids;
            RDFVariable RDFContact =
                    RDFVariable::fromType<nco::PersonContact>();
            RDFSelect quer;

/*
 *  // TODO use this to get one phone number, TODO check for querying all phone numbers
            query.addColumn("Phone", contact.optional().property<nco::hasPhoneNumber>().property<nco::phoneNumber>());
            query.addColumn("Email", contact.optional().property<nco::emailAddress>().property<nco::emailAddress>());
*/
            quer.addColumn("contact_uri", RDFContact);
            quer.addColumn("contactId",
                    RDFContact.property<nco::contactUID> ());
            quer.addColumn("firstname",
                    RDFContact.optional().property<nco::nameGiven>());
            quer.addColumn("secondname",
                    RDFContact.optional().property<nco::nameFamily> ());
            quer.addColumn("photo",
                    RDFContact.optional().property<nco::photo> ());

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
    // fastest way to get this working. refactor
    QContactManagerEngine *engine = qobject_cast<QContactManagerEngine *> (
            parent());
    QList<QContact> result;
    for(int i = 0; i < query->rowCount(); i++)
    {
        QContact contact;
        contact.setId(query->index(i, 1).data().toUInt());
        QContactName name;
        name.setFirst(query->index(i, 2).data().toString());
        name.setLast(query->index(i, 3).data().toString());
        contact.saveDetail(&name);

        QContactAvatar avatar;
        avatar.setAvatar(query->index(i, 4).data().toString());
        contact.saveDetail(&avatar);

        result.append(contact);
    }


    if (engine)
        engine->updateRequest(req, result, QContactManager::NoError, QList<
                QContactManager::Error> (), QContactAbstractRequest::Finished,
                true);
}
