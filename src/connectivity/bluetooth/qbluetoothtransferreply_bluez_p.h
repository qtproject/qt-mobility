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

#ifndef QBLUETOOTHTRANSFERREPLY_BLUEZ_P_H
#define QBLUETOOTHTRANSFERREPLY_BLUEZ_P_H

#include <QtCore/QIODevice>
#include <QtDBus/QtDBus>

#include <qbluetoothtransferrequest.h>
#include <qbluetoothtransfermanager.h>

#include "qbluetoothtransferreply.h"

QT_BEGIN_HEADER

class OrgOpenobexClientInterface;
class OrgOpenobexManagerInterface;
class AgentAdaptor;

QTM_BEGIN_NAMESPACE

class Q_CONNECTIVITY_EXPORT QBluetoothTransferReplyBluez : public QBluetoothTransferReply
{
    Q_OBJECT

public:
    QBluetoothTransferReplyBluez(QIODevice *input, QObject *parent = 0);
    ~QBluetoothTransferReplyBluez();

    QVariant attribute(QBluetoothTransferRequest::Attribute code) const;
    bool isFinished() const;
    bool isRunning() const;

    bool start();

    void startOPP(QString filename);

    void setAddress(const QBluetoothAddress &address);

    QBluetoothTransferReply::TransferError error() const;
    QString errorString() const;

protected:
    qint64 readData(char*, qint64);
    qint64 writeData(const char*, qint64);

private:
    OrgOpenobexClientInterface *client;
    OrgOpenobexManagerInterface *manager;
    AgentAdaptor *agent;

    QTemporaryFile *tempfile;
    QIODevice *source;

    bool m_running;
    bool m_finished;

    quint64 m_size;

    QBluetoothAddress address;

    QBluetoothTransferReply::TransferError m_error;
    QString m_errorStr;

    QString m_agent_path;

    QString m_transfer_path;

    static bool copyToTempFile(QIODevice *to, QIODevice *from);

private slots:
    void copyDone();

public slots:
    void abort();
    void Complete(const QDBusObjectPath &in0);
    void Error(const QDBusObjectPath &in0, const QString &in1);
    void Progress(const QDBusObjectPath &in0, qulonglong in1);
    void Release();
    QString Request(const QDBusObjectPath &in0);
    void sendReturned(QDBusPendingCallWatcher*);

};

QTM_END_NAMESPACE

QT_END_HEADER

#endif // QBLUETOOTHTRANSFERREPLY_H
