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

#ifndef QBLUETOOTHTRANSFERREPLY_H
#define QBLUETOOTHTRANSFERREPLY_H

#include <QtCore/QIODevice>

#include <qbluetoothtransferrequest.h>
#include <qbluetoothtransfermanager.h>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class Q_CONNECTIVITY_EXPORT QBluetoothTransferReply : public QIODevice
{
    Q_OBJECT

public:
    ~QBluetoothTransferReply();

    virtual void abort() = 0;
    QVariant attribute(QBluetoothTransferRequest::Attribute code) const;
    virtual bool isFinished() const = 0;
    virtual bool isRunning() const = 0;

    QBluetoothTransferManager *manager() const;

    QBluetoothTransferManager::Operation operation() const;

//    QBluetoothTransferRequest request() const;

    qint64 readBufferSize() const;
    virtual void setReadBufferSize(qint64 size);

signals:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void finished(QBluetoothTransferReply *);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

protected:
    explicit QBluetoothTransferReply(QObject *parent = 0);
    void setAttribute(QBluetoothTransferRequest::Attribute code, const QVariant &value);
    void setOperation(QBluetoothTransferManager::Operation operation);
    void setManager(QBluetoothTransferManager &manager);
//    void setRequest(QBluetoothTransferRequest *request);

private:
    QBluetoothTransferManager *m_manager;
    QBluetoothTransferManager::Operation m_operation;
    QMap<int, QVariant> m_attributes;
//    QBluetoothTransferRequest *m_request;
    qint64 m_buffersize;


};

QTM_END_NAMESPACE

QT_END_HEADER

#endif // QBLUETOOTHTRANSFERREPLY_H