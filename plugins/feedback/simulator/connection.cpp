/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
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

bool QtMobility::Simulator::Connection::pauseEffect(int effectId)
{
    return RemoteMetacall<bool>::call(mConnection->sendSocket(), TimeoutSync, "pauseFeedbackEffect",
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
    mPlugin->emitStateChanged(effectId);
}

#include "moc_connection_p.cpp"
