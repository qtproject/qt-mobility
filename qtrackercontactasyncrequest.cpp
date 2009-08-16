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

            QList<QUniqueId> ids;
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
