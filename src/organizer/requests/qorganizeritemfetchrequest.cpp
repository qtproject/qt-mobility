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

#include "qorganizeritemfetchrequest.h"
#include "qorganizeritemrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemFetchRequest
  \brief The QOrganizerItemFetchRequest class allows a client to asynchronously
    request organizer items from an organizer item store manager.
  \inmodule QtOrganizer


  For a QOrganizerItemFetchRequest, the resultsAvailable() signal will be emitted when the resultant
  organizer item (which may be retrieved by calling items()), are updated, as well as if
  the overall operation error (which may be retrieved by calling error()) is updated.

  \ingroup organizer-requests
 */

/*! Constructs a new organizer item fetch request whose parent is the specified \a parent */
QOrganizerItemFetchRequest::QOrganizerItemFetchRequest(QObject* parent)
    : QOrganizerAbstractRequest(new QOrganizerItemFetchRequestPrivate, parent)
{
}

/*! Frees memory in use by this request */
QOrganizerItemFetchRequest::~QOrganizerItemFetchRequest()
{
    QOrganizerAbstractRequestPrivate::notifyEngine(this);
}

/*! Sets the organizer item filter used to determine which organizer items will be retrieved to \a filter */
void QOrganizerItemFetchRequest::setFilter(const QOrganizerItemFilter& filter)
{
    Q_D(QOrganizerItemFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_filter = filter;
}

/*! Sets the sort order of the result to \a sorting.  Only has an effect if called prior to calling \c start() */
void QOrganizerItemFetchRequest::setSorting(const QList<QOrganizerItemSortOrder>& sorting)
{
    Q_D(QOrganizerItemFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_sorting = sorting;
}

/*!
  Sets the fetch hint which may be used by the backend to optimize organizer item retrieval
  to \a fetchHint.  A client should not make changes to a organizer item which has been retrieved
  using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the organizer item back to the manager (as the "new" restricted organizer item will
  replace the previously saved organizer item in the backend).
  \sa QOrganizerItemFetchHint
 */
void QOrganizerItemFetchRequest::setFetchHint(const QOrganizerItemFetchHint &fetchHint)
{
    Q_D(QOrganizerItemFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_fetchHint = fetchHint;
}

/*! Sets the start period of the request to \a date. Only has an effect if called prior to calling \c start() */
void QOrganizerItemFetchRequest::setStartDate(const QDateTime &date)
{
    Q_D(QOrganizerItemFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_startDate = date;
}

/*! Sets the end period of the request to \a date. Only has an effect if called prior to calling \c start() */
void QOrganizerItemFetchRequest::setEndDate(const QDateTime &date)
{
    Q_D(QOrganizerItemFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_endDate = date;
}

/*! Returns the filter that will be used to select organizer items to be returned */
QOrganizerItemFilter QOrganizerItemFetchRequest::filter() const
{
    Q_D(const QOrganizerItemFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_filter;
}

/*! Returns the sort ordering that will be used sort the results of this request */
QList<QOrganizerItemSortOrder> QOrganizerItemFetchRequest::sorting() const
{
    Q_D(const QOrganizerItemFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_sorting;
}

/*!
  Returns the fetch hint which may be used by the backend to optimize organizer item retrieval.
  A client should not make changes to an organizer item which has been retrieved
  using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the organizer item back to the manager (as the "new" restricted organizer item will
  replace the previously saved organizer item in the backend).
  \sa QOrganizerItemFetchHint
 */
QOrganizerItemFetchHint QOrganizerItemFetchRequest::fetchHint() const
{
    Q_D(const QOrganizerItemFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_fetchHint;
}

/*!
  Returns the start date of the request.  The start date
  is the lower bound of the time-period within which an
  item must occur (that is, either it or one of its occurrences
  must have a time-period defined by its start-date and end-date
  which overlaps with the time-period defined in this request)
  in order to be returned by the request.

  An empty or invalid start date signifies a start date of
  negative-infinity (that is, all items which occur at any
  point in time, up until the end date, will be returned).
 */
QDateTime QOrganizerItemFetchRequest::startDate() const
{
    Q_D(const QOrganizerItemFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_startDate;
}

/*!
  Returns the end date of the request.  The end date
  is the upper bound of the time-period within which an
  item must occur (that is, either it or one of its occurrences
  must have a time-period defined by its start-date and end-date
  which overlaps with the time-period defined in this request)
  in order to be returned by the request.

  An empty or invalid end date signifies an end date of
  positive-infinity (that is, all items which occur at any
  point in time after the start date, will be returned).
 */
QDateTime QOrganizerItemFetchRequest::endDate() const
{
    Q_D(const QOrganizerItemFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_endDate;
}

/*! Returns the list of organizer items retrieved by this request */
QList<QOrganizerItem> QOrganizerItemFetchRequest::items() const
{
    Q_D(const QOrganizerItemFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_organizeritems;
}

#include "moc_qorganizeritemfetchrequest.cpp"

QTM_END_NAMESPACE
