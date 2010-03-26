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

#include "qgeomapservice_nokia_p.h"
#include "qgeomaptilereply_nokia_p.h"

#define PI 3.14159265
#include <math.h>

QTM_BEGIN_NAMESPACE

QGeoMapServiceNokia::QGeoMapServiceNokia()
    : m_token(""),
    m_referrer(""),
    m_host("172.24.32.155"),
    m_proxy(QNetworkProxy(QNetworkProxy::NoProxy))
{
}

QGeoMapServiceNokia::~QGeoMapServiceNokia()
{
}

QString QGeoMapServiceNokia::token() const
{
    return m_token;
}

void QGeoMapServiceNokia::setToken(const QString &token)
{
    m_token = token;
}

QString QGeoMapServiceNokia::referrer() const
{
    return m_referrer;
}

void QGeoMapServiceNokia::setReferrer(const QString &referrer)
{
    m_referrer = referrer;
}

QString QGeoMapServiceNokia::host() const
{
    return m_host;
}

void QGeoMapServiceNokia::setHost(const QString &host)
{
    m_host = host;
}

QNetworkProxy QGeoMapServiceNokia::proxy() const
{
    return m_proxy;
}

void QGeoMapServiceNokia::setProxy(const QNetworkProxy &proxy)
{
    m_proxy = proxy;
    m_nam.setProxy(m_proxy);
}

QGeoMapTileReply* QGeoMapServiceNokia::getMapTile(const QGeoMapTileRequest& request)
{
    //check cache first
    QGeoMapTileReply* tileReply = NULL;

    if ((tileReply = m_cache.get(request))) {
        connect(tileReply,
                SIGNAL(finished()),
                this,
                SLOT(finishedReply()));
        connect(tileReply,
                SIGNAL(error(QGeoMapTileReply::ErrorCode, QString)),
                this,
                SLOT(errorReply(QGeoMapTileReply::ErrorCode, QString)));
        tileReply->done();
    } else {
        QString rawRequest = request.requestString(m_host, m_token, m_referrer);
        QNetworkRequest netRequest = QNetworkRequest(QUrl(rawRequest));
        QNetworkReply* netReply = m_nam.get(netRequest);

        tileReply = new QGeoMapTileReplyNokia(request, netReply);

        connect(tileReply,
                SIGNAL(finished()),
                this,
                SLOT(finishedReply()));
        connect(tileReply,
                SIGNAL(error(QGeoMapTileReply::ErrorCode, QString)),
                this,
                SLOT(errorReply(QGeoMapTileReply::ErrorCode, QString)));
    }

    return tileReply;
}

quint16 QGeoMapServiceNokia::maxZoomLevel() const
{
    return m_maxZoomLevel;
}

QList<MapVersion> QGeoMapServiceNokia::versions() const
{
    return m_versions;
}

QList<MapResolution> QGeoMapServiceNokia::resolutions() const
{
    return m_resolutions;
}

QList<MapFormat> QGeoMapServiceNokia::formats() const
{
    return m_formats;
}

QList<MapScheme> QGeoMapServiceNokia::schemes() const
{
    return m_schemes;
}

void QGeoMapServiceNokia::finishedReply()
{
    QGeoMapTileReply *reply = static_cast<QGeoMapTileReply *>(this->sender());

    if (reply->data().length() > 0)
        m_cache.cache(reply->request(), *reply);

    emit finished(reply);
}

void QGeoMapServiceNokia::errorReply(QGeoMapTileReply::ErrorCode errorCode, QString errorString)
{
    emit error(static_cast<QGeoMapTileReply *>(this->sender()), errorCode, errorString);
}

/*!
    The map tile server uses a Mercator projection to map geo coordinates into a 2D map.
    For each zoom level, that map is then split into 2^zoomLevel x 2^zoomLevel map tiles.

    For the given \a zoomLevel, this function  determines the row and column indices of the
    map tile from the 2D map which contains the geo coordinate \a coordinate and stores
    the indices in \a row and \a col respectively.

    \note This does not mean that the coordinate lies in the center of the calculated tile.
*/
void QGeoMapServiceNokia::getMercatorTileIndex(const QGeoCoordinate& coordinate, quint16 zoomLevel, quint32* col, quint32* row)
{
    qreal p = pow((double) 2, zoomLevel);

    double x = coordinate.longitude() / 360 + 0.5;
    double y = 0.5 - (log(tan((PI / 4.0) + (PI / 2.0) * coordinate.latitude() / 180)) / PI) / 2;

    if (y < 0)
        y = 0;

    if (y > 1)
        y = 1;

    *col = (quint32)(x * p);
    *row = (quint32)(y * p);
}

#include "moc_qgeomapservice_nokia_p.cpp"

QTM_END_NAMESPACE

