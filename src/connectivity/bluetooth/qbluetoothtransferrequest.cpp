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

#include "qbluetoothtransferrequest.h"
#include "qbluetoothaddress.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QBluetoothTransferRequest
    \brief The QBluetoothTransferRequest class holds a request to be sent with
           QBluetoothTransferManager.

    \ingroup connectivity-bluetooth
    \inmodule QtConnectivity

    QBluetoothTransferRequest is part of the Bluetooth Transfer API and is the class holding the
    information necessary to initiate a transfer over Bluetooth.

    \sa QBluetoothTransferReply, QBluetoothTransferManager
*/

/*!
    \enum QBluetoothTransferRequest::Attribute

    Attribute codes for QBluetoothTransferRequest and QBluetoothTransferReply.

    \value DescriptionAttribute A textural description of the object being transfered. May be
                                display in the UI of the remote device.
    \value TimeAttribute        Time attribute of the object being transfered.
    \value TypeAttribute        MIME type of the object being transfered.
    \value LengthAttribute      Length in bytes of the object being transfered.
    \value NameAttribute        Name of the object being transfered. May be displayed in the UI of
                                the remote device.
*/

/*!
    Constructs a new Bluetooth transfer request to the device wit address \a address.
*/
QBluetoothTransferRequest::QBluetoothTransferRequest(const QBluetoothAddress &address)
{
    Q_UNUSED(address);
}

/*!
    Constructs a new Bluetooth transfer request that is a copy of \a other.
*/
QBluetoothTransferRequest::QBluetoothTransferRequest(const QBluetoothTransferRequest &other)
{
    Q_UNUSED(other);
}

/*!
    Destorys the Bluetooth transfer request.
*/
QBluetoothTransferRequest::~QBluetoothTransferRequest()
{
}

/*!
    Returns the attribute associated with code \a code. If the attribute has not been set, it
    returns \a defaultValue.

    \sa setAttribute(), QBluetoothTransferRequest::Attribute
*/
QVariant QBluetoothTransferRequest::attribute(Attribute code, const QVariant &defaultValue) const
{
    Q_UNUSED(code);

    return defaultValue;
}

/*!
    Sets the attribute associated with code \a code to be value \a value. If the attribute is
    already set, the previous value is discarded. If \a value is an invalid QVariant, the attribute
    is unset.

    \sa attribute(), QBluetoothTransferRequest::Attribute
*/
void QBluetoothTransferRequest::setAttribute(Attribute code, const QVariant &value)
{
    Q_UNUSED(code);
    Q_UNUSED(value);
}

/*!
    Returns false if this object is not the same as \a other.

    \sa operator==()
*/
bool QBluetoothTransferRequest::operator!=(const QBluetoothTransferRequest &other) const
{
    Q_UNUSED(other);

    return false;
}

/*!
    Creates a copy of \a other.
*/
QBluetoothTransferRequest &QBluetoothTransferRequest::operator=(const QBluetoothTransferRequest &other)
{
    Q_UNUSED(other);

    return *this;
}

/*!
    Returns true if this object is the same as \a other.
*/
bool QBluetoothTransferRequest::operator==(const QBluetoothTransferRequest &other) const
{
    Q_UNUSED(other);

    return false;
}

QTM_END_NAMESPACE
