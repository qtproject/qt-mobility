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

#include "qlandmarksaverequest.h"
#include "qlandmark.h"


QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkSaveRequest
    \brief The QLandmarkSaveRequest class allows a client to asynchronously
    request that certain landmarks be saved by a landmark manager.
    \ingroup landmarks-request

    For a QLandmarkSaveRequest, the resultsAvailable() signal will be emitted
    when either an individual items error out (individaul errors may be retrieved
    by calling errorMap()), an  overall operation error occurs(which may be
    retrieved by calling error()), or when individual items have been
    saved (which may be retrieved by calling landmarks()).
*/

/*!
    Constructs a landmark save request with the given \a parent.
*/
QLandmarkSaveRequest::QLandmarkSaveRequest(QObject *parent)
    : QLandmarkAbstractRequest(parent)
{
}

/*!
    Destroys the request object.
*/
QLandmarkSaveRequest::~QLandmarkSaveRequest()
{
}

/*!
    Returns the list of landmarks which will be saved if called prior to
    callling \l QLandmarkAbstractRequest::start(), otherwise returns the list of landmarks as they
    were saved into the landmark manager.
*/
QList<QLandmark> QLandmarkSaveRequest::landmarks() const
{
    return QList<QLandmark>();
}

/*!
    Sets the list of \a landmarks to be saved.

    \sa setLandmark()
*/
void QLandmarkSaveRequest::setLandmarks(const QList<QLandmark> &landmarks)
{
}

/*!
    Convenience function to set a single \a landmark to be saved.

    \sa setLandmarks()
*/
void QLandmarkSaveRequest::setLandmark(const QLandmark &landmark)
{
}

/*!
    Returns the mapping of input landmark list indices to errors which occurred.
*/
QMap<int, QLandmarkManager::Error> QLandmarkSaveRequest::errorMap() const
{
    return QMap<int, QLandmarkManager::Error>();
}

#include "moc_qlandmarksaverequest.cpp"

QTM_END_NAMESPACE


