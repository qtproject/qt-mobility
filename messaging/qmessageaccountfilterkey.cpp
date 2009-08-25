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
#include "qmessageaccountfilterkey.h"
#include "qmessageaccountfilterkey_p.h"

/*!
    \class QMessageAccountFilterKey

    \preliminary
    \brief The QMessageAccountFilterKey class defines the parameters used for querying a subset of
    all available accounts from the messaging store.
    \ingroup messaging

    A QMessageAccountFilterKey is composed of an account property, an optional comparison operator
    and a comparison value. The QMessageAccountFilterKey class is used in conjunction with the 
    QMessageStore::queryAccounts() and QMessageStore::countAccounts() functions to filter results 
    which meet the criteria defined by the key.

    QMessageAccountFilterKeys can be combined using the logical operators (&), (|) and (~) to
    create more refined queries.

    \sa QMessageStore, QMessageAccount
*/

/*!
    \fn QMessageAccountFilterKey::setOptions(QMessageDataComparator::Options options)
  
    Set the options for the search key to \a options.

    \sa options()
*/

/*!
    \fn QMessageDataComparator::Options QMessageAccountFilterKey::options() const
  
    Return the options for the search key.
    
    Default is no options set.

    \sa setOptions()
*/

/*!
    \fn QMessageAccountFilterKey::QMessageAccountFilterKey()
  
    Creates a QMessageAccountFilterKey without specifying matching parameters.

    A default-constructed key (one for which isEmpty() returns true) matches all accounts. 

    \sa isEmpty()
*/

/*!
    \fn QMessageAccountFilterKey::QMessageAccountFilterKey(const QMessageAccountFilterKey &other)
  
    Constructs a copy of \a other.
    
    \sa operator=()
*/

/*!
    \fn QMessageAccountFilterKey::~QMessageAccountFilterKey()
    
    Destroys the key.
*/

/*!
    \fn QMessageAccountFilterKey::isEmpty() const
  
    Returns true if the key remains empty after default construction; otherwise returns false. 

    An empty key matches all accounts.

    The result of combining an empty key with a non-empty key is the original non-empty key. 
    This is true regardless of whether the combination is formed by an AND or an OR operation.

    The result of combining two empty keys is an empty key.
*/

/*!
    \fn QMessageAccountFilterKey::isSupported() const
  
    Returns true if the key is supported on the current platform; otherwise returns false.
*/

/*!
    \fn QMessageAccountFilterKey::operator~() const
  
    Returns a key that is the logical NOT of the value of this key.

    If this key is empty, the result will be a non-matching key; if this key is 
    non-matching, the result will be an empty key.

    \sa isEmpty()
*/

/*!
    \fn QMessageAccountFilterKey::operator&(const QMessageAccountFilterKey& other) const
  
    Returns a key that is the logical AND of this key and the value of key \a other.
*/

/*!
    \fn QMessageAccountFilterKey::operator|(const QMessageAccountFilterKey& other) const
  
    Returns a key that is the logical OR of this key and the value of key \a other.
*/

/*!
    \fn QMessageAccountFilterKey::operator&=(const QMessageAccountFilterKey& other)
  
    Performs a logical AND with this key and the key \a other and assigns the result
    to this key.
*/

/*!
    \fn QMessageAccountFilterKey::operator|=(const QMessageAccountFilterKey& other)
  
    Performs a logical OR with this key and the key \a other and assigns the result
    to this key.
*/

/*!
    \fn QMessageAccountFilterKey::operator==(const QMessageAccountFilterKey& other) const
  
    Returns \c true if the value of this key is the same as the key \a other. Returns 
    \c false otherwise.
*/

/*!
    \fn QMessageAccountFilterKey::operator=(const QMessageAccountFilterKey& other)
  
    Assign the value of the QMessageAccountFilterKey \a other to this.
*/

/*!
    \fn QMessageAccountFilterKey::id(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching accounts whose identifier matches \a id, according to \a cmp.

    \sa QMessageAccount::id()
*/

/*!
    \fn QMessageAccountFilterKey::id(const QMessageAccountIdList &ids, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching accounts whose identifier is a member of \a ids, according to \a cmp.

    \sa QMessageAccount::id()
*/

/*!
    \fn QMessageAccountFilterKey::id(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching accounts whose identifier is a member of the set yielded by \a key, according to \a cmp.

    \sa QMessageAccount::id()
*/

/*!
    \fn QMessageAccountFilterKey::fromAddress(const QString &value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching accounts whose from address matches \a value, according to \a cmp.

    \sa QMessageAccount::fromAddress()
*/

/*!
    \fn QMessageAccountFilterKey::fromAddress(const QString &value, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching accounts whose from address matches the substring \a value, according to \a cmp.

    \sa QMessageAccount::fromAddress()
*/

/*!
    \fn QMessageAccountFilterKey::name(const QString &value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching accounts whose name matches \a value, according to \a cmp.

    \sa QMessageAccount::name()
*/

/*!
    \fn QMessageAccountFilterKey::name(const QString &value, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching accounts whose name matches the substring \a value, according to \a cmp.

    \sa QMessageAccount::name()
*/
