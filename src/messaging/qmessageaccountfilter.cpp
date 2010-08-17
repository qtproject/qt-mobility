/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qmessageaccountfilter.h"
#include "qmessageaccountfilter_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QMessageAccountFilter

    \brief The QMessageAccountFilter class defines the parameters used for querying a subset of
    all available accounts from the messaging store.
    \ingroup messaging

    A QMessageAccountFilter is composed of an account property, an optional comparison operator
    and a comparison value. The QMessageAccountFilter class is used in conjunction with the 
    QMessageManager::queryAccounts() and QMessageManager::countAccounts() functions to filter results 
    which meet the criteria defined by the filter.

    QMessageAccountFilters can be combined using the logical operators (&), (|) and (~) to
    create more refined queries.

    \sa QMessageManager, QMessageAccount
*/

/*!
    \fn QMessageAccountFilter::QMessageAccountFilter()
  
    Creates a QMessageAccountFilter without specifying matching parameters.

    A default-constructed filter (one for which isEmpty() returns true) matches all accounts. 

    \sa isEmpty()
*/

/*!
    \fn QMessageAccountFilter::QMessageAccountFilter(const QMessageAccountFilter &other)
  
    Constructs a copy of \a other.
*/

/*!
    \fn QMessageAccountFilter::~QMessageAccountFilter()
    
    Destroys the filter.
*/

/*!
    \internal
    \fn QMessageAccountFilter::operator=(const QMessageAccountFilter& other)
*/

/*!
    \fn QMessageAccountFilter::setMatchFlags(QMessageDataComparator::MatchFlags matchFlags)
  
    Set the match flags for the search filter to \a matchFlags.

    \sa matchFlags()
*/

/*!
    \fn QMessageDataComparator::MatchFlags QMessageAccountFilter::matchFlags() const
  
    Return the match flags for the search filter.
    
    Default is no match flags set.

    \sa setMatchFlags()
*/

/*!
    \fn QMessageAccountFilter::isEmpty() const
  
    Returns true if the filter remains empty after default construction; otherwise returns false. 

    An empty filter matches all accounts.

    The result of combining an empty filter with a non-empty filter is the original non-empty filter. 
    This is true regardless of whether the combination is formed by an AND or an OR operation.

    The result of combining two empty filters is an empty filter.
*/

/*!
    \fn QMessageAccountFilter::isSupported() const
  
    Returns true if the filter is supported on the current platform; otherwise returns false.
*/

/*!
    \fn QMessageAccountFilter::operator~() const
  
    Returns a filter that is the logical NOT of the value of this filter (ignoring any
    matchFlags() set on the filter).

    If this filter is empty, the result will be a non-matching filter; if this filter is 
    non-matching, the result will be an empty filter.

    \sa isEmpty()
*/

/*!
    \fn QMessageAccountFilter::operator&(const QMessageAccountFilter& other) const
  
    Returns a filter that is the logical AND of this filter and the value of filter \a other.
*/

/*!
    \fn QMessageAccountFilter::operator|(const QMessageAccountFilter& other) const
  
    Returns a filter that is the logical OR of this filter and the value of filter \a other.
*/

/*!
    \fn QMessageAccountFilter::operator&=(const QMessageAccountFilter& other)
  
    Performs a logical AND with this filter and the filter \a other and assigns the result
    to this filter.
*/

/*!
    \fn QMessageAccountFilter::operator|=(const QMessageAccountFilter& other)
  
    Performs a logical OR with this filter and the filter \a other and assigns the result
    to this filter.
*/

/*! 
    \internal
    \fn QMessageAccountFilter::operator==(const QMessageAccountFilter& other) const
*/

/*! 
    \internal
*/
bool QMessageAccountFilter::operator!=(const QMessageAccountFilter& other) const
{
    return !operator==(other);
}

/*!
    \fn QMessageAccountFilter::byId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching accounts whose identifier matches \a id, according to \a cmp.

    \sa QMessageAccount::id()
*/

/*!
    \fn QMessageAccountFilter::byId(const QMessageAccountIdList &ids, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching accounts whose identifier is a member of \a ids, according to \a cmp.

    \sa QMessageAccount::id()
*/

/*!
    \fn QMessageAccountFilter::byId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching accounts whose identifier is a member of the set yielded by \a filter, according to \a cmp.

    \sa QMessageAccount::id()
*/

/*!
    \fn QMessageAccountFilter::byName(const QString &value, QMessageDataComparator::EqualityComparator cmp)
  
    Returns a filter matching accounts whose name matches \a value, according to \a cmp.

    \sa QMessageAccount::name()
*/

/*!
    \fn QMessageAccountFilter::byName(const QString &value, QMessageDataComparator::InclusionComparator cmp)
  
    Returns a filter matching accounts whose name matches the substring \a value, according to \a cmp.

    \sa QMessageAccount::name()
*/

QTM_END_NAMESPACE
