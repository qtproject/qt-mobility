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

/*!
    \class QMessageSortKey

    \preliminary
    \brief The QMessageSortKey class defines the parameters used for sorting a subset of 
    queried messages from the messaging store.
    \ingroup messaging

    A QMessageSortKey is composed of a message property to sort and a sort order. 
    The QMessageSortKey class is used in conjunction with the QMessageStore::queryMessages() 
    function to sort message results according to the criteria defined by the sort key.

    For example:
    To create a query for all messages sorted by their timestamp in decending order:
    \code
    QMessageSortKey sortKey(QMessageSortKey::timeStamp(Qt::DescendingOrder));
    QMessageIdList results = QMessageStore::instance()->queryMessages(QMessageFilterKey(), sortKey);
    \endcode
    
    \sa QMessageStore, QMessageFilterKey
*/

/*!
    \fn QMessageSortKey::QMessageSortKey()
  
    Create a QMessageSortKey with specifying matching parameters.

    A default-constructed key (one for which isEmpty() returns true) sorts no messages. 

    The result of combining an empty key with a non-empty key is the same as the original 
    non-empty key.

    The result of combining two empty keys is an empty key.
*/

/*!
    \fn QMessageSortKey::QMessageSortKey(const QMessageSortKey &other)
  
    Constructs a copy of \a other.
    
    \sa operator=()
*/

/*!
    \fn QMessageSortKey::~QMessageSortKey()
    
    Destroys the key.
*/

/*!
    \fn QMessageSortKey::isEmpty() const
  
    Returns true if the key remains empty after default construction; otherwise returns false.
*/

/*!
    \fn QMessageSortKey::operator+(const QMessageSortKey& other) const
  
    Returns a key that which is the result of concatenating the vaue of this key and the value 
    of key \a other.

    This function determines sorting precedence.
*/

/*!
    \fn QMessageSortKey::operator+=(const QMessageSortKey& other)
  
    Appends the value of the key \a other with the value of this key and assigns the result
    to this key.

    This function determines sorting precedence.
*/

/*!
    \fn QMessageSortKey::operator==(const QMessageSortKey& other) const
  
    Returns \c true if the value of this key is the same as the key \a other. Returns 
    \c false otherwise.
*/

/*!
    \fn QMessageSortKey::operator=(const QMessageSortKey& other)
  
    Assign the value of the QMessageSortKey \a other to this.
*/
    
/*!
    \fn QMessageSortKey::type(Qt::SortOrder order)
  
    Returns a key that sorts messages by their message type, according to \a order.

    \sa QMessage::type()
*/

/*!
    \fn QMessageSortKey::sender(Qt::SortOrder order)
  
    Returns a key that sorts messages by the address from which they were sent, according to \a order.

    \sa QMessage::from()
*/

/*!
    \fn QMessageSortKey::recipients(Qt::SortOrder order)
  
    Returns a key that sorts messages by the addresses to which they were sent, according to \a order.

    \sa QMessage::to()
*/

/*!
    \fn QMessageSortKey::subject(Qt::SortOrder order)
  
    Returns a key that sorts messages by their subject, according to \a order.

    \sa QMessage::subject()
*/

/*!
    \fn QMessageSortKey::timeStamp(Qt::SortOrder order)
  
    Returns a key that sorts messages by their origination timestamp, according to \a order.

    \sa QMessage::date()
*/

/*!
    \fn QMessageSortKey::receptionTimeStamp(Qt::SortOrder order)
  
    Returns a key that sorts messages by their reception timestamp, according to \a order.

    \sa QMessage::receivedDate()
*/

/*!
    \fn QMessageSortKey::status(QMessage::StatusFlag flag, Qt::SortOrder order)
  
    Returns a key that sorts messages by their status value \a flag, according to \a order.

    \sa QMessage::status()
*/

/*!
    \fn QMessageSortKey::priority(Qt::SortOrder order)
  
    Returns a key that sorts messages by their priority, according to \a order.

    \sa QMessage::priority()
*/

/*!
    \fn QMessageSortKey::size(Qt::SortOrder order)
  
    Returns a key that sorts messages by their size, according to \a order.

    \sa QMessage::size()
*/
