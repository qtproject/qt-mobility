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

#include "qcontactmanager.h"

#include "qcontactdetaildefinition.h"
#include "qcontactmanagerengine.h"
#include "qcontactabstractrequest.h"
#include "qcontactrequests.h"
#include "qcontactchangeset.h"
#include "qcontactdetails.h"

#include "qcontactsimulatorbackend_p.h"
#include "contactconnection_simulator_p.h"

#include <qcontactmemorybackenddata_simulator_p.h>
#include <private/qsimulatordata_p.h>

#include <QTimer>
#include <QUuid>
#include <QSharedData>

QTM_BEGIN_NAMESPACE

using namespace QtSimulatorPrivate;
using namespace Simulator;

/*!
 * \class QContactSimulatorEngine
 * \brief This class provides a simulator-managed in-memory backend.
 */

/* static data for manager class */
QContactSimulatorEngine* QContactSimulatorEngine::engine = 0;

/*!
 * Factory function for 'creating' a simulator backend.
 *
 * Always returns the same backend. Creates it if it doesn't yet exist.
 */
QContactSimulatorEngine* QContactSimulatorEngine::createSimulatorEngine(const QMap<QString, QString>& parameters)
{
    Q_UNUSED(parameters);
    if (engine) {
        return engine;
    } else {
        QContactMemoryEngineData *data = new QContactMemoryEngineData();
        // it is essential that this gets the same manager uri as the one used on the simulator side
        data->m_id = QLatin1String("simulator");
        data->m_anonymous = false;
        engine = new QContactSimulatorEngine(data);
        return engine;
    }
}

QContactSimulatorEngine::QContactSimulatorEngine(QContactMemoryEngineData *data)
    : QContactMemoryEngine(data)
    , mNotifySimulator(true)
{
    ContactConnection::instance()->setEngine(this);
}

void QContactSimulatorEngine::setNotifySimulator(bool on)
{
    mNotifySimulator = on;
}

bool QContactSimulatorEngine::saveContact(QContact* theContact, QContactChangeSet& changeSet, QContactManager::Error* error)
{
    if (!mNotifySimulator)
        return QContactMemoryEngine::saveContact(theContact, changeSet, error);

    QLocalSocket *sendSocket = ContactConnection::instance()->sendSocket();

    // save remotely
    SaveContactReply reply = RemoteMetacall<SaveContactReply>::call(sendSocket, TimeoutSync,
                                                                    "requestSaveContact", *theContact);

    // if it failed, error
    if (reply.error != QContactManager::NoError)
        return false;

    // save locally
    QContactChangeSet backupSet = changeSet;
    if (!QContactMemoryEngine::saveContact(theContact, changeSet, error))
        return false;

    // if the local and remote contacts don't match, revert local add and error
    if (theContact->localId() != reply.savedContact.localId()) {
        QContactMemoryEngine::removeContact(theContact->localId(), changeSet, error);
        changeSet = backupSet;
        *error = QContactManager::LockedError;

        // ### revert remote?

        return false;
    }

    return true;
}

bool QContactSimulatorEngine::removeContact(const QContactLocalId& contactId, QContactChangeSet& changeSet, QContactManager::Error* error)
{
    if (!mNotifySimulator)
        return QContactMemoryEngine::removeContact(contactId, changeSet, error);

    QLocalSocket *sendSocket = ContactConnection::instance()->sendSocket();

    // remove remotely
    int errorVal = RemoteMetacall<int>::call(sendSocket, TimeoutSync,
                                             "requestRemoveContact", contactId);
    *error = static_cast<QContactManager::Error>(errorVal);

    // if it failed, error
    if (*error != QContactManager::NoError)
        return false;

    // remove locally
    return QContactMemoryEngine::removeContact(contactId, changeSet, error);
}

bool QContactSimulatorEngine::saveRelationship(QContactRelationship* relationship, QContactChangeSet& changeSet, QContactManager::Error* error)
{
    if (!mNotifySimulator)
        return QContactMemoryEngine::saveRelationship(relationship, changeSet, error);

    QLocalSocket *sendSocket = ContactConnection::instance()->sendSocket();

    // save remotely
    SaveRelationshipReply reply = RemoteMetacall<SaveRelationshipReply>::call(sendSocket, TimeoutSync,
                                                                              "requestSaveRelationship", *relationship);

    // if it failed, error
    if (reply.error != QContactManager::NoError)
        return false;

    // save locally
    QContactChangeSet backupSet = changeSet;
    if (!QContactMemoryEngine::saveRelationship(relationship, changeSet, error))
        return false;

    // if the local and remote local ids don't match, revert
    if (relationship->first().localId() != reply.savedRelationship.first().localId()
        || relationship->second().localId() != reply.savedRelationship.second().localId()) {
        QContactMemoryEngine::removeRelationship(*relationship, changeSet, error);
        changeSet = backupSet;
        *error = QContactManager::LockedError;

        // ### revert remote?

        return false;
    }

    return true;
}

bool QContactSimulatorEngine::removeRelationship(const QContactRelationship& relationship, QContactChangeSet& changeSet, QContactManager::Error* error)
{
    if (!mNotifySimulator)
        return QContactMemoryEngine::removeRelationship(relationship, changeSet, error);

    QLocalSocket *sendSocket = ContactConnection::instance()->sendSocket();

    // remove remotely
    int errorVal = RemoteMetacall<int>::call(sendSocket, TimeoutSync,
                                             "requestRemoveRelationship", relationship);
    *error = static_cast<QContactManager::Error>(errorVal);

    // if it failed, error
    if (*error != QContactManager::NoError)
        return false;

    // remove locally
    return QContactMemoryEngine::removeRelationship(relationship, changeSet, error);
}

#include "moc_qcontactsimulatorbackend_p.cpp"

QTM_END_NAMESPACE
