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
#include "qmessagesortkey.h"
#include "qmessagestore.h"
#include "qmfhelpers_p.h"

#include <qmailmessagesortkey.h>

using namespace QmfHelpers;

class QMessageSortKeyPrivate
{
public:
    QMailMessageSortKey _key;

    static QMessageSortKey convert(const QMailMessageSortKey &key);
    static QMailMessageSortKey convert(const QMessageSortKey &key);
};

QMessageSortKey QMessageSortKeyPrivate::convert(const QMailMessageSortKey &key)
{
    QMessageSortKey result;
    result.d_ptr->_key = key;
    return result;
}

QMailMessageSortKey QMessageSortKeyPrivate::convert(const QMessageSortKey &key)
{
    return key.d_ptr->_key;
}

namespace QmfHelpers {

QMessageSortKey convert(const QMailMessageSortKey &key)
{
    return QMessageSortKeyPrivate::convert(key);
}

QMailMessageSortKey convert(const QMessageSortKey &key)
{
    return QMessageSortKeyPrivate::convert(key);
}

}

QMessageSortKey::QMessageSortKey()
    : d_ptr(new QMessageSortKeyPrivate)
{
}

QMessageSortKey::QMessageSortKey(const QMessageSortKey &other)
    : d_ptr(new QMessageSortKeyPrivate)
{
    this->operator=(other);
}

QMessageSortKey::~QMessageSortKey()
{
    delete d_ptr;
    d_ptr = 0;
}

bool QMessageSortKey::isEmpty() const
{
    return d_ptr->_key.isEmpty();
}

QMessageSortKey QMessageSortKey::operator+(const QMessageSortKey& other) const
{
    QMessageSortKey key;
    key.d_ptr->_key = d_ptr->_key & other.d_ptr->_key;
    return key;
}

QMessageSortKey& QMessageSortKey::operator+=(const QMessageSortKey& other)
{
    d_ptr->_key &= other.d_ptr->_key;
    return *this;
}

bool QMessageSortKey::operator==(const QMessageSortKey& other) const
{
    return (d_ptr->_key == other.d_ptr->_key);
}

const QMessageSortKey& QMessageSortKey::operator=(const QMessageSortKey& other)
{
    if (&other != this) {
        d_ptr->_key = other.d_ptr->_key;
    }

    return *this;
}

QMessageSortKey QMessageSortKey::type(Qt::SortOrder order)
{
    QMessageSortKey key;
    key.d_ptr->_key = QMailMessageSortKey::messageType(order);
    return key;
}

QMessageSortKey QMessageSortKey::sender(Qt::SortOrder order)
{
    QMessageSortKey key;
    key.d_ptr->_key = QMailMessageSortKey::sender(order);
    return key;
}

QMessageSortKey QMessageSortKey::recipients(Qt::SortOrder order)
{
    QMessageSortKey key;
    key.d_ptr->_key = QMailMessageSortKey::recipients(order);
    return key;
}

QMessageSortKey QMessageSortKey::subject(Qt::SortOrder order)
{
    QMessageSortKey key;
    key.d_ptr->_key = QMailMessageSortKey::subject(order);
    return key;
}

QMessageSortKey QMessageSortKey::timeStamp(Qt::SortOrder order)
{
    QMessageSortKey key;
    key.d_ptr->_key = QMailMessageSortKey::timeStamp(order);
    return key;
}

QMessageSortKey QMessageSortKey::receptionTimeStamp(Qt::SortOrder order)
{
    QMessageSortKey key;
    key.d_ptr->_key = QMailMessageSortKey::receptionTimeStamp(order);
    return key;
}

QMessageSortKey QMessageSortKey::status(Qt::SortOrder order)
{
    QMessageSortKey key;
    key.d_ptr->_key = QMailMessageSortKey::status(order);
    return key;
}

QMessageSortKey QMessageSortKey::priority(Qt::SortOrder order)
{
    QMessageSortKey key;

    if (order == Qt::DescendingOrder) {
        // Sort High descending, then Low ascending
        key.d_ptr->_key = QMailMessageSortKey::status(highPriorityMask()) & QMailMessageSortKey::status(lowPriorityMask(), Qt::AscendingOrder);
    } else {
        // Sort Low descending, then High ascending
        key.d_ptr->_key = QMailMessageSortKey::status(lowPriorityMask()) & QMailMessageSortKey::status(highPriorityMask(), Qt::AscendingOrder);
    }

    return key;
}

QMessageSortKey QMessageSortKey::size(Qt::SortOrder order)
{
    QMessageSortKey key;
    key.d_ptr->_key = QMailMessageSortKey::size(order);
    return key;
}

