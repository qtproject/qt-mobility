
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

#include "qdeclarativegeomaptextobject_p.h"

#include <QPen>
#include <QBrush>

QTM_BEGIN_NAMESPACE

QDeclarativeGeoMapTextObject::QDeclarativeGeoMapTextObject()
{
    QPen p = pen();
    p.setStyle(Qt::NoPen);
    setPen(p);

    m_coordinate = new QDeclarativeCoordinate(this);

    connect(m_coordinate,
            SIGNAL(latitudeChanged(double)),
            this,
            SLOT(coordinateLatitudeChanged(double)));

    connect(m_coordinate,
            SIGNAL(longitudeChanged(double)),
            this,
            SLOT(coordinateLongitudeChanged(double)));

    connect(m_coordinate,
            SIGNAL(altitudeChanged(double)),
            this,
            SLOT(coordinateAltitudeChanged(double)));

    m_hAlignment = QDeclarativeGeoMapTextObject::AlignHCenter;
    m_vAlignment = QDeclarativeGeoMapTextObject::AlignVCenter;
}

QDeclarativeGeoMapTextObject::~QDeclarativeGeoMapTextObject()
{
}

void QDeclarativeGeoMapTextObject::setDeclarativeCoordinate(const QDeclarativeCoordinate *coordinate)
{
    if (m_coordinate->coordinate() == coordinate->coordinate())
        return;

    m_coordinate->setCoordinate(coordinate->coordinate());
    setCoordinate(coordinate->coordinate());

    emit declarativeCoordinateChanged(m_coordinate);
}

QDeclarativeCoordinate* QDeclarativeGeoMapTextObject::declarativeCoordinate()
{
    return m_coordinate;
}

void QDeclarativeGeoMapTextObject::coordinateLatitudeChanged(double /*latitude*/)
{
    setCoordinate(m_coordinate->coordinate());
}

void QDeclarativeGeoMapTextObject::coordinateLongitudeChanged(double /*longitude*/)
{
    setCoordinate(m_coordinate->coordinate());
}

void QDeclarativeGeoMapTextObject::coordinateAltitudeChanged(double /*altitude*/)
{
    setCoordinate(m_coordinate->coordinate());
}

void QDeclarativeGeoMapTextObject::setColor(const QColor &color)
{
    if (m_color == color)
        return;

    m_color = color;
    QBrush m_brush(color);
    setBrush(m_brush);
    emit colorChanged(m_color);
}

QColor QDeclarativeGeoMapTextObject::color() const
{
    return m_color;
}

QDeclarativeGeoMapTextObject::HorizontalAlignment QDeclarativeGeoMapTextObject::horizontalAlignment() const
{
    return m_hAlignment;
}

void QDeclarativeGeoMapTextObject::setHorizontalAlignment(QDeclarativeGeoMapTextObject::HorizontalAlignment alignment)
{
    if (m_hAlignment == alignment)
        return;


    m_hAlignment = alignment;

    setAlignment(Qt::Alignment(m_hAlignment | m_vAlignment));

    emit horizontalAlignmentChanged(m_hAlignment);
}

QDeclarativeGeoMapTextObject::VerticalAlignment QDeclarativeGeoMapTextObject::verticalAlignment() const
{
    return m_vAlignment;
}

void QDeclarativeGeoMapTextObject::setVerticalAlignment(QDeclarativeGeoMapTextObject::VerticalAlignment alignment)
{
    if (m_vAlignment == alignment)
        return;

    m_vAlignment = alignment;

    setAlignment(Qt::Alignment(m_hAlignment | m_vAlignment));

    emit verticalAlignmentChanged(m_vAlignment);
}

#include "moc_qdeclarativegeomaptextobject_p.cpp"

QTM_END_NAMESPACE

