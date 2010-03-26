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

#include "qgeocodingservice.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeocodingService
    \brief The QGeocodingService class handles all communications to and from a geocoding service.
    \ingroup location

    This is the abstract base class for all classes that handle
    geocoding and reverse geocoding requests/replies to and from a geocoding service.
*/

/*!
    Default constructor.
*/
QGeocodingService::QGeocodingService() {}
/*!
    Virtual destructor.
*/
QGeocodingService::~QGeocodingService() {}

/*!
    \fn QGeocodingReply* QGeocodingService::geocode(const QGeocodingRequest& request)

    Submits a geocoding request \a request and returns the corresponding QGeocodingReply.

    \note Due to the asynchronous nature of requests, you should wait for the
    QGeocodingService::finished(QGeocodingReply*) signal from this object or the
    QGeocodingReply::finished() signal from the returned QGeocodingReply object before
    working with the reply.
*/

/*!
    \fn QGeocodingReply* QGeocodingService::reverseGeocode(const QReverseGeocodingRequest& request)

    Submits a reverse geocoding request \a request and returns the corresponding QGeocodingReply.

    \note Due to the asynchronous nature of requests, you should wait for the
    QGeocodingService::finished(QGeocodingReply*) signal from this object or the
    QGeocodingReply::finished() signal from the returned QGeocodingReply object before
    working with the reply.
*/

/*!
    \fn void QGeocodingService::finished(QGeocodingReply* reply)

    This signal is emitted when a QGeocodingReply is available in
    response to a previously submitted QGeocodingRequest
    or QReverseGeocodingRequest.
*/

/*!
    \fn void QGeocodingService::error(QGeocodingReply* reply, QGeoReply::ErrorCode code)

    This signal is emitted when a QGeocodingReply encountered an error
    in response to a previously submitted QGeocodingReques or QReverseGeocodingRequest.
    \note For QGeoNetworkManager, the error code will be identical to QNetworkReply::NetworkError.
*/

#include "moc_qgeocodingservice.cpp"

QTM_END_NAMESPACE
