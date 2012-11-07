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

#include "qlandmarkfetchbyidrequest.h"
#include "qlandmarkrequests_p.h"
#include "qlandmark.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkFetchByIdRequest
    \brief The QLandmarkFetchByIdRequest class allows a client to asynchronously
    request a list of landmarks by id from a landmark manager.

    For a QLandmarkFetchByIdRequest, the resultsAvailable() signal will be emitted when the resultant
    landmarks (which may be retrieved by calling landmarks()) are updated,
    whenever indiviual items error out(individual errors may be retrieved by
    calling errorMap()), or when an overall operation error occurs(which may be
    retrieved by calling error()).

    Please see the class documentation for QLandmarkAbstractRequest for more information about
    the usage of request classes and ownership semantics.

    \inmodule QtLocation

    \ingroup landmarks-request
    \since 1.1
*/

/*!
    Creates a new request object with the given \a manager \a parent.
*/
QLandmarkFetchByIdRequest::QLandmarkFetchByIdRequest(QLandmarkManager *manager, QObject *parent)
    : QLandmarkAbstractRequest(new QLandmarkFetchByIdRequestPrivate(manager), parent)
{
}

/*!
    Destroys the request object.
*/
QLandmarkFetchByIdRequest::~QLandmarkFetchByIdRequest()
{
}

/*!
    Returns a list of identifiers of landmarks which are to be retrieved by this request.
    \since 1.1
*/
QList<QLandmarkId> QLandmarkFetchByIdRequest::landmarkIds() const
{
    Q_D(const QLandmarkFetchByIdRequest);
    QMutexLocker ml(&d->mutex);
    return d->landmarkIds;
}

/*!
    Sets the \a landmarkIds to be retrieved by this request.
    \since 1.1
*/
void QLandmarkFetchByIdRequest::setLandmarkIds(const QList<QLandmarkId> &landmarkIds)
{
    Q_D(QLandmarkFetchByIdRequest);
    QMutexLocker ml(&d->mutex);
    d->landmarkIds = landmarkIds;
}

/*!
    Convenience function that sets  a single landmark, specified by \a landmarkId,
    to be retrieved by this request.

    \since 1.1
*/
void QLandmarkFetchByIdRequest::setLandmarkId(const QLandmarkId &landmarkId)
{
    Q_D(QLandmarkFetchByIdRequest);
    QMutexLocker ml(&d->mutex);
    d->landmarkIds.clear();
    d->landmarkIds.append(landmarkId);
}

/*!
    Returns the list of landmarks fetched by this request.

    \since 1.1
*/
QList<QLandmark> QLandmarkFetchByIdRequest::landmarks() const
{
    Q_D(const QLandmarkFetchByIdRequest);
    QMutexLocker ml(&d->mutex);
    return d->landmarks;
}

/*!
    Returns the mapping of input landmark ID list indices
    to the errors which occurred.
    \since 1.1
*/
QMap<int, QLandmarkManager::Error> QLandmarkFetchByIdRequest::errorMap() const
{
    Q_D(const QLandmarkFetchByIdRequest);
    QMutexLocker ml(&d->mutex);
    return d->errorMap;
}

#include "moc_qlandmarkfetchbyidrequest.cpp"

QTM_END_NAMESPACE
