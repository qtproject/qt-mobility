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
#include "qnearfieldutility_symbian.h"
#include "qnearfieldtagtype1_symbian_p.h"
#include "qnearfieldutility_symbian.h"

QTM_BEGIN_NAMESPACE

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
    delete mTag;
}

QByteArray QNearFieldTagType1Symbian::uid() const
{
    return _uid();
}

/*!
    \reimp
*/
QByteArray QNearFieldTagType1Symbian::readIdentification()
{
    return _uid();
}

/*!
    \reimp
*/
QByteArray QNearFieldTagType1Symbian::readAll()
{
    QByteArray command;
    command.append(char(0x00)); // RALL
    command.append(char(0x00));
    command.append(char(0x00));
    command.append(uid().left(4)); // UID
    // Hardware will append CRC bytes. The CRC value appended 
    // to the command will be ignored.
    command.append(char(0x00)); // CRC1
    command.append(char(0x00)); // CRC2

    return sendCommand(command);
}

/*!
    \reimp
*/
quint8 QNearFieldTagType1Symbian::readByte(quint8 address)
{
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

    const QByteArray response = sendCommand(command);

    if (response.isEmpty())
        return 0;

    // address doesn't match
    if (response.at(0) != address)
        return 0;

    return response.at(1);
}

/*!
    \reimp
*/
bool QNearFieldTagType1Symbian::writeByte(quint8 address, quint8 data, WriteMode mode)
{
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

    const QByteArray response = sendCommand(command);

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
}

/*!
    \reimp
*/
QByteArray QNearFieldTagType1Symbian::readSegment(quint8 segmentAddress)
{
    if (segmentAddress & 0xf0)
        return QByteArray();

    QByteArray command;
    command.append(char(0x10));                 // RSEG
    command.append(char(segmentAddress << 4));  // Segment address
    command.append(QByteArray(8, char(0x00)));  // Data (unused)
    command.append(uid().left(4));              // 4 bytes of UIDD
    
    // Hardware will append CRC bytes. The CRC value appended 
    // to the command will be ignored.
    command.append(char(0x00)); // CRC1
    command.append(char(0x00)); // CRC2

    const QByteArray response = sendCommand(command);

    if (response.isEmpty())
        return QByteArray();

    quint8 readSegmentAddress = response.at(0);
    if ((readSegmentAddress >> 4) != segmentAddress)
        return QByteArray();

    return response.mid(1);
}

/*!
    \reimp
*/
QByteArray QNearFieldTagType1Symbian::readBlock(quint8 blockAddress)
{
    QByteArray command;
    command.append(char(0x02));                 // READ8
    command.append(char(blockAddress));         // Block address
    command.append(QByteArray(8, char(0x00)));  // Data (unused)
    command.append(uid().left(4));              // 4 bytes of UID

    // Hardware will append CRC bytes. The CRC value appended 
    // to the command will be ignored.
    command.append(char(0x00)); // CRC1
    command.append(char(0x00)); // CRC2

    const QByteArray response = sendCommand(command);

    if (response.isEmpty())
        return QByteArray();

    quint8 readBlockAddress = response.at(0);
    if (readBlockAddress != blockAddress)
        return QByteArray();

    return response.mid(1);
}

/*!
    \reimp
*/
bool QNearFieldTagType1Symbian::writeBlock(quint8 blockAddress, const QByteArray &data,
                        WriteMode mode)
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

    // Hardware will append CRC bytes. The CRC value appended 
    // to the command will be ignored.
    command.append(char(0x00)); // CRC1
    command.append(char(0x00)); // CRC2
    const QByteArray response = sendCommand(command);

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
}
    
bool QNearFieldTagType1Symbian::hasNdefMessage()
{
    return _hasNdefMessage();
}

QList<QNdefMessage> QNearFieldTagType1Symbian::ndefMessages()
{
    return _ndefMessages();
}

void QNearFieldTagType1Symbian::setNdefMessages(const QList<QNdefMessage> &messages)
{
    _setNdefMessages(messages);
}

/*!
    \reimp
*/
QByteArray QNearFieldTagType1Symbian::sendCommand(const QByteArray &command)
{
    int timeout = 100 * 1000; // 100ms
    QByteArray result;

    if (command.length() > 0)
    {
        switch (command.at(0))
        {
            case 0x00: // RALL
            {
                result = _sendCommand<124>(command, timeout);
                break;
            }
            case 0x01: // READ
            case 0x53: // WRITE-E
            case 0x1a: // WRITE-NE
            {
                result = _sendCommand<4>(command, timeout);
                break;
            } 
            case 0x10: // RSEG
            {
                result = _sendCommand<131>(command, timeout);
                break;
            }
            case 0x02: // READ8
            case 0x54: // WRITE-E8
            case 0x1b: // WRITE_NE8
            {
                result = _sendCommand<11>(command, timeout);
                break;
            }
            default:
            {
                return result;
            }
        }
    }
    return result;            
}

/*!
    \reimp
*/
QList<QByteArray> QNearFieldTagType1Symbian::sendCommands(const QList<QByteArray> &commands)
{
    QList<QByteArray> result;
    foreach(const QByteArray cmd, commands)
    {
        result.append(sendCommand(cmd));
    }
    return result; 
}

#include "moc_qnearfieldtagtype1_symbian_p.cpp"

QTM_END_NAMESPACE
