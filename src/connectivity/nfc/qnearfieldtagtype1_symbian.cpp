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
#include <nfctag.h>
#include <QVariant>
#include "nearfieldutility_symbian.h"
#include "qnearfieldtagtype1_symbian_p.h"
#include "debug.h"

QTM_BEGIN_NAMESPACE

static QVariant decodeResponse(const QByteArray &command, const QByteArray &response)
{
    BEGIN
    END
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
    \class QNearFieldTagType1Symbian
    \brief The QNearFieldTagType1Symbian class provides symbian backend implementation for communicating with an NFC Tag
           Type 1 tag.

    \ingroup connectivity-nfc
    \inmodule QtConnectivity
*/

/*!
    Constructs a new tag type 1 near field target with \a tag and \a parent.
*/
QNearFieldTagType1Symbian::QNearFieldTagType1Symbian(MNearFieldTarget *tag, QObject *parent)
                                : QNearFieldTagType1(parent), QNearFieldTagImpl(tag)
{
}

/*!
    Destructor
*/
QNearFieldTagType1Symbian::~QNearFieldTagType1Symbian()
{
}

QByteArray QNearFieldTagType1Symbian::uid() const
{
    return _uid();
}

/*!
    \reimp
*/
QNearFieldTarget::RequestId QNearFieldTagType1Symbian::readIdentification()
{
    BEGIN
    QByteArray command;
    command.append(char(0x78));     // RID
    command.append(char(0x00));     // Address (unused)
    command.append(char(0x00));     // Data (unused)
    command.append(uid().left(4));  // 4 bytes of UID
    // Hardware will append CRC bytes. The CRC value appended 
    // to the command will be ignored.
    command.append(char(0x00)); // CRC1
    command.append(char(0x00)); // CRC2
    END
    return sendCommand(command);
}

/*!
    \reimp
*/
QNearFieldTarget::RequestId QNearFieldTagType1Symbian::readAll()
{
    BEGIN
    QByteArray command;
    command.append(char(0x00)); // RALL
    command.append(char(0x00));
    command.append(char(0x00));
    command.append(uid().left(4)); // UID
    // Hardware will append CRC bytes. The CRC value appended 
    // to the command will be ignored.
    command.append(char(0x00)); // CRC1
    command.append(char(0x00)); // CRC2
    END
    return sendCommand(command);
}

/*!
    \reimp
*/
QNearFieldTarget::RequestId QNearFieldTagType1Symbian::readByte(quint8 address)
{
    BEGIN
    // MSB must be 0
    if (address & 0x80)
        return 0;

    QByteArray command;
    command.append(char(0x01));     // READ
    command.append(char(address));  // Address
    command.append(char(0x00));     // Data (unused)
    command.append(uid().left(4));  // 4 bytes of UID

    // Hardware will append CRC bytes. The CRC value appended 
    // to the command will be ignored.
    command.append(char(0x00)); // CRC1
    command.append(char(0x00)); // CRC2
    END
    return sendCommand(command);
#if 0
    if (response.isEmpty())
        return 0;

    // address doesn't match
    if (response.at(0) != address)
        return 0;

    return response.at(1);
#endif
}

/*!
    \reimp
*/
QNearFieldTarget::RequestId QNearFieldTagType1Symbian::writeByte(quint8 address, quint8 data, WriteMode mode)
{
    BEGIN
    // MSB must be 0
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

    // Hardware will append CRC bytes. The CRC value appended 
    // to the command will be ignored.
    command.append(char(0x00)); // CRC1
    command.append(char(0x00)); // CRC2
    END
    return sendCommand(command);
#if 0
    if (response.isEmpty())
        return false;

    quint8 writeAddress = response.at(0);
    quint8 writeData = response.at(1);

    if (writeAddress != address)
        return false;

    if (mode == EraseAndWrite)
        return writeData == data;
    else if (mode == WriteOnly)
        return (writeData & data) == data;
    else
        return false;
#endif
}

/*!
    \reimp
*/
QNearFieldTarget::RequestId QNearFieldTagType1Symbian::readSegment(quint8 segmentAddress)
{
    BEGIN
    if (segmentAddress & 0xf0)
        return QNearFieldTarget::RequestId();

    QByteArray command;
    command.append(char(0x10));                 // RSEG
    command.append(char(segmentAddress << 4));  // Segment address
    command.append(QByteArray(8, char(0x00)));  // Data (unused)
    command.append(uid().left(4));              // 4 bytes of UIDD
    
    // Hardware will append CRC bytes. The CRC value appended 
    // to the command will be ignored.
    command.append(char(0x00)); // CRC1
    command.append(char(0x00)); // CRC2
    END
    return sendCommand(command);
#if 0
    if (response.isEmpty())
        return QByteArray();

    quint8 readSegmentAddress = response.at(0);
    if ((readSegmentAddress >> 4) != segmentAddress)
        return QByteArray();

    return response.mid(1);
#endif
}

/*!
    \reimp
*/
QNearFieldTarget::RequestId QNearFieldTagType1Symbian::readBlock(quint8 blockAddress)
{
    BEGIN
    QByteArray command;
    command.append(char(0x02));                 // READ8
    command.append(char(blockAddress));         // Block address
    command.append(QByteArray(8, char(0x00)));  // Data (unused)
    command.append(uid().left(4));              // 4 bytes of UID

    // Hardware will append CRC bytes. The CRC value appended 
    // to the command will be ignored.
    command.append(char(0x00)); // CRC1
    command.append(char(0x00)); // CRC2
    END
    return sendCommand(command);
#if 0
    if (response.isEmpty())
        return QByteArray();

    quint8 readBlockAddress = response.at(0);
    if (readBlockAddress != blockAddress)
        return QByteArray();

    return response.mid(1);
#endif
}

/*!
    \reimp
*/
QNearFieldTarget::RequestId QNearFieldTagType1Symbian::writeBlock(quint8 blockAddress, const QByteArray &data,
                        WriteMode mode)
{
    BEGIN
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

    // Hardware will append CRC bytes. The CRC value appended 
    // to the command will be ignored.
    command.append(char(0x00)); // CRC1
    command.append(char(0x00)); // CRC2
    END
    return sendCommand(command);
#if 0
    if (response.isEmpty())
        return false;

    quint8 writeBlockAddress = response.at(0);

    if (writeBlockAddress != blockAddress)
        return false;

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
#endif
}
    
bool QNearFieldTagType1Symbian::hasNdefMessage()
{
    BEGIN
    END
    return _hasNdefMessage();
}

void QNearFieldTagType1Symbian::readNdefMessages()
{
    BEGIN
    _ndefMessages();
    END
}

void QNearFieldTagType1Symbian::writeNdefMessages(const QList<QNdefMessage> &messages)
{
    BEGIN
    _setNdefMessages(messages);
    END
}

/*!
    \reimp
*/
QNearFieldTarget::RequestId QNearFieldTagType1Symbian::sendCommand(const QByteArray &command)
{
    BEGIN
    END
    return _sendCommand(command);
}

/*!
    \reimp
*/
QNearFieldTarget::RequestId QNearFieldTagType1Symbian::sendCommands(const QList<QByteArray> &commands)
{
    BEGIN
    END
    return _sendCommands(commands);
}

bool QNearFieldTagType1Symbian::waitForRequestCompleted(const RequestId &id, int msecs)
{
    BEGIN
    END
    return _waitForRequestCompleted(id, msecs);
}

bool QNearFieldTagType1Symbian::handleTagOperationResponse(const RequestId &id, const QByteArray &command, const QByteArray &response)
{
    BEGIN
    QVariant decodedResponse;
    if (!response.isNull())
    {
        decodedResponse = decodeResponse(command, response);
    }
    // to handle commands
    QVariant existResponse = requestResponse(id);
    if (existResponse.isValid())
    {
        LOG("this is one command response in sendCommands");
        // there is existed id. So it must be a sendcommands request response.
        if (existResponse.type() == QVariant::List)
        {
            LOG("already has some commands response");
            QVariantList list = existResponse.toList();
            list.append(decodedResponse);
            setResponseForRequest(id, list);
        }
        else
        {
            LOG("second command response of sendCommands"); 
            QVariantList list;
            list.append(existResponse);
            list.append(decodedResponse);
            setResponseForRequest(id, list);
        }
    }
    else
    {
        LOG("normal command response");
        setResponseForRequest(id, decodedResponse);
    }
    END
    return true;
}

#include "moc_qnearfieldtagtype1_symbian_p.cpp"

QTM_END_NAMESPACE
