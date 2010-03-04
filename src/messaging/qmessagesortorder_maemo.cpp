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
#include "qmessagesortorder.h"
#include "qmessagesortorder_p.h"
#include "qmessage_p.h"

#define COMPARE(x,y) \
if ((x) < (y)) { \
    return true; \
} else if ((y) < (x)) { \
    return false; \
} else { \
    continue; \
}

QTM_BEGIN_NAMESPACE

QMessageSortOrderPrivate::QMessageSortOrderPrivate(QMessageSortOrder *sortOrder)
 : q_ptr(sortOrder),
   _valid(true)
{
}

QMessageSortOrder QMessageSortOrderPrivate::from(QMessageSortOrderPrivate::Field field, Qt::SortOrder order)
{
    QMessageSortOrder result;
    QPair<QMessageSortOrderPrivate::Field, Qt::SortOrder> fieldOrder(field, order);
    result.d_ptr->_fieldOrderList.append(fieldOrder);
    return result;
}

QMessageSortOrderPrivate* QMessageSortOrderPrivate::implementation(const QMessageSortOrder &sortOrder)

{
    return sortOrder.d_ptr;
}

bool QMessageSortOrderPrivate::lessThan(const QMessageSortOrder &sortOrder,
                                       const QMessage &message1, const QMessage &message2)
{
    QMessageSortOrderPrivate *d(sortOrder.d_ptr);

    QList<QPair<Field, Qt::SortOrder> >::iterator it(d->_fieldOrderList.begin());
    while (it != d->_fieldOrderList.end()) {
        Field field((*it).first);
        Qt::SortOrder order((*it).second);
        ++it;

        const QMessage *left;
        const QMessage *right;
        if (order == Qt::AscendingOrder) {
            left = &message1;
            right = &message2;
        } else {
            left = &message2;
            right = &message1;
        }

        switch (field)
        {
        case Type: COMPARE(left->type(), right->type())
        case Sender: COMPARE(QMessagePrivate::senderName(*left), QMessagePrivate::senderName(*right));
        case Recipients: {
            QString leftStr;
            QString rightStr;
            foreach (QMessageAddress a, left->to() + left->cc() + left->bcc()) {
                leftStr.append(a.recipient() + ";");
            }
            foreach (QMessageAddress a, right->to() + right->cc() + right->bcc()) {
                rightStr.append(a.recipient() + ";");
            }
            COMPARE(leftStr, rightStr)
        }
        case Subject: COMPARE(left->subject(), right->subject())
        case TimeStamp: COMPARE(left->date(), right->date())
        case ReceptionTimeStamp: COMPARE(left->receivedDate(), right->receivedDate())
        case Read: COMPARE(left->status() & QMessage::Read, right->status() & QMessage::Read)
        case HasAttachments: COMPARE(left->status() & QMessage::HasAttachments, right->status() & QMessage::HasAttachments)
        case Incoming: COMPARE(left->status() & QMessage::Incoming, right->status() & QMessage::Incoming)
        case Removed: COMPARE(left->status() & QMessage::Removed, right->status() & QMessage::Removed)
        case Priority: COMPARE(right->priority(), left->priority())
        case Size: COMPARE(left->size(), right->size())
        }
    }
    return false; // equality
}

QMessageSortOrder::QMessageSortOrder()
 : d_ptr(new QMessageSortOrderPrivate(this))
{
}

QMessageSortOrder::QMessageSortOrder(const QMessageSortOrder &other)
 : d_ptr(new QMessageSortOrderPrivate(this))
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
        d_ptr->_fieldOrderList = other.d_ptr->_fieldOrderList;
        d_ptr->_valid = other.d_ptr->_valid;
    }

    return *this;
}

bool QMessageSortOrder::isEmpty() const
{
    return d_ptr->_fieldOrderList.isEmpty();
}

bool QMessageSortOrder::isSupported() const
{
    return d_ptr->_valid;
}

QMessageSortOrder QMessageSortOrder::operator+(const QMessageSortOrder& other) const
{
    QMessageSortOrder sum;
    sum.d_ptr->_fieldOrderList = d_ptr->_fieldOrderList + other.d_ptr->_fieldOrderList;
    return sum;
}

QMessageSortOrder& QMessageSortOrder::operator+=(const QMessageSortOrder& other)
{
    if (&other == this) {
        return *this;
    }
    d_ptr->_fieldOrderList += other.d_ptr->_fieldOrderList;
    return *this;
}

bool QMessageSortOrder::operator==(const QMessageSortOrder& other) const
{
    return (d_ptr->_fieldOrderList == other.d_ptr->_fieldOrderList);
}

QMessageSortOrder QMessageSortOrder::byType(Qt::SortOrder order)
{
    QMessageSortOrder result(QMessageSortOrderPrivate::from(QMessageSortOrderPrivate::Type, order));
    return result;
}

QMessageSortOrder QMessageSortOrder::bySender(Qt::SortOrder order)
{
    QMessageSortOrder result(QMessageSortOrderPrivate::from(QMessageSortOrderPrivate::Sender, order));
    return result;
}

QMessageSortOrder QMessageSortOrder::byRecipients(Qt::SortOrder order)
{
    QMessageSortOrder result(QMessageSortOrderPrivate::from(QMessageSortOrderPrivate::Recipients, order));
    return result;
}

QMessageSortOrder QMessageSortOrder::bySubject(Qt::SortOrder order)
{
    QMessageSortOrder result(QMessageSortOrderPrivate::from(QMessageSortOrderPrivate::Subject, order));
    return result;
}

QMessageSortOrder QMessageSortOrder::byTimeStamp(Qt::SortOrder order)
{
    QMessageSortOrder result(QMessageSortOrderPrivate::from(QMessageSortOrderPrivate::TimeStamp, order));
    return result;
}

QMessageSortOrder QMessageSortOrder::byReceptionTimeStamp(Qt::SortOrder order)
{
    QMessageSortOrder result(QMessageSortOrderPrivate::from(QMessageSortOrderPrivate::ReceptionTimeStamp, order));
    return result;
}

QMessageSortOrder QMessageSortOrder::byStatus(QMessage::Status flag, Qt::SortOrder order)
{
    QMessageSortOrder result;
    switch (flag) {
    case QMessage::Read:
        result = QMessageSortOrderPrivate::from(QMessageSortOrderPrivate::Read, order);
        break;
    case QMessage::HasAttachments:
        result = QMessageSortOrderPrivate::from(QMessageSortOrderPrivate::HasAttachments, order);
        break;
    case QMessage::Incoming:
        result = QMessageSortOrderPrivate::from(QMessageSortOrderPrivate::Incoming, order);
        break;
    case QMessage::Removed:
        result = QMessageSortOrderPrivate::from(QMessageSortOrderPrivate::Removed, order);
    break;
    }
    return result;
}

QMessageSortOrder QMessageSortOrder::byPriority(Qt::SortOrder order)
{
    QMessageSortOrder result(QMessageSortOrderPrivate::from(QMessageSortOrderPrivate::Priority, order));
    return result;
}

QMessageSortOrder QMessageSortOrder::bySize(Qt::SortOrder order)
{
    QMessageSortOrder result(QMessageSortOrderPrivate::from(QMessageSortOrderPrivate::Size, order));
    return result;
}

QTM_END_NAMESPACE
