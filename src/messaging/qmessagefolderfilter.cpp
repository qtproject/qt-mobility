/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qmessagefolderfilter.h"
#include "qmessagefolderfilter_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QMessageFolderFilter

    \brief The QMessageFolderFilter class defines the parameters used for querying a subset of
    all available folders from the messaging store.
    \ingroup messaging

    A QMessageFolderFilter is composed of a folder property, an optional comparison operator
    and a comparison value. The QMessageFolderFilter class is used in conjunction with the 
    QMessageManager::queryFolders() and QMessageManager::countFolders() functions to filter results 
    which meet the criteria defined by the filter.

    QMessageFolderFilters can be combined using the logical operators (&), (|) and (~) to
    create more refined queries.

    \sa QMessageManager, QMessageFolder
*/

/*!
    \fn QMessageFolderFilter::QMessageFolderFilter()
  
    Creates a QMessageFolderFilter without specifying matching parameters.

    A default-constructed filter (one for which isEmpty() returns true) matches all folders. 

    \sa isEmpty()
*/

/*!
    \fn QMessageFolderFilter::QMessageFolderFilter(const QMessageFolderFilter &other)
  
    Constructs a copy of \a other.
*/

/*!
    \fn QMessageFolderFilter::~QMessageFolderFilter()
    
    Destroys the filter.
*/

/*!
    \internal
    \fn QMessageFolderFilter::operator=(const QMessageFolderFilter& other)
*/

/*!
    \fn QMessageFolderFilter::setMatchFlags(QMessageDataComparator::MatchFlags matchFlags)
  
    Set the match flags for the search filter to \a matchFlags.

    \sa matchFlags()
*/

/*!
    \fn QMessageDataComparator::MatchFlags QMessageFolderFilter::matchFlags() const
  
    Return the match flags for the search filter.
    
    Default is no match flags set.

    \sa setMatchFlags()
*/

/*!
    \fn QMessageFolderFilter::isEmpty() const
  
    Returns true if the filter remains empty after default construction; otherwise returns false. 

    An empty filter matches all folders.

    The result of combining an empty filter with a non-empty filter is the original non-empty filter. 
    This is true regardless of whether the combination is formed by an AND or an OR operation.

    The result of combining two empty filters is an empty filter.
*/

/*!
    \fn QMessageFolderFilter::isSupported() const
  
    Returns true if the filter is supported on the current platform; otherwise returns false.
*/

/*!
    \fn QMessageFolderFilter::operator~() const
  
    Returns a filter that is the logical NOT of the value of this filter (ignoring any
    matchFlags() set on the filter).

    If this filter is empty, the result will be a non-matching filter; if this filter is 
    non-matching, the result will be an empty filter.

    \sa isEmpty()
*/

/*!
    \fn QMessageFolderFilter::operator&(const QMessageFolderFilter& other) const
  
    Returns a filter that is the logical AND of this filter and the value of filter \a other.
*/

/*!
    \fn QMessageFolderFilter::operator|(const QMessageFolderFilter& other) const
  
    Returns a filter that is the logical OR of this filter and the value of filter \a other.
*/

/*!
    \fn QMessageFolderFilter::operator&=(const QMessageFolderFilter& other)
  
    Performs a logical AND with this filter and the filter \a other and assigns the result
    to this filter.
*/

/*!
    \fn QMessageFolderFilter::operator|=(const QMessageFolderFilter& other)
  
    Performs a logical OR with this filter and the filter \a other and assigns the result
    to this filter.
*/

/*!
    \internal
    \fn QMessageFolderFilter::operator==(const QMessageFolderFilter& other) const
*/

/*!
    \internal
*/
bool QMessageFolderFilter::operator!=(const QMessageFolderFilter& other) const
{
    return !operator==(other);
}

/*!
    \fn QMessageFolderFilter::byId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching folders whose identifier matches \a id, according to \a cmp.

    \sa QMessageFolder::id()
*/

/*!
    \fn QMessageFolderFilter::byId(const QMessageFolderIdList &ids, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching folders whose identifier is a member of \a ids, according to \a cmp.

    \sa QMessageFolder::id()
*/

/*!
    \fn QMessageFolderFilter::byId(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching folders whose identifier is a member of the set yielded by \a filter, according to \a cmp.

    \sa QMessageFolder::id()
*/

/*!
    \fn QMessageFolderFilter::byName(const QString &value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching folders whose display name matches \a value, according to \a cmp.

    \sa QMessageFolder::name()
*/

/*!
    \fn QMessageFolderFilter::byName(const QString &value, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching folders whose display name matches the substring \a value, according to \a cmp.

    \sa QMessageFolder::name()
*/

/*!
    \fn QMessageFolderFilter::byPath(const QString &value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching folders whose path matches \a value, according to \a cmp.

    \sa QMessageFolder::path()
*/

/*!
    \fn QMessageFolderFilter::byPath(const QString &value, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching folders whose path matches the substring \a value, according to \a cmp.

    \sa QMessageFolder::path()
*/

/*!
    \fn QMessageFolderFilter::byParentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching folders whose parent account identifier matches \a id, according to \a cmp.

    \sa QMessageAccount::id()
*/

/*!
    \fn QMessageFolderFilter::byParentAccountId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching folders whose parent account identifier is a member of the set yielded by \a filter, according to \a cmp.

    \sa QMessageAccount::id()
*/

/*!
    \fn QMessageFolderFilter::byParentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching folders whose parent folder identifier matches \a id, according to \a cmp.

    \sa QMessageFolder::id()
*/

/*!
    \fn QMessageFolderFilter::byParentFolderId(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching folders whose parent folder identifier is a member of the set yielded by \a filter, according to \a cmp.

    \sa QMessageFolder::id()
*/

/*!
    \fn QMessageFolderFilter::byAncestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching folders whose ancestor folders' identifiers contain \a id, according to \a cmp.

    \sa QMessageFolder::id()
*/

/*!
    \fn QMessageFolderFilter::byAncestorFolderIds(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching folders whose ancestor folders' identifiers contain a member of the set yielded by \a filter, according to \a cmp.

    \sa QMessageFolder::id()
*/

QTM_END_NAMESPACE
