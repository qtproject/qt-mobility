/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qmessagesortorder.h"
#include "qmfhelpers_p.h"

#include <qmailmessagesortkey.h>

using namespace QTM_PREPEND_NAMESPACE(QmfHelpers);


QTM_BEGIN_NAMESPACE

class QMessageSortOrderPrivate
{
public:
    QMailMessageSortKey _key;

    //static QMessageSortOrder convert(const QMailMessageSortKey &key);
    static QMailMessageSortKey convert(const QMessageSortOrder &key);
};

/*
QMessageSortOrder QMessageSortOrderPrivate::convert(const QMailMessageSortKey &key)
{
    QMessageSortOrder result;
    result.d_ptr->_key = key;
    return result;
}
*/

QMailMessageSortKey QMessageSortOrderPrivate::convert(const QMessageSortOrder &key)
{
    return key.d_ptr->_key;
}

namespace QmfHelpers {

/*
QMessageSortOrder convert(const QMailMessageSortKey &key)
{
    return QMessageSortOrderPrivate::convert(key);
}
*/

QMailMessageSortKey convert(const QMessageSortOrder &key)
{
    return QMessageSortOrderPrivate::convert(key);
}

}

QMessageSortOrder::QMessageSortOrder()
    : d_ptr(new QMessageSortOrderPrivate)
{
}

QMessageSortOrder::QMessageSortOrder(const QMessageSortOrder &other)
    : d_ptr(new QMessageSortOrderPrivate)
{
    this->operator=(other);
}

QMessageSortOrder::~QMessageSortOrder()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessageSortOrder& QMessageSortOrder::operator=(const QMessageSortOrder& other)
{
    if (&other != this) {
        d_ptr->_key = other.d_ptr->_key;
    }

    return *this;
}

bool QMessageSortOrder::isEmpty() const
{
    return d_ptr->_key.isEmpty();
}

bool QMessageSortOrder::isSupported() const
{
    return true; // stub
}

QMessageSortOrder QMessageSortOrder::operator+(const QMessageSortOrder& other) const
{
    QMessageSortOrder key;
    key.d_ptr->_key = d_ptr->_key & other.d_ptr->_key;
    return key;
}

QMessageSortOrder& QMessageSortOrder::operator+=(const QMessageSortOrder& other)
{
    d_ptr->_key &= other.d_ptr->_key;
    return *this;
}

bool QMessageSortOrder::operator==(const QMessageSortOrder& other) const
{
    return (d_ptr->_key == other.d_ptr->_key);
}

QMessageSortOrder QMessageSortOrder::byType(Qt::SortOrder order)
{
    QMessageSortOrder key;
    key.d_ptr->_key = QMailMessageSortKey::messageType(order);
    return key;
}

QMessageSortOrder QMessageSortOrder::bySender(Qt::SortOrder order)
{
    QMessageSortOrder key;
    key.d_ptr->_key = QMailMessageSortKey::sender(order);
    return key;
}

QMessageSortOrder QMessageSortOrder::byRecipients(Qt::SortOrder order)
{
    QMessageSortOrder key;
    key.d_ptr->_key = QMailMessageSortKey::recipients(order);
    return key;
}

QMessageSortOrder QMessageSortOrder::bySubject(Qt::SortOrder order)
{
    QMessageSortOrder key;
    key.d_ptr->_key = QMailMessageSortKey::subject(order);
    return key;
}

QMessageSortOrder QMessageSortOrder::byTimeStamp(Qt::SortOrder order)
{
    QMessageSortOrder key;
    key.d_ptr->_key = QMailMessageSortKey::timeStamp(order);
    return key;
}

QMessageSortOrder QMessageSortOrder::byReceptionTimeStamp(Qt::SortOrder order)
{
    QMessageSortOrder key;
    key.d_ptr->_key = QMailMessageSortKey::receptionTimeStamp(order);
    return key;
}

QMessageSortOrder QMessageSortOrder::byStatus(QMessage::Status flag, Qt::SortOrder order)
{
    QMessageSortOrder key;
    key.d_ptr->_key = QMailMessageSortKey::status(convert(flag), order);
    return key;
}

QMessageSortOrder QMessageSortOrder::byPriority(Qt::SortOrder order)
{
    QMessageSortOrder key;

    if (order == Qt::DescendingOrder) {
        // Sort HighPriority descending, then LowPriority ascending
        key.d_ptr->_key = QMailMessageSortKey::status(highPriorityMask()) & QMailMessageSortKey::status(lowPriorityMask(), Qt::AscendingOrder);
    } else {
        // Sort LowPriority descending, then HighPriority ascending
        key.d_ptr->_key = QMailMessageSortKey::status(lowPriorityMask()) & QMailMessageSortKey::status(highPriorityMask(), Qt::AscendingOrder);
    }

    return key;
}

QMessageSortOrder QMessageSortOrder::bySize(Qt::SortOrder order)
{
    QMessageSortOrder key;
    key.d_ptr->_key = QMailMessageSortKey::size(order);
    return key;
}


QTM_END_NAMESPACE
