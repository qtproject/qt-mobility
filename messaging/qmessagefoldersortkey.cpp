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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessagefoldersortkey.h"
#include "qmessagefoldersortkey_p.h"

/*!
    \class QMessageFolderSortKey

    \preliminary
    \brief The QMessageFolderSortKey class defines the parameters used for sorting a subset of 
    queried folders from the messaging store.
    \ingroup messaging

    A QMessageFolderSortKey is composed of a folder property to sort and a sort order. 
    The QMessageFolderSortKey class is used in conjunction with the QMessageStore::queryFolders() 
    function to sort folder results according to the criteria defined by the sort key.
    
    \sa QMessageStore, QMessageFolderFilterKey
*/

/*!
    Create a QMessageFolderSortKey with specifying matching parameters.

    A default-constructed key (one for which isEmpty() returns true) sorts no folders. 

    The result of combining an empty key with a non-empty key is the same as the original 
    non-empty key.

    The result of combining two empty keys is an empty key.
*/
QMessageFolderSortKey::QMessageFolderSortKey()
{
}

/*!
    Returns true if the key remains empty after default construction; otherwise returns false.
*/
bool QMessageFolderSortKey::isEmpty() const
{
    return false; // stub
}

/*!
    Returns a key that which is the result of concatenating the vaue of this key and the value 
    of key \a other.
    
    This function determines sorting precedence.
*/
QMessageFolderSortKey QMessageFolderSortKey::operator+(const QMessageFolderSortKey& other) const
{
    Q_UNUSED(other)
    return QMessageFolderSortKey(); // stub
}

/*!
    Appends the value of the key \a other with the value of this key and assigns the result
    to this key.
    
    This function determines sorting precedence.
*/
QMessageFolderSortKey& QMessageFolderSortKey::operator+=(const QMessageFolderSortKey& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

/*!
    Returns \c true if the value of this key is the same as the key \a other. Returns 
    \c false otherwise.
*/
bool QMessageFolderSortKey::operator==(const QMessageFolderSortKey& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

/*!
    Assign the value of the QMessageFolderSortKey \a other to this.
*/
const QMessageFolderSortKey& QMessageFolderSortKey::operator=(const QMessageFolderSortKey& other)
{
    Q_UNUSED(other)
    return *this; // stub
}
    
/*!
    Returns a key that sorts folders by display name, according to \a order.

    \sa QMessageFolder::displayName()
*/
QMessageFolderSortKey QMessageFolderSortKey::displayName(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageFolderSortKey(); // stub
}

/*!
    Returns a key that sorts messages by path according to \a order.

    \sa QMessageFolder::path()
*/
QMessageFolderSortKey QMessageFolderSortKey::path(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageFolderSortKey(); // stub
}
