/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "dialog.h"

#include <bluetooth/qbluetoothuuid.h>
#include <bluetooth/qrfcommserver.h>

#include <QTimer>

#include <QDebug>

Dialog::Dialog(QWidget *parent)
: QDialog(parent), m_connected(false), rfcommServer(0)
{
    setupUi(this);

    connect(quitButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(connectButton, SIGNAL(clicked()), this, SLOT(connectClicked()));
    connect(sendButton, SIGNAL(clicked()), this, SLOT(sendClicked()));
}

Dialog::~Dialog()
{
}

void Dialog::connectClicked()
{
    connectButton->setEnabled(false);
    server->setEnabled(false);
    client->setEnabled(false);

    if (m_connected) {
        // disconnect
        if (server->isChecked()) {
            // disconnect as server
            // unregister SDP record
            serviceInfo.unregisterService();

            // close socket
            closeSockets();

            connectButton->setText(tr("Connect"));
            connectButton->setEnabled(true);
            server->setEnabled(true);
            client->setEnabled(true);
        } else if (client->isChecked()) {
            // disconnect as client
            // close socket
            closeSockets();
            disconnectedFromServer();
        }
    } else {
        // connect
        if (server->isChecked()) {
            // connect as server
            if (!rfcommServer) {
                rfcommServer = new QRfcommServer(this);

                connect(rfcommServer, SIGNAL(newConnection()), this, SLOT(clientConnected()));

                rfcommServer->listen();
            }

            /* service classes */
            //serviceInfo.setServiceClass(0);

            /* service name, description and provider */
            serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, tr("Bt Chat Server"));
            serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceDescription, tr("Example bluetooth chat server"));
            serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceProvider, tr("Nokia, Qt"));

            /* discoverability */
            serviceInfo.setAttribute(QBluetoothServiceInfo::BrowseGroupList, QBluetoothUuid(QBluetoothUuid::PublicBrowseGroup));

            /* protocol descriptor list */
            QBluetoothServiceInfo::Sequence protocolDescriptorList;
            QBluetoothServiceInfo::Sequence protocol;
            protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::L2cap));
            protocolDescriptorList.append(QVariant::fromValue(protocol));
            protocol.clear();
            protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm)) << QVariant::fromValue(quint8(rfcommServer->serverPort()));
            protocolDescriptorList.append(QVariant::fromValue(protocol));
            serviceInfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList, protocolDescriptorList);

            serviceInfo.registerService();

            m_connected = true;
            connectButton->setText(tr("Disconnect"));
            connectButton->setEnabled(true);
        } else if (client->isChecked()) {
            // connect as client
            // scan for devices
            // scan for services
            // connect to service
            QBluetoothSocket *socket = new QBluetoothSocket(QBluetoothSocket::RfcommSocket);
            socket->connectToService(QBluetoothAddress("001167602023"), 10);

            connect(socket, SIGNAL(readyRead()), this, SLOT(readSocket()));
            connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten()));
            connect(socket, SIGNAL(connected()), this, SLOT(connectedToServer()));
            connect(socket, SIGNAL(disconnected()), this, SLOT(disconnectedFromServer()));

            activeSockets.append(socket);
        }
    }
}

void Dialog::sendClicked()
{
    sendButton->setEnabled(false);
    sendText->setEnabled(false);

    QString text = sendText->text() + "\n\r";

    foreach (QBluetoothSocket *socket, activeSockets) {
        socket->write(text.toLocal8Bit());
        waitForSend.append(socket);
    }
}

void Dialog::bytesWritten()
{
    waitForSend.removeAll(qobject_cast<QBluetoothSocket *>(sender()));

    if (waitForSend.isEmpty()) {
        sendText->clear();
        sendButton->setEnabled(true);
        sendText->setEnabled(true);
    }
}

void Dialog::closeSockets()
{
    while (!activeSockets.isEmpty()) {
        QBluetoothSocket *socket = activeSockets.takeFirst();
        delete socket;
    }

    waitForSend.clear();

    if (rfcommServer) {
        delete rfcommServer;
        rfcommServer = 0;
    }
}

void Dialog::clientConnected()
{
    QBluetoothSocket *socket = rfcommServer->nextPendingConnection();
    if (socket) {
        connect(socket, SIGNAL(readyRead()), this, SLOT(readSocket()));
        connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
        activeSockets.append(socket);
    }
}

void Dialog::clientDisconnected()
{
    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());

    activeSockets.removeAll(socket);
    waitForSend.removeAll(socket);

    socket->deleteLater();
}

void Dialog::readSocket()
{
    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());

    if (socket) {
        QByteArray data = socket->readAll();
        chat->insertPlainText(data);
    }
}

void Dialog::connectedToServer()
{
    m_connected = true;
    connectButton->setText(tr("Disconnect"));
    connectButton->setEnabled(true);
}

void Dialog::disconnectedFromServer()
{
    m_connected = false;
    connectButton->setText(tr("Connect"));
    connectButton->setEnabled(true);
    server->setEnabled(true);
    client->setEnabled(true);

    QTimer::singleShot(0, this, SLOT(closeSockets()));
}

