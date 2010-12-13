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
#include <QFile>

QTM_BEGIN_NAMESPACE

QBluetoothTransferReplySymbian::QBluetoothTransferReplySymbian(QIODevice *input, QObject *parent)
:   QBluetoothTransferReply(parent), m_source(input), CActive(EPriorityStandard), m_client(NULL),
    m_running(false), m_finished(false)
{
}

/*!
    Destroys the QBluetoothTransferReply object.
*/
QBluetoothTransferReplySymbian::~QBluetoothTransferReplySymbian()
{
    Cancel();

    delete m_object;
    m_object = NULL;

    delete m_client;
    m_client = NULL;

}

void QBluetoothTransferReplySymbian::setAddress(const QBluetoothAddress &address)
{
    m_address = address;
}

bool QBluetoothTransferReplySymbian::start()
{
    m_running = true;

    TObexBluetoothProtocolInfo protocolInfo;
    TBTDevAddr deviceAddress(m_address.toUInt64());

    protocolInfo.iTransport.Copy(KBTSProtocol);
    protocolInfo.iAddr.SetBTAddr(deviceAddress);
    //protocolInfo.iAddr.SetPort( /*set port here*/ );

    if ( m_client ) {
        delete m_client;
        m_client = NULL;
    }

    TRAPD( err, m_client = CObexClient::NewL(protocolInfo));
    if (err)
        return false;

    m_client->Connect( iStatus );

    m_state = EConnecting;
    SetActive();
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

void QBluetoothTransferReplySymbian::DoCancel()
{
    // Deleting obexclient is the only way to cancel active requests
    if ( m_client ) {
        delete m_client;
        m_client = NULL;
        m_state = EIdle;
    }
}

void QBluetoothTransferReplySymbian::RunL()
{
    switch ( m_state ) {
        case EConnecting: {
            m_state = ESending;
            QFile *file = qobject_cast<QFile *>(m_source);

            if (file) {
                sendObject(file->fileName());
            } else {
               if (copyToTempFile(m_tempfile, m_source))
                   sendObject(m_tempfile->fileName());
            }
            break;
        }
        case ESending: {
            m_state = EDisconnecting;
            disconnect();
            break;
        }
        case EDisconnecting: {
            m_state = EIdle;
            m_finished = true;
            m_running = false;
            break;
        }
        case EIdle:
        default:
            break;
    }

}

void QBluetoothTransferReplySymbian::sendObject(QString filename)
{
    delete m_object;
    m_object = NULL;
    TRAPD(err, m_object = CObexFileObject::NewL());
    if (!err) {
        TPtrC16 str(reinterpret_cast<const TUint16*>(filename.utf16()));
        TRAPD(error, m_object->InitFromFileL( str ));
        if (!error) {
            m_client->Put( *m_object, iStatus );
            SetActive();
        }
    }
}

void QBluetoothTransferReplySymbian::disconnect()
{
    if ( m_state == ESending ) {
        m_client->Disconnect( iStatus );
        SetActive();
    }
}

bool QBluetoothTransferReplySymbian::copyToTempFile(QIODevice *to, QIODevice *from)
{
    char *block = new char[4096];
    int size;

    while ((size = from->read(block, 4096))) {
        if (size != to->write(block, size)) {
            return false;
        }
    }

    delete[] block;
    return true;
}

#include "moc_qbluetoothtransferreply_symbian_p.cpp"

QTM_END_NAMESPACE
