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

/*! Constructs a new contact fetch request */
QContactFetchRequest::QContactFetchRequest()
    : QContactAbstractRequest(new QContactFetchRequestPrivate)
{
}

/*! Cleans up the memory in use by this contact fetch request */
QContactFetchRequest::~QContactFetchRequest()
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

/*!
  \deprecated
  Sets the list of allowable detail definition names to \a definitionNames.  Any contacts retrieved
  by the request will have any details whose definition name is not on the restricted list
  removed prior to being returned.
  This function is deprecated and will be removed after the transition period has been elapsed.  Use setFetchHint() instead.
 */
void QContactFetchRequest::setDefinitionRestrictions(const QStringList& definitionNames)
{
    //Q_D(QContactFetchRequest);
    Q_UNUSED(definitionNames);
    qWarning("QContactFetchRequest::setDefinitionRestrictions() This function is deprecated and will be removed once the transition period has elapsed.  Use setFetchHint() instead!");
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

/*!
  \deprecated
  Returns the list of definition names which define which details contacts in the result list will be limited to.
  This function is deprecated and will be removed after the transition period has been elapsed.  Use fetchHint() instead.
 */
QStringList QContactFetchRequest::definitionRestrictions() const
{
    //Q_D(const QContactFetchRequest);
    qWarning("QContactFetchRequest::definitionRestrictions() This function is deprecated and will be removed once the transition period has elapsed.  Use fetchHint() instead!");
    return QStringList();
}

/*! Returns the list of contacts retrieved by this request */
QList<QContact> QContactFetchRequest::contacts() const
{
    Q_D(const QContactFetchRequest);
    return d->m_contacts;
}

#include "moc_qcontactfetchrequest.cpp"

QTM_END_NAMESPACE
