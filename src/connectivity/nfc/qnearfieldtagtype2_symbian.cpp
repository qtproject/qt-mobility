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
#include "qnearfieldtagtype2_symbian_p.h"
#include <qnearfieldtarget_p.h>

QTM_BEGIN_NAMESPACE

QNearFieldTagType2Symbian::QNearFieldTagType2Symbian(MNearFieldTarget *tag, QObject *parent)
                                : QNearFieldTagType2(parent), QNearFieldTagImpl(tag)
{
}

QNearFieldTagType2Symbian::~QNearFieldTagType2Symbian()
{
}

static QVariant decodeResponse(const QByteArray &response)
{
    if (!response.isEmpty())
    {
        return quint8(response.at(0)) == 0x0a;
    }
    else
    {
        return QVariant();
    }
}

QNearFieldTarget::RequestId QNearFieldTagType2Symbian::readBlock(quint8 blockAddress)
{
    QByteArray command;
    command.append(char(0x30));         // READ
    command.append(char(blockAddress)); // Block address
    // Hardware will append CRC bytes. The CRC value appended 
    // to the command will be ignored.
    command.append(char(0x00)); // CRC1
    command.append(char(0x00)); // CRC2

    return sendCommand(command);
}

QNearFieldTarget::RequestId QNearFieldTagType2Symbian::writeBlock(quint8 blockAddress, const QByteArray &data)
{
    if (data.length() != 4)
        return false;

    QByteArray command;
    command.append(char(0xa2));         // WRITE
    command.append(char(blockAddress)); // Block address
    command.append(data);               // Data

    // Hardware will append CRC bytes. The CRC value appended 
    // to the command will be ignored.
    command.append(char(0x00)); // CRC1
    command.append(char(0x00)); // CRC2

    return sendCommand(command);
}

QNearFieldTarget::RequestId QNearFieldTagType2Symbian::selectSector(quint8 sector)
{
    QByteArray command;
    command.append(char(0xc2));     // SECTOR SELECT (Command Packet 1)
    command.append(char(0xff));

    // Hardware will append CRC bytes. The CRC value appended 
    // to the command will be ignored.
    command.append(char(0x00)); // CRC1
    command.append(char(0x00)); // CRC2

    RequestId id = sendCommand(command);
    mSelectSectorRequests.insert(id, false);

    QObject::connect(this, 
                     SIGNAL(error(QNearFieldTarget::Error, const QNearFieldTarget::RequestId&)), 
                     this, 
                     SLOT(selectSectorError(QNearFieldTarget::Error, const QNearFieldTarget::RequestId&)));
    quint8 acknack = 0;
    if (!waitForRequestCompleted(id))
    {
        // check if the packet 1 is timeout.
        QMap<QNearFieldTarget::RequestId, bool>::const_iterator i = mSelectSectorRequests.find(id);
        if ( i != mSelectSectorRequests.end())
        {
            if (i.value() == true)
            {
                acknack = 0x0a;
            }
            mSelectSectorRequests.remove(id);
        }
    }
    
    this->disconnect(SIGNAL(error(QNearFieldTarget::Error, const QNearFieldTarget::RequestId&)));

    if (acknack != 0x0a)
    {
        // first command of selectSector failed
        return QNearFieldTarget::RequestId();
    }
    else
    {
        command.clear();
        command.append(char(sector));               // Sector number
        command.append(QByteArray(3, char(0x00)));  // RFU

        return sendCommand(command);
    }
}

void QNearFieldTagType2Symbian::selectSectorError(QNearFieldTarget::Error error, const QNearFieldTarget::RequestId &id)
{
    QMap<QNearFieldTarget::RequestId, bool>::const_iterator i = mSelectSectorRequests.find(id);
    if ( i != mSelectSectorRequests.end())
    {
        if (QNearFieldTarget::NoResponseError == error)
        {
            mSelectSectorRequests.insert(id, true);
        }
    }
}

QNearFieldTarget::RequestId QNearFieldTagType2Symbian::sendCommand(const QByteArray &command)
{
    return _sendCommand(command);
}

QNearFieldTarget::RequestId QNearFieldTagType2Symbian::sendCommands(const QList<QByteArray> &commands)
{
    return _sendCommands(commands);
}

bool QNearFieldTagType2Symbian::hasNdefMessage()
{
    return _hasNdefMessage();
}

void QNearFieldTagType2Symbian::readNdefMessages()
{
    return _ndefMessages();
}

void QNearFieldTagType2Symbian::writeNdefMessages(const QList<QNdefMessage> &messages)
{
    _setNdefMessages(messages);
}

QByteArray QNearFieldTagType2Symbian::uid() const
{
    return _uid();
}

bool QNearFieldTagType2Symbian::handleTagOperationResponse(const RequestId &id, const QByteArray &command, const QByteArray &response)
{
    Q_UNUSED(command);
    QVariant decodedResponse = decodeResponse(response);
    // to handle commands
    QVariant existResponse = requestResponse(id);
    if (existResponse.isValid())
    {
        // there is existed id. So it must be a sendcommands request response.
        if (existResponse.type() == QVariant::List)
        {
            QVariantList list = existResponse.toList();
            list.append(decodedResponse);
            setResponseForRequest(id, list);
        }
        else
        {
            QVariantList list;
            list.append(existResponse);
            list.append(decodedResponse);
            setResponseForRequest(id, list);
        }
    }
    else
    {
        setResponseForRequest(id, decodedResponse);
    }
    return true;
}

#include "moc_qnearfieldtagtype2_symbian_p.cpp"
QTM_END_NAMESPACE
