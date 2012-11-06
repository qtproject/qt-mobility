/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
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
#include "qmessageaccount.h"
#include "qmessagemanager.h"
#include "messagingutil_p.h"
#include <QRegExp>

QTM_BEGIN_NAMESPACE

/*!
    \class QMessageAccountFilter

    \brief The QMessageAccountFilter class defines the parameters used for querying a subset of
    all available accounts from the messaging store.

    \inmodule QtMessaging

    \ingroup messaging
    \since 1.0

    A QMessageAccountFilter is composed of an account property, an optional comparison operator
    and a comparison value. The QMessageAccountFilter class is used in conjunction with the
    QMessageManager::queryAccounts() and QMessageManager::countAccounts() functions to filter results
    which meet the criteria defined by the filter.

    QMessageAccountFilters can be combined using the overloaded operators (&), (|) and (~) as logical
    operators to create more refined queries.

    Evaluation of filters is delayed until they are used in a QMessageManager function
    such as queryAccounts, except where explicitly documented otherwise.

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
    \since 1.0
*/

/*!
    \fn QMessageAccountFilter::~QMessageAccountFilter()

    Destroys the filter.
*/

/*!
    \internal
    \fn QMessageAccountFilter::operator=(const QMessageAccountFilter& other)
    \since 1.0
*/

/*!
    \fn QMessageAccountFilter::setMatchFlags(QMessageDataComparator::MatchFlags matchFlags)

    Set the match flags for the search filter to \a matchFlags.

    \since 1.0
    \sa matchFlags()
*/

/*!
    \fn QMessageDataComparator::MatchFlags QMessageAccountFilter::matchFlags() const

    Return the match flags for the search filter.

    Default is no match flags set.

    \since 1.0
    \sa setMatchFlags()
*/

/*!
    \fn QMessageAccountFilter::isEmpty() const

    Returns true if the filter remains empty after default construction; otherwise returns false.

    An empty filter matches all accounts.

    The result of combining an empty filter with a non-empty filter using an AND operation is the
    original non-empty filter.

    The result of combining an empty filter with a non-empty filter using an OR operation is the
    empty filter.

    The result of combining two empty filters is an empty filter.
    \since 1.0
*/

/*!
    \fn QMessageAccountFilter::isSupported() const

    Returns true if the filter is supported on the current platform; otherwise returns false.
    \since 1.0
*/

/*!
    \fn QMessageAccountFilter::operator~() const

    Returns a filter that is the logical NOT of the value of this filter (ignoring any
    matchFlags() set on the filter).

    If this filter is empty, the result will be a non-matching filter; if this filter is
    non-matching, the result will be an empty filter.

    \since 1.0
    \sa isEmpty()
*/

/*!
    \fn QMessageAccountFilter::operator&(const QMessageAccountFilter& other) const

    Returns a filter that is the logical AND of this filter and the value of filter \a other.
    \since 1.0
*/

/*!
    \fn QMessageAccountFilter::operator|(const QMessageAccountFilter& other) const

    Returns a filter that is the logical OR of this filter and the value of filter \a other.
    \since 1.0
*/

/*!
    \fn QMessageAccountFilter::operator&=(const QMessageAccountFilter& other)

    Performs a logical AND with this filter and the filter \a other and assigns the result
    to this filter.
    \since 1.0
*/

/*!
    \fn QMessageAccountFilter::operator|=(const QMessageAccountFilter& other)

    Performs a logical OR with this filter and the filter \a other and assigns the result
    to this filter.
    \since 1.0
*/

/*!
    \internal
    \fn QMessageAccountFilter::operator==(const QMessageAccountFilter& other) const
    \since 1.0
*/

/*!
    \internal
    \since 1.0
*/
bool QMessageAccountFilter::operator!=(const QMessageAccountFilter& other) const
{
    return !operator==(other);
}

/*!
    \fn QMessageAccountFilter::byId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)

    Returns a filter matching accounts whose identifier matches \a id, according to \a cmp.

    \since 1.0
    \sa QMessageAccount::id()
*/

/*!
    \fn QMessageAccountFilter::byId(const QMessageAccountIdList &ids, QMessageDataComparator::InclusionComparator cmp)

    Returns a filter matching accounts whose identifier is a member of \a ids, according to \a cmp.

    \since 1.0
    \sa QMessageAccount::id()
*/

/*!
    \fn QMessageAccountFilter::byId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp)

    Returns a filter matching accounts whose identifier is a member of the set yielded by \a filter, according to \a cmp.

    \since 1.0
    \sa QMessageAccount::id()
*/

/*!
    \fn QMessageAccountFilter::byName(const QString &pattern, QMessageDataComparator::LikeComparator cmp)

    Returns a filter matching accounts who name matches \a pattern, according to \a cmp.

    This filter is evaluated when it is constructed.

    \since 1.2
    \sa QMessageAccount::name()
*/

/*!
    \fn QMessageAccountFilter::byName(const QString &value, QMessageDataComparator::EqualityComparator cmp)

    Returns a filter matching accounts whose name matches \a value, according to \a cmp.

    Not supported on Linux, Harmattan and Meego.com (use InclusionComparator).

    \since 1.0
    \sa QMessageAccount::name()
*/

/*!
    \fn QMessageAccountFilter::byName(const QString &value, QMessageDataComparator::InclusionComparator cmp)

    Returns a filter matching accounts whose name matches the substring \a value, according to \a cmp.

    \since 1.0
    \sa QMessageAccount::name()
*/

QMessageAccountFilter QMessageAccountFilter::byName(const QString &pattern, QMessageDataComparator::LikeComparator cmp)
{
    QMessageAccountIdList ids;
    foreach (QMessageAccountId const& id, QMessageManager().queryAccounts()) {
        bool matched(MessagingUtil::globMatch(pattern, QMessageAccount(id).name()));
        if ((matched && cmp == QMessageDataComparator::Like)
                || (!matched && cmp == QMessageDataComparator::NotLike)) {
            ids.push_back(id);
        }
    }

    return QMessageAccountFilter::byId(ids);
}


QTM_END_NAMESPACE
