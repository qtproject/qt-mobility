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

#ifndef QBLUETOOTHTRANSFERREPLYSYMBIAN_H
#define QBLUETOOTHTRANSFERREPLYSYMBIAN_H

#include "qbluetoothtransferreply.h"
#include <e32base.h>
#include <obex.h>

#include <QTemporaryFile>
#include <QIODevice>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

_LIT( KBTSProtocol, "RFCOMM" ); // The Bluetooth transport layer

class QBluetoothTransferReplySymbian : public QBluetoothTransferReply, public CActive
{
    Q_OBJECT

public:
    enum state
        {
            EIdle,
            EConnecting,
            ESending,
            EDisconnecting
        };

    QBluetoothTransferReplySymbian(QIODevice *input, QObject *parent = 0);
    ~QBluetoothTransferReplySymbian();

    void abort();
    bool isFinished() const;
    bool isRunning() const;

    bool start();

    void setAddress(const QBluetoothAddress &address);

    QBluetoothTransferReply::TransferError error() const;
    QString errorString() const;

protected:
    qint64 readData(char*, qint64);
    qint64 writeData(const char*, qint64);

private:
    void sendObject(QString filename);
    void disconnect();

    //From CActive
    void DoCancel();
    void RunL();

    static bool copyToTempFile(QIODevice *to, QIODevice *from);

private:
    QIODevice *m_source;
    QTemporaryFile *m_tempfile;

    QBluetoothAddress m_address;

    bool m_running;
    bool m_finished;

    CObexClient* m_client;
    CObexFileObject* m_object;
    
    state m_state;

};

QTM_END_NAMESPACE

QT_END_HEADER

#endif // QBLUETOOTHTRANSFERREPLYPRIVATE_H
