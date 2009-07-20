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
#ifdef QMESSAGING_OPTIONAL_FOLDER
#include "qmessagefolderfilterkey.h"
#include "qmessagefolderfilterkey_p.h"

/*!
    \class QMessageFolderFilterKey

    \preliminary
    \brief The QMessageFolderFilterKey class defines the parameters used for querying a subset of
    all available folders from the messaging store.
    \ingroup messaging

    A QMessageFolderFilterKey is composed of a folder property, an optional comparison operator
    and a comparison value. The QMessageFolderFilterKey class is used in conjunction with the 
    QMessageStore::queryFolders() and QMessageStore::countFolders() functions to filter results 
    which meet the criteria defined by the key.

    QMessageFolderFilterKeys can be combined using the logical operators (&), (|) and (~) to
    create more refined queries.

    \sa QMessageStore, QMessageFolder
*/

/*!
    \fn QMessageFolderFilterKey::setOptions(QMessageDataComparator::Options options)
  
    Set the options for the search key to \a options.

    \sa options()
*/

/*!
    \fn QMessageDataComparator::Options QMessageFolderFilterKey::options()
  
    Return the options for the search key.
    
    Default is no options set.

    \sa setOptions()
*/

/*!
    \fn QMessageFolderFilterKey::QMessageFolderFilterKey()
  
    Creates a QMessageFolderFilterKey without specifying matching parameters.

    A default-constructed key (one for which isEmpty() returns true) matches all folders. 

    \sa isEmpty()
*/

/*!
    \fn QMessageFolderFilterKey::QMessageFolderFilterKey(const QMessageFolderFilterKey &other)
  
    Constructs a copy of \a other.
    
    \sa operator=()
*/

/*!
    \fn QMessageFolderFilterKey::isEmpty() const
  
    Returns true if the key remains empty after default construction; otherwise returns false. 

    An empty key matches all folders.

    The result of combining an empty key with a non-empty key is the original non-empty key. 
    This is true regardless of whether the combination is formed by an AND or an OR operation.

    The result of combining two empty keys is an empty key.
*/

/*!
    \fn QMessageFolderFilterKey::operator~() const
  
    Returns a key that is the logical NOT of the value of this key.

    If this key is empty, the result will be a non-matching key; if this key is 
    non-matching, the result will be an empty key.

    \sa isEmpty()
*/

/*!
    \fn QMessageFolderFilterKey::operator&(const QMessageFolderFilterKey& other) const
  
    Returns a key that is the logical AND of this key and the value of key \a other.
*/

/*!
    \fn QMessageFolderFilterKey::operator|(const QMessageFolderFilterKey& other) const
  
    Returns a key that is the logical OR of this key and the value of key \a other.
*/

/*!
    \fn QMessageFolderFilterKey::operator&=(const QMessageFolderFilterKey& other)
  
    Performs a logical AND with this key and the key \a other and assigns the result
    to this key.
*/

/*!
    \fn QMessageFolderFilterKey::operator|=(const QMessageFolderFilterKey& other)
  
    Performs a logical OR with this key and the key \a other and assigns the result
    to this key.
*/

/*!
    \fn QMessageFolderFilterKey::operator==(const QMessageFolderFilterKey& other) const
  
    Returns \c true if the value of this key is the same as the key \a other. Returns 
    \c false otherwise.
*/

/*!
    \fn QMessageFolderFilterKey::operator=(const QMessageFolderFilterKey& other)
  
    Assign the value of the QMessageFolderFilterKey \a other to this.
*/

/*!
    \fn QMessageFolderFilterKey::id(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching folders whose identifier matches \a id, according to \a cmp.

    \sa QMessageFolder::id()
*/

/*!
    \fn QMessageFolderFilterKey::id(const QMessageFolderIdList &ids, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching folders whose identifier is a member of \a ids, according to \a cmp.

    \sa QMessageFolder::id()
*/

/*!
    \fn QMessageFolderFilterKey::id(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching folders whose identifier is a member of the set yielded by \a key, according to \a cmp.

    \sa QMessageFolder::id()
*/

/*!
    \fn QMessageFolderFilterKey::displayName(const QString &value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching folders whose display name matches \a value, according to \a cmp.

    \sa QMessageFolder::displayName()
*/

/*!
    \fn QMessageFolderFilterKey::displayName(const QString &value, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching folders whose display name matches the substring \a value, according to \a cmp.

    \sa QMessageFolder::displayName()
*/

/*!
    \fn QMessageFolderFilterKey::path(const QString &value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching folders whose path matches \a value, according to \a cmp.

    \sa QMessageFolder::path()
*/

/*!
    \fn QMessageFolderFilterKey::path(const QString &value, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching folders whose path matches the substring \a value, according to \a cmp.

    \sa QMessageFolder::path()
*/

/*!
    \fn QMessageFolderFilterKey::parentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching folders whose parent account identifier matches \a id, according to \a cmp.

    \sa QMessageAccount::id()
*/

/*!
    \fn QMessageFolderFilterKey::parentAccountId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching folders whose parent account identifier is a member of the set yielded by \a key, according to \a cmp.

    \sa QMessageAccount::id()
*/

/*!
    \fn QMessageFolderFilterKey::parentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching folders whose parent folder identifier matches \a id, according to \a cmp.

    \sa QMessageFolder::id()
*/

/*!
    \fn QMessageFolderFilterKey::parentFolderId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching folders whose parent folder identifier is a member of the set yielded by \a key, according to \a cmp.

    \sa QMessageFolder::id()
*/

/*!
    \fn QMessageFolderFilterKey::ancestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching folders whose ancestor folders' identifiers contain \a id, according to \a cmp.

    \sa QMessageFolder::id()
*/

/*!
    \fn QMessageFolderFilterKey::ancestorFolderIds(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching folders whose ancestor folders' identifiers contain a member of the set yielded by \a key, according to \a cmp.

    \sa QMessageFolder::id()
*/
#endif
