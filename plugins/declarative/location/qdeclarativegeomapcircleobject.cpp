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

#include "qdeclarativegeomapcircleobject_p.h"

#include <QColor>
#include <QBrush>

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass MapCircle

    \brief The MapCircle element displays a circle on a map.
    \inherits QGeoMapCircleObject

    \ingroup qml-location-maps

    The circle is specified in terms of a central coordinate and 
    a radius in metres.

    If \l center and \l radius are not specified and valid the 
    circle will not be displayed.

    The MapCircle element is part of the \bold{QtMobility.location 1.1} module.
*/

QDeclarativeGeoMapCircleObject::QDeclarativeGeoMapCircleObject()
{
    m_center = new QDeclarativeCoordinate(this);

    connect(m_center,
            SIGNAL(latitudeChanged(double)),
            this,
            SLOT(centerLatitudeChanged(double)));

    connect(m_center,
            SIGNAL(longitudeChanged(double)),
            this,
            SLOT(centerLongitudeChanged(double)));

    connect(m_center,
            SIGNAL(altitudeChanged(double)),
            this,
            SLOT(centerAltitudeChanged(double)));

    connect(&m_border,
            SIGNAL(colorChanged(QColor)),
            this,
            SLOT(borderColorChanged(QColor)));
    connect(&m_border,
            SIGNAL(widthChanged(int)),
            this,
            SLOT(borderWidthChanged(int)));
}

QDeclarativeGeoMapCircleObject::~QDeclarativeGeoMapCircleObject()
{
}

/*!
    \qmlproperty Coordinate MapCircle::center

    This property holds the coordinate at the center of the circle.

    The default value is an invalid coordinate.
*/

void QDeclarativeGeoMapCircleObject::setDeclarativeCenter(const QDeclarativeCoordinate *center)
{
    if (m_center->coordinate() == center->coordinate())
        return;

    m_center->setCoordinate(center->coordinate());
    setCenter(center->coordinate());

    emit declarativeCenterChanged(m_center);
}

QDeclarativeCoordinate* QDeclarativeGeoMapCircleObject::declarativeCenter()
{
    return m_center;
}

void QDeclarativeGeoMapCircleObject::centerLatitudeChanged(double /*latitude*/)
{
    setCenter(m_center->coordinate());
}

void QDeclarativeGeoMapCircleObject::centerLongitudeChanged(double /*longitude*/)
{
    setCenter(m_center->coordinate());
}

void QDeclarativeGeoMapCircleObject::centerAltitudeChanged(double /*altitude*/)
{
    setCenter(m_center->coordinate());
}

/*!
    \qmlproperty qreal MapCircle::radius

    This property holds the radius of the circle in metres.
    
    A negative value is used to indicate that the radius is invalid and 
    the default value is a radius of -1.0.
*/

/*!
    \qmlproperty color MapCircle::color

    This property holds the color used to fill the circle.

    The default value corresponds to a transparent color.
*/

void QDeclarativeGeoMapCircleObject::setColor(const QColor &color)
{
    if (m_color == color)
        return;

    m_color = color;
    QBrush m_brush(color);
    setBrush(m_brush);
    emit colorChanged(m_color);
}

QColor QDeclarativeGeoMapCircleObject::color() const
{
    return m_color;
}

/*!
    \qmlproperty int MapCircle::border.width
    \qmlproperty color MapCircle::border.color

    These properties hold the width and color used to draw the border of the circle.

    The width is in pixels and is independent of the zoom level of the map.

    The default values correspond to a black border with a width of 1 pixel.

    For no line, use a width of 0 or a transparent color.
*/
QDeclarativeGeoMapObjectBorder* QDeclarativeGeoMapCircleObject::border()
{
    return &m_border;
}

void QDeclarativeGeoMapCircleObject::borderColorChanged(const QColor &color)
{
    QPen p = pen();
    p.setColor(color);
    setPen(p);
}

void QDeclarativeGeoMapCircleObject::borderWidthChanged(int width)
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
    \qmlproperty int MapCircle::zValue

    This property holds the z-value of the circle.

    Map objects are drawn in z-value order, and objects with the 
    same z-value will be drawn in insertion order.
*/

/*!
    \qmlproperty bool MapCircle::visible

    This property holds a boolean corresponding to whether or not the 
    circle is visible.
*/

/*!
    \qmlproperty bool MapCircle::selected

    This property holds a boolean corresponding to whether or not the 
    circle is selected.
*/

#include "moc_qdeclarativegeomapcircleobject_p.cpp"

QTM_END_NAMESPACE

