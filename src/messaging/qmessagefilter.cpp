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
#include "qmessagefilter.h"
#include "qmessagefilter_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QMessageFilter

    \preliminary
    \brief The QMessageFilter class defines the parameters used for querying a subset of
    all available messages from the messaging store.
    \ingroup messaging

    A QMessageFilter is composed of a message property, an optional comparison operator
    and a comparison value. The QMessageFilter class is used in conjunction with the 
    QMessageManager::queryMessages(), QMessageManager::countMessages(), 
    QMessageService::queryMessages(), and QMessageService::countMessages()
    functions to filter results which meet the criteria defined by the filter.

    QMessageFilters can be combined using the logical operators (&), (|) and (~) to
    create more refined queries.

    For example:

    To create a query for all messages sent from "joe@user.com" with subject "meeting":
    \code
    QMessageFilter subjectFilter(QMessageFilter::bySubject("meeting"));
    QMessageFilter senderFilter(QMessageFilter::bySender("joe@user.com"));
    QMessageIdList results = QMessageManager().queryMessages(subjectFilter & senderFilter);
    \endcode

    \sa QMessageManager, QMessage
*/

/*!
    \fn QMessageFilter::QMessageFilter()
  
    Creates a QMessageFilter without specifying matching parameters.

    A default-constructed filter (one for which isEmpty() returns true) matches all messages. 

    \sa isEmpty()
*/

/*!
    \fn QMessageFilter::QMessageFilter(const QMessageFilter &other)
  
    Constructs a copy of \a other.
*/

/*!
    \fn QMessageFilter::~QMessageFilter()
    
    Destroys the filter.
*/

/*!
    \internal
    \fn QMessageFilter& QMessageFilter::operator=(const QMessageFilter& other)
*/

/*!
    \fn QMessageFilter::setMatchFlags(QMessageDataComparator::MatchFlags matchFlags)
  
    Set the match flags for the search filter to \a matchFlags.

    \sa matchFlags()
*/

/*!
    \fn QMessageFilter::matchFlags() const
  
    Return the match flags for the search filter.
    
    Default is no match flags set.

    \sa setMatchFlags()
*/

/*!
    \fn QMessageFilter::isEmpty() const
  
    Returns true if the filter remains empty after default construction; otherwise returns false. 

    An empty filter matches all messages.

    The result of combining an empty filter with a non-empty filter is the original non-empty filter. 
    This is true regardless of whether the combination is formed by an AND or an OR operation.

    The result of combining two empty filters is an empty filter.
*/

/*!
    \fn QMessageFilter::isSupported() const
  
    Returns true if the filter is supported on the current platform; otherwise returns false.
*/

/*!
    \fn QMessageFilter::operator~() const
  
    Returns a filter that is the logical NOT of the value of this filter (ignoring any
    matchFlags() set on the filter).

    If this filter is empty, the result will be a non-matching filter; if this filter is 
    non-matching, the result will be an empty filter.

    \sa isEmpty()
*/

/*!
    \fn QMessageFilter::operator&(const QMessageFilter& other) const
  
    Returns a filter that is the logical AND of this filter and the value of filter \a other.
*/

/*!
    \fn QMessageFilter::operator|(const QMessageFilter& other) const
  
    Returns a filter that is the logical OR of this filter and the value of filter \a other.
*/

/*!
    \fn QMessageFilter::operator&=(const QMessageFilter& other)
  
    Performs a logical AND with this filter and the filter \a other and assigns the result
    to this filter.
*/

/*!
    \fn QMessageFilter::operator|=(const QMessageFilter& other)
  
    Performs a logical OR with this filter and the filter \a other and assigns the result
    to this filter.
*/

/*!
    \internal
    \fn QMessageFilter::operator==(const QMessageFilter& other) const
*/

/*!
    \internal
*/
bool QMessageFilter::operator!=(const QMessageFilter& other) const
{
    return !operator==(other);
}

/*!
    \fn QMessageFilter::byId(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching messages whose identifier matches \a id, according to \a cmp.

    \sa QMessage::id()
*/

/*!
    \fn QMessageFilter::byId(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching messages whose identifier is a member of \a ids, according to \a cmp.

    \sa QMessage::id()
*/

/*!
    \fn QMessageFilter::byId(const QMessageFilter &filter, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching messages whose identifier is a member of the set yielded by \a filter, according to \a cmp.

    \sa QMessage::id()
*/

/*!
    \fn QMessageFilter::byType(QMessage::Type type, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching messages whose type matches \a type, according to \a cmp.

    \sa QMessage::type()
*/

/*!
    \fn QMessageFilter::byType(QMessage::TypeFlags type, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching accounts whose type is a bitwise match to \a type, according to \a cmp.

    \sa QMessage::type()
*/

/*!
    \fn QMessageFilter::bySender(const QString &value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching messages whose sender matches \a value, according to \a cmp.

    \sa QMessage::from()
*/

/*!
    \fn QMessageFilter::bySender(const QString &value, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching messages whose sender matches the substring \a value, according to \a cmp.

    \sa QMessage::from()
*/

/*!
    \fn QMessageFilter::byRecipients(const QString &value, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching messages whose recipients include the substring \a value, 
    according to \a cmp.

    \sa QMessage::to(), QMessage::cc(), QMessage::bcc()
*/

/*!
    \fn QMessageFilter::bySubject(const QString &value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching messages whose subject matches \a value, according to \a cmp.

    \sa QMessage::subject()
*/

/*!
    \fn QMessageFilter::bySubject(const QString &value, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching messages whose subject matches the 
    substring \a value, according to \a cmp.

    \sa QMessage::subject()
*/

/*!
    \fn QMessageFilter::byTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching messages whose timestamp matches \a value, according to \a cmp.

    \sa QMessage::date()
*/

/*!
    \fn QMessageFilter::byTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
  
    Returns a filter matching messages whose timestamp has the relation to \a value that is specified by \a cmp.

    \sa QMessage::date()
*/

/*!
    \fn QMessageFilter::byReceptionTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching messages whose reception timestamp matches \a value, according to \a cmp.

    \sa QMessage::receivedDate()
*/

/*!
    \fn QMessageFilter::byReceptionTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
  
    Returns a filter matching messages whose reception timestamp has the relation to \a value that is specified by \a cmp.

    \sa QMessage::receivedDate()
*/

/*!
    \fn QMessageFilter::byStatus(QMessage::Status value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching messages whose status matches \a value, according to \a cmp.

    \sa QMessage::status()
*/

/*!
    \fn QMessageFilter::byStatus(QMessage::StatusFlags mask, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching messages whose status is a bitwise match to \a mask, according to \a cmp.

    \sa QMessage::status()
*/

/*!
    \fn QMessageFilter::byPriority(QMessage::Priority value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching messages whose priority matches \a value, according to \a cmp.
    
    \sa QMessage::priority()
*/

/*!
    \fn QMessageFilter::bySize(int value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching messages whose size matches \a value, according to \a cmp.

    \sa QMessage::size()
*/

/*!
    \fn QMessageFilter::bySize(int value, QMessageDataComparator::RelationComparator cmp)
  
    Returns a filter matching messages whose size matches \a value, according to \a cmp.

    \sa QMessage::size()
*/

/*!
    \fn QMessageFilter::byParentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching messages whose parent account's identifier matches \a id, according to \a cmp.
    
    \sa QMessage::parentAccountId()
*/

/*!
    \fn QMessageFilter::byParentAccountId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching messages whose parent account's identifier is a member of the set 
    yielded by \a filter, according to \a cmp.

    \sa QMessage::parentAccountId()
*/

/*!
    \fn QMessageFilter::byStandardFolder(QMessage::StandardFolder folder, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching messages whose standard folder location matches \a folder, according to \a cmp.
    
    \sa QMessage::parentAccountId()
*/

/*!
    \fn QMessageFilter::byParentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching messages whose parent folder's identifier matches \a id, according to \a cmp.
    
    \sa QMessage::parentFolderId()
*/

/*!
    \fn QMessageFilter::byParentFolderId(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching messages whose parent folder's identifier is a member of the set 
    yielded by \a filter, according to \a cmp.

    \sa QMessage::parentFolderId()
*/

/*!
    \fn QMessageFilter::byAncestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching messages whose ancestor folders' identifiers contain \a id, according to \a cmp.
    
    \sa QMessage::parentFolderId()
*/

/*!
    \fn QMessageFilter::byAncestorFolderIds(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching messages whose ancestor folders' identifiers contain a member of the 
    set yielded by \a filter, according to \a cmp.
        
    \sa QMessage::parentFolderId()
*/

QTM_END_NAMESPACE
