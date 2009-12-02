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


#include <qtrackercontactidfetchrequest.h>

#include <qtcontacts.h>

#include <QtTracker/ontologies/nie.h>
#include <QtTracker/ontologies/nco.h>
using namespace SopranoLive;

QTrackerContactIdFetchRequest::QTrackerContactIdFetchRequest(QContactAbstractRequest* request,
                                                             QContactManagerEngine* parent) :
    QTrackerContactFetchRequest(request, parent)
{
}

void QTrackerContactIdFetchRequest::run()
{
    Q_ASSERT( req->type() == QContactAbstractRequest::ContactLocalIdFetchRequest );
    QContactLocalIdFetchRequest* r = qobject_cast<QContactLocalIdFetchRequest*> (req);
    if (!r) {
        QList<QContactManager::Error> dummy;
        QContactManagerEngine::updateRequestStatus(req, QContactManager::UnspecifiedError, dummy,
                                    QContactAbstractRequest::Finished);
        return;
    }

    QContactFilter filter = r->filter();
    QList<QContactSortOrder> sorting = r->sorting();
    Q_UNUSED(filter)
    Q_UNUSED(sorting)

    RDFVariable RDFContact = RDFVariable::fromType<nco::PersonContact>();
    applyFilterToContact(RDFContact, r->filter());

    RDFSelect quer;
    // only one column - id
    quer.addColumn("contactId", RDFContact.property<nco::contactUID> ());
    query = ::tracker()->modelQuery(quer);
    // need to store LiveNodes in order to receive notification from model
    QObject::connect(query.model(), SIGNAL(modelUpdated()), this, SLOT(modelUpdated()));
}


void QTrackerContactIdFetchRequest::modelUpdated()
{
    QSet<QContactLocalId> result;
    // for now this only serves get all contacts
    for(int i = 0; i < query->rowCount(); i++) {
        bool ok;
        // only one column - id \sa run()
        QContactLocalId id = query->index(i, 0).data().toUInt(&ok);
        if (ok) {
            // Append only, if we have a valid contact id
            // and it's not already added
            result.insert(id);
        }
    }

    QContactManagerEngine::updateRequest(req, result.toList(), QContactManager::NoError,
                              QList<QContactManager::Error> (),
                              QContactAbstractRequest::Finished,
                              false);
}

