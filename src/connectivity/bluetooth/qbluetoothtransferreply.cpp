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


#include "qbluetoothtransferreply.h"
#include "qbluetoothaddress.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QBluetoothTransferReply
    \brief The QBluetoothTransferReply class contains the data and headers for a request sent with
           QBluetoothTranferManager.

    \ingroup connectivity-bluetooth
    \inmodule QtConnectivity

    In additional to a copy of the QBluetoothTransferRequest object used to create the request,
    QBluetoothTransferReply contains the contents of the reply itself.

    QBluetoothTransferReply is a sequential-access QIODevice, which means that once data is read
    from the object, it no longer kept by the device. It is therefore the application's
    responsibility to keep this data if it needs to. Whenever more data is received and processed,
    the readyRead() signal is emitted.

    The downloadProgress() signal is also emitted when data is received, but the number of bytes
    contained in it may not represent the actual bytes received, if any transformation is done to
    the contents (for example, decompressing and removing the protocol overhead).

    Even though QBluetoothTransferReply is a QIODevice connected to the contents of the reply, it
    also emits the uploadProgress() signal, which indicates the progress of the upload for
    operations that have such content.
*/

/*!
    \enum QBluetoothTransferReply::TransferError

    This enum describes the type of error that occured

    \value NoError          No error.
    \value UnknownError     Unknown error, no better enum available
    \value FileNotFoundError Unable to open the file specified
    \value HostNotFoundError Unable to connect to the target host
    \value UserCancelledTransferError User terminated the transfer
*/



/*!
    \fn QBluetoothTransferReply::abort()

    Aborts this reply.
*/
void QBluetoothTransferReply::abort()
{

}

/*!
    \fn void QBluetoothTransferReply::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)

    This signal is emitted whenever data is received. The \a bytesReceived parameter contains the
    total number of bytes received so far out of \a bytesTotal expected for the entire transfer.
*/

/*!
    \fn void QBluetoothTransferReply::finished(QBluetoothTransferReply *reply)

    This signal is emitted when the transfer is complete for \a reply.
*/

/*!
    \fn void QBluetoothTransferReply::uploadProgress(qint64 bytesSent, qint64 bytesTotal)

    This signal is emitted whenever data is sent. The \a bytesSent parameter contains the total
    number of bytes sent so far out of \a bytesTotal.
*/

/*!
    Constructs a new QBluetoothTransferReply with parent \a parent.
*/
QBluetoothTransferReply::QBluetoothTransferReply(QObject *parent)
:   QObject(parent)
{
}

/*!
    Destroys the QBluetoothTransferReply object.
*/
QBluetoothTransferReply::~QBluetoothTransferReply()
{
}

/*!
    Returns the attribute associated with the code \a code. If the attribute has not been set, it
    returns an invalid QVariant.
*/
QVariant QBluetoothTransferReply::attribute(QBluetoothTransferRequest::Attribute code) const
{
    return m_attributes[code];
}

/*!
   \fn bool QBluetoothTransferReply::isFinished() const

    Returns true if this reply has finished; otherwise returns false.
*/
//bool QBluetoothTransferReply::isFinished() const
//{
//    return false;
//}

/*!
   \fn bool QBluetoothTransferReply::isRunning() const

    Returns true if this reply is running; otherwise returns false.
*/
//bool QBluetoothTransferReply::isRunning() const
//{
//    return false;
//}

/*!
    Returns the QBluetoothTransferManager that was used to create this QBluetoothTransferReply
    object.
*/
QBluetoothTransferManager *QBluetoothTransferReply::manager() const
{
    return m_manager;
}

/*!
    Returns the type of operation that this reply is for.
*/
QBluetoothTransferManager::Operation QBluetoothTransferReply::operation() const
{
    return m_operation;
}

/*
    Returns a copy of the QBluetoothTransferRequest object used to create this
    QBluetoothTransferReply.
*/
//QBluetoothTransferRequest QBluetoothTransferReply::request() const
//{
//    return QBluetoothTransferRequest(QBluetoothAddress());
//}

/*!
    Sets the operation of this QBluetoothTransferReply to \a operation.
*/
void QBluetoothTransferReply::setOperation(QBluetoothTransferManager::Operation operation)
{
    m_operation = operation;
}

/*!
  \fn QBluetoothTransferReply::setAttribute(QBluetoothTransferRequest::Attribute code, const QVariant &value)

    Set the attribute associated with the code \a code to the value \a value.
*/

/*!
  \fn QBluetoothTransferReply::setManager(QBluetoothTransferManager &manager)

  Set the reply's manager to manager \a manager.
*/


/*!
  \fn QString QBluetoothTransferReply::errorString() const

  String describing the error.  Can be displayed to the user.
*/

#include "moc_qbluetoothtransferreply.cpp"

QTM_END_NAMESPACE
