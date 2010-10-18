


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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeomaprectangleobject_p.h"

#include <QColor>
#include <QBrush>

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass MapRectangle

    \brief The MapRectangle element displays a rectangle on a map.
    \inherits QGeoMapRectangleObject

    \ingroup qml-location-maps

    The rectangle is specified in terms of the top left and bottom 
    right coordinates.

    If \l topLeft and \l bottomRight are not specified and valid the 
    rectangle will not be displayed.

    The MapRectangle element is part of the \bold{QtMobility.location 1.1} module.
*/

QDeclarativeGeoMapRectangleObject::QDeclarativeGeoMapRectangleObject()
{
    m_topLeft = new QDeclarativeCoordinate(this);

    connect(m_topLeft,
            SIGNAL(latitudeChanged(double)),
            this,
            SLOT(topLeftLatitudeChanged(double)));
    connect(m_topLeft,
            SIGNAL(longitudeChanged(double)),
            this,
            SLOT(topLeftLongitudeChanged(double)));
    connect(m_topLeft,
            SIGNAL(altitudeChanged(double)),
            this,
            SLOT(topLeftAltitudeChanged(double)));

    m_bottomRight = new QDeclarativeCoordinate(this);

    connect(m_bottomRight,
            SIGNAL(latitudeChanged(double)),
            this,
            SLOT(bottomRightLatitudeChanged(double)));
    connect(m_bottomRight,
            SIGNAL(longitudeChanged(double)),
            this,
            SLOT(bottomRightLongitudeChanged(double)));
    connect(m_bottomRight,
            SIGNAL(altitudeChanged(double)),
            this,
            SLOT(bottomRightAltitudeChanged(double)));

    connect(&m_border,
            SIGNAL(colorChanged(QColor)),
            this,
            SLOT(borderColorChanged(QColor)));
    connect(&m_border,
            SIGNAL(widthChanged(int)),
            this,
            SLOT(borderWidthChanged(int)));
}

QDeclarativeGeoMapRectangleObject::~QDeclarativeGeoMapRectangleObject()
{
}

/*!
    \qmlproperty Coordinate MapRectangle::topLeft
    
    This property holds the coordinate corresponding to the top left 
    corner of the rectangle.

    The default value is an invalid coordinate.
*/

void QDeclarativeGeoMapRectangleObject::setDeclarativeTopLeft(const QDeclarativeCoordinate *topLeft)
{
    if (m_topLeft->coordinate() == topLeft->coordinate())
        return;

    m_topLeft->setCoordinate(topLeft->coordinate());
    setTopLeft(topLeft->coordinate());

    emit declarativeTopLeftChanged(m_topLeft);
}

QDeclarativeCoordinate* QDeclarativeGeoMapRectangleObject::declarativeTopLeft()
{
    return m_topLeft;
}

void QDeclarativeGeoMapRectangleObject::topLeftLatitudeChanged(double /*latitude*/)
{
    setTopLeft(m_topLeft->coordinate());
}

void QDeclarativeGeoMapRectangleObject::topLeftLongitudeChanged(double /*longitude*/)
{
    setTopLeft(m_topLeft->coordinate());
}

void QDeclarativeGeoMapRectangleObject::topLeftAltitudeChanged(double /*altitude*/)
{
    setTopLeft(m_topLeft->coordinate());
}

/*!
    \qmlproperty Coordinate MapRectangle::bottomRight
    
    This property holds the coordinate corresponding to the bottom right
    corner of the rectangle.

    The default value is an invalid coordinate.
*/

void QDeclarativeGeoMapRectangleObject::setDeclarativeBottomRight(const QDeclarativeCoordinate *bottomRight)
{
    if (m_bottomRight->coordinate() == bottomRight->coordinate())
        return;

    m_bottomRight->setCoordinate(bottomRight->coordinate());
    setBottomRight(bottomRight->coordinate());

    emit declarativeBottomRightChanged(m_bottomRight);
}

QDeclarativeCoordinate* QDeclarativeGeoMapRectangleObject::declarativeBottomRight()
{
    return m_bottomRight;
}

void QDeclarativeGeoMapRectangleObject::bottomRightLatitudeChanged(double /*latitude*/)
{
    setBottomRight(m_bottomRight->coordinate());
}

void QDeclarativeGeoMapRectangleObject::bottomRightLongitudeChanged(double /*longitude*/)
{
    setBottomRight(m_bottomRight->coordinate());
}

void QDeclarativeGeoMapRectangleObject::bottomRightAltitudeChanged(double /*altitude*/)
{
    setBottomRight(m_bottomRight->coordinate());
}

/*!
    \qmlproperty color MapRectangle::color

    This property holds the color used to fill the circle.

    The default value corresponds to a transparent color.
*/

void QDeclarativeGeoMapRectangleObject::setColor(const QColor &color)
{
    if (m_color == color)
        return;

    m_color = color;
    QBrush m_brush(color);
    setBrush(m_brush);
    emit colorChanged(m_color);
}

QColor QDeclarativeGeoMapRectangleObject::color() const
{
    return m_color;
}

/*!
    \qmlproperty int MapRectangle::border.width
    \qmlproperty color MapRectangle::border.color

    These properties hold the width and color used to draw the border of the circle.

    The width is in pixels and is independent of the zoom level of the map.

    The default values correspond to a black border with a width of 1 pixel.

    For no line, use a width of 0 or a transparent color.
*/

QDeclarativeGeoMapObjectBorder* QDeclarativeGeoMapRectangleObject::border()
{
    return &m_border;
}

void QDeclarativeGeoMapRectangleObject::borderColorChanged(const QColor &color)
{
    QPen p = pen();
    p.setColor(color);
    setPen(p);
}

void QDeclarativeGeoMapRectangleObject::borderWidthChanged(int width)
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
    \qmlproperty int MapRectangle::zValue

    This property holds the z-value of the rectangle.

    Map objects are drawn in z-value order, and objects with the 
    same z-value will be drawn in insertion order.
*/

/*!
    \qmlproperty bool MapRectangle::visible

    This property holds a boolean corresponding to whether or not the 
    rectangle is visible.
*/

/*!
    \qmlproperty bool MapRectangle::selected

    This property holds a boolean corresponding to whether or not the 
    rectangle is selected.
*/

#include "moc_qdeclarativegeomaprectangleobject_p.cpp"

QTM_END_NAMESPACE

