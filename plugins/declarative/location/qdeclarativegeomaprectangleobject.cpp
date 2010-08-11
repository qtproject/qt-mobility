


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
    connect(this,
            SIGNAL(topLeftChanged(QGeoCoordinate)),
            this,
            SLOT(memberTopLeftChanged(QGeoCoordinate)));
    connect(this,
            SIGNAL(bottomRightChanged(QGeoCoordinate)),
            this,
            SLOT(memberBottomRightChanged(QGeoCoordinate)));
    connect(this,
            SIGNAL(brushChanged(QBrush)),
            this,
            SLOT(memberBrushChanged(QBrush)));
    m_topLeft = new QDeclarativeCoordinate(this);
    m_bottomRight = new QDeclarativeCoordinate(this);
}

QDeclarativeGeoMapRectangleObject::~QDeclarativeGeoMapRectangleObject()
{
}

void QDeclarativeGeoMapRectangleObject::setDeclarativeTopLeft(const QDeclarativeCoordinate *topLeft)
{
    m_topLeft->setCoordinate(topLeft->coordinate());
    setTopLeft(topLeft->coordinate());
}

QDeclarativeCoordinate* QDeclarativeGeoMapRectangleObject::declarativeTopLeft() const
{
    m_topLeft->setCoordinate(topLeft());
    return m_topLeft;
}

void QDeclarativeGeoMapRectangleObject::memberTopLeftChanged(const QGeoCoordinate &coordinate)
{
    if (coordinate == m_topLeft->coordinate())
        return;

    emit declarativeTopLeftChanged(new QDeclarativeCoordinate(coordinate, this));
}

void QDeclarativeGeoMapRectangleObject::setDeclarativeBottomRight(const QDeclarativeCoordinate *bottomRight)
{
    m_bottomRight->setCoordinate(bottomRight->coordinate());
    setBottomRight(bottomRight->coordinate());
}

QDeclarativeCoordinate* QDeclarativeGeoMapRectangleObject::declarativeBottomRight() const
{
    m_bottomRight->setCoordinate(bottomRight());
    return m_bottomRight;
}

void QDeclarativeGeoMapRectangleObject::memberBottomRightChanged(const QGeoCoordinate &coordinate)
{
    if (coordinate == m_bottomRight->coordinate())
        return;

    emit declarativeBottomRightChanged(new QDeclarativeCoordinate(coordinate, this));
}

void QDeclarativeGeoMapRectangleObject::setColor(const QColor &color)
{
    m_color = color;
    QBrush m_brush(color);
    setBrush(m_brush);
}

QColor QDeclarativeGeoMapRectangleObject::color() const
{
    return brush().color();
}

void QDeclarativeGeoMapRectangleObject::memberBrushChanged(const QBrush &brush)
{
    if (m_color == brush.color())
        return;

    emit colorChanged(brush.color());
}

#include "moc_qdeclarativegeomaprectangleobject_p.cpp"

QTM_END_NAMESPACE

