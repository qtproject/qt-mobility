
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

QDeclarativeGeoMapCircleObject::QDeclarativeGeoMapCircleObject()
{
    connect(this,
            SIGNAL(centerChanged(QGeoCoordinate)),
            this,
            SLOT(memberCenterChanged(QGeoCoordinate)));
    connect(this,
            SIGNAL(brushChanged(QBrush)),
            this,
            SLOT(memberBrushChanged(QBrush)));
    m_center = new QDeclarativeCoordinate(this);
}

QDeclarativeGeoMapCircleObject::~QDeclarativeGeoMapCircleObject()
{
}

void QDeclarativeGeoMapCircleObject::setDeclarativeCenter(const QDeclarativeCoordinate *center)
{
    m_center->setCoordinate(center->coordinate());
    setCenter(center->coordinate());
}

QDeclarativeCoordinate* QDeclarativeGeoMapCircleObject::declarativeCenter() const
{
    m_center->setCoordinate(center());
    return m_center;
}

void QDeclarativeGeoMapCircleObject::memberCenterChanged(const QGeoCoordinate &coordinate)
{
    if (coordinate == m_center->coordinate())
        return;

    emit declarativeCenterChanged(new QDeclarativeCoordinate(coordinate, this));
}

void QDeclarativeGeoMapCircleObject::setColor(const QColor &color)
{
    m_color = color;
    QBrush m_brush(color);
    setBrush(m_brush);
}

QColor QDeclarativeGeoMapCircleObject::color() const
{
    return brush().color();
}

void QDeclarativeGeoMapCircleObject::memberBrushChanged(const QBrush &brush)
{
    if (m_color == brush.color())
        return;

    emit colorChanged(brush.color());
}

#include "moc_qdeclarativegeomapcircleobject_p.cpp"

QTM_END_NAMESPACE

