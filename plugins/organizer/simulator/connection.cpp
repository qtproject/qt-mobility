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

#include "connection_p.h"

#include "qorganizerdata_simulator_p.h"
#include "qorganizersimulator_p.h"

#include <mobilityconnection_p.h>
#include <private/qsimulatordata_p.h>

#include <QtNetwork/QLocalSocket>

QTM_USE_NAMESPACE
using namespace QtSimulatorPrivate;
using namespace Simulator;

Connection::Connection(MobilityConnection *mobilityCon)
    : QObject(mobilityCon)
    , mConnection(mobilityCon)
    , mRegisteredWithSimulator(false)
    , mInitialDataReceived(false)
    , mManager(QLatin1String("simulator"))
{
    qt_registerOrganizerTypes();

    mobilityCon->addMessageHandler(this);
}

Connection *Connection::instance()
{
    static Connection *connection = 0;

    if (!connection)
        connection = new Connection(MobilityConnection::instance());
    return connection;
}

void Connection::setEngine(QOrganizerItemSimulatorEngine *engine)
{
    Q_ASSERT(!mRegisteredWithSimulator);

    mEngine = engine;

    RemoteMetacall<void>::call(mConnection->sendSocket(), NoSync, "setRequestsOrganizer");
    mRegisteredWithSimulator = true;

    getInitialData();
}

QLocalSocket *Connection::sendSocket()
{
    return mConnection->sendSocket();
}

void Connection::translateItemIds(QOrganizerItem *item, const QString &managerUri, const ItemIdTranslation &idTranslation)
{
    // translate the main id
    const QOrganizerItemLocalId previousLocalId = item->localId();
    QOrganizerItemId id;
    if (idTranslation.contains(previousLocalId)) {
        id.setManagerUri(managerUri);
        id.setLocalId(idTranslation.value(previousLocalId));
    }
    item->setId(id);

    // translate any ids and localids appearing in details
    foreach (const QOrganizerItemDetail &detail, item->details()) {
        QOrganizerItemDetail detailCopy(detail);
        QMapIterator<QString, QVariant> it(detail.variantValues());
        bool modified = false;
        while (it.hasNext()) {
            it.next();
            const QVariant &value = it.value();
            const int type = value.userType();
            if (type == qMetaTypeId<QOrganizerItemLocalId>()) {
                QOrganizerItemLocalId oldId = value.value<QOrganizerItemLocalId>();
                detailCopy.setValue(it.key(), QVariant::fromValue(idTranslation.value(oldId)));
                modified = true;
            }
            if (type == qMetaTypeId<QOrganizerItemId>()) {
                QOrganizerItemId id = value.value<QOrganizerItemId>();
                id.setManagerUri(managerUri);
                id.setLocalId(idTranslation.value(id.localId()));
                detailCopy.setValue(it.key(), QVariant::fromValue(id));
                modified = true;
            }
        }
        if (modified) {
            item->saveDetail(&detailCopy);
        }
    }

    // ### Any other ids in an item?
}

void Connection::getInitialData()
{
    mInitialDataReceived = false;
    RemoteMetacall<void>::call(mConnection->sendSocket(), NoSync, "triggerOrganizerDataResend");

    // blockingly handle all remote calls until the simulator signals the initial data
    // sending is done
    QByteArray readBuffer;
    QLocalSocket *readSocket = mConnection->receiveSocket();
    while (!mInitialDataReceived) {
        readSocket->waitForReadyRead(100);
        readBuffer += readSocket->readAll();
        forever {
            IncomingRemoteMetacall rpc;
            if (rpc.read(&readBuffer)) {
                if (rpc.call(readSocket, this))
                    continue;
                qWarning("Ignoring a call to %s,", rpc.signature().data());
            }
            break;
        }
    }
}

void Connection::setOrganizerManagerUri(QString uri)
{
    mManagerUri = uri;
}

void Connection::initialOrganizerDataSent()
{
    mInitialDataReceived = true;
}

void Connection::clearOrganizerItems()
{
    mLocalToRemoteItemIds.clear();
    mRemoteToLocalItemIds.clear();
    mNotifySimulator = false;
    mManager.removeItems(mManager.itemIds(), 0);
    mNotifySimulator = true;
}

void Connection::saveOrganizerItem(QOrganizerItem item)
{
    const QOrganizerItemLocalId remoteLocalId = item.localId();
    translateItemIds(&item, mManager.managerUri(), mRemoteToLocalItemIds);
    bool newItem = item.id().isNull();

    mNotifySimulator = false;
    mManager.saveItem(&item);
    mNotifySimulator = true;
    if (mManager.error())
        qDebug() << "Error saving:" << mManager.error();

    // if this is a new item, save the new id in the maps
    if (newItem) {
        mRemoteToLocalItemIds.insert(remoteLocalId, item.localId());
        mLocalToRemoteItemIds.insert(item.localId(), remoteLocalId);
    }
}

void Connection::removeOrganizerItem(QOrganizerItemLocalId id)
{
    if (!mRemoteToLocalItemIds.contains(id))
        return;

    QOrganizerItemLocalId localId = mManager.item(mRemoteToLocalItemIds.value(id)).localId();
    mRemoteToLocalItemIds.remove(id);
    mLocalToRemoteItemIds.remove(localId);

    mNotifySimulator = false;
    mManager.removeItem(localId);
    mNotifySimulator = true;
}

#include "moc_connection_p.cpp"
