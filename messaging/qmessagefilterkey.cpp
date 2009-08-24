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
#include "qmessagefilterkey.h"
#include "qmessagefilterkey_p.h"

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
    \fn QMessageFilterKey::setOptions(QMessageDataComparator::Options options)
  
    Set the options for the search key to \a options.

    \sa options()
*/

/*!
    \fn QMessageFilterKey::options() const
  
    Return the options for the search key.
    
    Default is no options set.

    \sa setOptions()
*/

/*!
    \fn QMessageFilterKey::QMessageFilterKey()
  
    Creates a QMessageFilterKey without specifying matching parameters.

    A default-constructed key (one for which isEmpty() returns true) matches all messages. 

    \sa isEmpty()
*/

/*!
    \fn QMessageFilterKey::QMessageFilterKey(const QMessageFilterKey &other)
  
    Constructs a copy of \a other.
    
    \sa operator=()
*/

/*!
    \fn QMessageFilterKey::~QMessageFilterKey()
    
    Destroys the key.
*/

/*!
    \fn QMessageFilterKey::isEmpty() const
  
    Returns true if the key remains empty after default construction; otherwise returns false. 

    An empty key matches all messages.

    The result of combining an empty key with a non-empty key is the original non-empty key. 
    This is true regardless of whether the combination is formed by an AND or an OR operation.

    The result of combining two empty keys is an empty key.
*/

/*!
    \fn QMessageFilterKey::isSupported() const
  
    Returns true if the key is supported on the current platform; otherwise returns false.
*/

/*!
    \fn QMessageFilterKey::operator~() const
  
    Returns a key that is the logical NOT of the value of this key.

    If this key is empty, the result will be a non-matching key; if this key is 
    non-matching, the result will be an empty key.

    \sa isEmpty()
*/

/*!
    \fn QMessageFilterKey::operator&(const QMessageFilterKey& other) const
  
    Returns a key that is the logical AND of this key and the value of key \a other.
*/

/*!
    \fn QMessageFilterKey::operator|(const QMessageFilterKey& other) const
  
    Returns a key that is the logical OR of this key and the value of key \a other.
*/

/*!
    \fn QMessageFilterKey::operator&=(const QMessageFilterKey& other)
  
    Performs a logical AND with this key and the key \a other and assigns the result
    to this key.
*/

/*!
    \fn QMessageFilterKey::operator|=(const QMessageFilterKey& other)
  
    Performs a logical OR with this key and the key \a other and assigns the result
    to this key.
*/

/*!
    \fn QMessageFilterKey::operator==(const QMessageFilterKey& other) const
  
    Returns \c true if the value of this key is the same as the key \a other. Returns 
    \c false otherwise.
*/

/*!
    \fn QMessageFilterKey& QMessageFilterKey::operator=(const QMessageFilterKey& other)
  
    Assign the value of the QMessageFilterKey \a other to this.
*/

/*!
    \fn QMessageFilterKey::id(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching messages whose identifier matches \a id, according to \a cmp.

    \sa QMessage::id()
*/

/*!
    \fn QMessageFilterKey::id(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching messages whose identifier is a member of \a ids, according to \a cmp.

    \sa QMessage::id()
*/

/*!
    \fn QMessageFilterKey::id(const QMessageFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching messages whose identifier is a member of the set yielded by \a key, according to \a cmp.

    \sa QMessage::id()
*/

/*!
    \fn QMessageFilterKey::type(QMessage::Type type, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching messages whose type matches \a type, according to \a cmp.

    \sa QMessage::type()
*/

/*!
    \fn QMessageFilterKey::type(QMessage::TypeFlags type, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching accounts whose type is a bitwise match to \a type, according to \a cmp.

    \sa QMessage::type()
*/

/*!
    \fn QMessageFilterKey::sender(const QString &value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching messages whose sender matches \a value, according to \a cmp.

    \sa QMessage::from()
*/

/*!
    \fn QMessageFilterKey::sender(const QString &value, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching messages whose sender matches the substring \a value, according to \a cmp.

    \sa QMessage::from()
*/

/*!
    \fn QMessageFilterKey::recipients(const QString &value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching messages whose recipients include \a value, according to \a cmp.

    \sa QMessage::to(), QMessage::cc(), QMessage::bcc()
*/

/*!
    \fn QMessageFilterKey::recipients(const QString &value, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching messages whose recipients include the substring \a value, 
    according to \a cmp.

    \sa QMessage::to(), QMessage::cc(), QMessage::bcc()
*/

/*!
    \fn QMessageFilterKey::subject(const QString &value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching messages whose subject matches \a value, according 
    to \a cmp.

    \sa QMessage::subject()
*/

/*!
    \fn QMessageFilterKey::subject(const QString &value, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching messages whose subject matches the 
    substring \a value, according to \a cmp.

    \sa QMessage::subject()
*/

/*!
    \fn QMessageFilterKey::timeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching messages whose timestamp matches \a value, according to \a cmp.

    \sa QMessage::date()
*/

/*!
    \fn QMessageFilterKey::timeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
  
    Returns a key matching messages whose timestamp has the relation to \a value that is specified by \a cmp.

    \sa QMessage::date()
*/

/*!
    \fn QMessageFilterKey::receptionTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching messages whose reception timestamp matches \a value, according to \a cmp.

    \sa QMessage::receivedDate()
*/

/*!
    \fn QMessageFilterKey::receptionTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
  
    Returns a key matching messages whose reception timestamp has the relation to \a value that is specified by \a cmp.

    \sa QMessage::receivedDate()
*/

/*!
    \fn QMessageFilterKey::status(QMessage::Status value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching messages whose status matches \a value, according to \a cmp.

    \sa QMessage::status()
*/

/*!
    \fn QMessageFilterKey::status(QMessage::StatusFlags mask, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching messages whose status is a bitwise match to \a mask, according to \a cmp.

    \sa QMessage::status()
*/

/*!
    \fn QMessageFilterKey::priority(QMessage::Priority value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching messages whose priority matches \a value, according to \a cmp.

    \sa QMessage::priority()
*/

/*!
    \fn QMessageFilterKey::size(int value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching messages whose size matches \a value, according to \a cmp.

    \sa QMessage::size()
*/

/*!
    \fn QMessageFilterKey::size(int value, QMessageDataComparator::RelationComparator cmp)
  
    Returns a key matching messages whose size matches \a value, according to \a cmp.

    \sa QMessage::size()
*/

/*!
    \fn QMessageFilterKey::customField(const QString &name, const QString &value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching messages having a custom field with the name \a name, whose 
    value matches \a value, according to \a cmp.

    \sa QMessage::customField()
*/

/*!
    \fn QMessageFilterKey::customField(const QString &name, const QString &value, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching messages having a custom field with the name \a name, whose value 
    matches the substring \a value, according to \a cmp.

    \sa QMessage::customField()
*/

/*!
    \fn QMessageFilterKey::parentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching messages whose parent account's identifier matches \a id, according to 
    \a cmp.
    
    \sa QMessage::parentAccountId()
*/

/*!
    \fn QMessageFilterKey::parentAccountId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching messages whose parent account's identifier is a member of the set 
    yielded by \a key, according to \a cmp.

    \sa QMessage::parentAccountId()
*/

#ifdef QMESSAGING_OPTIONAL_FOLDER
/*!
    \fn QMessageFilterKey::parentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a key matching messages whose parent folder's identifier matches \a id, according to 
    \a cmp.
    
    \sa QMessage::parentFolderId()
*/

/*!
    \fn QMessageFilterKey::parentFolderId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching messages whose parent folder's identifier is a member of the set 
    yielded by \a key, according to \a cmp.

    \sa QMessage::parentFolderId()
*/

/*!
    \fn QMessageFilterKey::ancestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching messages whose ancestor folders' identifiers contain \a id, according 
    to \a cmp.
    
    \sa QMessage::parentFolderId()
*/

/*!
    \fn QMessageFilterKey::ancestorFolderIds(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a key matching messages whose ancestor folders' identifiers contain a member of the 
    set yielded by \a key, according to \a cmp.
        
    \sa QMessage::parentFolderId()
*/
#endif
