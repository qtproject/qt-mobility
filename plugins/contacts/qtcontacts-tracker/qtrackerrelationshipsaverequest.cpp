/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QHash>
using namespace SopranoLive;

QTrackerRelationshipSaveRequest::QTrackerRelationshipSaveRequest(QContactAbstractRequest* req, QContactManagerEngine* parent)
: QObject(parent), QTrackerContactAsyncRequest(req)
{
    Q_ASSERT(req);
    Q_ASSERT(req->type() == QContactAbstractRequest::RelationshipSaveRequest);

    QContactRelationshipSaveRequest* r = qobject_cast<QContactRelationshipSaveRequest*>(req);
    if (!r)
    {
        commitError(QString());
    }

    QList<QContactRelationship> links = r->relationships();
    if(links.isEmpty()) {
        QMap<int, QContactManager::Error> errors;
        errors[0] = QContactManager::BadArgumentError;
        QContactManagerEngine::updateRelationshipSaveRequest(r, links, QContactManager::BadArgumentError, errors, QContactAbstractRequest::FinishedState);
        return;
    }
    QContactManagerEngine::updateRequestState(req, QContactAbstractRequest::ActiveState);


    // the logic is like this
    // 1) get contacts (first() and second())
    // 2) when that is done set all metacontacts of 1sts to 2nds

    QSet<QString> ids;
    foreach(QContactRelationship rel, links)
    {
        ids << QString::number(rel.first().localId());
        ids << QString::number(rel.second().localId());
    }

    RDFVariable contact;
    QStringList idstrings(QStringList(ids.toList()));
    contact.property<nco::contactUID>().isMemberOf(idstrings);
    m_nodes = ::tracker()->modelVariable(contact);

    // need to store LiveNodes in order to receive notification from model
    QObject::connect(m_nodes.model(), SIGNAL(modelUpdated()), this, SLOT(nodesDataReady()));

}

void QTrackerRelationshipSaveRequest::nodesDataReady()
{
    // now that nodes are ready we can enumerate them without blocking
    RDFTransactionPtr transaction_ = ::tracker()->initiateTransaction();
    connect(transaction_.data(), SIGNAL(commitFinished()), this, SLOT(commitFinished()));
    connect(transaction_.data(), SIGNAL(commitError(QString)), this, SLOT(commitError(QString)));

    QHash<QString, Live<nco::PersonContact> > lContacts;
    for(int i = 0; i < m_nodes->rowCount(); i++)
    {
        Live<nco::PersonContact> contact = m_nodes->liveNode(i);
        lContacts[contact->getContactUID()] = contact;
    }
    QContactRelationshipSaveRequest* r = qobject_cast<QContactRelationshipSaveRequest*>(req);
    if (!r)
        commitError(QString());
    QList<QContactRelationship> links = r->relationships();
    foreach(QContactRelationship rel, links)
    {
        Live<nco::PersonContact> first = lContacts.value(QString::number(rel.first().localId()));
        Live<nco::PersonContact> second = lContacts.value(QString::number(rel.second().localId()));
        //TODO: we should prefer the local contact information over the remote info.
        mergeContacts(first, second);
    }

    transaction_->commit(false);
    // temporary fix - signals not yet implemented in libqttracker
    commitFinished();
}

void QTrackerRelationshipSaveRequest::mergeContacts(const Live<nco::PersonContact>& preferedContact, const Live<nco::PersonContact>& inferiorContact)
{
   QList<LiveNode> mergedNodes = preferedContact->getHasIMAddresss();
   mergedNodes += inferiorContact->getHasIMAddresss();
   preferedContact->setHasIMAddresss( mergedNodes );
   inferiorContact->remove();
}

void QTrackerRelationshipSaveRequest::commitFinished()
{
    QContactRelationshipSaveRequest* r = qobject_cast<QContactRelationshipSaveRequest*>(req);
    if (r && r->isActive())
    {
        QContactManager::Error error = QContactManager::NoError;
        QMap<int, QContactManager::Error> errors;
        errors[0] = error;
        QContactManagerEngine::updateRelationshipSaveRequest(r, r->relationships(), error, errors, QContactAbstractRequest::FinishedState);
    }
    else
        qWarning()<<Q_FUNC_INFO<<r;
}

void QTrackerRelationshipSaveRequest::commitError(QString message)
{
    qWarning()<<Q_FUNC_INFO<<message;
    QContactRelationshipSaveRequest* r = qobject_cast<QContactRelationshipSaveRequest*>(req);
    if (r)
    {
        QContactManager::Error error = QContactManager::InvalidRelationshipError;
        QMap<int, QContactManager::Error> errors; 
        errors[0] = error;
        QContactManagerEngine::updateRelationshipSaveRequest(r, r->relationships(), error, errors, QContactAbstractRequest::FinishedState);
    }
    else
    {
        QMap<int, QContactManager::Error> errors;
        errors[0] = QContactManager::UnspecifiedError;
        QContactManagerEngine::updateRelationshipSaveRequest(r, QList<QContactRelationship>(), QContactManager::UnspecifiedError, errors, QContactAbstractRequest::FinishedState);
        return;
    }
}

