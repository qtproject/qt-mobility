


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

#include "qdeclarativegeomaprectangleobject_p.h"

#include <QColor>
#include <QBrush>

QTM_BEGIN_NAMESPACE

QDeclarativeGeoMapRectangleObject::QDeclarativeGeoMapRectangleObject()
{
    m_topLeft = new QDeclarativeCoordinate(this);
    m_bottomRight = new QDeclarativeCoordinate(this);

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

void QDeclarativeGeoMapRectangleObject::setDeclarativeTopLeft(const QDeclarativeCoordinate *topLeft)
{
    if (m_topLeft->coordinate() == topLeft->coordinate())
        return;

    m_topLeft->setCoordinate(topLeft->coordinate());
    setTopLeft(topLeft->coordinate());

    emit declarativeTopLeftChanged(m_topLeft);
}

QDeclarativeCoordinate* QDeclarativeGeoMapRectangleObject::declarativeTopLeft() const
{
    return m_topLeft;
}

void QDeclarativeGeoMapRectangleObject::setDeclarativeBottomRight(const QDeclarativeCoordinate *bottomRight)
{
    if (m_bottomRight->coordinate() == bottomRight->coordinate())
        return;

    m_bottomRight->setCoordinate(bottomRight->coordinate());
    setBottomRight(bottomRight->coordinate());

    emit declarativeBottomRightChanged(m_bottomRight);
}

QDeclarativeCoordinate* QDeclarativeGeoMapRectangleObject::declarativeBottomRight() const
{
    return m_bottomRight;
}

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
    setPen(p);
}

#include "moc_qdeclarativegeomaprectangleobject_p.cpp"

QTM_END_NAMESPACE

