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
#include "qmessagesortkey_p.h"
#include "qmessage_p.h"

QMessageSortKeyPrivate::QMessageSortKeyPrivate(QMessageSortKey *messageSortKey)
    :q_ptr(messageSortKey),
     _valid(true)
{
}

#define COMPARE(x,y) if ((x) < (y)) { \
                        return true; \
                    } else { \
                        if ((x) > (y)) { \
                            return false; \
                        } else { \
                            continue; \
                        } \
                    }

bool QMessageSortKeyPrivate::compare(const QMessageSortKey &key, const QMessage &l, const QMessage &r)
{
    QMessageSortKeyPrivate *d(key.d_ptr);
    if (!d)
        return l.subject() < r.subject(); //TODO change this to sort by reception time descending

    const QMessage *left;
    const QMessage *right;

    QList<QPair<Field, Qt::SortOrder> >::iterator it(d->_fieldOrderList.begin());
    while (it != d->_fieldOrderList.end()) {
        Field field((*it).first);
        Qt::SortOrder order((*it).second);
        ++it;
        if (order == Qt::AscendingOrder) {
            left = &l;
            right = &r; 
        } else {
            left = &r;
            right = &l; 
        }

        //TODO: Type and Priority will require multiple filter passes in QMessageStore
        //TODO: On windows mobile TimeStamp and RecievedTimeStamp should both use MESSAGE_DELIVERY_TIME
        //TODO: Recipients won't be supported on windows mobile, and will 
        //      require further investigation on desktop
        //TODO: Status may require multiple passes, or may not be implementable with MAPI

        switch (field)
        {
        case Type: COMPARE(left->type(), right->type())
        case Sender: COMPARE(QMessagePrivate::senderName(*left), QMessagePrivate::senderName(*right));
        case Recipients: {
            QString leftStr;
            QString rightStr;
            foreach (QMessageAddress a, left->to() + left->cc() + left->bcc())
                leftStr.append(a.recipient() + ";");
            foreach (QMessageAddress a, right->to() + right->cc() + right->bcc())
                rightStr.append(a.recipient() + ";");
            COMPARE(leftStr, rightStr)
                   }
        case Subject: COMPARE(left->subject(), right->subject())
        case TimeStamp: COMPARE(left->date(), right->date())
        case ReceptionTimeStamp: COMPARE(left->receivedDate(), right->receivedDate())
        case Read: COMPARE(left->status() & QMessage::Read, right->status() & QMessage::Read)
        case HasAttachments: COMPARE(left->status() & QMessage::Read, right->status() & QMessage::Read)
        case Incoming: COMPARE(left->status() & QMessage::Read, right->status() & QMessage::Read)
        case Removed: COMPARE(left->status() & QMessage::Read, right->status() & QMessage::Read)
        case Priority: COMPARE(left->priority(), right->priority())
        case Size: COMPARE(left->size(), right->size())
        }
    }
    return false; // equality
}

const int maxSortOrders(16);  // 16 levels of sort ordering should be more than sufficient
struct MapiSortOrderSet
{
    ULONG cSorts;
    ULONG cCategories;
    ULONG cExpanded;
    SSortOrder aSort[maxSortOrders];
};

void QMessageSortKeyPrivate::sortTable(QMessageStore::ErrorCode *lastError, const QMessageSortKey &key, LPMAPITABLE messagesTable)
{
    MapiSortOrderSet multiSort;
    QMessageSortKeyPrivate *d(key.d_ptr);
    QList<QPair<Field, Qt::SortOrder> > fieldOrderList(d->_fieldOrderList);
    multiSort.cSorts = qMin<int>(maxSortOrders, fieldOrderList.count());
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
            propTag = PR_MESSAGE_SIZE; /*TODO: Use PR_CONTENT_LENGTH on WinCE */
            break;
        default:
            qWarning("Unhandled sort criteria");
            propTag = PR_SUBJECT; // TODO handle all cases
        }
        multiSort.aSort[i].ulPropTag = propTag;
        multiSort.aSort[i].ulOrder = order;
    }

    //Note: WinCE does not support multiple level of sort leves and should return an error if more than 1 level is used
    //TODO: Update doc to reflect this
    if (messagesTable->SortTable(reinterpret_cast<SSortOrderSet*>(&multiSort), 0) != S_OK) {
        *lastError = QMessageStore::NotYetImplemented;
        return;
    }
    return;
}

QMessageSortKey QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::Field field, Qt::SortOrder order)
{
    QMessageSortKey result;
    QPair<QMessageSortKeyPrivate::Field, Qt::SortOrder> fieldOrder(field, order);
    result.d_ptr->_fieldOrderList.append(fieldOrder);
    return result;
}


QMessageSortKey::QMessageSortKey()
    :d_ptr(new QMessageSortKeyPrivate(this))
{
}

QMessageSortKey::~QMessageSortKey()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessageSortKey::QMessageSortKey(const QMessageSortKey &other)
    :d_ptr(new QMessageSortKeyPrivate(this))
{
    this->operator=(other);
}

bool QMessageSortKey::isEmpty() const
{
    return d_ptr->_fieldOrderList.isEmpty();
}

bool QMessageSortKey::isSupported() const
{
    return d_ptr->_valid;
}

QMessageSortKey QMessageSortKey::operator+(const QMessageSortKey& other) const
{
    QMessageSortKey sum;
    sum.d_ptr->_fieldOrderList = d_ptr->_fieldOrderList + other.d_ptr->_fieldOrderList;
#ifdef _WIN32_WCE
    // Multiple sort orders are not supported on WinCE
    sum.d_ptr->_valid = false;
#endif
    return sum;
}

QMessageSortKey& QMessageSortKey::operator+=(const QMessageSortKey& other)
{
    if (&other == this)
        return *this;
    d_ptr->_fieldOrderList += other.d_ptr->_fieldOrderList;
#ifdef _WIN32_WCE
    // Multiple sort orders are not supported on WinCE
    d__ptr->_valid = false;
#endif;
    return *this;
}

bool QMessageSortKey::operator==(const QMessageSortKey& other) const
{
    return (d_ptr->_fieldOrderList == other.d_ptr->_fieldOrderList);
}

const QMessageSortKey& QMessageSortKey::operator=(const QMessageSortKey& other)
{
    if (&other != this) {
        d_ptr->_fieldOrderList = other.d_ptr->_fieldOrderList;
        d_ptr->_valid = other.d_ptr->_valid;
    }

    return *this;
}

QMessageSortKey QMessageSortKey::type(Qt::SortOrder order)
{
    QMessageSortKey result(QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::Type, order)); // stub
    result.d_ptr->_valid = false; // Not yet implemented
    return result;
}

QMessageSortKey QMessageSortKey::sender(Qt::SortOrder order)
{
    // Partially implemented, can sort by sender name only not sender email address
    // TODO: Update doc to reflect this
    return QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::Sender, order);
}

QMessageSortKey QMessageSortKey::recipients(Qt::SortOrder order)
{
    QMessageSortKey result(QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::Recipients, order));
    result.d_ptr->_valid = false; // Not supported
    return result;
}

QMessageSortKey QMessageSortKey::subject(Qt::SortOrder order)
{
    QMessageSortKey result(QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::Subject, order));
    return result;
}

QMessageSortKey QMessageSortKey::timeStamp(Qt::SortOrder order)
{
    QMessageSortKey result(QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::TimeStamp, order));
#ifdef _WIN32_WCE
    result.d__ptr->_valid = false; // Not supported on WinCE
#endif
    return result;
}

QMessageSortKey QMessageSortKey::receptionTimeStamp(Qt::SortOrder order)
{
    return QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::ReceptionTimeStamp, order);
}

QMessageSortKey QMessageSortKey::status(QMessage::Status flag, Qt::SortOrder order)
{
    QMessageSortKey result;
    switch (flag) {
    case QMessage::Read:
        result = QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::Read, order);
    case QMessage::HasAttachments:
        result = QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::HasAttachments, order);
    case QMessage::Incoming:
        result = QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::Incoming, order);
    case QMessage::Removed:
        result = QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::Removed, order);
    }
    result.d_ptr->_valid = false; // Not yet implemented
    return result;
}

QMessageSortKey QMessageSortKey::priority(Qt::SortOrder order)
{
    QMessageSortKey result(QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::Priority, order));
    result.d_ptr->_valid = false; // Not yet implemented
    return result;
}

QMessageSortKey QMessageSortKey::size(Qt::SortOrder order)
{
    return QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::Size, order);
}
