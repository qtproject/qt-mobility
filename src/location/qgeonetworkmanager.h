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

#ifndef QLOCATION_GEONETWORKMANAGER_H
#define QLOCATION_GEONETWORKMANAGER_H

#include <QNetworkReply>
#include <QString>

#include "qgeoengine.h"

#define MAPTILE_MAX_ZOOMLEVEL 18

QTM_BEGIN_NAMESPACE

class QGeoNetworkManagerPrivate;
class Q_LOCATION_EXPORT QGeoNetworkManager : public QGeoEngine
{
    Q_OBJECT

public:
    QGeoNetworkManager(QString token, QString referrer);
    virtual ~QGeoNetworkManager();

    void setGeocodingServer(const QString& server);
    QString geocodingServer() const;
    QNetworkProxy geocodingProxy() const;
    void setGeocodingProxy(const QNetworkProxy& proxy);

    void setRouteServer(const QString& server);
    QString routeServer() const;
    QNetworkProxy routeProxy() const;
    void setRouteProxy(const QNetworkProxy& proxy);

    void setMapServer(const QString& server);
    QString mapServer() const;
    QNetworkProxy mapProxy() const;
    void setMapProxy(const QNetworkProxy& proxy);

    virtual QGeoRouteReply* get(const QGeoRouteRequest& request);
    virtual QGeocodingReply* get(const QGeocodingRequest& request);
    virtual QGeocodingReply* get(const QReverseGeocodingRequest& request);
    virtual QGeoMapTileReply* get(const QGeoMapTileRequest& request);

    virtual quint16 maxZoomLevel() const;
    virtual QList<MapVersion> versions() const;
    virtual QList<MapResolution> resolutions() const;
    virtual QList<MapFormat> formats() const;
    virtual QList<MapScheme> schemes() const;

private:
    Q_DISABLE_COPY(QGeoNetworkManager)

private:
    QGeoNetworkManagerPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QGeoNetworkManager)
};

QTM_END_NAMESPACE

#endif
