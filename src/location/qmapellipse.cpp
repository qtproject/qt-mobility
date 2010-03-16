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

#include "qmapellipse.h"
#include "qmapellipse_p.h"

QTM_BEGIN_NAMESPACE

QMapEllipsePrivate::QMapEllipsePrivate()
    :geoTopLeft(QGeoCoordinateMaps()),
     mapTopLeft(QPointF()),
     geoBottomRight(QGeoCoordinateMaps()),
     mapBottomRight(QPointF()),
     p(QPen()), b(QBrush())
{
}

/*!
*   \class QMapEllipse
*   \brief The QMapEllipse class represents an ellipse that has been added to the
           map.
*   \ingroup location
*/

/*!
    Constructs an ellipse in the \a mapView with bounding box corners specified by \a topLeft and
    \a bottomRight.  The ellipse outline is drawn with the given \a pen and filled
    with the given \a brush.  The ellipse is displayed at the layer specified by
    \a layerIndex.
*/
QMapEllipse::QMapEllipse(const QMapView *mapView, const QGeoCoordinateMaps& topLeft, const QGeoCoordinateMaps& bottomRight,
            const QPen& pen, const QBrush& brush, quint16 layerIndex)
    :QMapObject(*new QMapEllipsePrivate, mapView, QMapObject::EllipseObject, layerIndex)
{
    Q_D(QMapEllipse);
    d->geoTopLeft = topLeft;
    d->geoBottomRight = bottomRight;
    d->p =  pen;
    d->b = brush;
}

/*!
    \internal
*/
QMapEllipse::QMapEllipse(QMapEllipsePrivate &dd, const QMapView *mapView, const QGeoCoordinateMaps& topLeft, const QGeoCoordinateMaps& bottomRight,
            const QPen& pen, const QBrush& brush, quint16 layerIndex)
    :QMapObject(dd, mapView, QMapObject::EllipseObject, layerIndex)
{
    Q_D(QMapEllipse);
    d->geoTopLeft = topLeft;
    d->geoBottomRight = bottomRight;
    d->p =  pen;
    d->b = brush;
}

/*!
    Returns the top left geo coordinate of the bounding box of the ellipse.
*/
QGeoCoordinateMaps QMapEllipse::topLeft() const {
    Q_D(const QMapEllipse);
    return d->geoTopLeft;
}

/*!
    Returns the bottom right geo coordinate of the bounding box of the ellipse.
*/
QGeoCoordinateMaps QMapEllipse::bottomRight() const {
    Q_D(const QMapEllipse);
    return d->geoBottomRight;
}

/*!
    Returns the pen used for drawing the ellipse.
*/
QPen QMapEllipse::pen() const {
    Q_D(const QMapEllipse);
    return d->p;
}
/*!
*   Returns the brush used for drawing the ellipse.
*/
QBrush QMapEllipse::brush() const {
    Q_D(const QMapEllipse);
    return d->b;
}

QTM_END_NAMESPACE
