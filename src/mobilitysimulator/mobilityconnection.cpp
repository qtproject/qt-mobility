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

#include "mobilitysimulatorglobal.h"
#include "mobilityconnection_p.h"
#include "private/qsimulatorconnection_p.h"

#include <QtCore/QCoreApplication>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

QTM_BEGIN_NAMESPACE

using namespace QtSimulatorPrivate;

namespace Simulator {

static const VersionStruct mobilityVersion(1, 1, 0, 0);

MobilityConnection::MobilityConnection(QObject *parent)
    : QObject(parent)
    , mSendSocket(0)
    , mReceiveSocket(0)
{

}

MobilityConnection::~MobilityConnection()
{
    delete mReceiveSocket;
    mReceiveSocket = 0;
}

MobilityConnection *MobilityConnection::instance()
{
    static MobilityConnection *gInstance = 0;
    if (!gInstance) {
        // ensure the simulator is up and running before trying to
        // connect to its mobility subserver
        SimulatorConnection::instance();

        gInstance = new MobilityConnection();
        gInstance->connectToSimulator();
    }
    return gInstance;
}

void MobilityConnection::addMessageHandler(QObject *handler)
{
    mHandlers.append(handler);
}

QLocalSocket *MobilityConnection::sendSocket()
{
    return mSendSocket;
}

QLocalSocket *MobilityConnection::receiveSocket()
{
    return mReceiveSocket;
}

void MobilityConnection::onReadyRead()
{
    mReadBuffer += mReceiveSocket->readAll();
    forever {
        IncomingRemoteMetacall rpc;
        if (rpc.read(&mReadBuffer)) {
            // first, check if this can call a slot on this
            if (rpc.call(mReceiveSocket, this))
                continue;
            // then try the registered handlers until one matches
            bool handled = false;
            foreach (QObject *handler, mHandlers) {
                if (rpc.call(mReceiveSocket, handler)) {
                    handled = true;
                    break;
                }
            }
            if (handled)
                continue;
            qWarning("Ignoring a call to %s,", rpc.signature().data());
        }
        break;
    }
}

void MobilityConnection::connectToSimulator()
{
    // 1. check availability
    QLocalSocket *socket = new QLocalSocket;
    socket->connectToServer(QLatin1String(SIMULATOR_MOBILITY_SERVERNAME), QLocalSocket::ReadWrite);
    if (!socket->waitForConnected(1000)) {
        qFatal("Could not connect to mobility server");
        socket->deleteLater();
        return;
    }
    mSendSocket = socket;

    // 2. Create the local server used for initiating the backchannel.
    const qint64 pid = QCoreApplication::applicationPid();
    QLocalServer *server = new QLocalServer(this);
    if (!server->listen(qt_mobilityServerName(pid)))
        qFatal("Can't create local mobility server for this application.");

    // 3. Send initial application connect command.
    ApplicationConnectCommand command;
    ApplicationConnectCommand::Request &request = command.request;
    request.applicationPid = pid;
    request.applicationName[0] = 0;
    request.version = mobilityVersion;
    qt_sendAndReceiveApplicationConnect(socket, command);

    mSimulatorVersion = command.reply.version;

    // We usually want to get the initial state from the simulator directly, probably
    // before the event loop is started up. Hence we block until the simulator has established
    // the back channel.
    if (!server->waitForNewConnection(1000))
        qFatal("Simulator didn't establish mobility-backchannel on time");
    mReceiveSocket = server->nextPendingConnection();
    server->close();

    connect(mReceiveSocket, SIGNAL(readyRead()), SLOT(onReadyRead()));
}

} // end namespace Simulator

#include "moc_mobilityconnection_p.cpp"

QTM_END_NAMESPACE
