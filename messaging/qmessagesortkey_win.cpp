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
#include "qmessage.h"

QMessageSortKeyPrivate::QMessageSortKeyPrivate(QMessageSortKey *messageSortKey)
    :q_ptr(messageSortKey)
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
        case Sender: COMPARE(left->from().recipient(), right->from().recipient()) //TODO strip email only use name PR_SENDER_NAME
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
        case Status: COMPARE(left->status(), right->status())
        case Priority: COMPARE(left->priority(), right->priority())
        case Size: COMPARE(left->size(), right->size())
        }
    }
    return false; // equality
}

void QMessageSortKeyPrivate::sortTable(QMessageStore::ErrorCode *lastError, const QMessageSortKey &key, LPMAPITABLE messagesTable)
{
    // TODO support sort fields other than subject and multiple sort fields
    ULONG propTag(PR_SUBJECT);
    ULONG order(TABLE_SORT_ASCEND);

    QMessageSortKeyPrivate *d(key.d_ptr);
    if (d && !d->_fieldOrderList.isEmpty()) {
        if (d->_fieldOrderList.first().second == Qt::DescendingOrder)
            order = TABLE_SORT_DESCEND;
        switch (d->_fieldOrderList.first().first)
        {
        case Subject:
                propTag = PR_SUBJECT;
                break;
            case ReceptionTimeStamp:
                propTag = PR_MESSAGE_DELIVERY_TIME;
                break;
            case Sender:
                propTag = PR_SENDER_NAME;
                break;
            case Size:
                propTag = PR_CONTENT_LENGTH;
                break;
            default:
                propTag = PR_SUBJECT; // TODO handle all cases
        }
    }

    SizedSSortOrderSet(1, sortOrderSet) = { 1, 0, 0, { propTag, order } };
    if (messagesTable->SortTable(reinterpret_cast<SSortOrderSet*>(&sortOrderSet), 0) != S_OK) {
        *lastError = QMessageStore::NotYetImplemented;
        return;
    }
    return;
}

QMessageSortKey QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::Field field, Qt::SortOrder order)
{
    QMessageSortKey result;
    QPair<QMessageSortKeyPrivate::Field, Qt::SortOrder> fieldOrder(field, order);
    result.d_ptr = new QMessageSortKeyPrivate(&result);
    result.d_ptr->_fieldOrderList.append(fieldOrder);
    return result;
}


QMessageSortKey::QMessageSortKey()
    :d_ptr(0)
{
}

QMessageSortKey::~QMessageSortKey()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessageSortKey::QMessageSortKey(const QMessageSortKey &other)
    :d_ptr(0)
{
    this->operator=(other);
}

bool QMessageSortKey::isEmpty() const
{
    return !d_ptr;
}

QMessageSortKey QMessageSortKey::operator+(const QMessageSortKey& other) const
{
    Q_UNUSED(other)
    return QMessageSortKey(); // TODO implement it
}

QMessageSortKey& QMessageSortKey::operator+=(const QMessageSortKey& other)
{
    Q_UNUSED(other)
    return *this; // TODO implement it
}

bool QMessageSortKey::operator==(const QMessageSortKey& other) const
{
    if (!isEmpty()) {
        if (!other.isEmpty()) {
            return (d_ptr->_fieldOrderList == other.d_ptr->_fieldOrderList);
        }
        return false;
    } else {
        return other.isEmpty();
    }
}

const QMessageSortKey& QMessageSortKey::operator=(const QMessageSortKey& other)
{
    if (&other != this) {
        if (!other.isEmpty()) {
            if (!d_ptr) {
                d_ptr = new QMessageSortKeyPrivate(this);
            }
            d_ptr->_fieldOrderList = other.d_ptr->_fieldOrderList;
        } else {
            delete d_ptr;
            d_ptr = 0;
        }
    }

    return *this;
}

QMessageSortKey QMessageSortKey::type(Qt::SortOrder order)
{
    return QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::Type, order);
}

QMessageSortKey QMessageSortKey::sender(Qt::SortOrder order)
{
    return QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::Sender, order);
}

QMessageSortKey QMessageSortKey::recipients(Qt::SortOrder order)
{
    return QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::Recipients, order);
}

QMessageSortKey QMessageSortKey::subject(Qt::SortOrder order)
{
    return QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::Subject, order);
}

QMessageSortKey QMessageSortKey::timeStamp(Qt::SortOrder order)
{
    return QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::TimeStamp, order);
}

QMessageSortKey QMessageSortKey::receptionTimeStamp(Qt::SortOrder order)
{
    return QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::ReceptionTimeStamp, order);
}

QMessageSortKey QMessageSortKey::status(Qt::SortOrder order)
{
    return QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::Status, order);
}

QMessageSortKey QMessageSortKey::priority(Qt::SortOrder order)
{
    return QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::Priority, order);
}

QMessageSortKey QMessageSortKey::size(Qt::SortOrder order)
{
    return QMessageSortKeyPrivate::from(QMessageSortKeyPrivate::Size, order);
}
