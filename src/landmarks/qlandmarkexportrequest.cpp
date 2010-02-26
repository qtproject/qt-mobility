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

#include "qlandmarkexportrequest.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkExportRequest
    \brief The QLandmarkExportRequest class allows a client to asynchronously
    request that a landmark manager export a set of landmarks.

    For a QLandmarkExportRequest, the resultsAvailable() signal will be emitted
    if an overall operational error occurs.

    If no landmarks identifiers are set using setLandmarkIds() or
    an empty list is passed in, then all the landmarks will be exported.
    Invalid landmark identifiers that do not refer to landmarks will be ignored.

    \ingroup landmarks
*/

/*!
    Constructs a landmark export request
*/
QLandmarkExportRequest::QLandmarkExportRequest()
{
}

/*!
    Destroys the request object.
*/
QLandmarkExportRequest::~QLandmarkExportRequest()
{
}

/*!
    Returns the device to which landmarks are written.
*/
QIODevice *QLandmarkExportRequest::device() const
{
}

/*!
    Sets the \a device to which landmarks are written.
*/
void QLandmarkExportRequest::setDevice(QIODevice *device)
{
}

/*!
    Returns the data format for the export operation.
*/
QLandmarkManager::Format QLandmarkExportRequest::format() const
{
}

/*!
    Sets the data \a format for the export operation.
*/
void QLandmarkExportRequest::setFormat(QLandmarkManager::Format format)
{
}

/*!
    Returns the list of identifiers of landmarks that will be exported.
*/
QList<QLandmarkId> QLandmarkExportRequest::landmarkIds() const
{
}

/*!
    Sets the landmarks to be exported by specifying a list of
    \a landmarkIds
*/
void QLandmarkExportRequest::setLandmarkIds(QList<QLandmarkId> &landmarkIds)
{
}

#include "moc_qlandmarkexportrequest.cpp"

QTM_END_NAMESPACE


