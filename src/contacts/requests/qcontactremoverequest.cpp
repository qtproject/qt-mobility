/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#include "qcontactremoverequest.h"
#include "qcontactrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactRemoveRequest
  \brief The QContactRemoveRequest class allows a client to asynchronously
    request that certain contacts be removed from a contacts store.

  For a QContactRemoveRequest, the resultsUpdated() signal will be emitted when
  the individual item errors (which may be retrieved by calling errorMap()) are updated, or if the overall
  operation error (which may be retrieved by calling error()) is updated.

  Please see the class documentation of QContactAbstractRequest for more information about
  the usage of request classes and ownership semantics.


  \inmodule QtContacts
  \since 1.0

  \ingroup contacts-requests
 */

/*! Constructs a new contact remove request whose parent is the specified \a parent */
QContactRemoveRequest::QContactRemoveRequest(QObject* parent)
    : QContactAbstractRequest(new QContactRemoveRequestPrivate, parent)
{
}

/*! Frees any memory used by this request */
QContactRemoveRequest::~QContactRemoveRequest()
{
    QContactAbstractRequestPrivate::notifyEngine(this);
}

/*!
  Sets the id of the contact which will be removed to \a contactId.
  Equivalent to calling:
  \code
      setContactIds(QList<QContactLocalId>() << contactIds);
  \endcode
  \since 1.0
 */
void QContactRemoveRequest::setContactId(const QContactLocalId& contactId)
{
    Q_D(QContactRemoveRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_contactIds.clear();
    d->m_contactIds.append(contactId);
}

/*! Sets the list of ids of contacts which will be removed to \a contactIds
    \since 1.0
*/
void QContactRemoveRequest::setContactIds(const QList<QContactLocalId>& contactIds)
{
    Q_D(QContactRemoveRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_contactIds = contactIds;
}

/*! Returns the list of ids of contacts which will be removed
    \since 1.0
*/
QList<QContactLocalId> QContactRemoveRequest::contactIds() const
{
    Q_D(const QContactRemoveRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_contactIds;
}

/*! Returns the map of input contact list indices to errors which occurred
    \since 1.0
*/
QMap<int, QContactManager::Error> QContactRemoveRequest::errorMap() const
{
    Q_D(const QContactRemoveRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_errors;
}

#include "moc_qcontactremoverequest.cpp"

QTM_END_NAMESPACE
