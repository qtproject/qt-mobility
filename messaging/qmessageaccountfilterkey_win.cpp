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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessageaccountfilterkey.h"
#include "qmessageaccountfilterkey_p.h"


QMessageAccountFilterKey::QMessageAccountFilterKey()
{
}

QMessageAccountFilterKey::QMessageAccountFilterKey(const QMessageAccountFilterKey &other)
{
    Q_UNUSED(other)
}

QMessageAccountFilterKey::~QMessageAccountFilterKey()
{
}

void QMessageAccountFilterKey::setOptions(QMessageDataComparator::Options options)
{
    Q_UNUSED(options)
}

QMessageDataComparator::Options QMessageAccountFilterKey::options() const
{
    return 0;
}

bool QMessageAccountFilterKey::isEmpty() const
{
    return false; // stub
}

bool QMessageAccountFilterKey::isSupported() const
{
    return true; // TODO: Implement
}

QMessageAccountFilterKey QMessageAccountFilterKey::operator~() const
{
    return QMessageAccountFilterKey(); // stub
}

QMessageAccountFilterKey QMessageAccountFilterKey::operator&(const QMessageAccountFilterKey& other) const
{
    Q_UNUSED(other)
    return QMessageAccountFilterKey(); // stub
}

QMessageAccountFilterKey QMessageAccountFilterKey::operator|(const QMessageAccountFilterKey& other) const
{
    Q_UNUSED(other)
    return QMessageAccountFilterKey(); // stub
}

const QMessageAccountFilterKey& QMessageAccountFilterKey::operator&=(const QMessageAccountFilterKey& other)
{
    Q_UNUSED(other)
    return *this;
}

const QMessageAccountFilterKey& QMessageAccountFilterKey::operator|=(const QMessageAccountFilterKey& other)
{
    Q_UNUSED(other)
    return *this;
}

bool QMessageAccountFilterKey::operator==(const QMessageAccountFilterKey& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

const QMessageAccountFilterKey& QMessageAccountFilterKey::operator=(const QMessageAccountFilterKey& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

QMessageAccountFilterKey QMessageAccountFilterKey::byId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageAccountFilterKey(); // stub
}

QMessageAccountFilterKey QMessageAccountFilterKey::byId(const QMessageAccountIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(ids)
    Q_UNUSED(cmp)
    return QMessageAccountFilterKey(); // stub
}

QMessageAccountFilterKey QMessageAccountFilterKey::byId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageAccountFilterKey(); // stub
}

QMessageAccountFilterKey QMessageAccountFilterKey::byFromAddress(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageAccountFilterKey(); // stub
}

QMessageAccountFilterKey QMessageAccountFilterKey::byFromAddress(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageAccountFilterKey(); // stub
}

QMessageAccountFilterKey QMessageAccountFilterKey::byName(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageAccountFilterKey(); // stub
}

QMessageAccountFilterKey QMessageAccountFilterKey::byName(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageAccountFilterKey(); // stub
}
