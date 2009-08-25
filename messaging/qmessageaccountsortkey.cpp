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
#include "qmessageaccountsortkey.h"
#include "qmessageaccountsortkey_p.h"

/*!
    \class QMessageAccountSortKey

    \preliminary
    \brief The QMessageAccountSortKey class defines the parameters used for sorting a subset of 
    queried accounts from the messaging store.
    \ingroup messaging

    A QMessageAccountSortKey is composed of a account property to sort and a sort order. 
    The QMessageAccountSortKey class is used in conjunction with the QMessageStore::queryAccounts() 
    function to sort account results according to the criteria defined by the sort key.
    
    \sa QMessageStore, QMessageAccountFilterKey
*/

/*!
    \fn QMessageAccountSortKey::QMessageAccountSortKey()
  
    Create a QMessageAccountSortKey with specifying matching parameters.

    A default-constructed key (one for which isEmpty() returns true) sorts no accounts. 

    The result of combining an empty key with a non-empty key is the same as the original 
    non-empty key.

    The result of combining two empty keys is an empty key.
*/

/*!
    \fn QMessageAccountSortKey::QMessageAccountSortKey(const QMessageAccountSortKey &other)
  
    Constructs a copy of \a other.
    
    \sa operator=()
*/

/*!
    \fn QMessageAccountSortKey::~QMessageAccountSortKey()
    
    Destroys the key.
*/

/*!
    \fn QMessageAccountSortKey::isEmpty() const
  
    Returns true if the key remains empty after default construction; otherwise returns false.
*/

/*!
    \fn QMessageAccountSortKey::isSupported() const
  
    Returns true if the key is supported on the current platform; otherwise returns false.
*/

/*!
    \fn QMessageAccountSortKey::operator==(const QMessageAccountSortKey& other) const
  
    Returns \c true if the value of this key is the same as the key \a other. Returns 
    \c false otherwise.
*/

/*!
    \fn QMessageAccountSortKey::operator=(const QMessageAccountSortKey& other)
  
    Assign the value of the QMessageAccountSortKey \a other to this.
*/
    
/*!
    \fn QMessageAccountSortKey::name(Qt::SortOrder order)
  
    Returns a key that sorts accounts by name, according to \a order.

    \sa QMessageAccount::name()
*/
