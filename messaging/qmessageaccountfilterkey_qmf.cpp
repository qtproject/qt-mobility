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
#include "qmfhelpers_p.h"

#include <qmailaccountkey.h>

using namespace QmfHelpers;

class QMessageAccountFilterKeyPrivate
{
public:
    QMessageAccountFilterKeyPrivate() : _key(), _options(0) {}

    QMailAccountKey _key;
    QMessageDataComparator::Options _options;

    static QMessageAccountFilterKey convert(const QMailAccountKey &key);
    static QMailAccountKey convert(const QMessageAccountFilterKey &key);
};

QMessageAccountFilterKey QMessageAccountFilterKeyPrivate::convert(const QMailAccountKey &key)
{
    QMessageAccountFilterKey result;
    result.d_ptr->_key = key;
    return result;
}

QMailAccountKey QMessageAccountFilterKeyPrivate::convert(const QMessageAccountFilterKey &key)
{
    return key.d_ptr->_key;
}

namespace QmfHelpers {

QMessageAccountFilterKey convert(const QMailAccountKey &key)
{
    return QMessageAccountFilterKeyPrivate::convert(key);
}

QMailAccountKey convert(const QMessageAccountFilterKey &key)
{
    return QMessageAccountFilterKeyPrivate::convert(key);
}

}

QMessageAccountFilterKey::QMessageAccountFilterKey()
    : d_ptr(new QMessageAccountFilterKeyPrivate)
{
}

QMessageAccountFilterKey::QMessageAccountFilterKey(const QMessageAccountFilterKey &other)
    : d_ptr(new QMessageAccountFilterKeyPrivate)
{
    this->operator=(other);
}

QMessageAccountFilterKey::~QMessageAccountFilterKey()
{
    delete d_ptr;
    d_ptr = 0;
}

void QMessageAccountFilterKey::setOptions(QMessageDataComparator::Options options)
{
    d_ptr->_options |= options;
}

QMessageDataComparator::Options QMessageAccountFilterKey::options() const
{
    return d_ptr->_options;
}

bool QMessageAccountFilterKey::isEmpty() const
{
    return d_ptr->_key.isEmpty();
}

bool QMessageAccountFilterKey::isSupported() const
{
    return true; // TODO: Implement
}

QMessageAccountFilterKey QMessageAccountFilterKey::operator~() const
{
    QMessageAccountFilterKey result;
    result.d_ptr->_key = ~d_ptr->_key;
    return result;
}

QMessageAccountFilterKey QMessageAccountFilterKey::operator&(const QMessageAccountFilterKey& other) const
{
    QMessageAccountFilterKey result;
    result.d_ptr->_key = d_ptr->_key & other.d_ptr->_key;
    return result;
}

QMessageAccountFilterKey QMessageAccountFilterKey::operator|(const QMessageAccountFilterKey& other) const
{
    QMessageAccountFilterKey result;
    result.d_ptr->_key = d_ptr->_key | other.d_ptr->_key;
    return result;
}

const QMessageAccountFilterKey& QMessageAccountFilterKey::operator&=(const QMessageAccountFilterKey& other)
{
    d_ptr->_key &= other.d_ptr->_key;
    return *this;
}

const QMessageAccountFilterKey& QMessageAccountFilterKey::operator|=(const QMessageAccountFilterKey& other)
{
    d_ptr->_key |= other.d_ptr->_key;
    return *this;
}

bool QMessageAccountFilterKey::operator==(const QMessageAccountFilterKey& other) const
{
    return (d_ptr->_key == other.d_ptr->_key);
}

const QMessageAccountFilterKey& QMessageAccountFilterKey::operator=(const QMessageAccountFilterKey& other)
{
    if (&other != this) {
        d_ptr->_key = other.d_ptr->_key;
    }

    return *this;
}

QMessageAccountFilterKey QMessageAccountFilterKey::byId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageAccountFilterKey result;
    result.d_ptr->_key = QMailAccountKey::id(convert(id), convert(cmp));
    return result;
}

QMessageAccountFilterKey QMessageAccountFilterKey::byId(const QMessageAccountIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageAccountFilterKey result;
    result.d_ptr->_key = QMailAccountKey::id(convert(ids), convert(cmp));
    return result;
}

QMessageAccountFilterKey QMessageAccountFilterKey::byId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageAccountFilterKey result;
    result.d_ptr->_key = QMailAccountKey::id(convert(key), convert(cmp));
    return result;
}

QMessageAccountFilterKey QMessageAccountFilterKey::byFromAddress(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageAccountFilterKey result;
    result.d_ptr->_key = QMailAccountKey::fromAddress(value, convert(cmp));
    return result;
}

QMessageAccountFilterKey QMessageAccountFilterKey::byFromAddress(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageAccountFilterKey result;
    result.d_ptr->_key = QMailAccountKey::fromAddress(value, convert(cmp));
    return result;
}

QMessageAccountFilterKey QMessageAccountFilterKey::byName(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageAccountFilterKey result;
    result.d_ptr->_key = QMailAccountKey::name(value, convert(cmp));
    return result;
}

QMessageAccountFilterKey QMessageAccountFilterKey::byName(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageAccountFilterKey result;
    result.d_ptr->_key = QMailAccountKey::name(value, convert(cmp));
    return result;
}

