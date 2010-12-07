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


#include "qbluetoothtransferreply_bluez_p.h"
#include "qbluetoothaddress.h"

#include "bluez/obex_client_p.h"
#include "bluez/obex_manager_p.h"
#include "bluez/obex_agent_p.h"
#include "qbluetoothtransferreply.h"

#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>

#define AGENT_PATH "/qt/agent/test"

QTM_BEGIN_NAMESPACE

QBluetoothTransferReplyBluez::QBluetoothTransferReplyBluez(QIODevice *input, QObject *parent)
:   QBluetoothTransferReply(parent), tempfile(0), source(input), m_running(false), m_finished(false)
{
    client = new OrgOpenobexClientInterface(QLatin1String("org.openobex.client"), QLatin1String("/"),
                                           QDBusConnection::sessionBus());

//    manager = new OrgOpenobexManagerInterface(QLatin1String("org.openobex"), QLatin1String("/"),
//                                           QDBusConnection::sessionBus());

    agent = new AgentAdaptor(this);

    bool res = QDBusConnection::sessionBus().registerObject(AGENT_PATH, agent, QDBusConnection::ExportAllContents);
    res = QDBusConnection::sessionBus().registerService("org.qt.bt");
qDebug() << "Created dbus objects" << res;


}

/*!
    Destroys the QBluetoothTransferReply object.
*/
QBluetoothTransferReplyBluez::~QBluetoothTransferReplyBluez()
{
    delete client;
}

bool QBluetoothTransferReplyBluez::start()
{
    m_running = true;

    QFile *file = qobject_cast<QFile *>(source);

    if(!file){
qDebug() << "Not a QFile, making a copy";
        tempfile = new QTemporaryFile(this);
        tempfile->open();

        QFutureWatcher<bool> watcher;
        QObject::connect(&watcher, SIGNAL(finished()), this, SLOT(copyDone()));

        QFuture<bool> results = QtConcurrent::run(QBluetoothTransferReplyBluez::copyToTempFile, tempfile, source);
        watcher.setFuture(results);
    }
    else {
qDebug() << "QFile, not making a copy";
        startOPP(file->fileName());
    }
    return true;
}

bool QBluetoothTransferReplyBluez::copyToTempFile(QIODevice *to, QIODevice *from)
{
    char *block = new char[4096];
    int size;

    while((size = from->read(block, 4096))) {
        if(size != to->write(block, size)){
            return false;
        }
    }

    delete[] block;
qDebug() << "File copy done";
    return true;
}

void QBluetoothTransferReplyBluez::copyDone()
{
    startOPP(tempfile->fileName());
}

void QBluetoothTransferReplyBluez::startOPP(QString filename)
{
    QVariantMap device;
    QStringList files;

qDebug() << "Starting obex push" << filename;

    device.insert(QString::fromLatin1("Destination"), address.toString());
    files << filename;

    QDBusObjectPath path(QString::fromLocal8Bit(AGENT_PATH));
    QDBusPendingReply<> sendReply = client->SendFiles(device, files, path);
    sendReply.waitForFinished();
    if(sendReply.isError()){
        qDebug() << "Failed to send file" << sendReply.error().message();
        emit finished(this);
    }
qDebug() << "Send started reply: " << sendReply.isError() << path.path() << device << files;
}

void QBluetoothTransferReplyBluez::Complete(const QDBusObjectPath &in0)
{
    qDebug() << "Got complete: " << in0.path();
    m_finished = true;
    m_running = false;
    // done by Release()
//    emit finished(this);
}

void QBluetoothTransferReplyBluez::Error(const QDBusObjectPath &in0, const QString &in1)
{
    qDebug() << "Got error: " << in0.path() << in1;
    m_finished = true;
    m_running = false;
}

void QBluetoothTransferReplyBluez::Progress(const QDBusObjectPath &in0, qulonglong in1)
{
    qDebug() << "Got progress: " << in0.path() << in1;
}

void QBluetoothTransferReplyBluez::Release()
{
    qDebug() << "Got release";
    emit finished(this);
}

QString QBluetoothTransferReplyBluez::Request(const QDBusObjectPath &in0)
{
    qDebug() << "Got request" << in0.path();
    return QString();

}

/*!
    Returns true if this reply has finished; otherwise returns false.
*/
bool QBluetoothTransferReplyBluez::isFinished() const
{
    return m_finished;
}

/*!
    Returns true if this reply is running; otherwise returns false.
*/
bool QBluetoothTransferReplyBluez::isRunning() const
{
    return m_running;
}

void QBluetoothTransferReplyBluez::abort()
{
    qDebug() << "Abort() is not implemented";
}

void QBluetoothTransferReplyBluez::setAddress(const QBluetoothAddress &destination)
{
    address = destination;
}

qint64 QBluetoothTransferReplyBluez::readData(char*, qint64)
{
    return 0;
}

qint64 QBluetoothTransferReplyBluez::writeData(const char*, qint64)
{
    return 0;
}


#include "moc_qbluetoothtransferreply_bluez_p.cpp"

QTM_END_NAMESPACE
