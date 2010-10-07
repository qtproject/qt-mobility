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

/*!
    \class QNdefMessage
    \brief The QNdefMessage class provides an NFC NDEF message.

    \ingroup connectivity-nfc
    \inmodule QtConnectivity

    A QNdefMessage is a collection of 0 or more QNdefRecords.
*/

/*!
    \fn QNdefMessage::QNdefMessage()

    Constructs a new empty NDEF message.
*/

/*!
    \fn QNdefMessage::QNdefMessage(const QNdefRecord &record)

    Constructs a new NDEF message containing a single record \a record.
*/

/*!
    \fn QNdefMessage::QNdefMessage(const QNdefMessage &message)

    Constructs a new NDEF message that is a copy of \a message.
*/

/*!
    \fn QNdefMessage::QNdefMessage(const QList<QNdefRecord> &records)

    Constructs a new NDEF message that contains all of the records in \a records.
*/

/*!
    Returns an NDEF message parsed from the contents of \a message.

    The \a message paramater is interpreted as the raw message format defined in the NFC
    Specifications.
*/
QNdefMessage QNdefMessage::fromByteArray(const QByteArray &message)
{
    QNdefMessage result;

    bool seenMessageBegin = false;
    bool seenMessageEnd = false;

    QByteArray::const_iterator i = message.begin();
    while (i != message.end()) {
        quint8 flags = *i;

        QNdefRecord record;

        record.setUserTypeNameFormat(flags & 0x07);

        bool messageBegin = flags & 0x80;
        bool messageEnd = flags & 0x40;

        bool cf = flags & 0x20;
        bool sr = flags & 0x10;
        bool il = flags & 0x08;

        if (messageBegin && seenMessageBegin)
            qDebug() << "Got message begin but already parsed some records";
        if (messageEnd && seenMessageEnd)
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

        result.append(record);

        // move to start of next record
        ++i;
    }

    return result;
}

/*!
    Returns true if this NDEF message is equivalent to \a other; otherwise returns false.
*/
bool QNdefMessage::operator==(const QNdefMessage &other) const
{
    // both records are empty
    if (isEmpty() && other.isEmpty())
        return true;

    // compare empty to really empty
    if (isEmpty() && other.count() == 1 &&
        other.first().d->typeNameFormat == 0) {
        return true;
    }
    if (other.isEmpty() && count() == 1 &&
        first().d->typeNameFormat == 0) {
        return true;
    }

    if (count() != other.count())
        return false;

    for (int i = 0; i < count(); ++i) {
        if (at(i) != other.at(i))
            return false;
    }

    return true;
}

/*!
    Returns the NDEF message as a byte array.
*/
QByteArray QNdefMessage::toByteArray() const
{
    QByteArray m;

    for (int i = 0; i < count(); ++i) {
        const QNdefRecord &record = at(i);

        quint8 flags = record.userTypeNameFormat();

        if (i == 0)
            flags |= 0x80;
        if (i == count() - 1)
            flags |= 0x40;

        // cf (chunked records) not supported yet

        if (record.payload().length() < 255)
            flags |= 0x10;

        if (!record.id().isEmpty())
            flags |= 0x08;

        m.append(flags);
        m.append(record.type().length());

        if (flags & 0x10) {
            m.append(quint8(record.payload().length()));
        } else {
            quint32 length = record.payload().length();
            m.append(length >> 24);
            m.append(length >> 16);
            m.append(length >> 8);
            m.append(length & 0x000000ff);
        }

        if (flags & 0x08)
            m.append(record.id().length());

        if (!record.type().isEmpty())
            m.append(record.type());

        if (!record.id().isEmpty())
            m.append(record.id());

        if (!record.payload().isEmpty())
            m.append(record.payload());
    }

    return m;
}

QTM_END_NAMESPACE
