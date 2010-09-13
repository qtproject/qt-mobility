
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

#include "qdeclarativegraphicsgeomap_p.h"
#include <QDebug>

QTM_BEGIN_NAMESPACE


QDeclarativeGraphicsGeoMap::QDeclarativeGraphicsGeoMap(QGraphicsItem *parent)
        : QGraphicsGeoMap(parent)
{
    connect(this,
            SIGNAL(centerChanged(QGeoCoordinate)),
            this,
            SLOT(memberCenterChanged(QGeoCoordinate)));
    m_center = new QDeclarativeCoordinate(this);
}

QDeclarativeGraphicsGeoMap::~QDeclarativeGraphicsGeoMap()
{
}

void QDeclarativeGraphicsGeoMap::setDeclarativeCenter(const QDeclarativeCoordinate *center)
{
    m_center->setCoordinate(center->coordinate());
    setCenter(center->coordinate());
}

QDeclarativeCoordinate* QDeclarativeGraphicsGeoMap::declarativeCenter() const
{
    m_center->setCoordinate(center());
    return m_center;
}

QDeclarativeListProperty<QGeoMapObject> QDeclarativeGraphicsGeoMap::objects()
{
    return QDeclarativeListProperty<QGeoMapObject>(this,
            0,
            object_append,
            object_count,
            object_at,
            object_clear);
}

void QDeclarativeGraphicsGeoMap::object_append(QDeclarativeListProperty<QGeoMapObject> *prop, QGeoMapObject *mapObject)
{
    static_cast<QDeclarativeGraphicsGeoMap*>(prop->object)->addMapObject(mapObject);
}

int QDeclarativeGraphicsGeoMap::object_count(QDeclarativeListProperty<QGeoMapObject> *prop)
{
    return static_cast<QDeclarativeGraphicsGeoMap*>(prop->object)->mapObjects().size();
}

QGeoMapObject *QDeclarativeGraphicsGeoMap::object_at(QDeclarativeListProperty<QGeoMapObject> *prop, int index)
{
    return static_cast<QDeclarativeGraphicsGeoMap*>(prop->object)->mapObjects().at(index);
}

void QDeclarativeGraphicsGeoMap::object_clear(QDeclarativeListProperty<QGeoMapObject> *prop)
{
    static_cast<QDeclarativeGraphicsGeoMap*>(prop->object)->clearMapObjects();
}

QPointF QDeclarativeGraphicsGeoMap::toScreenPosition(const QDeclarativeCoordinate *coordinate) const
{
    return coordinateToScreenPosition(coordinate->coordinate());
}

QObject* QDeclarativeGraphicsGeoMap::toCoordinate(QPointF screenPosition) const
{
    return new QDeclarativeCoordinate(screenPositionToCoordinate(screenPosition),
                                      const_cast<QDeclarativeGraphicsGeoMap *>(this));
}

void QDeclarativeGraphicsGeoMap::memberCenterChanged(const QGeoCoordinate &coordinate)
{
    emit declarativeCenterChanged(new QDeclarativeCoordinate(coordinate, this));
}

#include "moc_qdeclarativegraphicsgeomap_p.cpp"

QTM_END_NAMESPACE

