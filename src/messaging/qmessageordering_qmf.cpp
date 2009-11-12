/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qmessageordering.h"
#include "qmessagestore.h"
#include "qmfhelpers_p.h"

#include <qmailmessagesortkey.h>

using namespace QmfHelpers;

class QMessageOrderingPrivate
{
public:
    QMailMessageSortKey _key;

    //static QMessageOrdering convert(const QMailMessageSortKey &key);
    static QMailMessageSortKey convert(const QMessageOrdering &key);
};

/*
QMessageOrdering QMessageOrderingPrivate::convert(const QMailMessageSortKey &key)
{
    QMessageOrdering result;
    result.d_ptr->_key = key;
    return result;
}
*/

QMailMessageSortKey QMessageOrderingPrivate::convert(const QMessageOrdering &key)
{
    return key.d_ptr->_key;
}

namespace QmfHelpers {

/*
QMessageOrdering convert(const QMailMessageSortKey &key)
{
    return QMessageOrderingPrivate::convert(key);
}
*/

QMailMessageSortKey convert(const QMessageOrdering &key)
{
    return QMessageOrderingPrivate::convert(key);
}

}

QMessageOrdering::QMessageOrdering()
    : d_ptr(new QMessageOrderingPrivate)
{
}

QMessageOrdering::QMessageOrdering(const QMessageOrdering &other)
    : d_ptr(new QMessageOrderingPrivate)
{
    this->operator=(other);
}

QMessageOrdering::~QMessageOrdering()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessageOrdering& QMessageOrdering::operator=(const QMessageOrdering& other)
{
    if (&other != this) {
        d_ptr->_key = other.d_ptr->_key;
    }

    return *this;
}

bool QMessageOrdering::isEmpty() const
{
    return d_ptr->_key.isEmpty();
}

bool QMessageOrdering::isSupported() const
{
    return true; // stub
}

QMessageOrdering QMessageOrdering::operator+(const QMessageOrdering& other) const
{
    QMessageOrdering key;
    key.d_ptr->_key = d_ptr->_key & other.d_ptr->_key;
    return key;
}

QMessageOrdering& QMessageOrdering::operator+=(const QMessageOrdering& other)
{
    d_ptr->_key &= other.d_ptr->_key;
    return *this;
}

bool QMessageOrdering::operator==(const QMessageOrdering& other) const
{
    return (d_ptr->_key == other.d_ptr->_key);
}

QMessageOrdering QMessageOrdering::byType(Qt::SortOrder order)
{
    QMessageOrdering key;
    key.d_ptr->_key = QMailMessageSortKey::messageType(order);
    return key;
}

QMessageOrdering QMessageOrdering::bySender(Qt::SortOrder order)
{
    QMessageOrdering key;
    key.d_ptr->_key = QMailMessageSortKey::sender(order);
    return key;
}

QMessageOrdering QMessageOrdering::byRecipients(Qt::SortOrder order)
{
    QMessageOrdering key;
    key.d_ptr->_key = QMailMessageSortKey::recipients(order);
    return key;
}

QMessageOrdering QMessageOrdering::bySubject(Qt::SortOrder order)
{
    QMessageOrdering key;
    key.d_ptr->_key = QMailMessageSortKey::subject(order);
    return key;
}

QMessageOrdering QMessageOrdering::byTimeStamp(Qt::SortOrder order)
{
    QMessageOrdering key;
    key.d_ptr->_key = QMailMessageSortKey::timeStamp(order);
    return key;
}

QMessageOrdering QMessageOrdering::byReceptionTimeStamp(Qt::SortOrder order)
{
    QMessageOrdering key;
    key.d_ptr->_key = QMailMessageSortKey::receptionTimeStamp(order);
    return key;
}

QMessageOrdering QMessageOrdering::byStatus(QMessage::Status flag, Qt::SortOrder order)
{
    QMessageOrdering key;
    key.d_ptr->_key = QMailMessageSortKey::status(convert(flag), order);
    return key;
}

QMessageOrdering QMessageOrdering::byPriority(Qt::SortOrder order)
{
    QMessageOrdering key;

    if (order == Qt::DescendingOrder) {
        // Sort HighPriority descending, then LowPriority ascending
        key.d_ptr->_key = QMailMessageSortKey::status(highPriorityMask()) & QMailMessageSortKey::status(lowPriorityMask(), Qt::AscendingOrder);
    } else {
        // Sort LowPriority descending, then HighPriority ascending
        key.d_ptr->_key = QMailMessageSortKey::status(lowPriorityMask()) & QMailMessageSortKey::status(highPriorityMask(), Qt::AscendingOrder);
    }

    return key;
}

QMessageOrdering QMessageOrdering::bySize(Qt::SortOrder order)
{
    QMessageOrdering key;
    key.d_ptr->_key = QMailMessageSortKey::size(order);
    return key;
}

