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
#include "qfeedback.h"

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
{
    qt_registerFeedbackTypes();

    mobilityCon->addMessageHandler(this);
}

Connection *Connection::instance()
{
    static Connection *connection = 0;

    if (!connection)
        connection = new Connection(MobilityConnection::instance());
    return connection;
}

void QtMobility::Simulator::Connection::connect(QFeedbackSimulator *plugin)
{
    Q_ASSERT(!mRegisteredWithSimulator);
    if (!mRegisteredWithSimulator) {
        mPlugin = plugin;
        RemoteMetacall<void>::call(mConnection->sendSocket(), NoSync, "setRequestsFeedback");
        mRegisteredWithSimulator = true;
        getInitialData();
    }
}

int QtMobility::Simulator::Connection::startEffect(int actuatorId, const QString &info, int duration)
{
    return RemoteMetacall<int>::call(mConnection->sendSocket(), TimeoutSync, "startFeedbackEffect",
                                     actuatorId, info, duration);
}

bool QtMobility::Simulator::Connection::resumeEffect(int effectId)
{
    return RemoteMetacall<bool>::call(mConnection->sendSocket(), TimeoutSync, "resumeFeedbackEffect",
                                      effectId);
}

bool QtMobility::Simulator::Connection::stopEffect(int effectId)
{
    return RemoteMetacall<bool>::call(mConnection->sendSocket(), TimeoutSync, "stopFeedbackEffect",
                                      effectId);
}

void QtMobility::Simulator::Connection::setEffectDuration(int effectId, int duration)
{
    RemoteMetacall<bool>::call(mConnection->sendSocket(), NoSync, "setFeedbackEffectDuration",
                               effectId, duration);
}

void QtMobility::Simulator::Connection::setActuatorEnabled(int actuatorId, bool enabled)
{
    RemoteMetacall<bool>::call(mConnection->sendSocket(), NoSync, "setFeedbackActuatorEnabled",
                               actuatorId, enabled);
}

void Connection::getInitialData()
{
    mInitialDataReceived = false;

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

void Connection::initialFeedbackDataSent()
{
    mInitialDataReceived = true;
}

void Connection::setActuator(QtMobility::ActuatorData data)
{
    mPlugin->setActuator(data);
}

void Connection::removeActuator(int id)
{
    // ### stop effects and such?
    mPlugin->removeActuator(id);
}

void QtMobility::Simulator::Connection::setDefaultActuator(int id)
{
    Q_ASSERT(mPlugin->mActuators.contains(id) || id == -1);
    mPlugin->mDefaultActuator = id;
}

void QtMobility::Simulator::Connection::setFeedbackEffectState(int effectId, int stateInt)
{
    QFeedbackEffect::State state = static_cast<QFeedbackEffect::State>(stateInt);
    mPlugin->forceEffectState(effectId, state);
}

#include "moc_connection_p.cpp"
