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


QTM_BEGIN_NAMESPACE

/*!
    \class QMessageOrdering

    \preliminary
    \brief The QMessageOrdering class defines the parameters used for sorting a subset of 
    queried messages from the messaging store.
    \ingroup messaging

    A QMessageOrdering is composed of a message property to sort and a sort order. 
    The QMessageOrdering class is used in conjunction with the QMessageStore::queryMessages() 
    and QMessageServiceAction::queryMessages() functions to sort message results according to 
    the criteria defined by the ordering.

    For example:
    To create a query for all messages sorted by their timestamp in decending order:
    \code
    QMessageOrdering ordering(QMessageOrdering::byTimeStamp(Qt::DescendingOrder));
    QMessageIdList results = QMessageStore::instance()->queryMessages(QMessageFilter(), ordering);
    \endcode
    
    \sa QMessageStore, QMessageFilter
*/

/*!
    \fn QMessageOrdering::QMessageOrdering()
  
    Create a QMessageOrdering with specifying matching parameters.

    A default-constructed ordering (one for which isEmpty() returns true) sorts no messages. 

    The result of combining an empty ordering with a non-empty ordering is the same as the original 
    non-empty ordering.

    The result of combining two empty keys is an empty ordering.
    
    Windows mobile and desktop platforms do not support ordering bySender() and byRecipients(), 
    additionally the Windows mobile platform does not support ordering byTimeStamp() and bySize().
*/

/*!
    \fn QMessageOrdering::QMessageOrdering(const QMessageOrdering &other)
  
    Constructs a copy of \a other.
*/

/*!
    \fn QMessageOrdering::~QMessageOrdering()
    
    Destroys the ordering.
*/

/*!
    \internal
    \fn QMessageOrdering::operator=(const QMessageOrdering& other)
*/
    
/*!
    \fn QMessageOrdering::isEmpty() const
  
    Returns true if the ordering remains empty after default construction; otherwise returns false.
*/

/*!
    \fn QMessageOrdering::isSupported() const
  
    Returns true if the ordering is supported on the current platform; otherwise returns false.
*/

/*!
    \fn QMessageOrdering::operator+(const QMessageOrdering& other) const
  
    Returns an ordering that which is the result of concatenating the vaue of this ordering and the value 
    of ordering \a other.

    This function determines sorting precedence.
    
    On the Windows Mobile platform bySubject() and byReceptionTimeStamp() orderings can not 
    be combined with each other.
*/

/*!
    \fn QMessageOrdering::operator+=(const QMessageOrdering& other)
  
    Appends the value of the ordering \a other with the value of this ordering and assigns the result
    to this ordering.

    This function determines sorting precedence.

    On the Windows Mobile platform bySubject() and byReceptionTimeStamp() orderings can not 
    be combined with each other.
*/

/*!
    \internal
    \fn QMessageOrdering::operator==(const QMessageOrdering& other) const
*/

/*!
    \internal
*/
bool QMessageOrdering::operator!=(const QMessageOrdering& other) const
{
    return !operator==(other);
}

/*!
    \fn QMessageOrdering::byType(Qt::SortOrder order)
  
    Returns an ordering that sorts messages by their message type, according to \a order.

    \sa QMessage::type()
*/

/*!
    \fn QMessageOrdering::bySender(Qt::SortOrder order)
  
    Returns an ordering that sorts messages by the address from which they were sent, according to \a order.
    
    Not supported on the Windows mobile and desktop platforms.

    \sa QMessage::from()
*/

/*!
    \fn QMessageOrdering::byRecipients(Qt::SortOrder order)
  
    Returns an ordering that sorts messages by the addresses to which they were sent, according to \a order.

    Not supported on the Windows mobile and desktop platforms.
    
    \sa QMessage::to()
*/

/*!
    \fn QMessageOrdering::bySubject(Qt::SortOrder order)
  
    Returns an ordering that sorts messages by their subject, according to \a order.

    \sa QMessage::subject()
*/

/*!
    \fn QMessageOrdering::byTimeStamp(Qt::SortOrder order)
  
    Returns an ordering that sorts messages by their origination timestamp, according to \a order.

    Not supported on the Windows mobile platform.
    
    \sa QMessage::date(), byReceptionTimeStamp()
*/

/*!
    \fn QMessageOrdering::byReceptionTimeStamp(Qt::SortOrder order)
  
    Returns an ordering that sorts messages by their reception timestamp, according to \a order.

    \sa QMessage::receivedDate()
*/

/*!
    \fn QMessageOrdering::byStatus(QMessage::Status flag, Qt::SortOrder order)
  
    Returns an ordering that sorts messages by their status value \a flag, according to \a order.

    \sa QMessage::status()
*/

/*!
    \fn QMessageOrdering::byPriority(Qt::SortOrder order)
  
    Returns an ordering that sorts messages by their priority, according to \a order.

    \sa QMessage::priority()
*/

/*!
    \fn QMessageOrdering::bySize(Qt::SortOrder order)
  
    Returns an ordering that sorts messages by their size, according to \a order.

    Not supported on the Windows mobile platform.
    
    \sa QMessage::size()
*/

QTM_END_NAMESPACE
