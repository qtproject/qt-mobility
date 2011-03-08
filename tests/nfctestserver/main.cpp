/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qllcpsocket.h>
#include <qllcpserver.h>

#include <QtCore/QCoreApplication>

QTM_USE_NAMESPACE

static const QLatin1String commandServer("urn:nfc:sn:com.nokia.qtmobility.commandserver");
static const QLatin1String helloServer("urn:nfc:sn:com.nokia.qtmobility.helloserver");
static const QLatin1String streamSuffix(".stream");
static const QLatin1String datagramSuffix(".datagram");

class SocketController : public QObject
{
    Q_OBJECT

public:
    enum ConnectionType {
        StreamConnection,
        DatagramConnection
    };

    SocketController(ConnectionType type, QObject *parent = 0);
    ~SocketController();

public slots:
    void connected();
    void disconnected();
    void error(QLlcpSocket::SocketError socketError);
    void stateChanged(QLlcpSocket::SocketState socketState);
    void readyRead();

private:
    QLlcpSocket *m_socket;
    ConnectionType m_connectionType;
    QString m_service;
};

SocketController::SocketController(ConnectionType type, QObject *parent)
:   QObject(parent), m_socket(new QLlcpSocket(this)), m_connectionType(type)
{
    connect(m_socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(m_socket, SIGNAL(error(QLlcpSocket::SocketError)),
            this, SLOT(error(QLlcpSocket::SocketError)));
    connect(m_socket, SIGNAL(stateChanged(QLlcpSocket::SocketState)),
            this, SLOT(stateChanged(QLlcpSocket::SocketState)));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    switch (m_connectionType) {
    case StreamConnection:
        m_service = helloServer + streamSuffix;
        break;
    case DatagramConnection:
        m_service = helloServer + datagramSuffix;
        break;
    }

    qDebug() << "Client connecting to" << m_service;
    m_socket->connectToService(0, m_service);
}

SocketController::~SocketController()
{
    delete m_socket;
}

void SocketController::connected()
{
    qDebug() << "Client connected";
    const QString data = QLatin1String("HELLO ") + m_service;
    switch (m_connectionType) {
    case StreamConnection:
        m_socket->write(data.toUtf8() + '\n');
        break;
    case DatagramConnection:
        m_socket->writeDatagram(data.toUtf8());
        break;
    }
}

void SocketController::disconnected()
{
    qDebug() << "Client disconnected, reconnecting";

    m_socket->connectToService(0, m_service);
}

void SocketController::error(QLlcpSocket::SocketError socketError)
{
    qDebug() << "Client got error:" << socketError;
}

void SocketController::stateChanged(QLlcpSocket::SocketState socketState)
{
    qDebug() << "Client state changed to" << socketState;
}

void SocketController::readyRead()
{
    switch (m_connectionType) {
    case StreamConnection:
        while (m_socket->canReadLine()) {
            const QByteArray line = m_socket->readLine().trimmed();

            qDebug() << "Client read line:" << line;

            if (line == "DISCONNECT") {
                m_socket->disconnectFromService();
                break;
            } else if (line == "CLOSE") {
                m_socket->close();
                break;
            }
        }
        break;
    case DatagramConnection:
        while (m_socket->hasPendingDatagrams()) {
            qint64 size = m_socket->pendingDatagramSize();
            QByteArray data;
            data.resize(size);
            m_socket->readDatagram(data.data(), data.size());

            if (data == "DISCONNECT") {
                m_socket->disconnectFromService();
                break;
            } else if (data == "CLOSE") {
                m_socket->close();
                break;
            }
        }
    }
}

class ServerController : public QObject
{
    Q_OBJECT

public:
    enum ConnectionType {
        StreamConnection,
        DatagramConnection
    };

    ServerController(ConnectionType type, QObject *parent = 0);
    ~ServerController();

private slots:
    void newConnection();

    void socketReadyRead();
    void socketBytesWritten(qint64 bytes);
    void socketDisconnected();

private:
    QLlcpServer *m_server;
    QLlcpSocket *m_socket;
    ConnectionType m_connectionType;
    QString m_service;
};

ServerController::ServerController(ConnectionType type, QObject *parent)
:   QObject(parent), m_server(new QLlcpServer(this)), m_socket(0), m_connectionType(type)
{
    connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    switch (m_connectionType) {
    case StreamConnection:
        m_service = commandServer + streamSuffix;
        break;
    case DatagramConnection:
        m_service = commandServer + datagramSuffix;
        break;
    }

    m_server->listen(m_service);

    if (m_server->isListening())
        qDebug() << "Server listening on" << m_service;
}

ServerController::~ServerController()
{
    delete m_socket;
    delete m_server;
}

void ServerController::newConnection()
{
    m_socket = m_server->nextPendingConnection();

    qDebug() << "Server got new connection";

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
    connect(m_socket, SIGNAL(bytesWritten(qint64)), this, SLOT(socketBytesWritten(qint64)));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
}

void ServerController::socketReadyRead()
{
    switch (m_connectionType) {
    case StreamConnection:
        while (m_socket->canReadLine()) {
            const QByteArray line = m_socket->readLine().trimmed();

            qDebug() << "Server read line:" << line;

            QByteArray command;
            QByteArray parameter;

            int index = line.indexOf(' ');
            if (index >= 0) {
                command = line.left(index);
                parameter = line.mid(index + 1);
            } else {
                command = line;
            }

            if (command == "ECHO") {
                m_socket->write(parameter + '\n');
            } else if (command == "DISCONNECT") {
                m_socket->disconnectFromService();
                break;
            } else if (command == "CLOSE") {
                m_socket->close();
                break;
            } else if (command == "URI") {
                m_socket->write(m_service.toLatin1());
                m_socket->write("\n");
            }
        }
        break;
    case DatagramConnection:
        while (m_socket->hasPendingDatagrams()) {
            qint64 size = m_socket->pendingDatagramSize();
            QByteArray data;
            data.resize(size);
            m_socket->readDatagram(data.data(), data.size());

            QByteArray command;
            QByteArray parameter;

            int index = data.indexOf(' ');
            if (index >= 0) {
                command = data.left(index);
                parameter = data.mid(index + 1);
            } else {
                command = data;
            }

            if (command == "ECHO") {
                m_socket->writeDatagram(parameter);
            } else if (command == "DISCONNECT") {
                m_socket->disconnectFromService();
                break;
            } else if (command == "CLOSE") {
                m_socket->close();
                break;
            } else if (command == "URI") {
                m_socket->writeDatagram(m_service.toLatin1());
            }
        }
        break;
    }
}

void ServerController::socketBytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
}

void ServerController::socketDisconnected()
{
    m_socket->deleteLater();
    m_socket = 0;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    new ServerController(ServerController::StreamConnection, &app);
    //new ServerController(ServerController::DatagramConnection, &app);
    new SocketController(SocketController::StreamConnection, &app);
    new SocketController(SocketController::DatagramConnection, &app);

    return app.exec();
}

#include "main.moc"
