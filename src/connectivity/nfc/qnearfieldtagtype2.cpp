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

#include "qnearfieldtagtype2.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QNearFieldTagType2
    \brief The QNearFieldTagType2 class provides an interface for communicating with an NFC Tag
           Type 2 tag.

    \ingroup connectivity-nfc
    \inmodule QtConnectivity
*/

/*!
    \fn Type QNearFieldTagType2::type() const
    \reimp
*/

/*!
    Constructs a new tag type 2 near field target with \a parent.
*/
QNearFieldTagType2::QNearFieldTagType2(QObject *parent)
:   QNearFieldTarget(parent)
{
}

/*!
    Returns the NFC Tag Type 2 specification version number that the tag supports.
*/
quint8 QNearFieldTagType2::version()
{
    selectSector(0);
    QByteArray data = readBlock(0);
    return data.at(13);
}

/*!
    Returns the memory size in bytes of the tag.
*/
int QNearFieldTagType2::memorySize()
{
    selectSector(0);
    QByteArray data = readBlock(0);
    return 8 * data.at(14);
}

/*!
    Reads and returns 4 bytes of data from the block specified by \a blockAddress. An empty byte
    array is returned if an error occurs.
*/
QByteArray QNearFieldTagType2::readBlock(quint8 blockAddress)
{
    QByteArray command;
    command.append(char(0x30));         // READ
    command.append(char(blockAddress)); // Block address

    const QByteArray response = sendCommand(command);

    if (response.isEmpty())
        return QByteArray();

    quint8 acknack = response.at(0);

    if (acknack != 0x0a)
        return QByteArray();

    return response;
}

/*!
    Writes 4 bytes of \a data to the block specified by \a blockAddress.

    Returns true on success; otherwise returns false.
*/
bool QNearFieldTagType2::writeBlock(quint8 blockAddress, const QByteArray &data)
{
    if (data.length() != 4)
        return false;

    QByteArray command;
    command.append(char(0xa2));         // WRITE
    command.append(char(blockAddress)); // Block address
    command.append(data);               // Data

    const QByteArray response = sendCommand(command);

    if (response.isEmpty())
        return false;

    quint8 acknack = response.at(0);

    return acknack == 0x0a;
}

/*!
    Selects the \a sector upon which subsequent readBlock() and writeBlock() operations will act.

    Returns true on success; otherwise returns false.
*/
bool QNearFieldTagType2::selectSector(quint8 sector)
{
    QByteArray command;
    command.append(char(0xc2));     // SECTOR SELECT (Command Packet 1)
    command.append(char(0xff));

    QByteArray response = sendCommand(command);

    if (response.isEmpty())
        return false;

    quint8 acknack = response.at(0);

    if (acknack != 0x0a)
        return false;

    command.clear();
    command.append(char(sector));               // Sector number
    command.append(QByteArray(3, char(0x00)));  // RFU

    response = sendCommand(command);

    // passive ack, empty response is ack
    return response.isEmpty();
}

#include "moc_qnearfieldtagtype2.cpp"

QTM_END_NAMESPACE
