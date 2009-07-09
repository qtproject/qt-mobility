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
#include "draft-api.h"

/*!
    \namespace QMessageDataComparator
    \ingroup messaging

    \brief The QMessageDataComparator namespace contains types used in specifying the comparison
    of MessageStore objects with user-defined values.
*/

/*!
    \enum QMessageDataComparator::EqualityComparator

    Defines the comparison operations that can be used to compare data elements 
    of QMessageStore objects for equality and inequality.

    \value Equal Represents the '==' operator.
    \value NotEqual Represents the '!=' operator.
*/

/*!
    \enum QMessageDataComparator::InclusionComparator

    Defines the comparison operations that can be used to compare data elements 
    of QMessageStore objects for inclusion or exclusion.

    \value Includes Represents an operation in which an associated property is tested to 
                    determine whether it includes a value.
    \value Excludes Represents an operation in which an associated property is tested to 
                    determine whether it is equal to the supplied value.
*/

/*!
    \enum QMessageDataComparator::RelationComparator

    Defines the comparison operations that can be used to compare data elements 
    of QMessageStore objects, according to a specific relation.

    \value LessThan Represents the '<' operator.
    \value LessThanEqual Represents the '<=' operator.
    \value GreaterThan Represents the '>' operator.
    \value GreaterThanEqual Represents the '>= operator'.
*/

/*!
    \enum QMessageDataComparator::Option

    This enum describes additional matching criteria when performing a search.
    
    \value FullWord        The key should only match a complete word
    \value CaseSensitive   The key should be case sensitive.
*/


class QMessageAccountFilterKeyPrivate : public QSharedData
{
public:
    QMessageAccountFilterKeyPrivate()
    {
    }
};

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
    Set the options for the search key to \a options.

    \sa options()
*/
void QMessageAccountFilterKey::setOptions(QMessageDataComparator::Options options)
{
    Q_UNUSED(options)
}

/*!
    Return the options for the search key.
    
    Default is no options set.

    \sa setOptions()
*/
QMessageDataComparator::Options QMessageAccountFilterKey::options()
{
    return 0;
}

/*!
    Creates a QMessageAccountFilterKey without specifying matching parameters.

    A default-constructed key (one for which isEmpty() returns true) matches all accounts. 

    \sa isEmpty()
*/
QMessageAccountFilterKey::QMessageAccountFilterKey()
{
}

/*!
    Returns true if the key remains empty after default construction; otherwise returns false. 

    An empty key matches all accounts.

    The result of combining an empty key with a non-empty key is the original non-empty key. 
    This is true regardless of whether the combination is formed by an AND or an OR operation.

    The result of combining two empty keys is an empty key.
*/
bool QMessageAccountFilterKey::isEmpty() const
{
    return false; // stub
}

/*!
    Returns a key that is the logical NOT of the value of this key.

    If this key is empty, the result will be a non-matching key; if this key is 
    non-matching, the result will be an empty key.

    \sa isEmpty()
*/
QMessageAccountFilterKey QMessageAccountFilterKey::operator~() const
{
    return QMessageAccountFilterKey(); // stub
}

/*!
    Returns a key that is the logical AND of this key and the value of key \a other.
*/
QMessageAccountFilterKey QMessageAccountFilterKey::operator&(const QMessageAccountFilterKey& other) const
{
    Q_UNUSED(other)
    return QMessageAccountFilterKey(); // stub
}

/*!
    Returns a key that is the logical OR of this key and the value of key \a other.
*/
QMessageAccountFilterKey QMessageAccountFilterKey::operator|(const QMessageAccountFilterKey& other) const
{
    Q_UNUSED(other)
    return QMessageAccountFilterKey(); // stub
}

/*!
    Performs a logical AND with this key and the key \a other and assigns the result
    to this key.
*/
const QMessageAccountFilterKey& QMessageAccountFilterKey::operator&=(const QMessageAccountFilterKey& other)
{
    Q_UNUSED(other)
    return *this;
}

/*!
    Performs a logical OR with this key and the key \a other and assigns the result
    to this key.
*/
const QMessageAccountFilterKey& QMessageAccountFilterKey::operator|=(const QMessageAccountFilterKey& other)
{
    Q_UNUSED(other)
    return *this;
}

/*!
    Returns \c true if the value of this key is the same as the key \a other. Returns 
    \c false otherwise.
*/
bool QMessageAccountFilterKey::operator==(const QMessageAccountFilterKey& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

/*!
    Assign the value of the QMessageAccountFilterKey \a other to this.
*/
const QMessageAccountFilterKey& QMessageAccountFilterKey::operator=(const QMessageAccountFilterKey& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

/*!
    Returns a key matching accounts whose identifier matches \a id, according to \a cmp.

    \sa QMessageAccount::id()
*/
QMessageAccountFilterKey QMessageAccountFilterKey::id(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageAccountFilterKey(); // stub
}

/*!
    Returns a key matching accounts whose identifier is a member of \a ids, according to \a cmp.

    \sa QMessageAccount::id()
*/
QMessageAccountFilterKey QMessageAccountFilterKey::id(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(ids)
    Q_UNUSED(cmp)
    return QMessageAccountFilterKey(); // stub
}

/*!
    Returns a key matching accounts whose identifier is a member of the set yielded by \a key, according to \a cmp.

    \sa QMessageAccount::id()
*/
QMessageAccountFilterKey QMessageAccountFilterKey::id(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageAccountFilterKey(); // stub
}

/*!
    Returns a key matching accounts whose from address matches \a value, according to \a cmp.

    \sa QMessageAccount::fromAddress()
*/
QMessageAccountFilterKey QMessageAccountFilterKey::fromAddress(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageAccountFilterKey(); // stub
}

/*!
    Returns a key matching accounts whose from address matches the substring \a value, according to \a cmp.

    \sa QMessageAccount::fromAddress()
*/
QMessageAccountFilterKey QMessageAccountFilterKey::fromAddress(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageAccountFilterKey(); // stub
}

/*!
    Returns a key matching accounts whose name matches \a value, according to \a cmp.

    \sa QMessageAccount::name()
*/
QMessageAccountFilterKey QMessageAccountFilterKey::name(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageAccountFilterKey(); // stub
}

/*!
    Returns a key matching accounts whose name matches the substring \a value, according to \a cmp.

    \sa QMessageAccount::name()
*/
QMessageAccountFilterKey QMessageAccountFilterKey::name(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageAccountFilterKey(); // stub
}


class QMessageAccountSortKeyPrivate : public QSharedData
{
public:
    QMessageAccountSortKeyPrivate()
    {
    }
};

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
    Create a QMessageAccountSortKey with specifying matching parameters.

    A default-constructed key (one for which isEmpty() returns true) sorts no accounts. 

    The result of combining an empty key with a non-empty key is the same as the original 
    non-empty key.

    The result of combining two empty keys is an empty key.
*/
QMessageAccountSortKey::QMessageAccountSortKey()
{
}

/*!
    Returns true if the key remains empty after default construction; otherwise returns false.
*/
bool QMessageAccountSortKey::isEmpty() const
{
    return false; // stub
}

/*!
    Returns \c true if the value of this key is the same as the key \a other. Returns 
    \c false otherwise.
*/
bool QMessageAccountSortKey::operator==(const QMessageAccountSortKey& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

/*!
    Assign the value of the QMessageAccountSortKey \a other to this.
*/
const QMessageAccountSortKey& QMessageAccountSortKey::operator=(const QMessageAccountSortKey& other)
{
    Q_UNUSED(other)
    return *this; // stub
}
    
/*!
    Returns a key that sorts accounts by name, according to \a order.

    \sa QMessageAccount::name()
*/
QMessageAccountSortKey QMessageAccountSortKey::name(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageAccountSortKey(); // stub
}


class QMessageFolderFilterKeyPrivate : public QSharedData
{
public:
    QMessageFolderFilterKeyPrivate()
    {
    }
};

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
    Set the options for the search key to \a options.

    \sa options()
*/
void QMessageFolderFilterKey::setOptions(QMessageDataComparator::Options options)
{
    Q_UNUSED(options)
}

/*!
    Return the options for the search key.
    
    Default is no options set.

    \sa setOptions()
*/
QMessageDataComparator::Options QMessageFolderFilterKey::options()
{
    return 0;
}

/*!
    Creates a QMessageFolderFilterKey without specifying matching parameters.

    A default-constructed key (one for which isEmpty() returns true) matches all folders. 

    \sa isEmpty()
*/
QMessageFolderFilterKey::QMessageFolderFilterKey()
{
}

/*!
    Returns true if the key remains empty after default construction; otherwise returns false. 

    An empty key matches all folders.

    The result of combining an empty key with a non-empty key is the original non-empty key. 
    This is true regardless of whether the combination is formed by an AND or an OR operation.

    The result of combining two empty keys is an empty key.
*/
bool QMessageFolderFilterKey::isEmpty() const
{
    return false; // stub
}

/*!
    Returns a key that is the logical NOT of the value of this key.

    If this key is empty, the result will be a non-matching key; if this key is 
    non-matching, the result will be an empty key.

    \sa isEmpty()
*/
QMessageFolderFilterKey QMessageFolderFilterKey::operator~() const
{
    return QMessageFolderFilterKey(); // stub
}

/*!
    Returns a key that is the logical AND of this key and the value of key \a other.
*/
QMessageFolderFilterKey QMessageFolderFilterKey::operator&(const QMessageFolderFilterKey& other) const
{
    Q_UNUSED(other)
    return QMessageFolderFilterKey(); // stub
}

/*!
    Returns a key that is the logical OR of this key and the value of key \a other.
*/
QMessageFolderFilterKey QMessageFolderFilterKey::operator|(const QMessageFolderFilterKey& other) const
{
    Q_UNUSED(other)
    return QMessageFolderFilterKey(); // stub
}

/*!
    Performs a logical AND with this key and the key \a other and assigns the result
    to this key.
*/
const QMessageFolderFilterKey& QMessageFolderFilterKey::operator&=(const QMessageFolderFilterKey& other)
{
    Q_UNUSED(other)
    return *this;
}

/*!
    Performs a logical OR with this key and the key \a other and assigns the result
    to this key.
*/
const QMessageFolderFilterKey& QMessageFolderFilterKey::operator|=(const QMessageFolderFilterKey& other)
{
    Q_UNUSED(other)
    return *this;
}

/*!
    Returns \c true if the value of this key is the same as the key \a other. Returns 
    \c false otherwise.
*/
bool QMessageFolderFilterKey::operator==(const QMessageFolderFilterKey& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

/*!
    Assign the value of the QMessageFolderFilterKey \a other to this.
*/
const QMessageFolderFilterKey& QMessageFolderFilterKey::operator=(const QMessageFolderFilterKey& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

/*!
    Returns a key matching folders whose identifier matches \a id, according to \a cmp.

    \sa QMessageFolder::id()
*/
QMessageFolderFilterKey QMessageFolderFilterKey::id(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

/*!
    Returns a key matching folders whose identifier is a member of \a ids, according to \a cmp.

    \sa QMessageFolder::id()
*/
QMessageFolderFilterKey QMessageFolderFilterKey::id(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(ids)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

/*!
    Returns a key matching folders whose identifier is a member of the set yielded by \a key, according to \a cmp.

    \sa QMessageFolder::id()
*/
QMessageFolderFilterKey QMessageFolderFilterKey::id(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

/*!
    Returns a key matching folders whose display name matches \a value, according to \a cmp.

    \sa QMessageFolder::displayName()
*/
QMessageFolderFilterKey QMessageFolderFilterKey::displayName(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

/*!
    Returns a key matching folders whose display name matches the substring \a value, according to \a cmp.

    \sa QMessageFolder::displayName()
*/
QMessageFolderFilterKey QMessageFolderFilterKey::displayName(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

/*!
    Returns a key matching folders whose path matches \a value, according to \a cmp.

    \sa QMessageFolder::path()
*/
QMessageFolderFilterKey QMessageFolderFilterKey::path(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

/*!
    Returns a key matching folders whose path matches the substring \a value, according to \a cmp.

    \sa QMessageFolder::path()
*/
QMessageFolderFilterKey QMessageFolderFilterKey::path(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

/*!
    Returns a key matching folders whose parent account identifier matches \a id, according to \a cmp.

    \sa QMessageAccount::id()
*/
QMessageFolderFilterKey QMessageFolderFilterKey::parentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

/*!
    Returns a key matching folders whose parent account identifier is a member of the set yielded by \a key, according to \a cmp.

    \sa QMessageAccount::id()
*/
QMessageFolderFilterKey QMessageFolderFilterKey::parentAccountId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

/*!
    Returns a key matching folders whose parent folder identifier matches \a id, according to \a cmp.

    \sa QMessageFolder::id()
*/
QMessageFolderFilterKey QMessageFolderFilterKey::parentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

/*!
    Returns a key matching folders whose parent folder identifier is a member of the set yielded by \a key, according to \a cmp.

    \sa QMessageFolder::id()
*/
QMessageFolderFilterKey QMessageFolderFilterKey::parentFolderId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

/*!
    Returns a key matching folders whose ancestor folders' identifiers contain \a id, according to \a cmp.

    \sa QMessageFolder::id()
*/
QMessageFolderFilterKey QMessageFolderFilterKey::ancestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

/*!
    Returns a key matching folders whose ancestor folders' identifiers contain a member of the set yielded by \a key, according to \a cmp.

    \sa QMessageFolder::id()
*/
QMessageFolderFilterKey QMessageFolderFilterKey::ancestorFolderIds(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}


class QMessageFolderSortKeyPrivate : public QSharedData
{
public:
    QMessageFolderSortKeyPrivate()
    {
    }
};

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


class QMessageFilterKeyPrivate : public QSharedData
{
public:
    QMessageFilterKeyPrivate()
    {
    }
};

/*!
    \class QMessageFilterKey

    \preliminary
    \brief The QMessageFilterKey class defines the parameters used for querying a subset of
    all available messages from the messaging store.
    \ingroup messaging

    A QMessageFilterKey is composed of a message property, an optional comparison operator
    and a comparison value. The QMessageFilterKey class is used in conjunction with the 
    QMessageStore::queryMessages() and QMessageStore::countMessages() functions to filter results 
    which meet the criteria defined by the key.

    QMessageFilterKeys can be combined using the logical operators (&), (|) and (~) to
    create more refined queries.

    For example:

    To create a query for all messages sent from "joe@user.com" with subject "meeting":
    \code
    QMessageFilterKey subjectKey(QMessageFilterKey::subject("meeting"));
    QMessageFilterKey senderKey(QMessageFilterKey::sender("joe@user.com"));
    QMessageIdList results = QMessageStore::instance()->queryMessages(subjectKey & senderKey);
    \endcode

    \sa QMessageStore, QMessage
*/

/*!
    Set the options for the search key to \a options.

    \sa options()
*/
void QMessageFilterKey::setOptions(QMessageDataComparator::Options options)
{
    Q_UNUSED(options)
}

/*!
    Return the options for the search key.
    
    Default is no options set.

    \sa setOptions()
*/
QMessageDataComparator::Options QMessageFilterKey::options()
{
    return 0;
}

/*!
    Creates a QMessageFilterKey without specifying matching parameters.

    A default-constructed key (one for which isEmpty() returns true) matches all messages. 

    \sa isEmpty()
*/
QMessageFilterKey::QMessageFilterKey()
{
}

/*!
    Returns true if the key remains empty after default construction; otherwise returns false. 

    An empty key matches all messages.

    The result of combining an empty key with a non-empty key is the original non-empty key. 
    This is true regardless of whether the combination is formed by an AND or an OR operation.

    The result of combining two empty keys is an empty key.
*/
bool QMessageFilterKey::isEmpty() const
{
    return false; // stub
}

/*!
    Returns a key that is the logical NOT of the value of this key.

    If this key is empty, the result will be a non-matching key; if this key is 
    non-matching, the result will be an empty key.

    \sa isEmpty()
*/
QMessageFilterKey QMessageFilterKey::operator~() const
{
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key that is the logical AND of this key and the value of key \a other.
*/
QMessageFilterKey QMessageFilterKey::operator&(const QMessageFilterKey& other) const
{
    Q_UNUSED(other)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key that is the logical OR of this key and the value of key \a other.
*/
QMessageFilterKey QMessageFilterKey::operator|(const QMessageFilterKey& other) const
{
    Q_UNUSED(other)
    return QMessageFilterKey(); // stub
}

/*!
    Performs a logical AND with this key and the key \a other and assigns the result
    to this key.
*/
const QMessageFilterKey& QMessageFilterKey::operator&=(const QMessageFilterKey& other)
{
    Q_UNUSED(other)
    return *this;
}

/*!
    Performs a logical OR with this key and the key \a other and assigns the result
    to this key.
*/
const QMessageFilterKey& QMessageFilterKey::operator|=(const QMessageFilterKey& other)
{
    Q_UNUSED(other)
    return *this;
}

/*!
    Returns \c true if the value of this key is the same as the key \a other. Returns 
    \c false otherwise.
*/
bool QMessageFilterKey::operator==(const QMessageFilterKey& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

/*!
    Assign the value of the QMessageFilterKey \a other to this.
*/
const QMessageFilterKey& QMessageFilterKey::operator=(const QMessageFilterKey& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

/*!
    Returns a key matching messages whose identifier matches \a id, according to \a cmp.

    \sa QMessage::id()
*/
QMessageFilterKey QMessageFilterKey::id(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose identifier is a member of \a ids, according to \a cmp.

    \sa QMessage::id()
*/
QMessageFilterKey QMessageFilterKey::id(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(ids)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose identifier is a member of the set yielded by \a key, according to \a cmp.

    \sa QMessage::id()
*/
QMessageFilterKey QMessageFilterKey::id(const QMessageFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose type matches \a type, according to \a cmp.

    \sa QMessage::type()
*/
QMessageFilterKey QMessageFilterKey::type(QMessage::Type type, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(type)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching accounts whose type is a bitwise match to \a type, according to \a cmp.

    \sa QMessage::type()
*/
QMessageFilterKey QMessageFilterKey::type(QMessage::TypeFlags type, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(type)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose sender matches \a value, according to \a cmp.

    \sa QMessage::from()
*/
QMessageFilterKey QMessageFilterKey::sender(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose sender matches the substring \a value, according to \a cmp.

    \sa QMessage::from()
*/
QMessageFilterKey QMessageFilterKey::sender(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose recipients include \a value, according to \a cmp.

    \sa QMessage::to(), QMessage::cc(), QMessage::bcc()
*/
QMessageFilterKey QMessageFilterKey::recipients(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose recipients include the substring \a value, 
    according to \a cmp.

    \sa QMessage::to(), QMessage::cc(), QMessage::bcc()
*/
QMessageFilterKey QMessageFilterKey::recipients(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose subject matches \a value, according 
    to \a cmp.

    \sa QMessage::subject()
*/
QMessageFilterKey QMessageFilterKey::subject(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose subject matches the 
    substring \a value, according to \a cmp.

    \sa QMessage::subject()
*/
QMessageFilterKey QMessageFilterKey::subject(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose body is textual and matches \a value, according 
    to \a cmp.

    \sa QMessage::body()
*/
QMessageFilterKey QMessageFilterKey::body(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose body is textual and matches the 
    substring \a value, according to \a cmp.

    \sa QMessage::body()
*/
QMessageFilterKey QMessageFilterKey::body(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose timestamp matches \a value, according to \a cmp.

    \sa QMessage::date()
*/
QMessageFilterKey QMessageFilterKey::timeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose timestamp has the relation to \a value that is specified by \a cmp.

    \sa QMessage::date()
*/
QMessageFilterKey QMessageFilterKey::timeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose reception timestamp matches \a value, according to \a cmp.

    \sa QMessage::receivedDate()
*/
QMessageFilterKey QMessageFilterKey::receptionTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose reception timestamp has the relation to \a value that is specified by \a cmp.

    \sa QMessage::receivedDate()
*/
QMessageFilterKey QMessageFilterKey::receptionTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose status matches \a value, according to \a cmp.

    \sa QMessage::status()
*/
QMessageFilterKey QMessageFilterKey::status(QMessage::Status value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose status is a bitwise match to \a mask, according to \a cmp.

    \sa QMessage::status()
*/
QMessageFilterKey QMessageFilterKey::status(QMessage::StatusFlags mask, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(mask)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose priority matches \a value, according to \a cmp.

    \sa QMessage::priority()
*/
QMessageFilterKey QMessageFilterKey::priority(QMessage::Priority value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose size matches \a value, according to \a cmp.

    \sa QMessage::size()
*/
QMessageFilterKey QMessageFilterKey::size(int value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose size matches \a value, according to \a cmp.

    \sa QMessage::size()
*/
QMessageFilterKey QMessageFilterKey::size(int value, QMessageDataComparator::RelationComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages having a custom field with the name \a name, whose 
    value matches \a value, according to \a cmp.

    \sa QMessage::customField()
*/
QMessageFilterKey QMessageFilterKey::customField(const QString &name, const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(name)
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages having a custom field with the name \a name, whose value 
    matches the substring \a value, according to \a cmp.

    \sa QMessage::customField()
*/
QMessageFilterKey QMessageFilterKey::customField(const QString &name, const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(name)
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose parent account's identifier matches \a id, according to 
    \a cmp.
    
    \sa QMessage::parentAccountId()
*/
QMessageFilterKey QMessageFilterKey::parentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose parent account's identifier is a member of the set 
    yielded by \a key, according to \a cmp.

    \sa QMessage::parentAccountId()
*/
QMessageFilterKey QMessageFilterKey::parentAccountId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose parent folder's identifier matches \a id, according to 
    \a cmp.
    
    \sa QMessage::parentFolderId()
*/
QMessageFilterKey QMessageFilterKey::parentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose parent folder's identifier is a member of the set 
    yielded by \a key, according to \a cmp.

    \sa QMessage::parentFolderId()
*/
QMessageFilterKey QMessageFilterKey::parentFolderId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose ancestor folders' identifiers contain \a id, according 
    to \a cmp.
    
    \sa QMessage::parentFolderId()
*/
QMessageFilterKey QMessageFilterKey::ancestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose ancestor folders' identifiers contain a member of the 
    set yielded by \a key, according to \a cmp.
        
    \sa QMessage::parentFolderId()
*/
QMessageFilterKey QMessageFilterKey::ancestorFolderIds(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}


class QMessageSortKeyPrivate : public QSharedData
{
public:
    QMessageSortKeyPrivate()
    {
    }
};

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
    Create a QMessageSortKey with specifying matching parameters.

    A default-constructed key (one for which isEmpty() returns true) sorts no messages. 

    The result of combining an empty key with a non-empty key is the same as the original 
    non-empty key.

    The result of combining two empty keys is an empty key.
*/
QMessageSortKey::QMessageSortKey()
{
}

/*!
    Returns true if the key remains empty after default construction; otherwise returns false.
*/
bool QMessageSortKey::isEmpty() const
{
    return false; // stub
}

/*!
    Returns a key that which is the result of concatenating the vaue of this key and the value 
    of key \a other.

    This function determines sorting precedence.
*/
QMessageSortKey QMessageSortKey::operator+(const QMessageSortKey& other) const
{
    Q_UNUSED(other)
    return QMessageSortKey(); // stub
}

/*!
    Appends the value of the key \a other with the value of this key and assigns the result
    to this key.

    This function determines sorting precedence.
*/
QMessageSortKey& QMessageSortKey::operator+=(const QMessageSortKey& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

/*!
    Returns \c true if the value of this key is the same as the key \a other. Returns 
    \c false otherwise.
*/
bool QMessageSortKey::operator==(const QMessageSortKey& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

/*!
    Assign the value of the QMessageSortKey \a other to this.
*/
const QMessageSortKey& QMessageSortKey::operator=(const QMessageSortKey& other)
{
    Q_UNUSED(other)
    return *this; // stub
}
    
/*!
    Returns a key that sorts messages by their message type, according to \a order.

    \sa QMessage::type()
*/
QMessageSortKey QMessageSortKey::type(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}

/*!
    Returns a key that sorts messages by the address from which they were sent, according to \a order.

    \sa QMessage::from()
*/
QMessageSortKey QMessageSortKey::sender(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}

/*!
    Returns a key that sorts messages by the addresses to which they were sent, according to \a order.

    \sa QMessage::to()
*/
QMessageSortKey QMessageSortKey::recipients(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}

/*!
    Returns a key that sorts messages by their subject, according to \a order.

    \sa QMessage::subject()
*/
QMessageSortKey QMessageSortKey::subject(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}

/*!
    Returns a key that sorts messages by their origination timestamp, according to \a order.

    \sa QMessage::date()
*/
QMessageSortKey QMessageSortKey::timeStamp(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}

/*!
    Returns a key that sorts messages by their reception timestamp, according to \a order.

    \sa QMessage::receivedDate()
*/
QMessageSortKey QMessageSortKey::receptionTimeStamp(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}

/*!
    Returns a key that sorts messages by their status values, according to \a order.

    \sa QMessage::status()
*/
QMessageSortKey QMessageSortKey::status(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}

/*!
    Returns a key that sorts messages by their priority, according to \a order.

    \sa QMessage::priority()
*/
QMessageSortKey QMessageSortKey::priority(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}

/*!
    Returns a key that sorts messages by their size, according to \a order.

    \sa QMessage::size()
*/
QMessageSortKey QMessageSortKey::size(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}


class QMessageStorePrivate : public QSharedData
{
public:
    QMessageStorePrivate()
    {
    }
};

/*!
    \class QMessageStore

    \preliminary
    \brief The QMessageStore class represents the main interface for storage and retrieval
    of messages, folders and accounts in the messaging store.

    \ingroup messaging

    The QMessageStore class is accessed through a singleton interface and provides functions 
    for adding, updating and deleting messages in the messaging store. 
    
    QMessageStore also provides queryMessages() and countMessages() functions for querying 
    and counting of QMessage, QMessageFolder and QMessageAccount objects by using the
    QMessageFilterKey, QMessageFolderFilterKey, QMessageAccountFilterKey classes 
    respectively.
    
    If a QMessageStore operation fails, the lastError() function will return an error code
    value indicating the failure mode encountered.  A successful operation will set the 
    lastError() result to QMessageStore::NoError.

    Messages in the messaging store are identified by QMessageId objects. The data associated 
    with a message is retrieved in the form of a QMessage object using message(). Likewise 
    a folder is retrieved in the form of a QMessageFolder object using folder(), and an 
    account is retrieved in the form of a QMessageAccount object using account().

    Messages can be inserted into the store using the addMessage() function, messages in the 
    store can be manipulated via the updateMessage() function ,and removed by the 
    removeMessage() functions.
    
    Messaging store manipulations involving messages are reported via the messagesAdded(), 
    messagesUpdated() and messagesRemoved() signals.

    \sa QMessage, QMessageId, QMessageContentContainerId, QMessageFilterKey, QMessageSortKey
*/

/*!
    \enum QMessageStore::RemovalOption

    Defines whether or not a message will be removed from the originating server.

    \value NoRemovalRecord     Do not remove the message from the originating server.
    \value CreateRemovalRecord Remove the message from the originating server.
*/

/*
    TODO capabilities AtomicBooleanSearch AtomicSlices Sms Mms Email Xmpp Presence AtomicExtendedSearching AtomicBodySearching
      ExtendedServices AtomicCustomSearching CaseInsensitiveSearching FullWordSearching
      
    Maybe capabilities are not necessary.
      Sms, Mms, Email, Xmpp are only enabled if a valid default account exists for that type.
    All other capabilities are searching related, lack of support for them can be detected by a QMailStore::lastError of NotYetImplemented.

    Activate/deactive on only adds, deletes or updates? Requires enum,
    enum NotificationType 
    {
        Add = 1,
        Removed,
        Changed
    }
    
    MAPI has move and copy signals, QMF does not.
*/

/*!
    \enum QMessageStore::ErrorCode

    Defines the result of attempting to perform a messaging store operation.

    \value NoError                The operation was successfully performed.
    \value InvalidId              The operation failed due to the specification of an invalid identifier.
    \value ConstraintFailure      The operation failed due to a constraint violation.
    \value ContentInaccessible    The operation failed because the content data cannot be accessed by the messaging store.
    \value NotYetImplemented      The operation failed because the messaging store does not yet implement the operation.
    \value FrameworkFault         The operation failed because the messaging store encountered an error in performing the operation.
    \value WorkingMemoryOverflow  The operation failed because the messaging store could not perform the operation within the constraint specified by setMaximumWorkingMemory().
*/

/*!
    Constructs the messaging store.
    
    If \a parent is not 0 then the messaging store will be deleted when \a parent is deleted.
    
    \sa instance()
*/
QMessageStore::QMessageStore(QObject *parent)
    : QObject(parent)
{
    Q_ASSERT(instance() != 0);
}

/*!
    Destroys the messaging store.
*/
QMessageStore::~QMessageStore()
{
}

/*!
    Returns the code of the last error condition reported by the messaging store.
*/
QMessageStore::ErrorCode QMessageStore::lastError() const
{
    return NotYetImplemented;
}

/*!
    Returns the \l{QMessageId}s of messages in the messaging store. If \a key is not empty 
    only messages matching the parameters set by \a key will be returned, otherwise 
    all message identifiers will be returned.
    If \a sortKey is not empty, then the identifiers will be sorted by the parameters 
    set by \a sortKey.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    
    \sa lastError(), countMessages(), setMaximumWorkingMemory()
*/
QMessageIdList QMessageStore::queryMessages(const QMessageFilterKey &key, const QMessageSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(key)
    Q_UNUSED(sortKey)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    return QMessageIdList(); // stub
}

/*!
    Returns the \l{QMessageId}s of messages in the messaging store. If \a key is not empty 
    only messages matching the parameters set by \a key will be returned, otherwise 
    all message identifiers will be returned.
    If \a sortKey is not empty, then the identifiers will be sorted by the parameters 
    set by \a sortKey.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    
    \sa lastError(), countMessages(), setMaximumWorkingMemory()
*/
QMessageFolderIdList QMessageStore::queryFolders(const QMessageFolderFilterKey &key, const QMessageFolderSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(key)
    Q_UNUSED(sortKey)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    return QMessageFolderIdList(); // stub
}

/*!
    Returns the \l{QMessageId}s of messages in the messaging store. If \a key is not empty 
    only messages matching the parameters set by \a key will be returned, otherwise 
    all message identifiers will be returned.
    If \a sortKey is not empty, then the identifiers will be sorted by the parameters 
    set by \a sortKey.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    
    \sa lastError(), countMessages(), setMaximumWorkingMemory()
*/
QMessageAccountIdList QMessageStore::queryAccounts(const QMessageAccountFilterKey &key, const QMessageAccountSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(key)
    Q_UNUSED(sortKey)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    return QMessageAccountIdList(); // stub
}

/*!
    Returns the count of the number of messages which match the 
    filtering criteria defined in QMessageFilterKey \a key. If 
    key is empty a count of all messages is returned.
    
    \sa lastError(), queryMessages(), setMaximumWorkingMemory()
*/
int QMessageStore::countMessages(const QMessageFilterKey& key) const
{
    Q_UNUSED(key)
    return 0; // stub
}

/*!
    Returns the count of the number of messages which match the 
    filtering criteria defined in QMessageFolderFilterKey \a key. If 
    key is empty a count of all messages is returned.
    
    \sa lastError(), queryMessages(), setMaximumWorkingMemory()
*/
int QMessageStore::countFolders(const QMessageFolderFilterKey& key) const
{
    Q_UNUSED(key)
    return 0; // stub
}

/*!
    Returns the count of the number of messages which match the 
    filtering criteria defined in QMessageAccountFilterKey \a key. If 
    key is empty a count of all messages is returned.
    
    \sa lastError(), queryMessages(), setMaximumWorkingMemory()
*/
int QMessageStore::countAccounts(const QMessageAccountFilterKey& key) const
{
    Q_UNUSED(key)
    return 0; // stub
}

/*!
    Removes the message with QMessageId \a id from the messaging store. If \a option is 
    QMessageStore::CreateRemovalRecord then a removal record will be created for the
    removed message.

    Returns \c true if the operation successfully updates the store; otherwise returns \c false.
    
    To ensure the change is propogated to any affected external server
    QMessageServiceAction::exportUpdates() should be subsequently called.

    \sa removeMessages(), addMessage(), updateMessage(), QMessageServiceAction::exportUpdates()
*/
bool QMessageStore::removeMessage(const QMessageId& id, RemovalOption option)
{
    Q_UNUSED(id)
    Q_UNUSED(option)
    return false; // stub
}

/*!
    Removes all messages identified by the key \a key from the messaging store.
    If \a option is QMessageStore::CreateRemovalRecord then removal records will be 
    created for each removed message.

    Returns \c true if the operation successfully updates the store; otherwise returns \c false. 
    
    To ensure the change is propogated to any affected external server
    QMessageServiceAction::exportUpdates() should be subsequently called.


    For example:

    To implement a function to remove a list messages identified by QMessageIds
    from the messaging store.
    \code
    bool removeMessages(const QMessageIdList& ids)
    {
        QMessageFilterKey idsFilter(ids);
        return QMessageStore::instance()->removeMessages(idsFilter);
    }
    \endcode

    \sa removeMessage(), addMessage(), updateMessage(), QMessageServiceAction::exportUpdates()
*/
bool QMessageStore::removeMessages(const QMessageFilterKey& key, QMessageStore::RemovalOption option)
{
    Q_UNUSED(key)
    Q_UNUSED(option)
    return true; // stub
}

/*!
    Updates the existing QMessage \a m on the messaging store.
    Returns \c true if the operation successfully updates the store; otherwise returns \c false. 
    
    To ensure the change is propogated to any affected external server
    QMessageServiceAction::exportUpdates() should be subsequently called.

    \sa message(), updateMessage(), removeMessage(), QMessageServiceAction::exportUpdates()
*/
bool QMessageStore::addMessage(QMessage *m)
{
    Q_UNUSED(m)
    return true; // stub
}

/*!
    Updates the existing QMessage \a m on the messaging store.
    Returns \c true if the operation successfully updates the store; otherwise returns \c false. 

    To ensure the change is propogated to any affected external server 
    QMessageServiceAction::exportUpdates() should be subsequently called.
    
    \sa addMessage(), removeMessage(), QMessageServiceAction::exportUpdates()
*/
bool QMessageStore::updateMessage(QMessage *m)
{
    Q_UNUSED(m)
    return true; // stub
}

/*!
   Returns the QMessage identified by \a id from the store.
*/
QMessage QMessageStore::message(const QMessageId& id) const
{
    Q_UNUSED(id)
    return QMessage(); // stub
}

/*!
   Returns the QMessageFolder identified by \a id from the store.
*/
QMessageFolder QMessageStore::folder(const QMessageFolderId& id) const
{
    Q_UNUSED(id)
    return QMessageFolder(); // stub
}

/*!
   Returns the QMessageAccount identified by \a id from the store.
*/
QMessageAccount QMessageStore::account(const QMessageAccountId& id) const
{
    Q_UNUSED(id)
    return QMessageAccount(); // stub
}

/*!
   If \a maximumBytes is 0, removes any constraint on the maximum memory 
   that can be allocated directly by the store when evaluating 
   countMessages() and queryMessages().
   
   Otherwise sets the maximum number of bytes than can be directly allocated 
   by the store when evaluating countMessages() and queryMessages() to 
   \a maximumBytes. A failure  to satisfy the working memory constraint is 
   reported by lastError() returning WorkingMemoryOverflow.
   
   The store only directly allocates working memory on platforms where 
   evaluating a boolean QMessageFilterKey based query requires a combination 
   of calls to the underlying platform query function.
   
   \sa maximumWorkingMemory(), ErrorCode, countMessages(), queryMessages(), lastError()
*/
void QMessageStore::setMaximumWorkingMemory(uint maximumBytes)
{
    Q_UNUSED(maximumBytes)
}

/*!
   Returns 0 if no constraint has been set by setMaximumWorkingMemory().
   
   Otherwise returns the maximum working memory as set by setMaximumWorkingMemory().
   
   The default maximum working memory is platform specific.
   
   \sa setMaximumWorkingMemory(), ErrorCode, countMessages(), queryMessages()
*/
uint QMessageStore::maximumWorkingMemory()
{
    return 0; // stub
}

/*!
    Returns the single instance of the QMessageStore class.

    If necessary, the store will be instantiated and initialized.
*/
QMessageStore* QMessageStore::instance()
{
    return 0;
}
    
/*!
    \fn void QMessageStore::messagesAdded(const QMessageIdList& ids)

    Signal that is emitted when the messages identified by the list \a ids are
    added to the messaging store.

    \sa messagesRemoved(), messagesUpdated(), startNotifications()
*/

/*!
    \fn void QMessageStore::messagesRemoved(const QMessageIdList& ids)

    Signal that is emitted when the messages in the list \a ids are
    removed from the messaging store.

    \sa messagesAdded(), messagesUpdated(), startNotifications()
*/

/*!
    \fn void QMessageStore::messagesUpdated(const QMessageIdList& ids)

    Signal that is emitted when the messages in the list \a ids are
    updated within the messaging store.

    \sa messagesAdded(), messagesRemoved(), startNotifications()
*/

/*!
    Starts emission of messagesAdded(), messagesRemoved() and messagesUpdated() signals
    for messages that match \a key.
    
    \sa messagesAdded(), messagesRemoved(), messagesUpdated(), stopNotifications()
*/
void QMessageStore::startNotifications(const QMessageFilterKey &key)
{
    Q_UNUSED(key)    
}

/*!
    Stops emission of messagesAdded(), messagesRemoved() and messagesUpdated() signals
    for messages that match \a key.
    
    \sa messagesAdded(), messagesRemoved(), messagesUpdated(), startNotifications()
*/
void QMessageStore::stopNotifications(const QMessageFilterKey &key)
{
    Q_UNUSED(key)    
}


class QMessageServiceActionPrivate : public QSharedData
{
public:
    QMessageServiceActionPrivate()
    {
    }
};

/*!
    \class QMessageServiceAction

    \preliminary
    \ingroup messaging

    \brief The QMessageServiceAction class provides the interface for requesting 
    messaging services.

    QMessageServiceAction provides the mechanisms for messaging clients to request services, 
    and to receive information in response.  All actions present the same 
    interface for communicating status, connectivity and progress information.

    All actions communicate changes in their operational state by emitting the activityChanged()
    signal.
    
    If an action operation fails, the lastErrorString() function will return a string
    indicating the failure mode encountered.  A successful operation will set the 
    lastError() result to a null string.
    
    A user may attempt to cancel an operation after it has been initiated.  The cancelOperation()
    slot is provided for this purpose.

    A QMessageServiceAction instance supports only a single request at any time.  A client
    may, however, use multiple QMessageServiceAction instances to send independent requests concurrently.
    Each QMessageServiceAction instance will report only the changes pertaining to the request
    that instance delivered.
*/

/*!
    \enum QMessageServiceAction::Activity

    This enum type is used to describe the activity state of the requested action.

    \value Pending          The action has not yet begun execution.
    \value InProgress       The action is currently executing.
    \value Successful       The action has completed successfully.
    \value Failed           The action could not be completed successfully, and has finished execution.
*/

/*!
    Constructs a message service action object.
    
    The \a parent is passed to the QObject constructor.
*/
QMessageServiceAction::QMessageServiceAction(QObject *parent)
    : QObject(parent)
{
}

/*!
    Destroys the message service action.
*/
QMessageServiceAction::~QMessageServiceAction()
{
}

/*!
  Transmit \a message using the account identified by \a accountId.
  
  \sa QMessage, QMessageAccountId
*/
void QMessageServiceAction::send(const QMessage &message, const QMessageAccountId &accountId)
{
    Q_UNUSED(message)
    Q_UNUSED(accountId)
}

/*!
  Open a composer application using \a message as a prototype.
  
  The default application for handling the type of \a message should be used.
  
  \sa QMessage::type()
*/
void QMessageServiceAction::compose(const QMessage &message)
{
    Q_UNUSED(message)
}

/*!
    Retrieve meta data of the message identified by \a id.  

    The meta data (including flags, from, to, subject, and date fields where applicable) of 
    the message identified by \a id should be retrieved.
    
    If the message can not be found on the originating server it will be marked as removed.

    \sa QMessageId, QMessage::Removed

*/
void QMessageServiceAction::retrieve(const QMessageId& id)
{
    Q_UNUSED(id)
}

/*!
    Retrieve the container identified by \a id, the contents of the container should also be 
    retrieved.
    
    \sa QMessageContentContainerId
*/
void QMessageServiceAction::retrieve(const QMessageContentContainerId& id)
{
    Q_UNUSED(id)
}

/*!
    Show the message identified by \a id.

    The default application for handling the type of message that \a id identifies should be used.

    \sa QMessageId, QMessage::type()
*/
void QMessageServiceAction::show(const QMessageId& id)
{
    Q_UNUSED(id)
}
    
/*!
    Synchronize any changes that have been queued by message store operations with external servers.

    \sa QMessageStore::addMessage(), QMessageStore::updateMessage(), QMessageStore::removeMessage(), QMessageStore::removeMessages()
*/
void QMessageServiceAction::exportUpdates()
{
}
    
/*!
    Returns the current activity state of the action.

    \sa activityChanged()
*/
QMessageServiceAction::Activity QMessageServiceAction::activity() const
{
    return Pending; // stub
}

/*!
    Attempts to cancel the last requested operation.
*/
void QMessageServiceAction::cancelOperation()
{
}

/*!
    \fn QMessageServiceAction::activityChanged(QMessageServiceAction::Activity a)

    This signal is emitted when the activity status of the action changes,
    with the new state described by \a a.

    \sa activity()
*/
void QMessageServiceAction::activityChanged(QMessageServiceAction::Activity a)
{
    Q_UNUSED(a)
}

/*!
    Returns a string indicating the last error condition reported by the action if any; 
    otherwise returns a null string.
*/
QString QMessageServiceAction::lastErrorString() const
{
    return QString::null;
}

//#include "draft-api.moc"
