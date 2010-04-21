/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qcontactfetchrequest.h"
#include "qcontactrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactFetchRequest
  \brief The QContactFetchRequest class allows a client to asynchronously
    request contacts from a contacts store manager.


  For a QContactFetchRequest, the resultsAvailable() signal will be emitted when the resultant
  contacts (which may be retrieved by calling contacts()), are updated, as well as if
  the overall operation error (which may be retrieved by calling error()) is updated.

  \ingroup contacts-requests
 */

/*! Constructs a new contact fetch request whose parent is the specified \a parent */
QContactFetchRequest::QContactFetchRequest(QObject* parent)
    : QContactAbstractRequest(new QContactFetchRequestPrivate, parent)
{
}

/*! Sets the contact filter used to determine which contacts will be retrieved to \a filter */
void QContactFetchRequest::setFilter(const QContactFilter& filter)
{
    Q_D(QContactFetchRequest);
    d->m_filter = filter;
}

/*! Sets the sort order of the result to \a sorting.  Only has an effect if called prior to calling \c start() */
void QContactFetchRequest::setSorting(const QList<QContactSortOrder>& sorting)
{
    Q_D(QContactFetchRequest);
    d->m_sorting = sorting;
}

/*!
  Sets the fetch hint which may be used by the backend to optimize contact retrieval
  to \a fetchHint.  A client should not make changes to a contact which has been retrieved
  using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the contact back to the manager (as the "new" restricted contact will
  replace the previously saved contact in the backend).
  \sa QContactFetchHint
 */
void QContactFetchRequest::setFetchHint(const QContactFetchHint &fetchHint)
{
    Q_D(QContactFetchRequest);
    d->m_fetchHint = fetchHint;
}

/*! Returns the filter that will be used to select contacts to be returned */
QContactFilter QContactFetchRequest::filter() const
{
    Q_D(const QContactFetchRequest);
    return d->m_filter;
}

/*! Returns the sort ordering that will be used sort the results of this request */
QList<QContactSortOrder> QContactFetchRequest::sorting() const
{
    Q_D(const QContactFetchRequest);
    return d->m_sorting;
}

/*!
  Returns the fetch hint which may be used by the backend to optimize contact retrieval.
  A client should not make changes to a contact which has been retrieved
  using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the contact back to the manager (as the "new" restricted contact will
  replace the previously saved contact in the backend).
  \sa QContactFetchHint
 */
QContactFetchHint QContactFetchRequest::fetchHint() const
{
    Q_D(const QContactFetchRequest);
    return d->m_fetchHint;
}

/*! Returns the list of contacts retrieved by this request */
QList<QContact> QContactFetchRequest::contacts() const
{
    Q_D(const QContactFetchRequest);
    return d->m_contacts;
}

#include "moc_qcontactfetchrequest.cpp"

QTM_END_NAMESPACE
