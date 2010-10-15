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

QOrganizerItemMemoryEngineData *QOrganizerSimulatorEngine::engineData = 0;

QOrganizerManagerEngine* QOrganizerItemSimulatorFactory::engine(const QMap<QString, QString>& parameters, QOrganizerManager::Error* error)
{
    Q_UNUSED(parameters);
    Q_UNUSED(error);

    // all simulator engines always share the same engineData
    QOrganizerItemMemoryEngineData* data = QOrganizerSimulatorEngine::engineData;
    if (data) {
        data->m_refCount.ref();
    } else {
        data = new QOrganizerItemMemoryEngineData();
        data->m_id = QLatin1String("simulatorengine");
        data->m_anonymous = false;

        QOrganizerSimulatorEngine::engineData = data;

        // give an engine to the simulator connection
        data->m_refCount.ref(); // push the refcount to two
        Simulator::Connection::instance()->setEngine(new QOrganizerSimulatorEngine(data));
    }
    return new QOrganizerSimulatorEngine(data);
}


QOrganizerItemEngineId* QOrganizerItemSimulatorFactory::createItemEngineId(const QMap<QString, QString>& parameters, const QString& engineIdString) const
{
    Q_UNUSED(parameters); // XXX This should be used
    return new QOrganizerItemMemoryEngineId(engineIdString);
}

QOrganizerCollectionEngineId* createCollectionEngineId(const QMap<QString, QString>& parameters, const QString& engineIdString) const
{
    Q_UNUSED(parameters);
    return new QOrganizerItemMemoryEngineId(engineIdString);
}

QString QOrganizerItemSimulatorFactory::managerName() const
{
    static const QString name(QLatin1String("simulator"));
    return name;
}
Q_EXPORT_PLUGIN2(qtorganizer_simulator, QOrganizerItemSimulatorFactory);


QString QOrganizerSimulatorEngine::managerName() const
{
    static const QString name(QLatin1String("simulator"));
    return name;
}

int QOrganizerSimulatorEngine::managerVersion() const
{
    /* TODO - implement this appropriately.  This is strictly defined by the engine, so return whatever you like */
    return 1;
}

bool QOrganizerSimulatorEngine::saveItem(QOrganizerItem* theOrganizerItem, QOrganizerItemChangeSet& changeSet, QOrganizerManager::Error* error)
{
    Connection *con = Connection::instance();

    if (!con->mNotifySimulator)
        return QOrganizerItemMemoryEngine::saveItem(theOrganizerItem, changeSet, error);

    // translate local id -> remote id
    QOrganizerItem item = *theOrganizerItem;
    con->translateItemIds(&item, con->mManagerUri, con->mLocalToRemote);
    bool newItem = item.id().isNull();

    // save remotely
    QLocalSocket *sendSocket = con->sendSocket();
    Simulator::SaveOrganizerItemReply reply = RemoteMetacall<Simulator::SaveOrganizerItemReply>::call(
                sendSocket, TimeoutSync, "requestSaveOrganizerItem", item);
    *error = reply.error;

    // if it failed, stop
    if (reply.error != QOrganizerManager::NoError)
        return false;

    // save locally
    if (!QOrganizerItemMemoryEngine::saveItem(theOrganizerItem, changeSet, error))
        return false; // it's already saved remotely - revert?

    if (newItem) {
        con->mRemoteToLocal.items.insert(reply.savedItem.localId(), theOrganizerItem->localId());
        con->mLocalToRemote.items.insert(theOrganizerItem->localId(), reply.savedItem.localId());
    }

    return true;
}

bool QOrganizerSimulatorEngine::removeItem(const QOrganizerItemLocalId& organizeritemId, QOrganizerItemChangeSet& changeSet, QOrganizerManager::Error* error)
{
    Connection *con = Connection::instance();

    if (!con->mNotifySimulator)
        return QOrganizerItemMemoryEngine::removeItem(organizeritemId, changeSet, error);

    QOrganizerItemId remoteId;
    remoteId.setManagerUri(con->mManagerUri);
    remoteId.setLocalId(con->mLocalToRemote.items.value(organizeritemId));

    QLocalSocket *sendSocket = con->sendSocket();
    Simulator::OrganizerItemId packedId;
    packedId.id = remoteId;
    int errorInt = RemoteMetacall<int>::call(
                sendSocket, TimeoutSync, "requestRemoveOrganizerItem", packedId);
    *error = static_cast<QOrganizerManager::Error>(errorInt);

    if (*error != QOrganizerManager::NoError)
        return false;

    if (!QOrganizerItemMemoryEngine::removeItem(organizeritemId, changeSet, error))
        return false; // it's already removed remotely - revert?

    con->mRemoteToLocal.items.remove(remoteId.localId());
    con->mLocalToRemote.items.remove(organizeritemId);

    return true;
}

bool QOrganizerSimulatorEngine::saveCollection(QOrganizerCollection *collection, QOrganizerManager::Error *error)
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
    if (reply.error != QOrganizerManager::NoError)
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

bool QOrganizerSimulatorEngine::removeCollection(const QOrganizerCollectionLocalId &collectionId, QOrganizerManager::Error *error)
{
    Connection *con = Connection::instance();

    if (!con->mNotifySimulator)
        return QOrganizerItemMemoryEngine::removeCollection(collectionId, error);

    QOrganizerCollectionId remoteId;
    remoteId.setManagerUri(con->mManagerUri);
    remoteId.setLocalId(con->mLocalToRemote.collections.value(collectionId));

    QLocalSocket *sendSocket = con->sendSocket();
    Simulator::OrganizerCollectionId packedId;
    packedId.id = remoteId;
    int errorInt = RemoteMetacall<int>::call(
                sendSocket, TimeoutSync, "requestRemoveOrganizerCollection", packedId);
    *error = static_cast<QOrganizerManager::Error>(errorInt);

    if (*error != QOrganizerManager::NoError)
        return false;

    // removing a collection removes its items - which has already been done on the simulator
    // side by the request above
    bool oldNotify = con->mNotifySimulator;
    con->mNotifySimulator = false;
    if (!QOrganizerItemMemoryEngine::removeCollection(collectionId, error))
        return false; // it's already removed remotely - revert?
    con->mNotifySimulator = oldNotify;

    con->mRemoteToLocal.collections.remove(remoteId.localId());
    con->mLocalToRemote.collections.remove(collectionId);

    return true;
}

bool QOrganizerSimulatorEngine::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerManager::Error* error)
{
    Connection *con = Connection::instance();

    if (!con->mNotifySimulator)
        return QOrganizerItemMemoryEngine::saveDetailDefinition(def, organizeritemType, changeSet, error);

    QLocalSocket *sendSocket = con->sendSocket();
    int errorInt = RemoteMetacall<int>::call(
                sendSocket, TimeoutSync, "requestSaveOrganizerDetailDefinition", def, organizeritemType);
    *error = static_cast<QOrganizerManager::Error>(errorInt);

    if (*error != QOrganizerManager::NoError)
        return false;

    if (!QOrganizerItemMemoryEngine::saveDetailDefinition(def, organizeritemType, changeSet, error))
        return false; // already saved remotely - revert?

    return true;
}

bool QOrganizerSimulatorEngine::removeDetailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerManager::Error* error)
{
    Connection *con = Connection::instance();

    if (!con->mNotifySimulator)
        return QOrganizerItemMemoryEngine::removeDetailDefinition(definitionId, organizeritemType, changeSet, error);

    QLocalSocket *sendSocket = con->sendSocket();
    int errorInt = RemoteMetacall<int>::call(
                sendSocket, TimeoutSync, "requestRemoveOrganizerDetailDefinition", definitionId, organizeritemType);
    *error = static_cast<QOrganizerManager::Error>(errorInt);

    if (*error != QOrganizerManager::NoError)
        return false;

    if (!QOrganizerItemMemoryEngine::removeDetailDefinition(definitionId, organizeritemType, changeSet, error))
        return false; // already removed remotely - revert?

    return true;
}

QOrganizerSimulatorEngine::QOrganizerSimulatorEngine(QOrganizerItemMemoryEngineData* data)
    : QOrganizerItemMemoryEngine(data)
{
}
