/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessageid.h"
#include <QByteArray>
#include <QDataStream>
#include <MAPIUtil.h>

typedef QByteArray MapiRecordKey;
typedef QByteArray MapiEntryId;

class QMessageIdPrivate
{
public:
    MapiRecordKey _recordKey;
    MapiEntryId _entryId;
    //TODO folder record key, store record key
    //TODO Move this declaration into qmessageid_p.h with windows guards
    //TODO Add a QMessaeIdPrivate* QMessageIdPrivate::d_ptr(QMessageId *q_ptr) method to get the d_ptr for a QMessageId, to help with efficiency concerns.
};

QMessageId::QMessageId()
    : d_ptr(0)
{
}

QMessageId::QMessageId(const QMessageId& other)
    : d_ptr(0)
{
    this->operator=(other);
}

QMessageId::QMessageId(const QString& id)
    : d_ptr(new QMessageIdPrivate)
{
    QDataStream idStream(QByteArray::fromBase64(id.toLatin1()));
    idStream >> d_ptr->_recordKey;
    if (!idStream.atEnd())
        idStream >> d_ptr->_entryId;
    // TODO Consider deleting d_ptr if _recordKey.count() != 16 as sanity check.
}

QMessageId::~QMessageId()
{
    delete d_ptr;
}

bool QMessageId::operator==(const QMessageId& other) const
{
    if (isValid()) {
        return (other.isValid() ? (d_ptr->_recordKey == other.d_ptr->_recordKey) : false);
    } else {
        return !other.isValid();
    }
}

QMessageId& QMessageId::operator=(const QMessageId& other)
{
    if (&other != this) {
        if (other.isValid()) {
            if (!d_ptr) {
                d_ptr = new QMessageIdPrivate;
            }
            d_ptr->_recordKey = other.d_ptr->_recordKey;
            d_ptr->_entryId = other.d_ptr->_entryId;
        } else {
            delete d_ptr;
            d_ptr = 0;
        }
    }

    return *this;
}

QString QMessageId::toString() const
{
    if (!isValid())
        return QString();
    QByteArray encodedId;
    QDataStream encodedIdStream(&encodedId, QIODevice::WriteOnly);
    encodedIdStream << d_ptr->_recordKey;
    encodedIdStream << d_ptr->_entryId;
    return encodedId.toBase64();
}

bool QMessageId::isValid() const
{
    return (d_ptr && !d_ptr->_recordKey.isEmpty()); //TODO again _recordKey should be 16 bytes
}
