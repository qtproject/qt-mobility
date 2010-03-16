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

#ifndef QLOCATION_MAPOBJECT_H
#define QLOCATION_MAPOBJECT_H

#include <QGraphicsItem>
#include <QRectF>
#include <QLineF>
#include <QList>
#include <QObject>

#include "qmapview.h"

QTM_BEGIN_NAMESPACE

/*!
* This is the abstract base class of all internal map objects. Map objects store all information
* that is needed to generate concrete Qt:QGraphicsItem objects when they become covered by the
* view port.
*/
class QMapObject //: QObject
{
    //Q_OBJECT

    friend class QMapView;
    friend class QMapViewPrivate;

public:
    enum MapObjectType {
        Marker,
        Line,
        Rect,
        Route,
        Ellipse,
        Polygon,
        Pixmap
    };

public:
    /*!
    * Constructor.
    */
    QMapObject(MapObjectType type, quint16 z = 0)
            : mapView(0), objType(type), z(z) {}

    /*!
    * @return The z index of this map object, as used for layering.
    *         Objects with higher z indices are stacked on top of objects
    *         with lower z indices.
    */
    inline quint16 zValue() const {
        return z;
    }
    inline MapObjectType type() const {
        return objType;
    }

    static QRectF boundingRect(const QLineF& line);
    static bool intersect(const QRectF& rect, const QLineF& line);

protected:
    /*!
    * Computes whether this map objects intersects with or completely lies in the given rectangle.
    * @param tileRect The rectangle in terms of map (pixel) coordinates.
    * @return true (intersects) / false (does not intersect)
    */
    virtual bool intersects(const QRectF& rect) const = 0;
    /*!
    * Computes the appropriate map coordinates (in pixels) according to the current QMapView setup.
    */
    virtual void compMapCoords() = 0;
    virtual void compIntersectingTiles(const QRectF& box);
    virtual void paint(QPainter* painter, const QRectF& viewPort) = 0;

    void setParentView(QMapView* mapView);

protected:
    //! The one-dimensional indices of the tiles that this map object either intersects or completely lies on.
    QList<quint64> intersectingTiles;
    QMapView* mapView;
    MapObjectType objType;
    quint16 z; //!< The z index, used for layering.
};

QTM_END_NAMESPACE

#endif
