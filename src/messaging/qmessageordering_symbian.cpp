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
#include "qmessageordering_p.h"

#define COMPARE(x,y) \
if ((x) < (y)) { \
    return true; \
} else if ((y) < (x)) { \
    return false; \
} else { \
    continue; \
}

QMessageOrderingPrivate::QMessageOrderingPrivate(QMessageOrdering *ordering)
 : q_ptr(ordering),
   _valid(true)
{
}

QMessageOrdering QMessageOrderingPrivate::from(QMessageOrderingPrivate::Field field, Qt::SortOrder order)
{
    QMessageOrdering result;
    QPair<QMessageOrderingPrivate::Field, Qt::SortOrder> fieldOrder(field, order);
    result.d_ptr->_fieldOrderList.append(fieldOrder);
    return result;
}

QMessageOrderingPrivate* QMessageOrderingPrivate::implementation(const QMessageOrdering &ordering)

{
    return ordering.d_ptr;
}

bool QMessageOrderingPrivate::lessThan(const QMessageOrdering &ordering,
                                       const QMessage &message1, const QMessage &message2)
{
    QMessageOrderingPrivate *d(ordering.d_ptr);

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
        //TODO: case Sender: 
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
        case Priority: COMPARE(left->priority(), right->priority())
        case Size: COMPARE(left->size(), right->size())
        }
    }
    return false; // equality
}

QMessageOrdering::QMessageOrdering()
 : d_ptr(new QMessageOrderingPrivate(this))
{
}

QMessageOrdering::QMessageOrdering(const QMessageOrdering &other)
 : d_ptr(new QMessageOrderingPrivate(this))
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
		d_ptr->_fieldOrderList = other.d_ptr->_fieldOrderList;
		d_ptr->_valid = other.d_ptr->_valid;
	}
	
	return *this;
}

bool QMessageOrdering::isEmpty() const
{
	return d_ptr->_fieldOrderList.isEmpty();
}

bool QMessageOrdering::isSupported() const
{
	return d_ptr->_valid;
}

QMessageOrdering QMessageOrdering::operator+(const QMessageOrdering& other) const
{
	QMessageOrdering sum;
	sum.d_ptr->_fieldOrderList = d_ptr->_fieldOrderList + other.d_ptr->_fieldOrderList;
	return sum;
}

QMessageOrdering& QMessageOrdering::operator+=(const QMessageOrdering& other)
{
	if (&other == this) {
		 return *this;
	}
	d_ptr->_fieldOrderList += other.d_ptr->_fieldOrderList;
	return *this;
}

bool QMessageOrdering::operator==(const QMessageOrdering& other) const
{
	return (d_ptr->_fieldOrderList == other.d_ptr->_fieldOrderList);
}

QMessageOrdering QMessageOrdering::byType(Qt::SortOrder order)
{
	QMessageOrdering result(QMessageOrderingPrivate::from(QMessageOrderingPrivate::Type, order));
	return result;
}

QMessageOrdering QMessageOrdering::bySender(Qt::SortOrder order)
{
	QMessageOrdering result(QMessageOrderingPrivate::from(QMessageOrderingPrivate::Sender, order));
	return result;
}

QMessageOrdering QMessageOrdering::byRecipients(Qt::SortOrder order)
{
	QMessageOrdering result(QMessageOrderingPrivate::from(QMessageOrderingPrivate::Recipients, order));
	return result;
}

QMessageOrdering QMessageOrdering::bySubject(Qt::SortOrder order)
{
	QMessageOrdering result(QMessageOrderingPrivate::from(QMessageOrderingPrivate::Subject, order));
	return result;
}

QMessageOrdering QMessageOrdering::byTimeStamp(Qt::SortOrder order)
{
	QMessageOrdering result(QMessageOrderingPrivate::from(QMessageOrderingPrivate::TimeStamp, order));
	return result;
}

QMessageOrdering QMessageOrdering::byReceptionTimeStamp(Qt::SortOrder order)
{
	QMessageOrdering result(QMessageOrderingPrivate::from(QMessageOrderingPrivate::ReceptionTimeStamp, order));
	return result;
}

QMessageOrdering QMessageOrdering::byStatus(QMessage::Status flag, Qt::SortOrder order)
{
	QMessageOrdering result;
	switch (flag) {
	case QMessage::Read:
		result = QMessageOrderingPrivate::from(QMessageOrderingPrivate::Read, order);
	case QMessage::HasAttachments:
		result = QMessageOrderingPrivate::from(QMessageOrderingPrivate::HasAttachments, order);
	case QMessage::Incoming:
		result = QMessageOrderingPrivate::from(QMessageOrderingPrivate::Incoming, order);
	case QMessage::Removed:
		result = QMessageOrderingPrivate::from(QMessageOrderingPrivate::Removed, order);
	}
	return result;
}

QMessageOrdering QMessageOrdering::byPriority(Qt::SortOrder order)
{
	QMessageOrdering result(QMessageOrderingPrivate::from(QMessageOrderingPrivate::Priority, order));
	return result;
}

QMessageOrdering QMessageOrdering::bySize(Qt::SortOrder order)
{
	QMessageOrdering result(QMessageOrderingPrivate::from(QMessageOrderingPrivate::Size, order));
	return result;
}
