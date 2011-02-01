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

#include "qdeclarativebtdiscoverymodel_p.h"

#include <QPixmap>

#include <bluetooth/qbluetoothdeviceinfo.h>
#include <bluetooth/qbluetoothaddress.h>

#include "qdeclarativebluetoothservice_p.h"

/*!
    \qmlclass ContactModel QDeclarativeBluetoothDiscoveryModel
    \brief The ContactModel element provides access to contacts from the contacts store.
    \ingroup qml-contacts

    This element is part of the \bold{QtMobility.contacts 1.1} module.

    ContactModel provides a model of contacts from the contacts store.
    The contents of the model can be specified with \l filter, \l sortOrders and \l fetchHint properties.
    Whether the model is automatically updated when the store or \l contacts changes, can be
    controlled with \l ContactModel::autoUpdate property.

    There are two ways of accessing the contact data: via model by using views and delegates,
    or alternatively via \l contacts list property. Of the two, the model access is preferred.
    Direct list access (i.e. non-model) is not guaranteed to be in order set by \l sortOrder.

    At the moment the model roles provided by ContactModel are display, decoration and \c contact.
    Through the \c contact role can access any data provided by the Contact element.

    \sa RelationshipModel, Contact, {QContactManager}
*/





class QDeclarativeBluetoothDiscoveryModelPrivate
{
public:
    QDeclarativeBluetoothDiscoveryModelPrivate()
        :m_agent(0),
        m_error(QBluetoothServiceDiscoveryAgent::NoError),
        m_minimal(true),
        m_working(false),
        m_componentCompleted(false),
        m_discovery(true)
    {
    }
    ~QDeclarativeBluetoothDiscoveryModelPrivate()
    {
        if (m_agent)
            delete m_agent;
    }

    QBluetoothServiceDiscoveryAgent *m_agent;

    QBluetoothServiceDiscoveryAgent::Error m_error;
//    QList<QBluetoothServiceInfo> m_services;
    QList<QDeclarativeBluetoothService *> m_services;
    bool m_minimal;
    bool m_working;
    bool m_componentCompleted;
    QString m_uuid;
    bool m_discovery;
};

QDeclarativeBluetoothDiscoveryModel::QDeclarativeBluetoothDiscoveryModel(QObject *parent) :
    QAbstractListModel(parent),
    d(new QDeclarativeBluetoothDiscoveryModelPrivate)
{

    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(Qt::DisplayRole, "name");
    roleNames.insert(Qt::DecorationRole, "icon");
    roleNames.insert(ServiceRole, "service");
    setRoleNames(roleNames);

    d->m_agent = new QBluetoothServiceDiscoveryAgent(this);
    connect(d->m_agent, SIGNAL(serviceDiscovered(const QBluetoothServiceInfo&)), this, SLOT(serviceDiscovered(const QBluetoothServiceInfo&)));
    connect(d->m_agent, SIGNAL(finished()), this, SLOT(finishedDiscovery()));
    connect(d->m_agent, SIGNAL(error(QBluetoothServiceDiscoveryAgent::Error)), this, SLOT(errorDiscovery(QBluetoothServiceDiscoveryAgent::Error)));

}
void QDeclarativeBluetoothDiscoveryModel::componentComplete()
{
    d->m_componentCompleted = true;
    setDiscovery(d->m_discovery);
}

void QDeclarativeBluetoothDiscoveryModel::setDiscovery(bool discovery_)
{
    d->m_discovery = discovery_;

    if (!d->m_componentCompleted)
        return;

    d->m_working = false;

    d->m_agent->stop();

    if(!discovery_) {
        emit discoveryChanged();
        return;
    }

    if(!d->m_uuid.isEmpty())
        d->m_agent->setUuidFilter(QBluetoothUuid(d->m_uuid));

    d->m_working = true;

    if (d->m_minimal) {
        qDebug() << "Doing minimal";
        d->m_agent->start(QBluetoothServiceDiscoveryAgent::MinimalDiscovery);
    }
    else
        d->m_agent->start(QBluetoothServiceDiscoveryAgent::FullDiscovery);

    emit discoveryChanged();
}

void QDeclarativeBluetoothDiscoveryModel::errorDiscovery(QBluetoothServiceDiscoveryAgent::Error error)
{
    d->m_error = error;
    emit errorChanged();
}

/*!
  \qmlproperty string ContactModel::error

  This property holds the latest error code returned by the contact manager.

  This property is read only.
  */
QString QDeclarativeBluetoothDiscoveryModel::error() const
{
    switch (d->m_error){
    case QBluetoothServiceDiscoveryAgent::NoError:
        break;
    default:
        return QLatin1String("UnknownError");
    }
    return QLatin1String("NoError");

}

int QDeclarativeBluetoothDiscoveryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return d->m_services.count();
}

QVariant QDeclarativeBluetoothDiscoveryModel::data(const QModelIndex &index, int role) const
{
    QDeclarativeBluetoothService *service = d->m_services.value(index.row());
    QBluetoothServiceInfo *info = service->serviceInfo();

    switch(role) {
        case Qt::DisplayRole:
        {
            QString label = info->device().name();
            if(label.isEmpty())
                label += info->device().address().toString();
            label += " " + info->serviceName();
            return label;
        }
        case Qt::DecorationRole:
            return QLatin1String("image://thumbnail/default");
        case ServiceRole:
        {
            return QVariant::fromValue(service);
        }
    }
    return QVariant();
}

void QDeclarativeBluetoothDiscoveryModel::serviceDiscovered(const QBluetoothServiceInfo &service)
{
    QDeclarativeBluetoothService *bs = new QDeclarativeBluetoothService(service, this);

    for(int i = 0; i < d->m_services.count(); i++) {
        if(bs->deviceAddress() == d->m_services.at(i)->deviceAddress()){
            delete bs;
            return;
        }
    }

    beginResetModel(); // beginInsertRows(...) doesn't work for full discovery...
    d->m_services.append(bs);
    endResetModel();
    emit newServiceDiscovered(bs);
}

void QDeclarativeBluetoothDiscoveryModel::finishedDiscovery()
{
    qDebug() << "Done!";
    d->m_working = false;
    emit discoveryChanged();
}

bool QDeclarativeBluetoothDiscoveryModel::minimalDiscovery()
{
    return d->m_minimal;
}

void QDeclarativeBluetoothDiscoveryModel::setMinimalDiscovery(bool minimalDiscovery_)
{
    d->m_minimal = minimalDiscovery_;
    emit minimalDiscoveryChanged();
}

bool QDeclarativeBluetoothDiscoveryModel::discovery()
{
    return d->m_working;
}

QString QDeclarativeBluetoothDiscoveryModel::uuidFilter() const
{
    return d->m_uuid;
}

void QDeclarativeBluetoothDiscoveryModel::setUuidFilter(QString uuid)
{
    QBluetoothUuid qbuuid(uuid);
    if(qbuuid.isNull()) {
        qWarning() << "Invalid UUID providded " << uuid;
        return;
    }
    d->m_uuid = uuid;
    emit uuidFilterChanged();
}
