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


QTM_BEGIN_NAMESPACE

/*!
    \class QMessageSortOrder

    \brief The QMessageSortOrder class defines the parameters used for sorting a subset of 
    queried messages from the messaging store.
    \ingroup messaging

    A QMessageSortOrder is composed of a message property to sort and a sort order. 
    The QMessageSortOrder class is used in conjunction with the QMessageManager::queryMessages() 
    and QMessageService::queryMessages() functions to sort message results according to 
    the criteria defined by the sort order.

    For example:
    To create a query for all messages sorted by their timestamp in decending order:
    \code
    QMessageSortOrder sortOrder(QMessageSortOrder::byTimeStamp(Qt::DescendingOrder));
    QMessageIdList results = QMessageManager().queryMessages(QMessageFilter(), sortOrder);
    \endcode
    
    \sa QMessageManager, QMessageFilter
*/

/*!
    \fn QMessageSortOrder::QMessageSortOrder()
  
    Create a QMessageSortOrder with specifying matching parameters.

    A default-constructed sort order (one for which isEmpty() returns true) sorts no messages. 

    The result of combining an empty sort order with a non-empty sort order is the same as the original 
    non-empty sort order.

    The result of combining two empty sort orders is an empty sort order.
    
    Windows mobile and desktop platforms do not support sorting bySender() and byRecipients(), 
    additionally the Windows mobile platform does not support sorting byTimeStamp() and bySize().
*/

/*!
    \fn QMessageSortOrder::QMessageSortOrder(const QMessageSortOrder &other)
  
    Constructs a copy of \a other.
*/

/*!
    \fn QMessageSortOrder::~QMessageSortOrder()
    
    Destroys the sort order.
*/

/*!
    \internal
    \fn QMessageSortOrder::operator=(const QMessageSortOrder& other)
*/
    
/*!
    \fn QMessageSortOrder::isEmpty() const
  
    Returns true if the sort order remains empty after default construction; otherwise returns false.
*/

/*!
    \fn QMessageSortOrder::isSupported() const
  
    Returns true if the sort order is supported on the current platform; otherwise returns false.
*/

/*!
    \fn QMessageSortOrder::operator+(const QMessageSortOrder& other) const
  
    Returns a sort order that which is the result of concatenating the vaue of this sort order and the value 
    of sort order \a other.

    This function determines sorting precedence.
    
    On the Windows Mobile platform bySubject() and byReceptionTimeStamp() sort orders can not 
    be combined with each other.
*/

/*!
    \fn QMessageSortOrder::operator+=(const QMessageSortOrder& other)
  
    Appends the value of the sort order \a other with the value of this sort order and assigns the result
    to this sort order.

    This function determines sorting precedence.

    On the Windows Mobile platform bySubject() and byReceptionTimeStamp() sort orders can not 
    be combined with each other.
*/

/*!
    \internal
    \fn QMessageSortOrder::operator==(const QMessageSortOrder& other) const
*/

/*!
    \internal
*/
bool QMessageSortOrder::operator!=(const QMessageSortOrder& other) const
{
    return !operator==(other);
}

/*!
    \fn QMessageSortOrder::byType(Qt::SortOrder order)
  
    Returns a sort order that sorts messages by their message type, according to \a order.

    \sa QMessage::type()
*/

/*!
    \fn QMessageSortOrder::bySender(Qt::SortOrder order)
  
    Returns a sort order that sorts messages by the address from which they were sent, according to \a order.
    
    Not supported on the Windows mobile and desktop platforms.

    \sa QMessage::from()
*/

/*!
    \fn QMessageSortOrder::byRecipients(Qt::SortOrder order)
  
    Returns a sort order that sorts messages by the addresses to which they were sent, according to \a order.

    Not supported on the Windows mobile and desktop platforms.
    
    \sa QMessage::to()
*/

/*!
    \fn QMessageSortOrder::bySubject(Qt::SortOrder order)
  
    Returns a sort order that sorts messages by their subject, according to \a order.

    \sa QMessage::subject()
*/

/*!
    \fn QMessageSortOrder::byTimeStamp(Qt::SortOrder order)
  
    Returns a sort order that sorts messages by their origination timestamp, according to \a order.

    Not supported on the Windows mobile platform.
    
    \sa QMessage::date(), byReceptionTimeStamp()
*/

/*!
    \fn QMessageSortOrder::byReceptionTimeStamp(Qt::SortOrder order)
  
    Returns a sort order that sorts messages by their reception timestamp, according to \a order.

    \sa QMessage::receivedDate()
*/

/*!
    \fn QMessageSortOrder::byStatus(QMessage::Status flag, Qt::SortOrder order)
  
    Returns a sort order that sorts messages by their status value \a flag, according to \a order.

    \sa QMessage::status()
*/

/*!
    \fn QMessageSortOrder::byPriority(Qt::SortOrder order)
  
    Returns a sort order that sorts messages by their priority, according to \a order.

    \sa QMessage::priority()
*/

/*!
    \fn QMessageSortOrder::bySize(Qt::SortOrder order)
  
    Returns a sort order that sorts messages by their size, according to \a order.

    Not supported on the Windows mobile platform.
    
    \sa QMessage::size()
*/

QTM_END_NAMESPACE
