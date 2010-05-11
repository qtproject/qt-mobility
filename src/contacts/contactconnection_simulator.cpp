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

#include "contactconnection_simulator_p.h"

#include <mobilityconnection_p.h>
#include <qcontactmemorybackenddata_simulator_p.h>
#include <qcontactsimulatorbackend_p.h>
#include <qcontactdetail_p.h>
#include <private/qsimulatordata_p.h>

#include <QtNetwork/QLocalSocket>

QTM_BEGIN_NAMESPACE

using namespace QtSimulatorPrivate;

namespace Simulator {

ContactConnection::ContactConnection(MobilityConnection *mobilityCon)
    : QObject(mobilityCon)
    , mConnection(mobilityCon)
    , mRegisteredWithSimulator(false)
    , mInitialDataReceived(false)
{
    qt_registerContactsTypes();
    mobilityCon->addMessageHandler(this);
}

ContactConnection *ContactConnection::instance()
{
    static ContactConnection *contactConnection = 0;

    if (!contactConnection)
        contactConnection = new ContactConnection(MobilityConnection::instance());
    return contactConnection;
}

void ContactConnection::setEngine(QContactSimulatorEngine *engine)
{
    if (!mRegisteredWithSimulator) {
        RemoteMetacall<void>::call(mConnection->sendSocket(), NoSync, "setRequestsContactInfo");
        mRegisteredWithSimulator = true;
    }

    mEngine = engine;
    getInitialData();
}

QLocalSocket *ContactConnection::sendSocket()
{
    return mConnection->sendSocket();
}

void ContactConnection::getInitialData()
{
    mInitialDataReceived = false;
    RemoteMetacall<void>::call(mConnection->sendSocket(), NoSync, "triggerContactDataResend");

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

void ContactConnection::initialContactDataSent()
{
    mInitialDataReceived = true;
}

void ContactConnection::setContactData(const QContactSimulatorData &data)
{
    QContactMemoryEngineData *edata = mEngine->d;
    edata->m_selfContactId = data.m_selfContactId;
    edata->m_contacts = data.m_contacts;
    edata->m_contactIds = data.m_contactIds;
    edata->m_relationships = data.m_relationships;
    edata->m_orderedRelationships = data.m_orderedRelationships;
    edata->m_definitionIds = data.m_definitionIds;
    edata->m_definitions = data.m_definitions;
    edata->m_nextContactId = data.m_nextContactId;
    QContactDetailPrivate::lastDetailKey = data.m_lastDetailId;
}

void ContactConnection::addContact(QContact contact)
{
    mEngine->setNotifySimulator(false);
    QContactManager::Error error;
    QContactChangeSet changeSet;

    // if the created contact would get a too high id, delete
    // the contacts that the simulator knows nothing about
    if (mEngine->d->m_nextContactId + 1 > contact.localId()) {
        for (QContactLocalId id = contact.localId(); id <= mEngine->d->m_nextContactId; ++id)
            mEngine->removeContact(id, changeSet, &error);
    }

    // make sure the inserted contact gets the same id as in the simulator
    mEngine->d->m_nextContactId = contact.localId() - 1;

    // add (set localid to 0 first, otherwise the engine thinks we want to update a contact)
    QContactId newId = contact.id();
    newId.setLocalId(0);
    contact.setId(newId);
    mEngine->saveContact(&contact, changeSet, &error);

    mEngine->setNotifySimulator(true);
}

void ContactConnection::changeContact(QContact contact)
{
    QContactChangeSet changeSet;
    mEngine->setNotifySimulator(false);
    QContactManager::Error error;
    mEngine->saveContact(&contact, changeSet, &error);
    mEngine->setNotifySimulator(true);
}

void ContactConnection::removeContact(uint contactId)
{
    QContactChangeSet changeSet;
    mEngine->setNotifySimulator(false);
    QContactManager::Error error;
    mEngine->removeContact(contactId, changeSet, &error);
    mEngine->setNotifySimulator(true);
}

void ContactConnection::setRelationships(uint id, const QVariantList &relationships)
{
    QContactManager::Error error;
    QContactChangeSet changeSet;
    QContact contact = mEngine->contact(id, QContactFetchHint(), &error);
    if (error != QContactManager::NoError)
        return;

    // adjust relationships to be in sync
    QList<QContactRelationship> clientRelationships(
            mEngine->relationships(QString(),
                                   contact.id(),
                                   QContactRelationship::Either,
                                   &error));
    QList<QContactRelationship> simulatorRelationships;
    foreach (const QVariant &v, relationships)
        simulatorRelationships.append(v.value<QContactRelationship>());

    mEngine->setNotifySimulator(false);

    // remove superfluous
    foreach (const QContactRelationship &cr, clientRelationships) {
        if (!simulatorRelationships.contains(cr)) {
            mEngine->removeRelationship(cr, changeSet, &error);
        }
    }

    // add new
    foreach (const QContactRelationship &sr, simulatorRelationships) {
        if (!clientRelationships.contains(sr)) {
            QContactRelationship toAdd = sr;
            mEngine->saveRelationship(&toAdd, changeSet, &error);
        }
    }

    mEngine->setNotifySimulator(true);
}

} // namespace Simulator

#include "moc_contactconnection_simulator_p.cpp"

QTM_END_NAMESPACE
