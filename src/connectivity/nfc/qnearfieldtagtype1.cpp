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

#include <QtCore/QByteArray>

QTM_BEGIN_NAMESPACE

QNearFieldTagType1::QNearFieldTagType1(QObject *parent)
:   QNearFieldTarget(parent)
{
}

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

QList<QNdefMessage> QNearFieldTagType1::ndefMessages()
{
    QByteArray data = readAll();

    quint8 hr0 = data.at(0);
    // Check if target is a NFC TagType1 tag
    if (!(hr0 & 0x10))
        return QList<QNdefMessage>();

    // Check if NDEF Message Magic number is present
    quint8 nmn = data.at(10);
    if (nmn != 0xe1)
        return QList<QNdefMessage>();

    QList<QNdefMessage> ndefMessages;

    int i = 14; // skip hr0, hr1, uid and cc
    while (i < data.length()) {
        quint8 tag = data.at(i);

        switch (tag) {
        case 0x00:  // NULL TLV
            ++i;
            break;
        case 0x01:  // Lock Control TLV
            // fall-through
        case 0x02:  // Memory Control TLV
            ++i;
            if (data.at(i) != 0x03) {
                qWarning("Invalid Lock/Memory Control TLV");
                break;
            }
            i += 3;
            break;
                case 0x03: { // NDEF Message TLV
                        ++i;
                        int length = data.at(i);
                        ++i;
                        if (length == 0xff) {
                            length = (data.at(i) << 8) | data.at(i+1);
                            i += 2;
                            if (length < 0xff || length == 0xffff) {
                                qWarning("Invalid 3 byte length");
                                break;
                            }
                        }

                        ndefMessages.append(QNdefMessage::fromByteArray(data.mid(i, length)));

                        i += length;
                        break;
                    }
                case 0xfd: { // Proprietary TLV
                        ++i;
                        int length = data.at(i);
                        ++i;
                        if (length == 0xff) {
                            length = (data.at(i) << 8) | data.at(i+1);
                            i += 2;
                            if (length < 0xff || length == 0xffff) {
                                qWarning("Invalid 3 byte length");
                                break;
                            }
                        }

                        QByteArray proprietaryData = data.mid(i, length);

                        i += length;
                        break;
                    }
                case 0xfe:  // Terminator TLV
                    i = data.length();  // done
                    break;
                }
    }

    return ndefMessages;
}

QByteArray QNearFieldTagType1::readAll()
{
    QByteArray command;
    command.append(char(0x00));   // RALL
    command.append(char(0x00));   // Address (unused)
    command.append(char(0x00));   // Data (unused)
    command.append(uid().left(4));// 4 bytes of UID

    QByteArray response = sendCommand(command);

    // no targets responded to command
    if (response.isEmpty())
        return QByteArray();

    return response;
}

#include "moc_qnearfieldtagtype1.cpp"

QTM_END_NAMESPACE
