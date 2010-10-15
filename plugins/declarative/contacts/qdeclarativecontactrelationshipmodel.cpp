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

#include <QDebug>
#include <QPixmap>

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
    QDeclarativeContactRelationship m_relationshipTypeHolder;
    QContactLocalId m_participantId;
    QDeclarativeContactRelationship::RelationshipRole m_role;
    QList<QContactRelationship> m_relationships;
    QList<QDeclarativeContactRelationship*> m_declarativeRelationships;
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
QString QDeclarativeContactRelationshipModel::error() const
{
    switch (d->m_manager->error()) {
    case QContactManager::DoesNotExistError:
        return QLatin1String("Not exist");
    case QContactManager::AlreadyExistsError:
        return QLatin1String("Already exist");
    case QContactManager::InvalidDetailError:
        return QLatin1String("Invalid detail");
    case QContactManager::InvalidRelationshipError:
        return QLatin1String("Invalid relationship");
    case QContactManager::LockedError:
        return QLatin1String("Locked error");
    case QContactManager::DetailAccessError:
        return QLatin1String("Detail access error");
    case QContactManager::PermissionsError:
        return QLatin1String("Permissions error");
    case QContactManager::OutOfMemoryError:
        return QLatin1String("Out of memory");
    case QContactManager::NotSupportedError:
        return QLatin1String("Not supported");
    case QContactManager::BadArgumentError:
        return QLatin1String("Bad argument");
    case QContactManager::UnspecifiedError:
        return QLatin1String("Unspecified error");
    case QContactManager::VersionMismatchError:
        return QLatin1String("Version mismatch");
    case QContactManager::LimitReachedError:
        return QLatin1String("Limit reached");
    case QContactManager::InvalidContactTypeError:
        return QLatin1String("Invalid contact type");
    default:
        break;
    }
    return QLatin1String("Status ok");
}
void QDeclarativeContactRelationshipModel::setManager(const QString& manager)
{
    if (d->m_manager == 0 || manager != d->m_manager->managerName() ) {
        d->m_manager = new QContactManager(manager,QMap<QString,QString>(), this);
        connect(d->m_manager,SIGNAL(relationshipsAdded(const QList<QContactLocalId>&)), this, SLOT(fetchAgain()));
        connect(d->m_manager,SIGNAL(relationshipsRemoved(const QList<QContactLocalId>&)), this, SLOT(fetchAgain()));
        emit managerChanged();
    }
}

QContactLocalId QDeclarativeContactRelationshipModel::participantId() const
{
    return d->m_participantId;
}
void QDeclarativeContactRelationshipModel::setParticipantId(const QContactLocalId& id)
{
    if (d->m_participantId != id) {
        d->m_participantId = id;
        emit participantIdChanged();
    }
}

QVariant QDeclarativeContactRelationshipModel::relationshipType() const
{
    return d->m_relationshipTypeHolder.relationshipType();
}
void QDeclarativeContactRelationshipModel::setRelationshipType(const QVariant& type)
{
    if (type != relationshipType()) {
        d->m_relationshipTypeHolder.setRelationshipType(type);
        emit relationshipTypeChanged();
    }
}

QDeclarativeContactRelationship::RelationshipRole QDeclarativeContactRelationshipModel::role() const
{
    return d->m_role;
}
void QDeclarativeContactRelationshipModel::setRole(QDeclarativeContactRelationship::RelationshipRole role)
{
    if (d->m_role != role) {
        d->m_role = role;
        emit roleChanged();
    }
}


int QDeclarativeContactRelationshipModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return d->m_declarativeRelationships.count();
}

QVariant QDeclarativeContactRelationshipModel::data(const QModelIndex &index, int role) const
{
    QDeclarativeContactRelationship* dcr = d->m_declarativeRelationships.value(index.row());
    if (role == RelationshipRole) {
        return QVariant::fromValue(dcr);
    } else if (role == Qt::DisplayRole) {
        return QString("%1 %2 %3").arg(dcr->first()).arg(dcr->relationship().relationshipType()).arg(dcr->second());
    } else if (role == Qt::DecorationRole) {
        //show something?
        return QPixmap();
    }
    return QVariant();
}

void QDeclarativeContactRelationshipModel::fetchAgain()
{
    if (d->m_manager) {
        QContactRelationshipFetchRequest* req = new QContactRelationshipFetchRequest(this);
        req->setManager(d->m_manager);

        QContactId id;
        id.setManagerUri(d->m_manager->managerUri());
        id.setLocalId(d->m_participantId);
        if (d->m_role == QDeclarativeContactRelationship::First || d->m_role == QDeclarativeContactRelationship::Either)
            req->setFirst(id);

        if (d->m_role == QDeclarativeContactRelationship::Second || d->m_role == QDeclarativeContactRelationship::Either)
            req->setSecond(id);


        req->setRelationshipType(d->m_relationshipTypeHolder.relationship().relationshipType());
        connect(req,SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(requestUpdated()));

        req->start();
    }
}

void QDeclarativeContactRelationshipModel::addRelationship(QDeclarativeContactRelationship* dcr)
{
    if (dcr) {
        QContactRelationship cr = dcr->relationship();
        QContactRelationshipSaveRequest* req = new QContactRelationshipSaveRequest(this);
        req->setManager(d->m_manager);
        req->setRelationship(cr);
        connect(req, SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(relationshipsSaved()));
        req->start();
    }
}

void QDeclarativeContactRelationshipModel::removeRelationship(QDeclarativeContactRelationship* dcr)
{
    if (dcr) {
        QContactRelationship cr = dcr->relationship();
        QContactRelationshipRemoveRequest* req = new QContactRelationshipRemoveRequest(this);
        req->setManager(d->m_manager);
        req->setRelationship(cr);
        connect(req,SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(relationshipsRemoved()));
        req->start();
    }
}

void QDeclarativeContactRelationshipModel::requestUpdated()
{
    QContactRelationshipFetchRequest* req = qobject_cast<QContactRelationshipFetchRequest*>(sender());

    if (req->isFinished() && req->error() == QContactManager::NoError) {

        QList<QContactRelationship> relationships = req->relationships();

        reset();
        beginInsertRows(QModelIndex(), 0, relationships.count());

        foreach(QDeclarativeContactRelationship* dcr, d->m_declarativeRelationships) {
            dcr->deleteLater();
        }
        d->m_declarativeRelationships.clear();
        d->m_relationships.clear();


        foreach (const QContactRelationship& cr, relationships) {
            QDeclarativeContactRelationship* dcr = new QDeclarativeContactRelationship(this);
            dcr->setRelationship(cr);
            d->m_declarativeRelationships.append(dcr);
            d->m_relationships.append(cr);
        }
        endInsertRows();

        req->deleteLater();
        emit relationshipsChanged();
    }
}

void QDeclarativeContactRelationshipModel::relationshipsSaved()
{
    QContactRelationshipSaveRequest* req = qobject_cast<QContactRelationshipSaveRequest*>(sender());

    if (req->isFinished()) {
        QList<QContactRelationship> rs = req->relationships();
        QList<int> errorIds = req->errorMap().keys();

        for( int i = 0; i < rs.count(); i++) {
            if (!errorIds.contains(i)) {
                //saved
                QContactRelationship r = rs.at(i);

                if (!d->m_relationships.contains(r)) {
                    //new relationship saved
                    QDeclarativeContactRelationship* dcr = new QDeclarativeContactRelationship(this);
                    dcr->setRelationship(r);
                    beginInsertRows(QModelIndex(), d->m_declarativeRelationships.count(), d->m_declarativeRelationships.count());
                    d->m_declarativeRelationships.append(dcr);
                    d->m_relationships.append(r);
                    endInsertRows();
                }
            }
        }
        req->deleteLater();
    }
}

void QDeclarativeContactRelationshipModel::relationshipsRemoved()
{
    QContactRelationshipRemoveRequest* req = qobject_cast<QContactRelationshipRemoveRequest*>(sender());

    if (req->isFinished()) {
        QList<QContactRelationship> rs = req->relationships();
        QList<int> errorIds = req->errorMap().keys();


        for( int i = 0; i < rs.count(); i++) {
            if (!errorIds.contains(i)) {
                int row = 0;
                QContactRelationship r = rs.at(i);
                for (; row < d->m_relationships.count(); row++) {
                    if (d->m_relationships.at(row) == r)
                        break;
                }
                if (row < d->m_relationships.count()) {
                    beginRemoveRows(QModelIndex(), row, row);
                    d->m_declarativeRelationships.removeAt(row);
                    d->m_relationships.removeAt(row);
                    endRemoveRows();
                } else {
                    //impossible?
                    qWarning() << "this relationship '" << row << "' was already removed!";
                }
            }
        }
        req->deleteLater();
    }
}
