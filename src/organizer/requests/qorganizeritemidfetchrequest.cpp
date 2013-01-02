/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
  \since 1.1


  For a QOrganizerItemIdFetchRequest, the resultsAvailable() signal will be emitted when the resultant
  manager organizer item ids (which may be retrieved by calling ids()), are updated, as well as if
  the overall operation error (which may be retrieved by calling error()) is updated.

  \ingroup organizer-requests
 */

/*! Constructs a new organizer item id fetch request whose parent is the specified \a parent
    \since 1.1
*/
QOrganizerItemIdFetchRequest::QOrganizerItemIdFetchRequest(QObject* parent)
    : QOrganizerAbstractRequest(new QOrganizerItemIdFetchRequestPrivate, parent)
{
}

/*! Frees memory in use by this request
    \since 1.2
*/
QOrganizerItemIdFetchRequest::~QOrganizerItemIdFetchRequest()
{
    QOrganizerAbstractRequestPrivate::notifyEngine(this);
}

/*! Sets the filter which will be used to select the organizer items whose ids will be returned to \a filter
    \since 1.1
*/
void QOrganizerItemIdFetchRequest::setFilter(const QOrganizerItemFilter& filter)
{
    Q_D(QOrganizerItemIdFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_filter = filter;
}

/*! Sets the future sort ordering of the result of the request to \a sorting.  This function only has
    effect on the result if called prior to calling \c start()
    \since 1.1
*/
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
   \since 1.1
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
   \since 1.1
 */
void QOrganizerItemIdFetchRequest::setEndDate(const QDateTime &date)
{
    Q_D(QOrganizerItemIdFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_endDate = date;
}

/*! Returns the filter which will be used to select the organizer items whose ids will be returned
    \since 1.1
*/
QOrganizerItemFilter QOrganizerItemIdFetchRequest::filter() const
{
    Q_D(const QOrganizerItemIdFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_filter;
}

/*! Returns the sort ordering which will be used to sort the result
    \since 1.1
*/
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
   \since 1.1
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
   \since 1.1
 */
QDateTime QOrganizerItemIdFetchRequest::endDate() const
{
    Q_D(const QOrganizerItemIdFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_endDate;
}

/*! Returns the list of ids of organizer items which matched the request
    \since 1.1
*/
QList<QOrganizerItemId> QOrganizerItemIdFetchRequest::itemIds() const
{
    Q_D(const QOrganizerItemIdFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_ids;
}

#include "moc_qorganizeritemidfetchrequest.cpp"

QTM_END_NAMESPACE
