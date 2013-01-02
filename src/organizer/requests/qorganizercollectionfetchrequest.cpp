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

#include "qorganizercollectionfetchrequest.h"
#include "qorganizeritemrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerCollectionFetchRequest
  \brief The QOrganizerCollectionFetchRequest class allows a client to asynchronously
    request collections from an organizer manager.

  \inmodule QtOrganizer
  \since 1.1


  For a QOrganizerCollectionFetchRequest, the resultsAvailable() signal will be emitted when the resultant
  collections (which may be retrieved by calling collections()), are updated, as well as if
  the overall operation error (which may be retrieved by calling error()) is updated.

  \ingroup organizeritems-requests
 */

/*! Constructs a new organizeritem fetch request whose parent is the specified \a parent
  \since 1.1
*/
QOrganizerCollectionFetchRequest::QOrganizerCollectionFetchRequest(QObject* parent)
    : QOrganizerAbstractRequest(new QOrganizerCollectionFetchRequestPrivate, parent)
{
}

/*! Frees memory in use by this request
  \since 1.2
*/
QOrganizerCollectionFetchRequest::~QOrganizerCollectionFetchRequest()
{
    QOrganizerAbstractRequestPrivate::notifyEngine(this);
}

/*! Returns the collections retrieved by this request
  \since 1.1
*/
QList<QOrganizerCollection> QOrganizerCollectionFetchRequest::collections() const
{
    Q_D(const QOrganizerCollectionFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_collections;
}

#include "moc_qorganizercollectionfetchrequest.cpp"

QTM_END_NAMESPACE
