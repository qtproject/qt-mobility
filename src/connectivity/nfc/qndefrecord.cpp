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


#include "qndefrecord.h"
#include "qndefrecord_p.h"

QTM_BEGIN_NAMESPACE

QNdefRecord::QNdefRecord()
{
}

QNdefRecord::QNdefRecord(const QNdefRecord &other)
:   d(other.d)
{
}

/*!
    \internal

    Constructs an NDEF record that is a copy of \a other if \a other is of the expected type name
    format identified by \a typeNameFormat and type as identified by \a type; otherwise an empty
    NDEF record of the expected type name format and type is created.
*/
QNdefRecord::QNdefRecord(const QNdefRecord &other, quint8 typeNameFormat, const QByteArray &type)
{
    if (other.d->typeNameFormat == typeNameFormat && other.d->type == type) {
        d = other.d;
    } else {
        d = new QNdefRecordPrivate;
        d->typeNameFormat = typeNameFormat;
        d->type = type;
    }
}

/*!
    \internal

    Constructs an NDEF record with a type name format identified by \a typeNameFormat and type as
    identified by \a type.
*/
QNdefRecord::QNdefRecord(quint8 typeNameFormat, const QByteArray &type)
:   d(new QNdefRecordPrivate)
{
    d->typeNameFormat = typeNameFormat;
    d->type = type;
}

/*!
    Destroys the NDEF record.
*/
QNdefRecord::~QNdefRecord()
{
}

/*!
    Assigns this NDEF record to \a other.
*/
QNdefRecord &QNdefRecord::operator=(const QNdefRecord &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

/*!
    Sets the type name format of the NDEF record to \a typeNameFormat.
*/
void QNdefRecord::setTypeNameFormat(TypeNameFormat typeNameFormat)
{
    if (!d)
        d = new QNdefRecordPrivate;

    d->typeNameFormat = typeNameFormat;
}

/*!
    Returns the type name format of the NDEF record.
*/
QNdefRecord::TypeNameFormat QNdefRecord::typeNameFormat() const
{
    if (!d)
        return Empty;

    switch (d->typeNameFormat) {
    case 0x00:
        return Empty;
    case 0x01:
        return NfcRtd;
    case 0x02:
        return Mime;
    case 0x03:
        return Uri;
    case 0x04:
        return ExternalRtd;
    case 0x06:
        return Unknown;
    case 0x05:
    default:
        return Unknown;
    }
}

/*!
    Sets the user type name format of the NDEF record to \a typeNameFormat. The value of 0x06
    cannot be set.
*/
void QNdefRecord::setUserTypeNameFormat(quint8 typeNameFormat)
{
    if (typeNameFormat == 0x06)
        return;

    if (!d)
        d = new QNdefRecordPrivate;

    d->typeNameFormat = typeNameFormat;
}

/*!
    Returns the user type name format of the NDEF record.
*/
quint8 QNdefRecord::userTypeNameFormat() const
{
    if (!d)
        return 0;

    return d->typeNameFormat;
}

/*!
    Sets the type of the NDEF record to \a type.
*/
void QNdefRecord::setType(const QByteArray &type)
{
    if (!d)
        d = new QNdefRecordPrivate;

    d->type = type;
}

/*!
    Returns the type of the NDEF record.
*/
QByteArray QNdefRecord::type() const
{
    if (!d)
        return QByteArray();

    return d->type;
}

/*!
    Sets the id of the NDEF record to \a id.
*/
void QNdefRecord::setId(const QByteArray &id)
{
    if (!d)
        d = new QNdefRecordPrivate;

    d->id = id;
}

/*!
    Returns the id of the NDEF record.
*/
QByteArray QNdefRecord::id() const
{
    if (!d)
        return QByteArray();

    return d->id;
}

/*!
    Sets the payload of the NDEF record to \a payload.
*/
void QNdefRecord::setPayload(const QByteArray &payload)
{
    if (!d)
        d = new QNdefRecordPrivate;

    d->payload = payload;
}

/*!
    Returns the payload of the NDEF record.
*/
QByteArray QNdefRecord::payload() const
{
    if (!d)
        return QByteArray();

    return d->payload;
}

/*!
    Returns true if \a other and this NDEF record are the same.
*/
bool QNdefRecord::operator==(const QNdefRecord &other) const
{
    if (d == other.d)
        return true;

    if (!d || !other.d)
        return false;

    if (d->typeNameFormat != other.d->typeNameFormat)
        return false;

    if (d->type != other.d->type)
        return false;

    if (d->id != other.d->id)
        return false;

    if (d->payload != other.d->payload)
        return false;

    return true;
}

QTM_END_NAMESPACE
