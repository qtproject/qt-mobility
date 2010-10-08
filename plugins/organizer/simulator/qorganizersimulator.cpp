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

#include "qorganizersimulator_p.h"
#include "qorganizeritem.h"
#include "qtorganizer.h"
#include "connection_p.h"
#include "qorganizerdata_simulator_p.h"

#include <private/qsimulatordata_p.h>

//QTM_USE_NAMESPACE

using namespace Simulator;
using namespace QtSimulatorPrivate;

QOrganizerItemMemoryEngineData *QOrganizerItemSimulatorEngine::engineData = 0;

QOrganizerItemManagerEngine* QOrganizerItemSimulatorFactory::engine(const QMap<QString, QString>& parameters, QOrganizerItemManager::Error* error)
{
    Q_UNUSED(parameters);
    Q_UNUSED(error);

    // all simulator engines always share the same engineData
    QOrganizerItemMemoryEngineData* data = QOrganizerItemSimulatorEngine::engineData;
    if (data) {
        data->m_refCount.ref();
    } else {
        data = new QOrganizerItemMemoryEngineData();
        data->m_id = QLatin1String("simulatorengine");
        data->m_anonymous = false;

        // we never want the data to be deleted, so push the refcount to two!
        data->m_refCount.ref();

        QOrganizerItemSimulatorEngine::engineData = data;

        // give an engine to the simulator connection
        Simulator::Connection::instance()->setEngine(new QOrganizerItemSimulatorEngine(data));
    }
    return new QOrganizerItemSimulatorEngine(data);
}

QOrganizerItemEngineLocalId* QOrganizerItemSimulatorFactory::createItemEngineLocalId() const
{
    return new QOrganizerItemMemoryEngineLocalId;
}

QOrganizerCollectionEngineLocalId* QOrganizerItemSimulatorFactory::createCollectionEngineLocalId() const
{
    /* TODO - return the localid specific to the engine */
    return new QOrganizerCollectionMemoryEngineLocalId;
}

QString QOrganizerItemSimulatorFactory::managerName() const
{
    /* TODO - put your engine name here */
    return QString("simulator");
}
Q_EXPORT_PLUGIN2(qtorganizer_simulator, QOrganizerItemSimulatorFactory);


QOrganizerItemSimulatorEngine::~QOrganizerItemSimulatorEngine()
{
    // have to be extremely careful with the QOrganizerItemMemoryEngine destructor
    // it decrements the refcount and deletes engineData if the count hits 0
    // luckily, we set up the refcount to always be >= 2
    Q_ASSERT(engineData->m_refCount >= 2);
}

QString QOrganizerItemSimulatorEngine::managerName() const
{
    /* TODO - put your engine name here */
    return QLatin1String("simulator");
}

int QOrganizerItemSimulatorEngine::managerVersion() const
{
    /* TODO - implement this appropriately.  This is strictly defined by the engine, so return whatever you like */
    return 1;
}

bool QOrganizerItemSimulatorEngine::saveItem(QOrganizerItem* theOrganizerItem, const QOrganizerCollectionLocalId& collectionId, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    Connection *con = Connection::instance();

    if (!con->mNotifySimulator)
        return QOrganizerItemMemoryEngine::saveItem(theOrganizerItem, collectionId, changeSet, error);

    // translate local id -> remote id
    QOrganizerItem item = *theOrganizerItem;
    con->translateItemIds(&item, con->mManagerUri, con->mLocalToRemote);
    bool newItem = item.id().isNull();

    // translate collection id
    QOrganizerCollectionId remoteCollectionId;
    remoteCollectionId.setManagerUri(con->mManagerUri);
    remoteCollectionId.setLocalId(con->mLocalToRemote.collections.value(collectionId));

    // save remotely
    QLocalSocket *sendSocket = con->sendSocket();
    Simulator::SaveOrganizerItemReply reply = RemoteMetacall<Simulator::SaveOrganizerItemReply>::call(
                sendSocket, TimeoutSync, "requestSaveOrganizerItem", item, remoteCollectionId);
    *error = reply.error;

    // if it failed, stop
    if (reply.error != QOrganizerItemManager::NoError)
        return false;

    // save locally
    if (!QOrganizerItemMemoryEngine::saveItem(theOrganizerItem, collectionId, changeSet, error))
        return false; // it's already saved remotely - revert?

    if (newItem) {
        con->mRemoteToLocal.items.insert(reply.savedItem.localId(), theOrganizerItem->localId());
        con->mLocalToRemote.items.insert(theOrganizerItem->localId(), reply.savedItem.localId());
    }

    return true;
}

bool QOrganizerItemSimulatorEngine::removeItem(const QOrganizerItemLocalId& organizeritemId, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    Connection *con = Connection::instance();

    if (!con->mNotifySimulator)
        return QOrganizerItemMemoryEngine::removeItem(organizeritemId, changeSet, error);

    QOrganizerItemId remoteId;
    remoteId.setManagerUri(con->mManagerUri);
    remoteId.setLocalId(con->mLocalToRemote.items.value(organizeritemId));

    QLocalSocket *sendSocket = con->sendSocket();
    int errorInt = RemoteMetacall<int>::call(
                sendSocket, TimeoutSync, "requestRemoveOrganizerItem", remoteId);
    *error = static_cast<QOrganizerItemManager::Error>(errorInt);

    if (*error != QOrganizerItemManager::NoError)
        return false;

    if (!QOrganizerItemMemoryEngine::removeItem(organizeritemId, changeSet, error))
        return false; // it's already removed remotely - revert?

    con->mRemoteToLocal.items.remove(remoteId.localId());
    con->mLocalToRemote.items.remove(organizeritemId);

    return true;
}

bool QOrganizerItemSimulatorEngine::saveCollection(QOrganizerCollection *collection, QOrganizerItemManager::Error *error)
{
    Connection *con = Connection::instance();

    if (!con->mNotifySimulator)
        return QOrganizerItemMemoryEngine::saveCollection(collection, error);

    // translate local id -> remote id
    QOrganizerCollection remoteCollection = *collection;
    con->translateCollectionIds(&remoteCollection, con->mManagerUri, con->mLocalToRemote);
    bool newCollection = remoteCollection.id().isNull();

    // save remotely
    QLocalSocket *sendSocket = con->sendSocket();
    Simulator::SaveOrganizerCollectionReply reply =
            RemoteMetacall<Simulator::SaveOrganizerCollectionReply>::call(
                sendSocket, TimeoutSync, "requestSaveOrganizerCollection", remoteCollection);
    *error = reply.error;

    // if it failed, stop
    if (reply.error != QOrganizerItemManager::NoError)
        return false;

    // save locally
    if (!QOrganizerItemMemoryEngine::saveCollection(collection, error))
        return false; // it's already saved remotely - revert?

    if (newCollection) {
        con->mRemoteToLocal.collections.insert(reply.savedCollection.localId(), collection->localId());
        con->mLocalToRemote.collections.insert(collection->localId(), reply.savedCollection.localId());
    }

    return true;
}

bool QOrganizerItemSimulatorEngine::removeCollection(const QOrganizerCollectionLocalId &collectionId, QOrganizerItemManager::Error *error)
{
    return QOrganizerItemManagerEngine::removeCollection(collectionId, error);
}

bool QOrganizerItemSimulatorEngine::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    Connection *con = Connection::instance();

    if (!con->mNotifySimulator)
        return QOrganizerItemMemoryEngine::saveDetailDefinition(def, organizeritemType, changeSet, error);
    return QOrganizerItemMemoryEngine::saveDetailDefinition(def, organizeritemType, changeSet, error);
}

bool QOrganizerItemSimulatorEngine::removeDetailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    Connection *con = Connection::instance();

    if (!con->mNotifySimulator)
        return QOrganizerItemMemoryEngine::removeDetailDefinition(definitionId, organizeritemType, changeSet, error);
    return QOrganizerItemMemoryEngine::removeDetailDefinition(definitionId, organizeritemType, changeSet, error);
}

QOrganizerItemSimulatorEngine::QOrganizerItemSimulatorEngine(QOrganizerItemMemoryEngineData* data)
    : QOrganizerItemMemoryEngine(data)
{
}
