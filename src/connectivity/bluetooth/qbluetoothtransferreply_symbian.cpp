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


#include "qbluetoothtransferreply_symbian_p.h"

#include <QDebug>

QTM_BEGIN_NAMESPACE

QBluetoothTransferReplySymbian::QBluetoothTransferReplySymbian(QIODevice *input, QObject *parent)
:   QBluetoothTransferReply(parent), m_source(input)
{
}

/*!
    Destroys the QBluetoothTransferReply object.
*/
QBluetoothTransferReplySymbian::~QBluetoothTransferReplySymbian()
{
}

void QBluetoothTransferReplySymbian::setAddress(const QBluetoothAddress &address)
{
    m_address = address;
}

bool QBluetoothTransferReplySymbian::start()
{
    return true;
}

/*!
    Returns true if this reply has finished; otherwise returns false.
*/
bool QBluetoothTransferReplySymbian::isFinished() const
{
    return m_finished;
}

/*!
    Returns true if this reply is running; otherwise returns false.
*/
bool QBluetoothTransferReplySymbian::isRunning() const
{
    return m_running;
}

void QBluetoothTransferReplySymbian::abort()
{
    qDebug() << "Abort() is not implemented";
}

qint64 QBluetoothTransferReplySymbian::readData(char*, qint64)
{
    return 0;
}

qint64 QBluetoothTransferReplySymbian::writeData(const char*, qint64)
{
    return 0;
}

QBluetoothTransferReply::TransferError QBluetoothTransferReplySymbian::error() const
{
    return NoError;
}

QString QBluetoothTransferReplySymbian::errorString() const
{
    return QString();
}


#include "moc_qbluetoothtransferreply_symbian_p.cpp"

QTM_END_NAMESPACE
