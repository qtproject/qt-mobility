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
#include <QtEndian>
#include "nearfieldutility_symbian.h"
#include "qnearfieldtagtype4_symbian_p.h"

QTM_BEGIN_NAMESPACE

QNearFieldTagType4Symbian::QNearFieldTagType4Symbian(MNearFieldTarget *tag, QObject *parent)
                                : QNearFieldTagType4(parent), QNearFieldTagImpl(tag)
{
}

QNearFieldTagType4Symbian::~QNearFieldTagType4Symbian()
{
}

QByteArray QNearFieldTagType4Symbian::uid() const
{
    return _uid();
}
    
bool QNearFieldTagType4Symbian::hasNdefMessage()
{
    return _hasNdefMessage();
}

void QNearFieldTagType4Symbian::readNdefMessages()
{
    _ndefMessages();
}

void QNearFieldTagType4Symbian::writeNdefMessages(const QList<QNdefMessage> &messages)
{
    _setNdefMessages(messages);
}

QNearFieldTarget::RequestId QNearFieldTagType4Symbian::sendCommand(const QByteArray &command)
{
    return _sendCommand(command);
}

QNearFieldTarget::RequestId QNearFieldTagType4Symbian::sendCommands(const QList<QByteArray> &commands)
{
    return _sendCommands(commands);
}

QNearFieldTarget::RequestId QNearFieldTagType4Symbian::select(const QByteArray &name)
{
    BEGIN
    QByteArray command;
    command.append(char(0x00)); // CLA
    command.append(char(0xA4)); // INS
    command.append(char(0x04)); // P1, select by name
    command.append(char(0x00)); // First or only occurrence
    command.append(char(0x07)); // Lc
    command.append(name);
    END
    return _sendCommand(command);
}

QNearFieldTarget::RequestId QNearFieldTagType4Symbian::select(quint16 fileIdentifier)
{
    BEGIN
    QByteArray command;
    command.append(char(0x00)); // CLA
    command.append(char(0xA4)); // INS
    command.append(char(0x00)); // P1, select by file identifier
    command.append(char(0x00)); // First or only occurrence
    command.append(char(0x02)); // Lc
    quint16 temp = qToBigEndian<quint16>(fileIdentifier);
    command.append(reinterpret_cast<const char*>(&temp), 
                   sizeof(quint16));
    END
    return _sendCommand(command);    
}

QNearFieldTarget::RequestId QNearFieldTagType4Symbian::read(quint16 length, quint16 startOffset)
{
    BEGIN
    QByteArray command;
    command.append(char(0x00)); // CLA
    command.append(char(0xB0)); // INS
    quint16 temp = qToBigEndian<quint16>(startOffset);
    command.append(reinterpret_cast<const char*>(&temp), 
                   sizeof(quint16)); // P1/P2 offset
    temp = qToBigEndian<quint16>(length);
    command.append(reinterpret_cast<const char*>(&temp),
                   sizeof(quint16)); // Le
    END
    return _sendCommand(command);
}

QNearFieldTarget::RequestId QNearFieldTagType4Symbian::write(const QByteArray &data, quint16 startOffset)
{
    BEGIN
    QByteArray command;
    command.append(char(0x00)); // CLA
    command.append(char(0xD6)); // INS
    quint16 temp = qToBigEndian<quint16>(startOffset);
    command.append(reinterpret_cast<const char *>(&temp), sizeof(quint16));
    quint16 length = data.count();
    if ((length > 0xFF) || (length < 0x01))
    {
        END
        return QNearFieldTarget::RequestId();
    }
    else
    {
        quint16 temp = qToBigEndian<quint16>(length);
        command.append(reinterpret_cast<const char *>(&temp), 
                       sizeof(quint16));
    }
    
    command.append(data);
    END
    return _sendCommand(command);
                       
}

void QNearFieldTagType4Symbian::handleTagOperationResponse(const RequestId &id, const QByteArray &command, const QByteArray &response)
{
    BEGIN
    Q_UNUSED(command);
    QVariant decodedResponse = decodeResponse(command, response);
    setResponseForRequest(id, decodedResponse);
    END
}

#include "moc_qnearfieldtagtype4_symbian_p.cpp"

QTM_END_NAMESPACE
