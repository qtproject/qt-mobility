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

#include "qgeomapviewport_nokia_p.h"
#include "qgeomappingmanager_nokia_p.h"

#include <QNetworkProxy>

#define PI 3.14159265
#include <math.h>

QGeoMapViewportNokia::QGeoMapViewportNokia(QGeoMappingManager *manager,
        const QMap<QString, QString> &parameters,
        QGeoServiceProvider::Error *error,
        QString *errorString)
        : QGeoMapViewport(manager)
{
}

QGeoMapViewportNokia::~QGeoMapViewportNokia()
{
    //delete m_cache;
}

void QGeoMapViewportNokia::setZoomLevel(int zoomLevel)
{
}

void QGeoMapViewportNokia::paint(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
}

void QGeoMapViewportNokia::setCenter(const QGeoCoordinate &center)
{
}

QGeoCoordinate QGeoMapViewportNokia::center() const
{
    return QGeoCoordinate();
}

void QGeoMapViewportNokia::pan(int startX, int startY, int endX, int endY)
{
}

QGeoBoundingBox QGeoMapViewportNokia::viewBounds() const
{
    return QGeoBoundingBox();
}

/*!
    Translates the input \a coordinate to a pixel representation in the view.
    The pixel position is relative to the viewport (i.e. (0,0) defines the top left visible point in the map).
*/
QPointF QGeoMapViewportNokia::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    qint32 numColRow = 1;
    numColRow <<= zoomLevel();
    double lng = coordinate.longitude(); //x
    double lat = coordinate.latitude(); //y

    lng = lng / 360.0 + 0.5;

    lat = 0.5 - (log(tan((PI / 4.0) + (PI / 2.0) * lat / 180.0)) / PI) / 2.0;
    lat = qMax(0.0, lat);
    lat = qMin(1.0, lat);

    QPointF point(static_cast<qint64>(lng * ((qreal) numColRow) * ((qreal) m_tileSize.width())),
                  static_cast<qint64>(lat * ((qreal) numColRow) * ((qreal) m_tileSize.height())));
    point -= m_viewPort.topLeft();
    return point;
}

qreal rmod(const qreal a, const qreal b)
{
    quint64 div = static_cast<quint64>(a / b);
    return a - static_cast<qreal>(div) * b;
}

/*!
    Translates the pixel \a screenPosition in the view to a geometric coordinate.
    The pixel position is relative to the viewport (i.e. (0,0) defines the top left visible point in the map).
*/
QGeoCoordinate QGeoMapViewportNokia::screenPositionToCoordinate(QPointF screenPosition) const
{
    qint32 numColRow = 1;
    numColRow <<= zoomLevel();
    screenPosition += m_viewPort.topLeft();
    QPointF mercCoord(screenPosition.x() / (((qreal) numColRow) * ((qreal) m_tileSize.width())),
                      screenPosition.y() / (((qreal) numColRow) * ((qreal) m_tileSize.height())));

    qreal x = mercCoord.x();
    qreal y = mercCoord.y();

    if (y < 0.0f)
        y = 0.0f;
    else if (y > 1.0f)
        y = 1.0f;

    qreal lat;

    if (y == 0.0f)
        lat = 90.0f;
    else if (y == 1.0f)
        lat = -90.0f;
    else
        lat = (180.0f / PI) * (2.0f * atan(exp(PI * (1.0f - 2.0f * y))) - (PI / 2.0f));

    qreal lng;
    if (x >= 0) {
        lng = rmod(x, 1.0f);
    } else {
        lng = rmod(1.0f - rmod(-1.0f * x, 1.0f), 1.0f);
    }

    lng = lng * 360.0f - 180.0f;

    return QGeoCoordinate(lat, lng);
}
