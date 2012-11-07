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

#include "qorganizercollectionsaverequest.h"
#include "qorganizeritemrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerCollectionSaveRequest
  \brief The QOrganizerCollectionSaveRequest class allows a client to asynchronously
    request organizeritems from a organizeritems store manager.

  \inmodule QtOrganizer
  \since 1.1


  For a QOrganizerCollectionSaveRequest, the resultsAvailable() signal will be emitted when the resultant
  organizeritems (which may be retrieved by calling organizeritems()), are updated, as well as if
  the overall operation error (which may be retrieved by calling error()) is updated.

  \ingroup organizeritems-requests

  \inmodule QtOrganizer
 */

/*! Constructs a new organizeritem fetch request whose parent is the specified \a parent
    \since 1.1
*/
QOrganizerCollectionSaveRequest::QOrganizerCollectionSaveRequest(QObject* parent)
    : QOrganizerAbstractRequest(new QOrganizerCollectionSaveRequestPrivate, parent)
{
}

/*! Frees memory in use by this request
    \since 1.2
*/
QOrganizerCollectionSaveRequest::~QOrganizerCollectionSaveRequest()
{
    QOrganizerAbstractRequestPrivate::notifyEngine(this);
}

/*! Clears the list of collections which will be saved, and sets the collection which will be saved to \a collection
  \since 1.1
*/
void QOrganizerCollectionSaveRequest::setCollection(const QOrganizerCollection& collection)
{
    Q_D(QOrganizerCollectionSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_collections.clear();
    d->m_collections.append(collection);
}

/*! Sets the list of collections which will be saved to \a collections
    \since 1.1
*/
void QOrganizerCollectionSaveRequest::setCollections(const QList<QOrganizerCollection>& collections)
{
    Q_D(QOrganizerCollectionSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_collections = collections;
}

/*! Returns the collections which will be saved by this request if called prior to calling \l start(),
    otherwise returns the (possibly updated) collections which were saved.
    \since 1.1
*/
QList<QOrganizerCollection> QOrganizerCollectionSaveRequest::collections() const
{
    Q_D(const QOrganizerCollectionSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_collections;
}

/*! Returns any errors which occurred during the request
    \since 1.1
*/
QMap<int, QOrganizerManager::Error> QOrganizerCollectionSaveRequest::errorMap() const
{
    Q_D(const QOrganizerCollectionSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_errors;
}

#include "moc_qorganizercollectionsaverequest.cpp"

QTM_END_NAMESPACE
