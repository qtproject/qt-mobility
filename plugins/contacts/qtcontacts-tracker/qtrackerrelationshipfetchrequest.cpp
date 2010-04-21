/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qtrackerrelationshipfetchrequest.h"

#include <QtTracker/Tracker>
using namespace SopranoLive;


// TODO better error handling when saving
QTrackerRelationshipFetchRequest::QTrackerRelationshipFetchRequest(QContactAbstractRequest* req, QContactManagerEngine* parent)
: QObject(parent), QTrackerContactAsyncRequest(req)
{
    Q_ASSERT(req);
    Q_ASSERT(req->type() == QContactAbstractRequest::RelationshipFetchRequest);
    Q_ASSERT(parent);

    QContactRelationshipFetchRequest* r = qobject_cast<QContactRelationshipFetchRequest*>(req);

    if( !r )
    {
        QContactManagerEngine::updateRelationshipFetchRequest(r, QList<QContactRelationship>(), QContactManager::UnspecifiedError, QContactAbstractRequest::FinishedState);
        return;
    }
    if (not r->relationshipType().isEmpty() && QContactRelationship::IsSameAs != r->relationshipType())
    {
        QContactManagerEngine::updateRelationshipFetchRequest(r, r->relationships(), QContactManager::NotSupportedError, QContactAbstractRequest::FinishedState);
        return;
    }
    QList<QContactManager::Error> dummy;
    QContactManagerEngine::updateRequestState(req, QContactAbstractRequest::ActiveState);

    RDFVariable RDFContact = RDFVariable::fromType<nco::PersonContact>();
    RDFVariable RDFContactThis = RDFVariable::fromType<nco::PersonContact>();

    RDFVariable RDFMetacontact;

    RDFContact.property<nco::metacontact>() = RDFMetacontact;
    RDFContactThis.property<nco::metacontact>() = RDFMetacontact;
    RDFVariable contactIdThis = RDFContactThis.property<nco::contactUID>();

    contactIdThis.equal(LiteralValue(QString::number(r->first().localId())));


    RDFSelect quer;
    quer.addColumn("contact_id", RDFContact.property<nco::contactUID>());
    quer.addColumn("contactthis_id", contactIdThis);
    query = ::tracker()->modelQuery(quer);

    QObject::connect(query.model(), SIGNAL(modelUpdated()), this, SLOT(modelUpdated()));

}

QTrackerRelationshipFetchRequest::~QTrackerRelationshipFetchRequest()
{
}

void QTrackerRelationshipFetchRequest::modelUpdated()
{
    QContactManagerEngine *engine = qobject_cast<QContactManagerEngine *> (parent());
    QContactRelationshipFetchRequest* r = qobject_cast<QContactRelationshipFetchRequest*>(req);
    if( !engine )
    {
        QContactManagerEngine::updateRelationshipFetchRequest(r, QList<QContactRelationship>(), QContactManager::UnspecifiedError, QContactAbstractRequest::FinishedState);
        return;
    }

    QList<QContactRelationship> result;
    for(int i = 0; i < query->rowCount(); i++) {
        bool ok, ok1;
        QContactId idfirst;
        idfirst.setLocalId(query->index(i, 1).data().toUInt(&ok));
        QContactId idsecond;
        idsecond.setLocalId(query->index(i, 0).data().toUInt(&ok1));

        if (ok && ok1 ) {
            if (idfirst.localId() == idsecond.localId())
                continue;
            idfirst.setManagerUri(engine->managerUri());
            idsecond.setManagerUri(idfirst.managerUri());

            QContactRelationship rel;
            rel.setFirst(idfirst);
            rel.setSecond(idsecond);
            rel.setRelationshipType(QContactRelationship::IsSameAs);
            result.append(rel);
        }
    }

    QContactManagerEngine::updateRelationshipFetchRequest(r, result, QContactManager::NoError, QContactAbstractRequest::FinishedState);
}

