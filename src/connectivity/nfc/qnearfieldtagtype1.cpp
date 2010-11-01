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

#include "qnearfieldtagtype1.h"
#include "qndefmessage.h"
#include "qtlvreader_p.h"

#include <QtCore/QByteArray>
#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \class QNearFieldTagType1
    \brief The QNearFieldTagType1 class provides an interface for communicating with an NFC Tag
           Type 1 tag.

    \ingroup connectivity-nfc
    \inmodule QtConnectivity
*/

/*!
    \enum QNearFieldTagType1::WriteMode
    \brief This enum describes the write modes that are supported.

    \value EraseAndWrite    The memory is erased before the new value is written.
    \value WriteOnly        The memory is not erased before the new value is written. The effect of
                            this mode is that the final value store is the bitwise or of the data
                            to be written and the original data value.
*/

/*!
    \fn Type QNearFieldTagType1::type() const
    \reimp
*/

/*!
    Constructs a new tag type 1 near field target with \a parent.
*/
QNearFieldTagType1::QNearFieldTagType1(QObject *parent)
:   QNearFieldTarget(parent)
{
}

/*!
    \reimp
*/
bool QNearFieldTagType1::hasNdefMessage()
{
    const QByteArray data = readAll();

    if (data.isEmpty())
        return false;

    quint8 hr0 = data.at(0);

    // Check if target is a NFC TagType1 tag
    if (!(hr0 & 0x10))
        return false;

    // Check if NDEF Message Magic number is present
    quint8 nmn = data.at(10);
    if (nmn != 0xe1)
        return false;

    // Check if TLV contains NDEF Message
    return true;
}

/*!
    \reimp
*/
QList<QNdefMessage> QNearFieldTagType1::ndefMessages()
{
    const QByteArray id = readIdentification();

    // Check if target is a NFC TagType1 tag
    quint8 hr0 = id.at(0);
    if (!(hr0 & 0x10))
        return QList<QNdefMessage>();

    if (readByte(8) != 0xe1)
        return QList<QNdefMessage>();

    QTlvReader reader(this);

    QList<QNdefMessage> ndefMessages;

    while (!reader.atEnd()) {
        reader.readNext();

        // NDEF Message TLV
        if (reader.tag() == 0x03)
            ndefMessages.append(QNdefMessage::fromByteArray(reader.data()));
    }

    return ndefMessages;
}

/*!
    Returns the NFC Tag Type 1 specification version number that the tag supports.
*/
quint8 QNearFieldTagType1::version()
{
    return readByte(9);
}

/*!
    Returns the memory size in bytes of the tag.
*/
int QNearFieldTagType1::memorySize()
{
    quint8 tms = readByte(10);

    return 8 * (tms + 1);
}

/*!
    Returns identification information read from the tag.

    The returned byte array contains HR0, HR1, UID0, UID1, UID2, UID3 in order. An empty byte array
    is returned if an error occurs.
*/
QByteArray QNearFieldTagType1::readIdentification()
{
    QByteArray command;
    command.append(char(0x78));     // RID
    command.append(char(0x00));     // Address (unused)
    command.append(char(0x00));     // Data (unused)
    command.append(uid().left(4));  // 4 bytes of UID

    return sendCommand(command);
}

/*!
    Reads and returns HR0, HR1 and all data in the static memory area of the tag.

    The returned byte array contains HR0, HR1 followed by 120 bytes of static data. An empty byte
    array is returned if an error occurs.
*/
QByteArray QNearFieldTagType1::readAll()
{
    QByteArray command;
    command.append(char(0x00));   // RALL
    command.append(char(0x00));   // Address (unused)
    command.append(char(0x00));   // Data (unused)
    command.append(uid().left(4));// 4 bytes of UID

    return sendCommand(command);
}

/*!
    Reads and returns a single byte from the static memory area of the tag. The \a address
    parameter specifices the linear byte address to read.
*/
quint8 QNearFieldTagType1::readByte(quint8 address)
{
    if (address & 0x80)
        return 0;

    QByteArray command;
    command.append(char(0x01));     // READ
    command.append(char(address));  // Address
    command.append(char(0x00));     // Data (unused)
    command.append(uid().left(4));  // 4 bytes of UID

    const QByteArray response = sendCommand(command);

    if (response.isEmpty())
        return 0;

    if (response.at(0) != address)
        qDebug() << Q_FUNC_INFO << "response was for wrong address";

    return response.at(1);
}

/*!
    Writes a single \a data byte to the linear byte \a address on the tag. If \a mode is
    EraseAndWrite the byte is erased before writing. If \a mode is WriteOnly the contents are not
    earsed before writing. This is equivelant to writing the result of the bitwise or of \a data
    and the original value.

    Returns true on success; otherwise returns false.
*/
bool QNearFieldTagType1::writeByte(quint8 address, quint8 data, WriteMode mode)
{
    if (address & 0x80)
        return false;

    QByteArray command;

    if (mode == EraseAndWrite)
        command.append(char(0x53)); // WRITE-E
    else if (mode == WriteOnly)
        command.append(char(0x1a)); // WRITE-NE
    else
        return false;

    command.append(char(address));  // Address
    command.append(char(data));     // Data
    command.append(uid().left(4));  // 4 bytes of UID

    const QByteArray response = sendCommand(command);

    if (response.isEmpty())
        return false;

    quint8 writeAddress = response.at(0);
    quint8 writeData = response.at(1);

    if (writeAddress != address)
        qDebug() << Q_FUNC_INFO << "response was for wrong address";

    if (mode == EraseAndWrite)
        return writeData == data;
    else if (mode == WriteOnly)
        return (writeData & data) == data;
    else
        return false;
}

/*!
    Reads and returns 120 bytes of data from the segment specified by \a segmentAddress. An empty
    byte array is returned if an error occurs.
*/
QByteArray QNearFieldTagType1::readSegment(quint8 segmentAddress)
{
    if (segmentAddress & 0xf0)
        return QByteArray();

    QByteArray command;
    command.append(char(0x10));                 // RSEG
    command.append(char(segmentAddress << 4));  // Segment address
    command.append(QByteArray(8, char(0x00)));  // Data (unused)
    command.append(uid().left(4));              // 4 bytes of UID

    const QByteArray response = sendCommand(command);

    if (response.isEmpty())
        return QByteArray();

    quint8 readSegmentAddress = response.at(0);
    if ((readSegmentAddress >> 4) != segmentAddress)
        qDebug() << Q_FUNC_INFO << "response was for wrong segment address";

    return response.mid(1);
}

/*!
    Reads and returns 8 bytes of data from the block specified by \a blockAddress. An empty byte
    array is returned if an error occurs.
*/
QByteArray QNearFieldTagType1::readBlock(quint8 blockAddress)
{
    QByteArray command;
    command.append(char(0x02));                 // READ8
    command.append(char(blockAddress));         // Block address
    command.append(QByteArray(8, char(0x00)));  // Data (unused)
    command.append(uid().left(4));              // 4 bytes of UID

    const QByteArray response = sendCommand(command);

    if (response.isEmpty())
        return QByteArray();

    quint8 readBlockAddress = response.at(0);
    if (readBlockAddress != blockAddress)
        qDebug() << Q_FUNC_INFO << "response was for wrong block address";

    return response.mid(1);
}

/*!
    Writes 8 bytes of \a data to the block specified by \a blockAddress. If \a mode is
    EraseAndWrite the bytes are erased before writing. If \a mode is WriteOnly the contents are not
    earsed before writing. This is equivelant to writing the result of the bitwise or of \a data
    and the original value.

    Returns true on success; otherwise returns false.

*/
bool QNearFieldTagType1::writeBlock(quint8 blockAddress, const QByteArray &data, WriteMode mode)
{
    if (data.length() != 8)
        return false;

    QByteArray command;

    if (mode == EraseAndWrite)
        command.append(char(0x54));     // WRITE-E8
    else if (mode == WriteOnly)
        command.append(char(0x1b));     // WRITE-NE8
    else
        return false;

    command.append(char(blockAddress)); // Block address
    command.append(data);               // Data
    command.append(uid().left(4));      // 4 bytes of UID

    const QByteArray response = sendCommand(command);

    if (response.isEmpty())
        return false;

    quint8 writeBlockAddress = response.at(0);

    if (writeBlockAddress != blockAddress)
        qDebug() << Q_FUNC_INFO << "response was for wrong block address";

    if (mode == EraseAndWrite) {
        return response.mid(1) == data;
    } else if (mode == WriteOnly) {
        const QByteArray writeData = response.mid(1);
        for (int i = 0; i < writeData.length(); ++i) {
            if ((writeData.at(i) & data.at(i)) != data.at(i))
                return false;
        }
        return true;
    } else {
        return false;
    }
}

#include "moc_qnearfieldtagtype1.cpp"

QTM_END_NAMESPACE
