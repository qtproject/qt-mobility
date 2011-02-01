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

#include "qgeotiledmapcircleobjectinfo_p.h"

#include "qgeotiledmapobjectinfo_p.h"
#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeomapcircleobject.h"

#include <QGraphicsItem>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <math.h>

QTM_BEGIN_NAMESPACE

QGeoTiledMapCircleObjectInfo::QGeoTiledMapCircleObjectInfo(QGeoTiledMapData *mapData, QGeoMapObject *mapObject)
    : QGeoTiledMapObjectInfo(mapData, mapObject)
{
    circle = static_cast<QGeoMapCircleObject*>(mapObject);

    connect(circle,
            SIGNAL(centerChanged(QGeoCoordinate)),
            this,
            SLOT(centerChanged(QGeoCoordinate)));
    connect(circle,
            SIGNAL(radiusChanged(qreal)),
            this,
            SLOT(radiusChanged(qreal)));
    connect(circle,
            SIGNAL(penChanged(QPen)),
            this,
            SLOT(penChanged(QPen)));
    connect(circle,
            SIGNAL(brushChanged(QBrush)),
            this,
            SLOT(brushChanged(QBrush)));

    polygonItem = new QGraphicsPolygonItem;
    graphicsItem = polygonItem;

    brushChanged(circle->brush());
    penChanged(circle->pen());

    updateValidity();
    if (valid())
        update();
}

QGeoTiledMapCircleObjectInfo::~QGeoTiledMapCircleObjectInfo() {}

static const double qgeocoordinate_EARTH_MEAN_RADIUS = 6371.0072;

inline static double qgeocoordinate_degToRad(double deg)
{
    return deg * M_PI / 180;
}
inline static double qgeocoordinate_radToDeg(double rad)
{
    return rad * 180 / M_PI;
}

void QGeoTiledMapCircleObjectInfo::updateValidity()
{
    setValid((circle->center().isValid() && (circle->radius() >= 0.0)));
}

void QGeoTiledMapCircleObjectInfo::centerChanged(const QGeoCoordinate &/*center*/)
{
    updateValidity();
    if (valid())
        update();

}

void QGeoTiledMapCircleObjectInfo::radiusChanged(qreal /*radius*/)
{
    updateValidity();
    if (valid())
        update();
}

void QGeoTiledMapCircleObjectInfo::update()
{
    QList<QGeoCoordinate> path;

    QGeoCoordinate center = circle->center();
    double radius = circle->radius() / (qgeocoordinate_EARTH_MEAN_RADIUS * 1000);

    // To simplify things, we're using formulae from astronomy, namely those from the nautic triangle for converting from horizontal system to equatorial system.
    // First, we calculate the input coordinates (horizontal system)
    // altitude - derivation:
    /*
                   A________B
                   /\ h) (.|
                  /  \/   \|
                 / 90°\    |
                /\____/\   |
               /        \  |
              /          \ |
             /            \|
            /              |C
           /               |\
          /                | \
         /                 |  \
        /                  |   \
       /                   |    \
      /\                   |\   /\
    F/ r)                 E|.) (x \D
     ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
    explanations:
        A is the central point
        E is the peripheral point

        AD is the tangent at the central point
        BE is the tangent at the peripheral point

        h = <)ACB is the altitude
        r = <)FDA is the radius, in radians, of the small circle we want to achieve
        x = <)DCE is an auxiliary angle

        <)EDC, <)DCF, <)BAC and <)AFD are rectangular (as denoted by the dot or 90°)
        AB and FD are parallel

    knowns:
        r

    unknowns:
        h

    derivation:
        h = x | the triangles ABC and CDE have the same angles (h/x, 90° and the angle between the two tangents)

        r + x + 90° = 180° | sum of interior angles
        <=> r + h + 90° = 180° | h = x
        <=> h = 90° - r
    */
    //double h = M_PI/2 - radius;
    double cosh = sin(radius); // cos(M_PI/2 - radius); // cos(h);
    double sinh = cos(radius); // sin(M_PI/2 - radius); // sin(h);
    // Location latitude
    double phi = qgeocoordinate_degToRad(center.latitude());
    double cosphi = cos(phi), sinphi = sin(phi);
    // Star time (since we're not doing actual astronomy here, we use a greenwich star time of 0)
    double theta = qgeocoordinate_degToRad(center.longitude());

    double sinphi_sinh = sinphi * sinh;
    double cosphi_cosh = cosphi * cosh;
    double sinphi_cosh = sinphi * cosh;
    double cosphi_sinh = cosphi * sinh;

    //qDebug("circle: h=%f lat=%f lng=%f radius=%fm",h,phi,theta, radius*(qgeocoordinate_EARTH_MEAN_RADIUS * 1000));
    // Azimut - we iterate over a full circle, in 128 steps for now. TODO: find a better step count, adjust by display size etc
    int steps = 128;
    for (int i = 0; i < steps; ++i) {
        double a = M_PI / 2 + 2 * M_PI * i / steps;
        // next, we convert from horizontal system -> equatorial system
        // First, sin(delta)
        double sindelta = sinphi_sinh - cosphi_cosh * cos(a);

        // calculate the value of cos(delta)*cos(tau)...
        double cosdelta_costau = cosphi_sinh + sinphi_cosh * cos(a);
        // ...and cos(delta)*sin(tau)
        double cosdelta_sintau = -sin(a) * cosh;

        // now we obtain the actual value of the hour angle...
        // convert from cartesian to polar ( cos(delta)*cos(tau) | cos(delta)*sin(tau) ) -> ( tau | cos(delta) )
        double tau = atan2(cosdelta_sintau, cosdelta_costau);
        double cosdelta = sqrt(cosdelta_sintau * cosdelta_sintau + cosdelta_costau * cosdelta_costau);

        // ...and the declination
        // convert from cartesian to polar ( cos(delta) | sin(delta) ) -> ( delta | 1 )
        double delta = atan2(sindelta, cosdelta);
        //qAssert(qFuzzyCompare(sqrt(sindelta*sindelta + cosdelta*cosdelta), 1)); // taken out because it takes too much cpu

        // we calculate right ascension from tau
        double alpha = theta - tau;

        // we interpret right ascension as latitude and declination as longitude
        double lat = qgeocoordinate_radToDeg(delta);
        double lng = qgeocoordinate_radToDeg(alpha);
        if (lng < -180)
            lng += 360;
        if (lng > 180)
            lng -= 360;

        //qDebug("lat=%f lng=%f",lat,lng);
        path.append(QGeoCoordinate(lat, lng));
    }

    points = createPolygon(path, tiledMapData, true, center.latitude() > 0 ? -100 : tiledMapData->worldReferenceSize().height() + 100); // 100px beyond the closest pole
    //makepoly(points, path, mapData, true, center.latitude() > 0 ? -100 : mapData->maxZoomSize.height()+100); // 100px beyond the closest pole

    polygonItem->setPolygon(points);

    updateItem();
}

void QGeoTiledMapCircleObjectInfo::penChanged(const QPen &pen)
{
    polygonItem->setPen(pen);
    updateItem();
}

void QGeoTiledMapCircleObjectInfo::brushChanged(const QBrush &brush)
{
    polygonItem->setBrush(brush);
    updateItem();
}

#include "moc_qgeotiledmapcircleobjectinfo_p.cpp"

QTM_END_NAMESPACE
