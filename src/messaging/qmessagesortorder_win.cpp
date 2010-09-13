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
#include "qmessagesortorder_p.h"
#include "qmessage_p.h"


QTM_BEGIN_NAMESPACE

QMessageSortOrderPrivate::QMessageSortOrderPrivate(QMessageSortOrder *sortOrder)
    :q_ptr(sortOrder),
     _valid(true)
{
}

#define COMPARE(x,y) \
if ((x) < (y)) { \
    return true; \
} else if ((y) < (x)) { \
    return false; \
} else { \
    continue; \
}

bool QMessageSortOrderPrivate::lessThan(const QMessageSortOrder &sortOrder, const QMessage &l, const QMessage &r)
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
            left = &l;
            right = &r; 
        } else {
            left = &r;
            right = &l; 
        }

        switch (field)
        {
        case Type: COMPARE(left->type(), right->type())
        case Sender: COMPARE(QMessagePrivate::senderName(*left), QMessagePrivate::senderName(*right));
        case Recipients: {
            QString leftStr;
            QString rightStr;
            foreach (QMessageAddress a, left->to() + left->cc() + left->bcc()) {
                leftStr.append(a.addressee() + ";");
            }
            foreach (QMessageAddress a, right->to() + right->cc() + right->bcc()) {
                rightStr.append(a.addressee() + ";");
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
        case Priority: COMPARE(right->priority(), left->priority()) // Low priority comes first
        case Size: COMPARE(left->size(), right->size())
        }
    }
    return false; // equality
}

const int maxSortOrders(16);  // 16 levels of sort sortOrder should be more than sufficient
struct MapiSortOrderSet
{
    ULONG cSorts;
    ULONG cCategories;
    ULONG cExpanded;
    SSortOrder aSort[maxSortOrders];
};

void QMessageSortOrderPrivate::sortTable(QMessageManager::Error *error, const QMessageSortOrder &sortOrder, LPMAPITABLE messagesTable)
{
    MapiSortOrderSet multiSort;
    QMessageSortOrderPrivate *d(sortOrder.d_ptr);
    QList<QPair<Field, Qt::SortOrder> > fieldOrderList(d->_fieldOrderList);
    multiSort.cCategories = 0;
    multiSort.cExpanded = 0;

    if (!d || fieldOrderList.isEmpty())
        return;
    
    for (int i = 0; i < qMin<int>(maxSortOrders, fieldOrderList.count()); ++i) {
        ULONG order(TABLE_SORT_ASCEND);
        if (fieldOrderList[i].second == Qt::DescendingOrder)
            order = TABLE_SORT_DESCEND;
        
        ULONG propTag(PR_SUBJECT);
        switch (fieldOrderList[i].first)
        {
        case Subject:
            propTag = PR_SUBJECT;
            break;
        case TimeStamp:
            propTag = PR_CLIENT_SUBMIT_TIME;
            break;
        case ReceptionTimeStamp:
            propTag = PR_MESSAGE_DELIVERY_TIME;
            break;
        case Sender:
            propTag = PR_SENDER_NAME; // MAPI is limited to sorting by sender name only, sender name + sender address does not appear to be supported
            break;
        case Size:
            propTag = PR_MESSAGE_SIZE;
            break;
        case Type:
        case Read:
        case HasAttachments:
        case Incoming:
        case Removed:
        case Priority:
            fieldOrderList.removeAt(i);
            --i;
            continue;
        default:
            qWarning("Unhandled sort criteria");
            propTag = PR_MESSAGE_DELIVERY_TIME;
        }
        multiSort.aSort[i].ulPropTag = propTag;
        multiSort.aSort[i].ulOrder = order;
    }

    //Note: Windows Mobile does not support multiple sort levels
    multiSort.cSorts = qMin<int>(maxSortOrders, fieldOrderList.count());
    if (messagesTable->SortTable(reinterpret_cast<SSortOrderSet*>(&multiSort), 0) != S_OK) {
        *error = QMessageManager::NotYetImplemented;
        return;
    }
    return;
}

QMessageSortOrder QMessageSortOrderPrivate::from(QMessageSortOrderPrivate::Field field, Qt::SortOrder order)
{
    QMessageSortOrder result;
    QPair<QMessageSortOrderPrivate::Field, Qt::SortOrder> fieldOrder(field, order);
    result.d_ptr->_fieldOrderList.append(fieldOrder);
    return result;
}

// Is the sortOrder purely a filter type sortOrder, rather than a native sortOrder or composite sortOrder?
bool QMessageSortOrderPrivate::isFilterType(const QMessageSortOrder &sortOrder)
{
    QMessageSortOrderPrivate *d(sortOrder.d_ptr);
    QList<QPair<Field, Qt::SortOrder> >::iterator it(d->_fieldOrderList.begin());
    while (it != d->_fieldOrderList.end()) {
        Field field((*it).first);
        ++it;

        if ((field != Type) && (field != Read) && (field != HasAttachments) && (field != Incoming) && (field != Removed) && (field != Priority))
            return false;
    }
    return true;
}

// Break a filter up into multiple filters so that a heap merge can be performed on each sub-filter
QList<QMessageFilter> QMessageSortOrderPrivate::normalize(const QList<QMessageFilter> &filters, const QMessageSortOrder &sortOrder)
{
    QMessageSortOrderPrivate *d(sortOrder.d_ptr);
    QList<QMessageFilter> result(filters);

    QList<QPair<Field, Qt::SortOrder> >::iterator it(d->_fieldOrderList.begin());
    while (it != d->_fieldOrderList.end()) {
        Field field((*it).first);
        ++it;

        switch (field)
        {
            case Type:
            {
                QList<QMessageFilter> previous(result);
                result.clear();
                foreach(QMessageFilter filter, previous) {
                    result.append(QMessageFilter::byType(QMessage::NoType) & filter);
                    result.append(QMessageFilter::byType(QMessage::Mms) & filter);
                    result.append(QMessageFilter::byType(QMessage::Sms) & filter);
                    result.append(QMessageFilter::byType(QMessage::Email) & filter);
                    result.append(QMessageFilter::byType(QMessage::InstantMessage) & filter);
                }
            } break;
            case Read:
            {
                QList<QMessageFilter> previous(result);
                result.clear();
                foreach(QMessageFilter filter, previous) {
                    result.append(QMessageFilter::byStatus(QMessage::Read, QMessageDataComparator::Includes) & filter);
                    result.append(QMessageFilter::byStatus(QMessage::Read, QMessageDataComparator::Excludes) & filter);
                }
            } break;
            case HasAttachments:
            {
                QList<QMessageFilter> previous(result);
                result.clear();
                foreach(QMessageFilter filter, previous) {
                    result.append(QMessageFilter::byStatus(QMessage::HasAttachments, QMessageDataComparator::Includes) & filter);
                    result.append(QMessageFilter::byStatus(QMessage::HasAttachments, QMessageDataComparator::Excludes) & filter);
                }
            } break;
            case Incoming:
            {
                QList<QMessageFilter> previous(result);
                result.clear();
                foreach(QMessageFilter filter, previous) {
                    result.append(QMessageFilter::byStatus(QMessage::Incoming, QMessageDataComparator::Includes) & filter);
                    result.append(QMessageFilter::byStatus(QMessage::Incoming, QMessageDataComparator::Excludes) & filter);
                }
            } break;
            case Removed:
            {
                QList<QMessageFilter> previous(result);
                result.clear();
                foreach(QMessageFilter filter, previous) {
                    result.append(QMessageFilter::byStatus(QMessage::Removed, QMessageDataComparator::Includes) & filter);
                    result.append(QMessageFilter::byStatus(QMessage::Removed, QMessageDataComparator::Excludes) & filter);
                }
            } break;
            case Priority:
            {
                QList<QMessageFilter> previous(result);
                result.clear();
                foreach(QMessageFilter filter, previous) {
                    result.append(QMessageFilter::byPriority(QMessage::HighPriority) & filter);
                    result.append(QMessageFilter::byPriority(QMessage::NormalPriority) & filter);
                    result.append(QMessageFilter::byPriority(QMessage::LowPriority) & filter);
                }
            } break;
            default:
                break;
        }
    }
    return result;
}


QMessageSortOrder::QMessageSortOrder()
    :d_ptr(new QMessageSortOrderPrivate(this))
{
}

QMessageSortOrder::~QMessageSortOrder()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessageSortOrder::QMessageSortOrder(const QMessageSortOrder &other)
    :d_ptr(new QMessageSortOrderPrivate(this))
{
    this->operator=(other);
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
#ifdef _WIN32_WCE
    bool thisIsFilterType(QMessageSortOrderPrivate::isFilterType(*this));
    bool otherIsFilterType(QMessageSortOrderPrivate::isFilterType(other));
    // Multiple sort orders are not supported on WinCE
    if (!thisIsFilterType && !otherIsFilterType)
        sum.d_ptr->_valid = false;
#endif
    if (!this->isSupported() || !other.isSupported())
        sum.d_ptr->_valid = false;
    return sum;
}

QMessageSortOrder& QMessageSortOrder::operator+=(const QMessageSortOrder& other)
{
    if (&other == this)
        return *this;
    d_ptr->_fieldOrderList += other.d_ptr->_fieldOrderList;
#ifdef _WIN32_WCE
    bool thisIsFilterType(QMessageSortOrderPrivate::isFilterType(*this));
    bool otherIsFilterType(QMessageSortOrderPrivate::isFilterType(other));
    // Multiple sort orders are not supported on WinCE
    if (!thisIsFilterType && !otherIsFilterType)
        d_ptr->_valid = false;
#endif;
    if (!other.isSupported())
        d_ptr->_valid = false;
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
    // Partially implemented, can sort by sender name only not sender email address
    QMessageSortOrder result(QMessageSortOrderPrivate::from(QMessageSortOrderPrivate::Sender, order));
    result.d_ptr->_valid = false; // Not yet implemented
    return result;
}

QMessageSortOrder QMessageSortOrder::byRecipients(Qt::SortOrder order)
{
    QMessageSortOrder result(QMessageSortOrderPrivate::from(QMessageSortOrderPrivate::Recipients, order));
    result.d_ptr->_valid = false; // Not supported
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
#ifdef _WIN32_WCE
    result.d_ptr->_valid = false; // Not supported on WinCE
#endif
    return result;
}

QMessageSortOrder QMessageSortOrder::byReceptionTimeStamp(Qt::SortOrder order)
{
    return QMessageSortOrderPrivate::from(QMessageSortOrderPrivate::ReceptionTimeStamp, order);
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
#ifdef _WIN32_WCE
    result.d_ptr->_valid = false; // Not supported on WinCE
#endif
    return result;
}

QTM_END_NAMESPACE
