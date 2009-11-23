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
#include "qmessagefolderordering.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QMessageFolderOrdering

    \preliminary
    \brief The QMessageFolderOrdering class defines the parameters used for sorting a subset of 
    queried folders from the messaging store.
    \ingroup messaging

    A QMessageFolderOrdering is composed of a folder property to sort and a sort order. 
    The QMessageFolderOrdering class is used in conjunction with the QMessageStore::queryFolders() 
    function to sort folder results according to the criteria defined by the ordering.
    
    \sa QMessageStore, QMessageFolderFilter
*/

/*!
    \fn QMessageFolderOrdering::QMessageFolderOrdering()
  
    Create a QMessageFolderOrdering with specifying matching parameters.

    A default-constructed ordering (one for which isEmpty() returns true) sorts no folders. 

    The result of combining an empty ordering with a non-empty ordering is the same as the original 
    non-empty ordering.

    The result of combining two empty keys is an empty ordering.
*/

/*!
    \fn QMessageFolderOrdering::QMessageFolderOrdering(const QMessageFolderOrdering &other)
  
    Constructs a copy of \a other.
*/

/*!
    \fn QMessageFolderOrdering::~QMessageFolderOrdering()
    
    Destroys the ordering.
*/

/*!
    \internal
    \fn QMessageFolderOrdering::operator=(const QMessageFolderOrdering& other)
*/
    
/*!
    \fn QMessageFolderOrdering::isEmpty() const
  
    Returns true if the ordering remains empty after default construction; otherwise returns false.
*/

/*!
    \fn QMessageFolderOrdering::isSupported() const
  
    Returns true if the ordering is supported on the current platform; otherwise returns false.
*/

/*!
    \fn QMessageFolderOrdering::operator+(const QMessageFolderOrdering& other) const
  
    Returns an ordering that which is the result of concatenating the vaue of this ordering and the value 
    of \a other.
    
    This function determines sorting precedence.
*/

/*!
    \fn QMessageFolderOrdering::operator+=(const QMessageFolderOrdering& other)
  
    Appends the value of the ordering \a other with the value of this ordering and assigns the result
    to this ordering.
    
    This function determines sorting precedence.
*/

/*!
    \internal
    \fn QMessageFolderOrdering::operator==(const QMessageFolderOrdering& other) const
*/

/*!
    \internal
*/
bool QMessageFolderOrdering::operator!=(const QMessageFolderOrdering& other) const
{
    return !operator==(other);
}

/*!
    \fn QMessageFolderOrdering::byDisplayName(Qt::SortOrder order)
  
    Returns an ordering that sorts folders by display name, according to \a order.

    \sa QMessageFolder::displayName()
*/

/*!
    \fn QMessageFolderOrdering::byPath(Qt::SortOrder order)
  
    Returns an ordering that sorts messages by path according to \a order.

    \sa QMessageFolder::path()
*/

QTM_END_NAMESPACE
