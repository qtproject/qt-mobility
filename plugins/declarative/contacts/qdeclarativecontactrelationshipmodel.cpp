/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativecontactrelationshipmodel_p.h"
#include "qcontactmanager.h"
#include "qcontactrequests.h"


class QDeclarativeContactRelationshipModelPrivate
{
public:
    QDeclarativeContactRelationshipModelPrivate()
        :m_manager(0),
        m_participantId(0),
        m_role(QDeclarativeContactRelationship::Either)
    {
    }

    ~QDeclarativeContactRelationshipModelPrivate()
    {
        delete m_manager;
    }

    QContactManager* m_manager;
    QString m_relationshipType;
    int m_participantId;
    QDeclarativeContactRelationship::Role m_role;
    QList<QDeclarativeContactRelationship*> m_relationships;
    QContactRelationshipFetchRequest m_fetchRequest;
    QContactRelationshipRemoveRequest m_removeRequest;
    QContactRelationshipSaveRequest m_saveRequest;
};

QDeclarativeContactRelationshipModel::QDeclarativeContactRelationshipModel(QObject *parent)
    :QAbstractListModel(parent),
    d(new QDeclarativeContactRelationshipModelPrivate)
{
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(RelationshipRole, "relationship");
    setRoleNames(roleNames);


    connect(this, SIGNAL(managerChanged()), SLOT(fetchAgain()));
    connect(this, SIGNAL(participantIdChanged()), SLOT(fetchAgain()));
    connect(this, SIGNAL(relationshipTypeChanged()), SLOT(fetchAgain()));
    connect(this, SIGNAL(roleChanged()), SLOT(fetchAgain()));

    connect(d->m_manager,SIGNAL(relationshipsAdded(const QList<QContactLocalId>&)), this, SLOT(fetchAgain()));
    connect(d->m_manager,SIGNAL(relationshipsRemoved(const QList<QContactLocalId>&)), this, SLOT(fetchAgain()));
    connect(&d->m_removeRequest,SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(relationshipRemoved()));
    connect(&d->m_saveRequest,SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(relationshipSaved()));
    connect(&d->m_fetchRequest,SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(relationshipFetched()));
}

QDeclarativeContactRelationshipModel::~QDeclarativeContactRelationshipModel()
{
    delete d;
}

QString QDeclarativeContactRelationshipModel::manager() const
{
    if (d->m_manager)
        return d->m_manager->managerName();
    return QString();
}
void QDeclarativeContactRelationshipModel::setManager(const QString& manager)
{
    if (d->m_manager == 0 || manager != d->m_manager->managerName() ) {
        d->m_manager = new QContactManager(manager,QMap<QString,QString>(), this);
        emit managerChanged();
    }
}

int QDeclarativeContactRelationshipModel::participantId() const
{
    return d->m_participantId;
}
void QDeclarativeContactRelationshipModel::setParticipantId(int id)
{
    if (d->m_participantId != id) {
        d->m_participantId = id;
        emit participantIdChanged();
    }
}

QString QDeclarativeContactRelationshipModel::relationshipType() const
{
    return d->m_relationshipType;
}
void QDeclarativeContactRelationshipModel::setRelationshipType(const QString& type)
{
    if (d->m_relationshipType != type) {
        d->m_relationshipType = type;
        emit relationshipTypeChanged();
    }
}

QDeclarativeContactRelationship::Role QDeclarativeContactRelationshipModel::role() const
{
    return d->m_role;
}
void QDeclarativeContactRelationshipModel::setRole(QDeclarativeContactRelationship::Role role)
{
    if (d->m_role != role) {
        d->m_role = role;
        emit roleChanged();
    }
}


int QDeclarativeContactRelationshipModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return d->m_relationships.count();
}

QVariant QDeclarativeContactRelationshipModel::data(const QModelIndex &index, int role) const
{
    QDeclarativeContactRelationship* dcr = d->m_relationships.value(index.row());
    if (role == RelationshipRole) {
        return QVariant::fromValue(dcr);
    }
    return QVariant();
}

QDeclarativeListProperty<QDeclarativeContactRelationship> QDeclarativeContactRelationshipModel::relationships()
{
    return QDeclarativeListProperty<QDeclarativeContactRelationship>(this, d->m_relationships);
}

void QDeclarativeContactRelationshipModel::fetchAgain()
{
    if (d->m_manager) {
        //make sure the request is not running
        d->m_fetchRequest.waitForFinished();

        d->m_fetchRequest.setManager(d->m_manager);

        QContactId id;
        id.setManagerUri(d->m_manager->managerUri());
        id.setLocalId(d->m_participantId);
        if (d->m_role == QDeclarativeContactRelationship::First || d->m_role == QDeclarativeContactRelationship::Either)
            d->m_fetchRequest.setFirst(id);

        if (d->m_role == QDeclarativeContactRelationship::Second || d->m_role == QDeclarativeContactRelationship::Either)
            d->m_fetchRequest.setSecond(id);

        d->m_fetchRequest.setRelationshipType(d->m_relationshipType);

        d->m_fetchRequest.start();
    }
}

void QDeclarativeContactRelationshipModel::removeRelationship(QDeclarativeContactRelationship* dcr)
{
    if (dcr) {
        QContactRelationship cr = dcr->relationship();
        d->m_removeRequest.setManager(d->m_manager);
        d->m_removeRequest.setRelationship(cr);

        //make sure the request is not running
        d->m_removeRequest.waitForFinished();

        d->m_removeRequest.start();
    }
}

void QDeclarativeContactRelationshipModel::relationshipFetched()
{
    if (d->m_fetchRequest.isFinished() && d->m_fetchRequest.error() == QContactManager::NoError) {

        foreach(QDeclarativeContactRelationship* dcr, d->m_relationships) {
            dcr->deleteLater();
        }
        d->m_relationships.clear();

        foreach (const QContactRelationship& cr, d->m_fetchRequest.relationships()) {
            QDeclarativeContactRelationship* dcr = new QDeclarativeContactRelationship(this);
            dcr->setRelationship(cr);
            connect(dcr, SIGNAL(valueChanged()), this, SLOT(saveRelationship()));
            d->m_relationships.append(dcr);;
        }

        emit relationshipsChanged();
    }
}

void QDeclarativeContactRelationshipModel::saveRelationship()
{
    QDeclarativeContactRelationship* dcr = qobject_cast<QDeclarativeContactRelationship*>(QObject::sender());
    if (dcr) {
        QContactRelationship cr = dcr->relationship();
        d->m_saveRequest.setManager(d->m_manager);
        d->m_saveRequest.setRelationship(cr);

        //make sure the request is not running
        d->m_saveRequest.waitForFinished();

        d->m_saveRequest.start();
    }
}

void QDeclarativeContactRelationshipModel::removeRelationship()
{
    QDeclarativeContactRelationship* dcr = qobject_cast<QDeclarativeContactRelationship*>(QObject::sender());
    removeRelationship(dcr);
}


void QDeclarativeContactRelationshipModel::relationshipSaved()
{
    if (d->m_saveRequest.isFinished() && d->m_saveRequest.error() != QContactManager::NoError) {
        fetchAgain();
    }
}

void QDeclarativeContactRelationshipModel::relationshipRemoved()
{
    if (d->m_removeRequest.isFinished() && d->m_removeRequest.error() == QContactManager::NoError) {
        fetchAgain();
    }
}
