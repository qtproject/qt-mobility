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

#include "qbluetoothaddress.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QBluetoothAddress
    \brief The QBluetoothAddress class provides a Bluetooth address.

    This class holds a Bluetooth address in a platform- and protocol- independent manner.
*/

/*!
    \fn inline bool QBluetoothAddress::operator!=(const QBluetoothAddress &other) const

    Compares this Bluetooth address with \a other.

    Returns true if the Bluetooth addresses are not equal, otherwise returns true.
*/

/*!
    Constructs an invalid Bluetooth address.
*/
QBluetoothAddress::QBluetoothAddress()
: m_valid(false), m_address(0)
{
}

/*!
    Constructs a new Bluetooth address and assigns \a address to it.
*/
QBluetoothAddress::QBluetoothAddress(quint64 address)
: m_valid(true), m_address(address)
{
}

/*!
    Constructs a new Bluetooth address and assigns \a address to it.

    The format of \a address can be either XX:XX:XX:XX:XX:XX or XXXXXXXXXXXX,
    where X is a hexadecimal digit.  Case is not important.
*/
QBluetoothAddress::QBluetoothAddress(const QString &address)
{
    QString a = address;

    if (a.length() == 17)
        a.replace(':', QByteArray());

    if (a.length() == 12) {
        m_address = a.toULongLong(&m_valid, 16);
    } else {
        m_valid = false;
        m_address = 0;
    }
}

/*!
    Constructs a new Bluetooth address which is a copy of \a other.
*/
QBluetoothAddress::QBluetoothAddress(const QBluetoothAddress &other)
{
    m_valid = other.m_valid;
    m_address = other.m_address;
}

/*!
    Assigns \a other to this Bluetooth address.
*/
QBluetoothAddress &QBluetoothAddress::operator=(const QBluetoothAddress &other)
{
    m_valid = other.m_valid;
    m_address = other.m_address;

    return *this;
}

/*!
    Returns true if the Bluetooth address is valid, otherwise returns false.
*/
bool QBluetoothAddress::isValid() const
{
    return m_valid;
}

/*!
    Returns true if the Bluetooth address is less than \a other; otherwise
    returns false.

    Invalid Bluetooth addresses are always less than valid addresses.
*/
bool QBluetoothAddress::operator<(const QBluetoothAddress &other) const
{
    if (!m_valid && !other.m_valid)
        return false;

    if (m_valid && !other.m_valid)
        return false;

    if (!m_valid && other.m_valid)
        return true;

    return m_address < other.m_address;
}

/*!
    Compares this Bluetooth address to \a other.

    Returns true if the Bluetooth address are equal, otherwise returns false.
*/
bool QBluetoothAddress::operator==(const QBluetoothAddress &other) const
{
    if (!m_valid && !other.m_valid)
        return true;

    if (m_valid != other.m_valid)
        return false;

    return m_address == other.m_address;
}

/*!
    Returns this Bluetooth address as a quint64.
*/
quint64 QBluetoothAddress::toUInt64() const
{
    if (!m_valid)
        return 0;

    return m_address;
}

/*!
    Returns the Bluetooth address as a string of the form XX:XX:XX:XX:XX:XX.
*/
QString QBluetoothAddress::toString() const
{
    if (!m_valid)
        return QString();

    QString s("%1:%2:%3:%4:%5:%6");

    for (int i = 5; i >= 0; --i) {
        const quint8 a = (m_address >> (i*8)) & 0xff;
        s = s.arg(a, 2, 16, QLatin1Char('0'));
    }

    return s.toUpper();
}

QTM_END_NAMESPACE
