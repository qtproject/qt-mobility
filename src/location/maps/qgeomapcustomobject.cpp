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

#include "qgeomapcustomobject.h"
#include "qgeomapcustomobject_p.h"
#include "qgeoboundingbox.h"

#include <QGraphicsItem>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapCustomObject
    \brief The QGeoMapCustomObject class is a QGeoMapObject used to draw
    a pixmap on a map.

    \inmodule QtLocation

    \ingroup maps-mapping-objects

    The pixmap object is rendered such that the upper left corner of
    QGeoMapCustomObject::pixmap will be drawn QGeoMapCustomObject::offset
    pixels away from the position of QGeoMapCustomObject::coordinate on the
    map.
*/

/*!
    Constructs a new pixmap object.
*/
QGeoMapCustomObject::QGeoMapCustomObject()
    : d_ptr(new QGeoMapCustomObjectPrivate())
{
}

/*!
    Constructs a new pixmap object which will draw the pixmap \a pixmap at an
    offset of \a offset pixels from the coordinate \a coordinate.
*/
QGeoMapCustomObject::QGeoMapCustomObject(const QGeoCoordinate &coordinate, const QPoint &offset)
    : d_ptr(new QGeoMapCustomObjectPrivate())
{
    setOrigin(coordinate);
    d_ptr->offset = offset;
}

/*!
    Destroys this pixmap object.
*/
QGeoMapCustomObject::~QGeoMapCustomObject()
{
    delete d_ptr;
}

/*!
    \reimp
*/
QGeoMapObject::Type QGeoMapCustomObject::type() const
{
    return QGeoMapObject::CustomType;
}

void QGeoMapCustomObject::update()
{
    emit triggerUpdate();
}

QGraphicsItem* QGeoMapCustomObject::graphicsItem() const
{
    return d_ptr->graphicsItem;
}

void QGeoMapCustomObject::setGraphicsItem(QGraphicsItem *graphicsItem)
{
    if (d_ptr->graphicsItem == graphicsItem)
        return;

    d_ptr->graphicsItem = graphicsItem;

    emit graphicsItemChanged(graphicsItem);
}

/*!
    \property QGeoMapCustomObject::offset
    \brief This property holds the offset in pixels at which the pixmap will be
    drawn by this pixmap object.

    The default value of this property is QPoint(0, 0). If this value is not
    changed the upper left coordinate of the pixmap will be drawn at the
    coordinate specified by QGeoMapCustomObject::coordinate.

    The offset is in pixels and is independent of the zoom level of the map.
    The offset property is provided so that pixmaps such as arrows can be drawn
    with the point of the arrow placed exactly on the associated coordinate.
*/
QPoint QGeoMapCustomObject::offset() const
{
    return d_ptr->offset;
//    QPointF pt = d_ptr->offset;
//
//    QPoint rounded;
//    rounded.setX(int(pt.x() > 0 ? pt.x() + 0.5 : pt.x() - 0.5));
//    rounded.setY(int(pt.y() > 0 ? pt.y() + 0.5 : pt.y() - 0.5));
//
//    return rounded;
}

void QGeoMapCustomObject::setOffset(const QPoint &offset)
{
    if (d_ptr->offset != offset) {
        d_ptr->offset = offset;
        emit offsetChanged(offset);
    }
}

/*!
\fn void QGeoMapCustomObject::offsetChanged(const QPoint &offset)
    
    This signal is emitted when the on-screen offset from the coordinate 
    at which this pixmap object should be drawn has changed.

    The new value will be \a offset.
*/

/*******************************************************************************
*******************************************************************************/

QGeoMapCustomObjectPrivate::QGeoMapCustomObjectPrivate()
    : graphicsItem(0) {}

QGeoMapCustomObjectPrivate::~QGeoMapCustomObjectPrivate()
{
}

#include "moc_qgeomapcustomobject.cpp"

QTM_END_NAMESPACE

