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
#include "qnearfieldtarget_p.h"
#include "qndefmessage.h"
#include "qtlv_p.h"

#include <QtCore/QByteArray>
#include <QtCore/QVariant>

#include <QtCore/QDebug>

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

class QNearFieldTagType1Private
{
public:
    QMap<QNearFieldTarget::RequestId, QByteArray> m_pendingInternalCommands;
};

static QVariant decodeResponse(const QByteArray &command, const QByteArray &response)
{
    switch (command.at(0)) {
    case 0x01:  // READ
        if (command.at(1) == response.at(0))
            return quint8(response.at(1));
        break;
    case 0x53: { // WRITE-E
        quint8 address = command.at(1);
        quint8 data = command.at(2);
        quint8 writeAddress = response.at(0);
        quint8 writeData = response.at(1);

        return ((writeAddress == address) && (writeData == data));
    }
    case 0x1a: { // WRITE-NE
        quint8 address = command.at(1);
        quint8 data = command.at(2);
        quint8 writeAddress = response.at(0);
        quint8 writeData = response.at(1);

        return ((writeAddress == address) && ((writeData & data) == data));
    }
    case 0x10: { // RSEG
        quint8 segmentAddress = quint8(command.at(1)) >> 4;
        quint8 readSegmentAddress = quint8(response.at(0)) >> 4;
        if (readSegmentAddress == segmentAddress)
            return response.mid(1);
        break;
    }
    case 0x02: { // READ8
        quint8 blockAddress = command.at(1);
        quint8 readBlockAddress = response.at(0);
        if (readBlockAddress == blockAddress)
            return response.mid(1);
        break;
    }
    case 0x54: { // WRITE-E8
        quint8 blockAddress = command.at(1);
        QByteArray data = command.mid(2, 8);
        quint8 writeBlockAddress = response.at(0);
        QByteArray writeData = response.mid(1);

        return ((writeBlockAddress == blockAddress) && (writeData == data));
    }
    case 0x1b: { // WRITE-NE8
        quint8 blockAddress = command.at(1);
        QByteArray data = command.mid(2, 8);
        quint8 writeBlockAddress = response.at(0);
        QByteArray writeData = response.mid(1);

        if (writeBlockAddress != blockAddress)
            return false;

        for (int i = 0; i < writeData.length(); ++i) {
            if ((writeData.at(i) & data.at(i)) != data.at(i))
                return false;
        }

        return true;
    }
    }

    return QVariant();
}

/*!
    Constructs a new tag type 1 near field target with \a parent.
*/
QNearFieldTagType1::QNearFieldTagType1(QObject *parent)
:   QNearFieldTarget(parent), d_ptr(new QNearFieldTagType1Private)
{
}

/*!
    Destroys the tag type 1 near field target.
*/
QNearFieldTagType1::~QNearFieldTagType1()
{
    delete d_ptr;
}

/*!
    \reimp
*/
bool QNearFieldTagType1::hasNdefMessage()
{
    RequestId id = readAll();
    if (!waitForRequestCompleted(id))
        return false;

    const QByteArray data = requestResponse(id).toByteArray();

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
void QNearFieldTagType1::readNdefMessages()
{
    RequestId id = readIdentification();
    if (!waitForRequestCompleted(id))
        return;

    const QByteArray data = requestResponse(id).toByteArray();

    // Check if target is a NFC TagType1 tag
    quint8 hr0 = data.at(0);
    if (!(hr0 & 0x10))
        return;

    id = readByte(8);
    if (!waitForRequestCompleted(id))
        return;

    quint8 ndefMagicNumber = requestResponse(id).toUInt();
    if (ndefMagicNumber != 0xe1)
        return;

    QTlvReader reader(this);
    while (!reader.atEnd()) {
        if (!reader.readNext())
            break;

        // NDEF Message TLV
        if (reader.tag() == 0x03)
            emit ndefMessageRead(QNdefMessage::fromByteArray(reader.data()));
    }
}

/*!
    \reimp
*/
void QNearFieldTagType1::writeNdefMessages(const QList<QNdefMessage> &messages)
{
    RequestId id = readIdentification();
    if (!waitForRequestCompleted(id))
        return;

    const QByteArray data = requestResponse(id).toByteArray();

    // Check if target is NFC TagType1 tag
    quint8 hr0 = data.at(0);
    if (!(hr0 & 0x10))
        return;

    id = readByte(8);
    if (!waitForRequestCompleted(id))
        return;

    quint8 ndefMagicNumber = requestResponse(id).toUInt();

    if (ndefMagicNumber != 0xe1)
        return;

    typedef QPair<quint8, QByteArray> Tlv;
    QList<Tlv> tlvs;

    QTlvReader reader(this);
    while (!reader.atEnd()) {
        if (!reader.readNext())
            break;

        switch (reader.tag()) {
        case 0x01:
        case 0x02:
        case 0xfd:
            tlvs.append(qMakePair(reader.tag(), reader.data()));
            break;
        default:
            ;
        }
    }

    QTlvWriter writer(this);
    foreach (const Tlv &tlv, tlvs)
        writer.writeTlv(tlv.first, tlv.second);

    foreach (const QNdefMessage &message, messages)
        writer.writeTlv(0x03, message.toByteArray());

    writer.writeTlv(0xfe);

    emit ndefMessagesWritten();
}

/*!
    Returns the NFC Tag Type 1 specification version number that the tag supports.
*/
quint8 QNearFieldTagType1::version()
{
    RequestId id = readByte(9);
    if (!waitForRequestCompleted(id))
        return 0;

    quint8 versionNumber = requestResponse(id).toUInt();
    return versionNumber;
}

/*!
    Returns the memory size in bytes of the tag.
*/
int QNearFieldTagType1::memorySize()
{
    RequestId id = readByte(10);
    if (!waitForRequestCompleted(id))
        return 0;

    quint8 tms = requestResponse(id).toUInt();

    return 8 * (tms + 1);
}

/*!
    Returns identification information read from the tag.

    The returned byte array contains HR0, HR1, UID0, UID1, UID2, UID3 in order. An empty byte array
    is returned if an error occurs.
*/
QNearFieldTarget::RequestId QNearFieldTagType1::readIdentification()
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
QNearFieldTarget::RequestId QNearFieldTagType1::readAll()
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
QNearFieldTarget::RequestId QNearFieldTagType1::readByte(quint8 address)
{
    if (address & 0x80)
        return RequestId();

    QByteArray command;
    command.append(char(0x01));     // READ
    command.append(char(address));  // Address
    command.append(char(0x00));     // Data (unused)
    command.append(uid().left(4));  // 4 bytes of UID

    RequestId id = sendCommand(command);

    Q_D(QNearFieldTagType1);

    d->m_pendingInternalCommands.insert(id, command);

    return id;
}

/*!
    Writes a single \a data byte to the linear byte \a address on the tag. If \a mode is
    EraseAndWrite the byte is erased before writing. If \a mode is WriteOnly the contents are not
    earsed before writing. This is equivelant to writing the result of the bitwise or of \a data
    and the original value.

    Returns true on success; otherwise returns false.
*/
QNearFieldTarget::RequestId QNearFieldTagType1::writeByte(quint8 address, quint8 data,
                                                          WriteMode mode)
{
    if (address & 0x80)
        return RequestId();

    QByteArray command;

    if (mode == EraseAndWrite)
        command.append(char(0x53)); // WRITE-E
    else if (mode == WriteOnly)
        command.append(char(0x1a)); // WRITE-NE
    else
        return RequestId();

    command.append(char(address));  // Address
    command.append(char(data));     // Data
    command.append(uid().left(4));  // 4 bytes of UID

    RequestId id = sendCommand(command);

    Q_D(QNearFieldTagType1);

    d->m_pendingInternalCommands.insert(id, command);

    return id;
}

/*!
    Reads and returns 120 bytes of data from the segment specified by \a segmentAddress. An empty
    byte array is returned if an error occurs.
*/
QNearFieldTarget::RequestId QNearFieldTagType1::readSegment(quint8 segmentAddress)
{
    if (segmentAddress & 0xf0)
        return RequestId();

    QByteArray command;
    command.append(char(0x10));                 // RSEG
    command.append(char(segmentAddress << 4));  // Segment address
    command.append(QByteArray(8, char(0x00)));  // Data (unused)
    command.append(uid().left(4));              // 4 bytes of UID

    RequestId id = sendCommand(command);

    Q_D(QNearFieldTagType1);

    d->m_pendingInternalCommands.insert(id, command);

    return id;
}

/*!
    Reads and returns 8 bytes of data from the block specified by \a blockAddress. An empty byte
    array is returned if an error occurs.
*/
QNearFieldTarget::RequestId QNearFieldTagType1::readBlock(quint8 blockAddress)
{
    QByteArray command;
    command.append(char(0x02));                 // READ8
    command.append(char(blockAddress));         // Block address
    command.append(QByteArray(8, char(0x00)));  // Data (unused)
    command.append(uid().left(4));              // 4 bytes of UID

    RequestId id = sendCommand(command);

    Q_D(QNearFieldTagType1);

    d->m_pendingInternalCommands.insert(id, command);

    return id;
}

/*!
    Writes 8 bytes of \a data to the block specified by \a blockAddress. If \a mode is
    EraseAndWrite the bytes are erased before writing. If \a mode is WriteOnly the contents are not
    earsed before writing. This is equivelant to writing the result of the bitwise or of \a data
    and the original value.

    Returns true on success; otherwise returns false.

*/
QNearFieldTarget::RequestId QNearFieldTagType1::writeBlock(quint8 blockAddress,
                                                           const QByteArray &data,
                                                           WriteMode mode)
{
    if (data.length() != 8)
        return RequestId();

    QByteArray command;

    if (mode == EraseAndWrite)
        command.append(char(0x54));     // WRITE-E8
    else if (mode == WriteOnly)
        command.append(char(0x1b));     // WRITE-NE8
    else
        return RequestId();

    command.append(char(blockAddress)); // Block address
    command.append(data);               // Data
    command.append(uid().left(4));      // 4 bytes of UID

    RequestId id = sendCommand(command);

    Q_D(QNearFieldTagType1);

    d->m_pendingInternalCommands.insert(id, command);

    return id;
}

bool QNearFieldTagType1::handleResponse(const RequestId &id, const QByteArray &response)
{
    Q_D(QNearFieldTagType1);

    if (d->m_pendingInternalCommands.contains(id)) {
        const QByteArray command = d->m_pendingInternalCommands.take(id);

        QVariant decodedResponse = decodeResponse(command, response);
        setResponseForRequest(id, decodedResponse);
        return true;
    }

    return QNearFieldTarget::handleResponse(id, response);
}

#include "moc_qnearfieldtagtype1.cpp"

QTM_END_NAMESPACE
