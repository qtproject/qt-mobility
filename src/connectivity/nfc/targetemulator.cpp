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

#include "targetemulator_p.h"

#include <QtCore/QSettings>

// Copied from qbytearray.cpp
// Modified to initialise the crc with 0x6363 instead of 0xffff and to not invert the final result.
static const quint16 crc_tbl[16] = {
    0x0000, 0x1081, 0x2102, 0x3183,
    0x4204, 0x5285, 0x6306, 0x7387,
    0x8408, 0x9489, 0xa50a, 0xb58b,
    0xc60c, 0xd68d, 0xe70e, 0xf78f
};

static quint16 nfcChecksum(const char *data, uint len)
{
    register quint16 crc = 0x6363;
    uchar c;
    const uchar *p = reinterpret_cast<const uchar *>(data);
    while (len--) {
        c = *p++;
        crc = ((crc >> 4) & 0x0fff) ^ crc_tbl[((crc ^ c) & 15)];
        c >>= 4;
        crc = ((crc >> 4) & 0x0fff) ^ crc_tbl[((crc ^ c) & 15)];
    }
    return crc;
}

TagBase::TagBase()
{
}

TagBase::~TagBase()
{
}

static inline quint8 blockByteToAddress(quint8 block, quint8 byte)
{
    return ((block & 0x0f) << 3) | (byte & 0x07);
}

NfcTagType1::NfcTagType1()
:   hr0(0x11), hr1(0x00), staticMemory(120, '\0')
{
    // Locked blocks
    staticMemory[(0x0e << 3) | 0x00] = 0x01;
    staticMemory[(0x0e << 3) | 0x01] = 0x60;
}

void NfcTagType1::load(QSettings *settings)
{
    settings->beginGroup(QLatin1String("TagType1"));

    hr0 = settings->value(QLatin1String("HR0"), 0x11).toUInt();

    if (!(hr0 & 0x10)) {
        settings->endGroup();
        return;
    }

    hr1 = settings->value(QLatin1String("HR1"), 0x00).toUInt();

    staticMemory = settings->value(QLatin1String("Data")).toByteArray();

    quint8 nmn = staticMemory.at(8);

    quint8 vno = staticMemory.at(9);
    if (vno != 0x10)
        qWarning("Only NFC TagType1 v1.0 behavior is supported.");

    quint8 tms = staticMemory.at(10);
    if (staticMemory.length() != 8 * (tms + 1))
        qWarning("Static memory size does not match TMS value.");

    quint8 rwa = staticMemory.at(11);
    switch (rwa >> 4) {
    case 0:
        // Unrestricted read access tag
        break;
    default:
        // tag with unknown read attributes
        ;
    }

    switch (rwa & 0x0f) {
    case 0:
        // Unrestricted write access tag
        break;
    case 0x0f:
        // Read only tag
        break;
    default:
        // tag with unknown write attributes
        ;
    }

    quint16 lock = (quint8(staticMemory[blockByteToAddress(0x0e, 1)]) << 8) |
                   quint8(staticMemory[blockByteToAddress(0x0e, 0)]);

    settings->endGroup();
}

QByteArray NfcTagType1::uid() const
{
    return staticMemory.left(7);
}

quint8 NfcTagType1::readData(quint8 block, quint8 byte)
{
    return staticMemory.at((block << 3) | byte);
}

QByteArray NfcTagType1::processCommand(const QByteArray &command)
{
    QByteArray response;

    if (command.length() == 9) {
        // static memory model command
        quint8 opcode = command.at(0);
        quint8 address = command.at(1);
        quint8 data = command.at(2);
        QByteArray uid = command.mid(3, 4);
        quint16 crc = (quint8(command.at(8)) << 8) | quint8(command.at(7));

        // check UID
        if (uid != staticMemory.left(4))
            return QByteArray();

        // check checksum
        if (crc != nfcChecksum(command.left(7).constData(), 7))
            return QByteArray();

        switch (opcode) {
        case 0x00:  // RALL
            response.append(hr0);
            response.append(hr1);
            response.append(staticMemory.left(120));
            break;
        case 0x01:  // READ
            response.append(address);
            if (address & 0x80)
                response.append(char(0x00));
            else
                response.append(staticMemory.at(address));
            break;
        case 0x54: { // WRITE-E8
            quint8 block = address >> 3;
            if (block == 0x00 || block == 0x0d || block == 0x0e)    // locked blocks
                break;

            quint16 lock = (readData(0x0e, 0x01) << 8) | readData(0x0e, 0x00);
            if ((0x01 << block) && lock)    // locked blocks
                break;

            staticMemory[address] = data;

            response.append(address);
            response.append(data);
            break;
        }
        case 0x1b: { // WRITE-NE8
            quint8 block = address >> 3;
            if (block == 0x00 || block == 0x0d)  // locked blocks
                break;

            quint16 lock = (readData(0x0e, 0x01) << 8) | readData(0x0e, 0x00);
            if ((0x01 << block) && lock)    // locked blocks
                break;

            staticMemory[address] = staticMemory.at(address) | data;

            response.append(address);
            response.append(staticMemory.at(address));
            break;
        }
        case 0x78:  // RID
            response.append(hr0);
            response.append(hr1);
            response.append(staticMemory.left(4));
            break;
        }
    } else if (command.length() == 16) {
        // dynamic memory model command
    }

    if (!response.isEmpty()) {
        quint16 crc = nfcChecksum(response.constData(), response.length());
        response.append(quint8(crc & 0xff));
        response.append(quint8(crc >> 8));
    }

    return response;
}
