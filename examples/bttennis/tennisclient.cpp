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

#include "tennisclient.h"

#include <qbluetoothsocket.h>

#include <QtCore/QDataStream>
#include <QtCore/QByteArray>

TennisClient::TennisClient(QObject *parent)
:   QObject(parent), socket(0), stream(0)
{
}

TennisClient::~TennisClient()
{
    stopClient();
}

//! [startClient]
void TennisClient::startClient(const QBluetoothServiceInfo &remoteService)
{
    if (socket)
        return;

    serviceInfo = remoteService;

    // Connect to service
    socket = new QBluetoothSocket(QBluetoothSocket::L2capSocket);
    qDebug() << "Create socket";
    socket->connectToService(remoteService);
    qDebug() << "ConnecttoService done";

    connect(socket, SIGNAL(readyRead()), this, SLOT(readSocket()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(error(QBluetoothSocket::SocketError)));

    stream = new QDataStream(socket);
}
//! [startClient]

//! [stopClient]
void TennisClient::stopClient()
{
    delete stream;
    stream = 0;

    delete socket;
    socket = 0;
}
//! [stopClient]

//! [readSocket]
void TennisClient::readSocket()
{
    if (!socket)
        return;

    while (socket->bytesAvailable()) {
        QChar c;
        QChar eol;

//        qDebug() << socket->readAll().length();

        *stream >> c;
        if(c == QChar('m')) {
            int x, y;
            *stream >> x >> y;
            emit moveBall(x, y);
        }
        else if(c == QChar('s')){
            int left, right;
            *stream >> left >> right;
            emit score(left, right);
        }
        else if(c == QChar('l')){
            int y;
            *stream >> y;
            emit moveLeftPaddle(y);
        }
        else {
            qDebug() << "Unkown command" << c;
        }
    }
}
//! [readSocket]

//! [moveRightPaddle]
void TennisClient::moveRightPaddle(int y)
{
    if(stream) {
        QByteArray b;
        QDataStream s(&b, QIODevice::WriteOnly);
        s << QChar('r') << y;
        socket->write(b);
    }
}
//! [moveRightPaddle]

//! [connected]
void TennisClient::connected()
{
    emit connected(socket->peerName());
}
//! [connected]

void TennisClient::error(QBluetoothSocket::SocketError err)
{
    qDebug() << Q_FUNC_INFO << "error" << err;
}
