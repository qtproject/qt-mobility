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

#ifndef QLOCATION_GEOENGINE_H
#define QLOCATION_GEOENGINE_H

#include <QNetworkAccessManager>
#include <QHash>
#include <QNetworkReply>
#include <QNetworkProxy>

#include "qrouterequest.h"
#include "qgeocodingrequest.h"
#include "qreversegeocodingrequest.h"
#include "qmaptilerequest.h"
#include "qmaptile.h"

#include "qroutereply.h"
#include "qgeocodingreply.h"
#include "qmaptilereply.h"

QTM_BEGIN_NAMESPACE

class Q_LOCATION_EXPORT QGeoEngine : public QObject
{
    Q_OBJECT

public:
    QGeoEngine();
    virtual ~QGeoEngine();

    virtual QRouteReply* get(const QRouteRequest& request) = 0;
    virtual QGeocodingReply* get(const QGeocodingRequest& request) = 0;
    virtual QGeocodingReply* get(const QReverseGeocodingRequest& request) = 0;
    virtual QMapTileReply* get(const QMapTileRequest& request) = 0;
    virtual quint16 maxZoomLevel() const = 0;

    virtual QList<MapVersion> versions() const = 0;
    virtual QList<MapResolution> resolutions() const = 0;
    virtual QList<MapFormat> formats() const = 0;
    virtual QList<MapScheme> schemes() const = 0;

private:
    Q_DISABLE_COPY(QGeoEngine)

signals:
    void finished(QRouteReply* reply);
    void finished(QGeocodingReply* reply);
    void finished(QMapTileReply* reply);

    void error(QRouteReply* reply, QRouteReply::ErrorCode errorCode, QString errorString = QString());
    void error(QGeocodingReply* reply, QGeocodingReply::ErrorCode errorCode, QString errorString = QString());
    void error(QMapTileReply* reply, QMapTileReply::ErrorCode errorCode, QString errorString = QString());

public:
    static void getMercatorTileIndex(const QGeoCoordinate& coordinate, quint16 zoomLevel, quint32* col, quint32* row);
};

QTM_END_NAMESPACE

#endif
