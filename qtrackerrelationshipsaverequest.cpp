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

#include "qtrackerrelationshipsaverequest.h"

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nco.h>
using namespace SopranoLive;

QTrackerRelationshipSaveRequest::QTrackerRelationshipSaveRequest(QContactAbstractRequest* req, QContactManagerEngine* parent)
: QObject(parent), QTrackerContactAsyncRequest(req)
{
    Q_ASSERT(req);
    Q_ASSERT(req->type() == QContactAbstractRequest::RelationshipSaveRequest);

    QContactRelationshipSaveRequest* r = qobject_cast<QContactRelationshipSaveRequest*>(req);
    if (!r)
    {
        QList<QContactManager::Error> errors(QList<QContactManager::Error>()<<QContactManager::UnspecifiedError);
        QContactManagerEngine::updateRequest(req, QList<QContactRelationship>(), QContactManager::UnspecifiedError, errors, QContactAbstractRequest::Finished);
        return;
    }

    QList<QContactRelationship> links = r->relationships();
    if(links.isEmpty()) {
        QList<QContactManager::Error> errors(QList<QContactManager::Error>()<<QContactManager::BadArgumentError);
        QContactManagerEngine::updateRequest(req, links, QContactManager::BadArgumentError, errors, QContactAbstractRequest::Finished);
        return;
    }
    QList<QContactManager::Error> dummy;
    QContactManagerEngine::updateRequestStatus(req, QContactManager::NoError, dummy,
            QContactAbstractRequest::Active);

    RDFTransactionPtr transaction_ = ::tracker()->initiateTransaction();
    foreach(QContactRelationship rel, links)
    {
        saveRelationship(rel, transaction_->service());
    }
    transaction_->commit();
}

void QTrackerRelationshipSaveRequest::saveRelationship(const QContactRelationship &rel,RDFServicePtr service)
{
    // 1) create metacontact if it doesnt exist and add it to first
    {
        RDFVariable contactFirst;
        contactFirst.property<nco::contactUID> () = LiteralValue(QString::number(rel.first().localId()));

        RDFUpdate up;

        // here we will specify to add new node for metacontact in 1st node
        RDFVariable metacontact = contactFirst.optional().property<nco::metacontact> ();
        RDFFilter doesntExist = metacontact.isBound().not_();// do not create if it already exist
        QUrl newmetacontact = service->createLiveNode().uri();

        QList<RDFVariableStatement> insertions;
        insertions << RDFVariableStatement(contactFirst, nco::metacontact::iri(), newmetacontact)
        << RDFVariableStatement(newmetacontact, rdf::type::iri(), nco::MetaContact::iri());

        // this means that filter applies to both insertions
        up.addInsertion(insertions);
        service->executeQuery(up);
    }
    // 2) use first's metacontact and set it to second
    {
        RDFVariable contactSecond;
        contactSecond.property<nco::contactUID> () = LiteralValue(QString::number(rel.second().localId()));
        RDFVariable contactFirst;
        contactFirst.property<nco::contactUID> () = LiteralValue(QString::number(rel.first().localId()));
        RDFVariable metaOfFirst = contactFirst.property<nco::metacontact>();

        RDFUpdate up;

        RDFVariable existingmetacontact = contactSecond.optional().property<nco::metacontact>();
        up.addDeletion(contactSecond,nco::metacontact::iri(), existingmetacontact);
        up.addInsertion(contactSecond, nco::metacontact::iri(), metaOfFirst);
        service->executeQuery(up);
    }
}


