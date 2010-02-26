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

#include "qlandmarkimportrequest.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkImportRequest
    \brief The QLandmarkImportRequest class allows a client to asynchronously
    request that a landmark manager import a set of landmarks.

    For a QLandmarkImportRequest, the resultsAvailable() signal will be emitted
    when landmarks have been successfully added by the manager.  The list
    of added landmarks may be retrieved by the ids() function.  The resultsAvailable()
    signal may also be emitted if an overall operational error occurs.

    \ingroup landmarks
*/

/*!
    Constructs a landmark import request
*/
QLandmarkImportRequest::QLandmarkImportRequest()
{
}

/*!
    Destroys the request object.
*/
QLandmarkImportRequest::~QLandmarkImportRequest()
{
}

/*!
    Returns the device from which landmarks are read.
*/
QIODevice *QLandmarkImportRequest::device() const
{
}

/*!
    Sets the \a device from which landmarks are read.
*/
void QLandmarkImportRequest::setDevice(QIODevice *device)
{
}


/*!
    Returns the expected data format for the import operation.
*/
QLandmarkManager::Format QLandmarkImportRequest::format() const
{
}

/*!
    Sets the expected data \a format for the import operation.
*/
void QLandmarkImportRequest::setFormat(QLandmarkManager::Format format)
{
}

/*!
    Returns a list of identifiers of landmarks that have been imported.
*/
QList<QLandmarkId> QLandmarkImportRequest::ids() const
{
}

#include "moc_qlandmarkimportrequest.cpp"

QTM_END_NAMESPACE


