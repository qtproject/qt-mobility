/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qmessagefoldersortorder.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QMessageFolderSortOrder

    \brief The QMessageFolderSortOrder class defines the parameters used for sorting a subset of 
    queried folders from the messaging store.
    \ingroup messaging

    A QMessageFolderSortOrder is composed of a folder property to sort and a sort order. 
    The QMessageFolderSortOrder class is used in conjunction with the QMessageManager::queryFolders() 
    function to sort folder results according to the criteria defined by the sort order.
    
    \sa QMessageManager, QMessageFolderFilter
*/

/*!
    \fn QMessageFolderSortOrder::QMessageFolderSortOrder()
  
    Create a QMessageFolderSortOrder with specifying matching parameters.

    A default-constructed sort order (one for which isEmpty() returns true) sorts no folders. 

    The result of combining an empty sort order with a non-empty sort order is the same as the original 
    non-empty sort order.

    The result of combining two empty sort orders is an empty sort order.
*/

/*!
    \fn QMessageFolderSortOrder::QMessageFolderSortOrder(const QMessageFolderSortOrder &other)
  
    Constructs a copy of \a other.
*/

/*!
    \fn QMessageFolderSortOrder::~QMessageFolderSortOrder()
    
    Destroys the sort order.
*/

/*!
    \internal
    \fn QMessageFolderSortOrder::operator=(const QMessageFolderSortOrder& other)
*/
    
/*!
    \fn QMessageFolderSortOrder::isEmpty() const
  
    Returns true if the sort order remains empty after default construction; otherwise returns false.
*/

/*!
    \fn QMessageFolderSortOrder::isSupported() const
  
    Returns true if the sort order is supported on the current platform; otherwise returns false.
*/

/*!
    \fn QMessageFolderSortOrder::operator+(const QMessageFolderSortOrder& other) const
  
    Returns a sort order that which is the result of concatenating the vaue of this sort order and the value 
    of \a other.
    
    This function determines sorting precedence.
*/

/*!
    \fn QMessageFolderSortOrder::operator+=(const QMessageFolderSortOrder& other)
  
    Appends the value of the sort order \a other with the value of this sort order and assigns the result
    to this sort order.
    
    This function determines sorting precedence.
*/

/*!
    \internal
    \fn QMessageFolderSortOrder::operator==(const QMessageFolderSortOrder& other) const
*/

/*!
    \internal
*/
bool QMessageFolderSortOrder::operator!=(const QMessageFolderSortOrder& other) const
{
    return !operator==(other);
}

/*!
    \fn QMessageFolderSortOrder::byName(Qt::SortOrder order)
  
    Returns a sort order that sorts folders by display name, according to \a order.

    \sa QMessageFolder::name()
*/

/*!
    \fn QMessageFolderSortOrder::byPath(Qt::SortOrder order)
  
    Returns a sort order that sorts messages by path according to \a order.

    \sa QMessageFolder::path()
*/

QTM_END_NAMESPACE
