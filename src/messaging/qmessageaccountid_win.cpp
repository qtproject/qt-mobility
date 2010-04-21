/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessageaccountid_p.h"
#include <QByteArray>
#include <QDataStream>
#include <MAPIUtil.h>

QTM_BEGIN_NAMESPACE

#ifdef _WIN32_WCE

QMessageAccountId QMessageAccountIdPrivate::from(const MapiEntryId &storeKey)
{
    QMessageAccountId result;
    result.d_ptr->_storeRecordKey = storeKey;
    return result;
}

MapiEntryId QMessageAccountIdPrivate::storeRecordKey(const QMessageAccountId &id)
{
    return id.d_ptr->_storeRecordKey;
}

#else

QMessageAccountId QMessageAccountIdPrivate::from(const MapiRecordKey &storeKey)
{
    QMessageAccountId result;
    result.d_ptr->_storeRecordKey = storeKey;
    return result;
}

MapiRecordKey QMessageAccountIdPrivate::storeRecordKey(const QMessageAccountId &id)
{
    return id.d_ptr->_storeRecordKey;
}

#endif


QMessageAccountId::QMessageAccountId()
    : d_ptr(new QMessageAccountIdPrivate(this))
{
}

QMessageAccountId::QMessageAccountId(const QMessageAccountId& other)
    : d_ptr(new QMessageAccountIdPrivate(this))
{
    this->operator=(other);
}

QMessageAccountId::QMessageAccountId(const QString& id)
    : d_ptr(new QMessageAccountIdPrivate(this))
{
	QDataStream idStream(QByteArray::fromBase64(WinHelpers::stripIdPrefix(id).toLatin1()));
    idStream >> d_ptr->_storeRecordKey;
}

QMessageAccountId::~QMessageAccountId()
{
    delete d_ptr;
}

bool QMessageAccountId::operator==(const QMessageAccountId& other) const
{
    if (isValid()) {
        if (other.isValid()) {
            return (d_ptr->_storeRecordKey == other.d_ptr->_storeRecordKey);
        }
        return false;
    } else {
        return !other.isValid();
    }
}

QMessageAccountId& QMessageAccountId::operator=(const QMessageAccountId& other)
{
    if (&other != this) {
        d_ptr->_storeRecordKey = other.d_ptr->_storeRecordKey;
    }

    return *this;
}

bool QMessageAccountId::operator<(const QMessageAccountId& other) const
{
    return (d_ptr->_storeRecordKey < other.d_ptr->_storeRecordKey);
}

QString QMessageAccountId::toString() const
{
    if (!isValid())
        return QString();

    QByteArray encodedId;
    QDataStream encodedIdStream(&encodedId, QIODevice::WriteOnly);
    encodedIdStream << d_ptr->_storeRecordKey;

	return WinHelpers::addIdPrefix(encodedId.toBase64());
}

bool QMessageAccountId::isValid() const
{
    return !d_ptr->_storeRecordKey.isEmpty();
}

uint qHash(const QMessageAccountId &id)
{
    return qHash(id.d_ptr->_storeRecordKey);
}

QTM_END_NAMESPACE
