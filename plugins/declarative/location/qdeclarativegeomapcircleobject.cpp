
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
    m_center = new QDeclarativeCoordinate(this);
}

QDeclarativeGeoMapCircleObject::~QDeclarativeGeoMapCircleObject()
{
}

void QDeclarativeGeoMapCircleObject::setDeclarativeCenter(const QDeclarativeCoordinate *center)
{
    if (m_center->coordinate() == center->coordinate())
        return;

    m_center->setCoordinate(center->coordinate());
    setCenter(center->coordinate());

    emit declarativeCenterChanged(m_center);
}

QDeclarativeCoordinate* QDeclarativeGeoMapCircleObject::declarativeCenter() const
{
    return m_center;
}

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

#include "moc_qdeclarativegeomapcircleobject_p.cpp"

QTM_END_NAMESPACE

