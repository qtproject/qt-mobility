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

#include "qndefmessage.h"
#include "qndefrecord_p.h"

#include <QtCore/QDebug>

QTM_BEGIN_NAMESPACE

QNdefMessage::QNdefMessage()
{
}

QNdefMessage::QNdefMessage(const QNdefMessage &other)
    :   m_records(other.m_records)
{
}

QNdefMessage::QNdefMessage(const QList<QNdefRecord> &records)
    :   m_records(records)
{
    m_records[0].d->messageBegin = true;
    m_records[m_records.count() - 1].d->messageEnd = true;
}

QNdefMessage::QNdefMessage(const QByteArray &message)
{
    bool messageBegin = false;
    bool messageEnd = false;

    QByteArray::const_iterator i = message.begin();
    while (i != message.end()) {
        quint8 flags = *i;

        QNdefRecord record;

        record.setUserTypeNameFormat(flags & 0x07);

        record.d->messageBegin = flags & 0x80;
        record.d->messageEnd = flags & 0x40;

        bool cf = flags & 0x20;
        bool sr = flags & 0x10;
        bool il = flags & 0x08;

        if (record.d->messageBegin && messageBegin)
            qDebug() << "Got message begin but already parsed some records";
        if (record.d->messageEnd && messageEnd)
            qDebug() << "Got message end but already parsed final record";
        if (cf)
            qDebug() << "Chunked records not supported, yet";

        quint8 typeLength = *(++i);

        quint32 payloadLength;
        if (sr)
            payloadLength = *(++i);
        else {
            payloadLength = quint8(*(++i)) << 24;
            payloadLength |= quint8(*(++i)) << 16;
            payloadLength |= quint8(*(++i)) << 8;
            payloadLength |= quint8(*(++i)) << 0;
        }

        quint8 idLength;
        if (il)
            idLength = *(++i);
        else
            idLength = 0;

        if (typeLength > 0) {
            QByteArray type(++i, typeLength);
            record.setType(type);
            i += typeLength - 1;
        }

        if (idLength > 0) {
            QByteArray id(++i, idLength);
            record.setId(id);
            i += idLength - 1;
        }

        if (payloadLength > 0) {
            QByteArray payload(++i, payloadLength);
            record.setPayload(payload);
            i += payloadLength - 1;
        }

        m_records.append(record);

        // move to start of next record
        ++i;
    }
}

QNdefMessage::~QNdefMessage()
{
}

bool QNdefMessage::operator==(const QNdefMessage &other) const
{
    // both records are empty
    if (m_records.isEmpty() && other.m_records.isEmpty())
        return true;

    // compare empty to really empty
    if (m_records.isEmpty() && other.m_records.count() == 1 &&
        other.m_records.first().d->typeNameFormat == 0) {
        return true;
    }
    if (other.m_records.isEmpty() && m_records.count() == 1 &&
        m_records.first().d->typeNameFormat == 0) {
        return true;
    }

    if (m_records.count() != other.m_records.count())
        return false;

    for (int i = 0; i < m_records.count(); ++i) {
        if (m_records.at(i) != other.m_records.at(i))
            return false;
    }

    return true;
}

const QList<QNdefRecord> &QNdefMessage::records() const
{
    return m_records;
}

QTM_END_NAMESPACE
