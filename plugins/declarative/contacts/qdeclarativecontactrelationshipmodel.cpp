/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
#include <QtDeclarative/qdeclarativeinfo.h>
#include "qdeclarativecontactrelationshipmodel_p.h"
#include "qcontactmanager.h"
#include "qcontactrequests.h"

#include <QDebug>
#include <QPixmap>

/*!
   \qmlclass RelationshipModel QDeclarativeContactRelationshipModel
   \brief The RelationshipModel provides a model of contact relationships from the contacts store.

   \ingroup qml-contacts

   This element is part of the \bold{QtMobility.contacts 1.1} module.

    The contents of the model can be specified with \l participantId, \l role and \l relationshipType properties.
    Whether the model is automatically updated when the store or filter changes, can be controlled
    with \l RelationshipModel::autoUpdate property.

    There are two ways of accessing the relationship data: through model by using views and delegates,
    or alternatively via \l relationships list property.

    At the moment only data role provided by the model is \c relationship (\l Relationship).
    Through that one can access any data provided by the Relationship element.

   \sa Relationship {QContactRelationship}
 */

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

/*!
  \qmlproperty string RelationshipModel::manager

  This property holds the manager uri of the contact backend engine.
  */
QString QDeclarativeContactRelationshipModel::manager() const
{
    if (d->m_manager)
        return d->m_manager->managerName();
    return QString();
}


/*!
  \qmlproperty string RelationshipModel::error

  This property holds the latest error code returned by the contact manager.

  This property is read only.
  */
QString QDeclarativeContactRelationshipModel::error() const
{
    switch (d->m_manager->error()) {
    case QContactManager::DoesNotExistError:
        return QLatin1String("DoesNotExist");
    case QContactManager::AlreadyExistsError:
        return QLatin1String("AlreadyExists");
    case QContactManager::InvalidDetailError:
        return QLatin1String("InvalidDetail");
    case QContactManager::InvalidRelationshipError:
        return QLatin1String("InvalidRelationship");
    case QContactManager::LockedError:
        return QLatin1String("LockedError");
    case QContactManager::DetailAccessError:
        return QLatin1String("DetailAccessError");
    case QContactManager::PermissionsError:
        return QLatin1String("PermissionsError");
    case QContactManager::OutOfMemoryError:
        return QLatin1String("OutOfMemory");
    case QContactManager::NotSupportedError:
        return QLatin1String("NotSupported");
    case QContactManager::BadArgumentError:
        return QLatin1String("BadArgument");
    case QContactManager::UnspecifiedError:
        return QLatin1String("UnspecifiedError");
    case QContactManager::VersionMismatchError:
        return QLatin1String("VersionMismatch");
    case QContactManager::LimitReachedError:
        return QLatin1String("LimitReached");
    case QContactManager::InvalidContactTypeError:
        return QLatin1String("InvalidContactType");
    default:
        break;
    }
    return QLatin1String("NoError");
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


/*!
  \qmlproperty int RelationshipModel::participantId

  This property holds the participant id which the list of relationships returned by RelationshipModel should contain.

  \sa RelationshipFilter::relatedContactId
  \sa RelationshipModel::role
  */
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

/*!
  \qmlproperty variant RelationshipModel::relationshipType

  This property holds the relationship type which the list of relationships returned by RelationshipModel should contain.

  \sa Relationship::type
  */
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

/*!
  \qmlproperty enumeration RelationshipModel::role

  This property holds the relationship role which the list of relationships returned by RelationshipModel should contain.

  \sa RelationshipFilter::relatedContactRole
  */
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

/*!
  \qmlproperty bool RelationshipModel::autoUpdate

  This property indicates whether or not the relationship model should be updated automatically, default value is true.
  */
bool QDeclarativeContactRelationshipModel::autoUpdate() const
{
    //TODO
    return true;
}
void QDeclarativeContactRelationshipModel::setAutoUpdate(bool autoUpdate)
{
    Q_UNUSED(autoUpdate);
    //TODO
}

/*!
  \qmlproperty list<Relationship> RelationshipModel::relationships

  This property holds a list of relationships.

  \sa Relationship
  */
QDeclarativeListProperty<QDeclarativeContactRelationship> QDeclarativeContactRelationshipModel::relationships()
{
    return QDeclarativeListProperty<QDeclarativeContactRelationship>(this, d->m_declarativeRelationships);
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

/*!
  \qmlmethod RelationshipModel::addRelationship(relationship)
  Addes the given \a relationship to the backend store.
  */
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

/*!
  \qmlmethod RelationshipModel::removeRelationship(relationship)
  Removes the given \a relationship from the backend store.
  */
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
                    qmlInfo(this) << tr("this relationship '") << row << tr("' was already removed!");
                }
            }
        }
        req->deleteLater();
    }
}
