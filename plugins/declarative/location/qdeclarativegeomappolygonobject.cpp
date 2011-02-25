
/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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

QDeclarativeGeoMapPolygonObject::QDeclarativeGeoMapPolygonObject()
{
    connect(&m_border,
            SIGNAL(colorChanged(QColor)),
            this,
            SLOT(borderColorChanged(QColor)));
    connect(&m_border,
            SIGNAL(widthChanged(int)),
            this,
            SLOT(borderWidthChanged(int)));
}

QDeclarativeGeoMapPolygonObject::~QDeclarativeGeoMapPolygonObject()
{
    qDeleteAll(m_path);
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
    poly->m_path.append(coordinate);
    QList<QGeoCoordinate> p = poly->path();
    p.append(coordinate->coordinate());
    poly->setPath(p);
}

int QDeclarativeGeoMapPolygonObject::path_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    return static_cast<QDeclarativeGeoMapPolygonObject*>(prop->object)->m_path.count();
}

QDeclarativeCoordinate* QDeclarativeGeoMapPolygonObject::path_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, int index)
{
    return static_cast<QDeclarativeGeoMapPolygonObject*>(prop->object)->m_path.at(index);
}

void QDeclarativeGeoMapPolygonObject::path_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    QDeclarativeGeoMapPolygonObject* poly = static_cast<QDeclarativeGeoMapPolygonObject*>(prop->object);
    QList<QDeclarativeCoordinate*> p = poly->m_path;
    qDeleteAll(p);
    p.clear();
    poly->setPath(QList<QGeoCoordinate>());
}

/*!
    \qmlproperty color MapPolygon::color

    This property holds the color used to fill the circle.

    The default value corresponds to a transparent color.
*/

void QDeclarativeGeoMapPolygonObject::setColor(const QColor &color)
{
    if (m_color == color)
        return;

    m_color = color;
    QBrush m_brush(color);
    setBrush(m_brush);
    emit colorChanged(m_color);
}

QColor QDeclarativeGeoMapPolygonObject::color() const
{
    return m_color;
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
    return &m_border;
}

void QDeclarativeGeoMapPolygonObject::borderColorChanged(const QColor &color)
{
    QPen p = pen();
    p.setColor(color);
    setPen(p);
}

void QDeclarativeGeoMapPolygonObject::borderWidthChanged(int width)
{
    QPen p = pen();
    p.setWidth(width);
    if (width == 0)
        p.setStyle(Qt::NoPen);
    else
        p.setStyle(Qt::SolidLine);
    setPen(p);
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

