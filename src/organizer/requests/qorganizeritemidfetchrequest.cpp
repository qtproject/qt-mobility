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

#include "qorganizeritemidfetchrequest.h"
#include "qorganizeritemrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemIdFetchRequest
  \brief The QOrganizerItemIdFetchRequest class allows a client to asynchronously
    request a list of organizer item ids from a organizer item store manager.
  \inmodule QtOrganizer


  For a QOrganizerItemIdFetchRequest, the resultsAvailable() signal will be emitted when the resultant
  manager organizer item ids (which may be retrieved by calling ids()), are updated, as well as if
  the overall operation error (which may be retrieved by calling error()) is updated.

  \ingroup organizer-requests
 */

/*! Constructs a new organizer item id fetch request whose parent is the specified \a parent */
QOrganizerItemIdFetchRequest::QOrganizerItemIdFetchRequest(QObject* parent)
    : QOrganizerAbstractRequest(new QOrganizerItemIdFetchRequestPrivate, parent)
{
}

/*! Sets the filter which will be used to select the organizer items whose ids will be returned to \a filter */
void QOrganizerItemIdFetchRequest::setFilter(const QOrganizerItemFilter& filter)
{
    Q_D(QOrganizerItemIdFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_filter = filter;
}

/*! Sets the future sort ordering of the result of the request to \a sorting.  This function only has
    effect on the result if called prior to calling \c start() */
void QOrganizerItemIdFetchRequest::setSorting(const QList<QOrganizerItemSortOrder>& sorting)
{
    Q_D(QOrganizerItemIdFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_sorting = sorting;
}

/*!
   Sets the date-time which is the lower bound for the range for items whose ids will be returned to \a date.
   An invalid (default-constructed) date-time signifies that no lower bound is given (matches everything
   up to the end date).
   Note that an item matches if either it or any of its occurrences occur within the defined range.
 */
void QOrganizerItemIdFetchRequest::setStartDate(const QDateTime &date)
{
    Q_D(QOrganizerItemIdFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_startDate = date;
}

/*!
   Sets the date-time which is the upper bound for the range for items whose ids will be returned to \a date.
   An invalid (default-constructed) date-time signifies that no upper bound is given (matches everything
   after the start date).
   Note that an item matches if either it or any of its occurrences occur within the defined range.
 */
void QOrganizerItemIdFetchRequest::setEndDate(const QDateTime &date)
{
    Q_D(QOrganizerItemIdFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_endDate = date;
}

/*! Returns the filter which will be used to select the organizer items whose ids will be returned */
QOrganizerItemFilter QOrganizerItemIdFetchRequest::filter() const
{
    Q_D(const QOrganizerItemIdFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_filter;
}

/*! Returns the sort ordering which will be used to sort the result */
QList<QOrganizerItemSortOrder> QOrganizerItemIdFetchRequest::sorting() const
{
    Q_D(const QOrganizerItemIdFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_sorting;
}

/*!
   Returns the date-time which is the lower bound for the range for items whose ids will be returned.
   An invalid (default-constructed) date-time signifies that no lower bound is given (matches everything
   up until the end date).
   Note that an item matches if either it or any of its occurrences occur within the defined range.
 */
QDateTime QOrganizerItemIdFetchRequest::startDate() const
{
    Q_D(const QOrganizerItemIdFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_startDate;
}

/*!
   Returns the date-time which is the upper bound for the range for items whose ids will be returned.
   An invalid (default-constructed) date-time signifies that no upper bound is given (matches everything
   after the start date).
   Note that an item matches if either it or any of its occurrences occur within the defined range.
 */
QDateTime QOrganizerItemIdFetchRequest::endDate() const
{
    Q_D(const QOrganizerItemIdFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_endDate;
}

/*! Returns the list of ids of organizer items which matched the request */
QList<QOrganizerItemId> QOrganizerItemIdFetchRequest::itemIds() const
{
    Q_D(const QOrganizerItemIdFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_ids;
}

#include "moc_qorganizeritemidfetchrequest.cpp"

QTM_END_NAMESPACE
