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

#include "qorganizeritemfetchbyidrequest.h"
#include "qorganizeritemrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemFetchByIdRequest
  \brief The QOrganizerItemFetchByIdRequest class allows a client to asynchronously
    request items from a items store manager, given a list of item IDs.

  The items fetched by the backend should have a one-to-one correspondence to the
  IDs passed into this class.  That is, the nth item in the returned list should
  have an ID which is equal to the nth ID in the list of IDs.  Any invalid ID should
  correspond to an empty QOrganizerItem.

  For a QOrganizerItemFetchByIdRequest, the resultsAvailable() signal will be emitted when the
  resultant items (which may be retrieved by calling items()), are updated, as well
  as if the overall operation error (which may be retrieved by calling error()) is
  updated.

  Please see the class documentation of QOrganizerAbstractRequest for more information about
  the usage of request classes and ownership semantics.


  \inmodule QtOrganizer

  \ingroup organizer-requests
 */

/*! Constructs a new item fetch request whose parent is the specified \a parent */
QOrganizerItemFetchByIdRequest::QOrganizerItemFetchByIdRequest(QObject* parent)
    : QOrganizerAbstractRequest(new QOrganizerItemFetchByIdRequestPrivate, parent)
{
}

/*! Frees any memory used by this request */
QOrganizerItemFetchByIdRequest::~QOrganizerItemFetchByIdRequest()
{
    QOrganizerAbstractRequestPrivate::notifyEngine(this);
}

/*!
  Sets the list of ids of the items that the backend should retrieve to \a ids.
 */
void QOrganizerItemFetchByIdRequest::setIds(const QList<QOrganizerItemId>& ids)
{
    Q_D(QOrganizerItemFetchByIdRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_ids = ids;
}

/*!
  Sets the fetch hint which may be used by the backend to optimize item retrieval
  to \a fetchHint.  A client should not make changes to a item which has been retrieved
  using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the item back to the manager (as the "new" restricted item will
  replace the previously saved item in the backend).
  \sa QOrganizerItemFetchHint
 */
void QOrganizerItemFetchByIdRequest::setFetchHint(const QOrganizerItemFetchHint &fetchHint)
{
    Q_D(QOrganizerItemFetchByIdRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_fetchHint = fetchHint;
}

/*!
  Returns the list of ids of the items that the backend should retrieve.
 */
QList<QOrganizerItemId> QOrganizerItemFetchByIdRequest::ids() const
{
    Q_D(const QOrganizerItemFetchByIdRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_ids;
}

/*!
  Returns the fetch hint which may be used by the backend to optimize item retrieval.
  A client should not make changes to a item which has been retrieved
  using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the item back to the manager (as the "new" restricted item will
  replace the previously saved item in the backend).
  \sa QOrganizerItemFetchHint
 */
QOrganizerItemFetchHint QOrganizerItemFetchByIdRequest::fetchHint() const
{
    Q_D(const QOrganizerItemFetchByIdRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_fetchHint;
}

/*! Returns the list of items retrieved by this request */
QList<QOrganizerItem> QOrganizerItemFetchByIdRequest::items() const
{
    Q_D(const QOrganizerItemFetchByIdRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_items;
}

/*! Returns the map of input definition list indices to errors which occurred */
QMap<int, QOrganizerManager::Error> QOrganizerItemFetchByIdRequest::errorMap() const
{
    Q_D(const QOrganizerItemFetchByIdRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_errors;
}

#include "moc_qorganizeritemfetchbyidrequest.cpp"

QTM_END_NAMESPACE
