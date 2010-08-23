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

#include "qbluetoothtransfermanager.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QBluetoothTransferManager
    \brief The QBluetoothTransferManager class allows the application to send data objects to other
           devices.

    \ingroup connectivity-bluetooth
    \inmodule QtConnectivity

    QBluetoothTransferManager uses OBEX to send get and put commands to remote devices.
*/

/*!
    \enum QBluetoothTransferManager::Operation

    This enum describes the type of operation that a transfer request is for.

    \value GetOperation     The get operation is used to retrieve an object from a remote device.
    \value PutOperation     The put operation is used to send an object to a remote device.
*/

/*!
    \fn void QBluetoothTransferManager::finished(QBluetoothTransferReply *reply)

    This signal is emitted when the transfer for \a reply finishes.
*/

/*!
    Constructs a new QBluetoothTransferManager with \a parent.
*/
QBluetoothTransferManager::QBluetoothTransferManager(QObject *parent)
:   QObject(parent)
{
}

/*!
    Destroys the QBluetoothTransferManager.
*/
QBluetoothTransferManager::~QBluetoothTransferManager()
{
}

/*!
    Sends the contents of \a data to the remote device \a request and returns a new
    QBluetoothTransferReply, that can be used to track the request's progress.
*/
QBluetoothTransferReply *QBluetoothTransferManager::put(const QBluetoothTransferRequest &request,
                                                        QIODevice *data)
{
    Q_UNUSED(request);
    Q_UNUSED(data);

    return 0;
}

/*!
    Sends a request for the contents of the target \a request. Returns a QBluetoothTransferReply,
    that can be used to track the request's progress.
*/
QBluetoothTransferReply *QBluetoothTransferManager::get(const QBluetoothTransferRequest &request)
{
    Q_UNUSED(request);

    return 0;
}

#include "moc_qbluetoothtransfermanager.cpp"

QTM_END_NAMESPACE
