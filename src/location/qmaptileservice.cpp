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

#include "qmaptileservice.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QMapTileService
    \brief The QMapTileService class handles all communication to and from a geographic map service.
    \ingroup location

    This is the abstract base class for all classes that handle
    map tile requests/replies to and from a service which provides geographic maps.
*/

/*!
    Default constructor.
*/
QMapTileService::QMapTileService() {}
/*!
    Virtual destructor.
*/
QMapTileService::~QMapTileService() {}

/*!
    \fn QMapTileReply* QMapTileService::getMapTile(const QGeoMapTileRequest& request)

    Submits a map tile request \a request and returns the corresponding QMapTileReply.

    \note Due to the asynchronous nature of requests, you should wait for the
    QMapTileService::finished(QMapTileReply*) signal from this object or the
    QMapTileReply::finished() signal from the returned QMapTileReply object before
    working with the reply.
*/

/*!
    \fn quint16 QMapTileService::maxZoomLevel() const

    Returns the maximum zoom level supported by this geo engine.
*/

/*!
    \fn QList<MapVersion> QMapTileService::versions() const

    Returns a list of all available \l {MapVersion} {MapVersions}.
*/

/*!
    \fn QList<MapResolution> QMapTileService::resolutions() const

    Returns a list of all available \l {MapResolution} {MapResolutions}.
*/

/*!
    \fn QList<MapFormat> QMapTileService::formats() const

    Returns a list of all available \l {MapFormat} {MapFormats}.
*/

/*!
    \fn QList<MapScheme> QMapTileService::schemes() const

    Returns a list of all available \l {MapScheme} {MapSchemes}.
*/

/*!
    \fn void QMapTileService::finished(QMapTileReply* reply)

    This signal is emitted when a QMapTileReply is available in
    response to a previously submitted QGeoMapTileRequest.
*/

/*!
    \fn void QMapTileService::error(QMapTileReply* reply, QGeoReply::ErrorCode code)

    This signal is emitted when a QMapTileReply encountered an error
    in response to a previously submitted QGeoMapTileRequest.
    \note For QGeoNetworkManager, the error code will be identical to QNetworkReply::NetworkError.
*/

#include "moc_qmaptileservice.cpp"

QTM_END_NAMESPACE
