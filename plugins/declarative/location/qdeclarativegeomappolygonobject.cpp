
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

#include "qdeclarativegeomappolygonobject_p.h"

#include <QColor>
#include <QBrush>

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass MapPolygon

    \brief The MapPolygon element displays a polygon on a map.
    \inherits QGeoMapPolygonObject

    \ingroup qml-location-maps

    The polygon is specified in terms of an ordered list of
    coordinates.  Any invalid coordinates in the list will be ignored.

    If the list contains less than 3 valid coordinates the polygon
    will not be displayed.

    The MapPolygon element is part of the \bold{QtMobility.location 1.1} module.
*/

QDeclarativeGeoMapPolygonObject::QDeclarativeGeoMapPolygonObject(QDeclarativeItem *parent)
    : QDeclarativeGeoMapObject(parent)
{
    polygon_ = new QGeoMapPolygonObject();
    setMapObject(polygon_);

    connect(&border_,
            SIGNAL(colorChanged(QColor)),
            this,
            SLOT(borderColorChanged(QColor)));
    connect(&border_,
            SIGNAL(widthChanged(int)),
            this,
            SLOT(borderWidthChanged(int)));
}

QDeclarativeGeoMapPolygonObject::~QDeclarativeGeoMapPolygonObject()
{
    qDeleteAll(path_);
    delete polygon_;
}

/*!
    \qmlproperty list<Coordinate> MapPolygon::path
    \default

    This property holds the ordered list of coordinates which
    define the polygon.
*/

QDeclarativeListProperty<QDeclarativeCoordinate> QDeclarativeGeoMapPolygonObject::declarativePath()
{
    return QDeclarativeListProperty<QDeclarativeCoordinate>(this,
            0,
            path_append,
            path_count,
            path_at,
            path_clear);
}

void QDeclarativeGeoMapPolygonObject::path_append(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, QDeclarativeCoordinate *coordinate)
{
    QDeclarativeGeoMapPolygonObject* poly = static_cast<QDeclarativeGeoMapPolygonObject*>(prop->object);
    poly->path_.append(coordinate);
    QList<QGeoCoordinate> p = poly->polygon_->path();
    p.append(coordinate->coordinate());
    poly->polygon_->setPath(p);
}

int QDeclarativeGeoMapPolygonObject::path_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    return static_cast<QDeclarativeGeoMapPolygonObject*>(prop->object)->path_.count();
}

QDeclarativeCoordinate* QDeclarativeGeoMapPolygonObject::path_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, int index)
{
    return static_cast<QDeclarativeGeoMapPolygonObject*>(prop->object)->path_.at(index);
}

void QDeclarativeGeoMapPolygonObject::path_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    QDeclarativeGeoMapPolygonObject* poly = static_cast<QDeclarativeGeoMapPolygonObject*>(prop->object);
    QList<QDeclarativeCoordinate*> p = poly->path_;
    qDeleteAll(p);
    p.clear();
    poly->polygon_->setPath(QList<QGeoCoordinate>());
}

/*!
    \qmlproperty color MapPolygon::color

    This property holds the color used to fill the circle.

    The default value corresponds to a transparent color.
*/

void QDeclarativeGeoMapPolygonObject::setColor(const QColor &color)
{
    if (color_ == color)
        return;

    color_ = color;
    QBrush m_brush(color);
    polygon_->setBrush(m_brush);
    emit colorChanged(color_);
}

QColor QDeclarativeGeoMapPolygonObject::color() const
{
    return color_;
}

/*!
    \qmlproperty int MapPolygon::border.width
    \qmlproperty color MapPolygon::border.color

    These properties hold the width and color used to draw the border of the circle.

    The width is in pixels and is independent of the zoom level of the map.

    The default values correspond to a black border with a width of 1 pixel.

    For no line, use a width of 0 or a transparent color.
*/

QDeclarativeGeoMapObjectBorder* QDeclarativeGeoMapPolygonObject::border()
{
    return &border_;
}

void QDeclarativeGeoMapPolygonObject::borderColorChanged(const QColor &color)
{
    QPen p = polygon_->pen();
    p.setColor(color);
    polygon_->setPen(p);
}

void QDeclarativeGeoMapPolygonObject::borderWidthChanged(int width)
{
    QPen p = polygon_->pen();
    p.setWidth(width);
    if (width == 0)
        p.setStyle(Qt::NoPen);
    else
        p.setStyle(Qt::SolidLine);
    polygon_->setPen(p);
}

/*!
    \qmlproperty int MapPolygon::zValue

    This property holds the z-value of the polygon.

    Map objects are drawn in z-value order, and objects with the
    same z-value will be drawn in insertion order.
*/

/*!
    \qmlproperty bool MapPolygon::visible

    This property holds a boolean corresponding to whether or not the
    polygon is visible.
*/

/*!
    \qmlproperty bool MapPolygon::selected

    This property holds a boolean corresponding to whether or not the
    polygon is selected.
*/

#include "moc_qdeclarativegeomappolygonobject_p.cpp"

QTM_END_NAMESPACE

