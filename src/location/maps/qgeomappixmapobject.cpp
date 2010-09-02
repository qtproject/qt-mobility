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

#include "qgeomappixmapobject.h"
#include "qgeomappixmapobject_p.h"
#include "qgeoboundingbox.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapPixmapObject
    \brief The QGeoMapPixmapObject class is a QGeoMapObject used to draw
    a pixmap on a map.

    \inmodule QtLocation

    \ingroup maps-mapping-objects

    The pixmap object is rendered such that the upper left corner of
    QGeoMapPixmapObject::pixmap will be drawn QGeoMapPixmapObject::offset
    pixels away from the position of QGeoMapPixmapObject::coordinate on the
    map.
*/

/*!
    Constructs a new pixmap object with the parent \a parent.
*/
QGeoMapPixmapObject::QGeoMapPixmapObject(QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapPixmapObjectPrivate(this, parent)) {}

/*!
    Constructs a new pixmap object which will draw the pixmap \a pixmap at an
    offset of \a offset pixels from the coordinate \a coordinate, with the
    parent \a parent.
*/
QGeoMapPixmapObject::QGeoMapPixmapObject(const QGeoCoordinate &coordinate, const QPoint &offset, const QPixmap &pixmap, QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapPixmapObjectPrivate(this, parent))
{
    Q_D(QGeoMapPixmapObject);

    d->coordinate = coordinate;
    d->pixmap = pixmap;
    d->offset = offset;
}

/*!
    Destroys this pixmap object.
*/
QGeoMapPixmapObject::~QGeoMapPixmapObject()
{
}

/*!
    \property QGeoMapPixmapObject::coordinate
    \brief This property holds the coordinate that specifies where the pixmap
    will be drawn by this pixmap object.

    The default value of this property is an invalid coordinate. While the
    value of this property is invalid the pixmap object will not be displayed.
*/
QGeoCoordinate QGeoMapPixmapObject::coordinate() const
{
    Q_D(const QGeoMapPixmapObject);
    return d->coordinate;
}

void QGeoMapPixmapObject::setCoordinate(const QGeoCoordinate &coordinate)
{
    Q_D(QGeoMapPixmapObject);
    if (d->coordinate != coordinate) {
        d->coordinate = coordinate;
        objectUpdate();
        emit coordinateChanged(d->coordinate);
    }
}

/*!
    \property QGeoMapPixmapObject::pixmap
    \brief This property holds the pixmap that will be drawn by this pixmap
    object.

    The default value of this property is a null pixmap. While the value of
    this property is the null pixmap the pixmap object will not be displayed.

    The pixmap will be drawn such that the upper left corner of the pixmap
    will be drawn QGeoMapPixmapObject::offset pixels away from the position of
    QGeoMapPixmapObject::coordinate on the map.
*/
QPixmap QGeoMapPixmapObject::pixmap() const
{
    Q_D(const QGeoMapPixmapObject);
    return d->pixmap;
}

void QGeoMapPixmapObject::setPixmap(const QPixmap &pixmap)
{
    Q_D(QGeoMapPixmapObject);

    if (d->pixmap.isNull() && pixmap.isNull())
        return;

    if ((d->pixmap.isNull() && !pixmap.isNull())
            || (!d->pixmap.isNull() && pixmap.isNull())
            || (d->pixmap.toImage() != pixmap.toImage())) {
        d->pixmap = pixmap;
        objectUpdate();
        emit pixmapChanged(d->pixmap);
    }
}

/*!
    \property QGeoMapPixmapObject::offset
    \brief This property holds the offset in pixels at which the pixmap will be
    drawn by this pixmap object.

    The default value of this property is QPoint(0, 0). If this value is not
    changed the upper left coordinate of the pixmap will be drawn at the
    coordinate specified by QGeoMapPixmapObject::coordinate.

    The offset is in pixels and is independent of the zoom level of the map.
    The offset property is provided so that pixmaps such as arrows can be drawn
    with the point of the arrow placed exactly on the associated coordinate.
*/
QPoint QGeoMapPixmapObject::offset() const
{
    Q_D(const QGeoMapPixmapObject);
    return d->offset;
}

void QGeoMapPixmapObject::setOffset(const QPoint &offset)
{
    Q_D(QGeoMapPixmapObject);
    if (d->offset != offset) {
        d->offset = offset;
        objectUpdate();
        emit offsetChanged(d->offset);
    }
}



/*******************************************************************************
*******************************************************************************/

QGeoMapPixmapObjectPrivate::QGeoMapPixmapObjectPrivate(QGeoMapObject *impl, QGeoMapObject *parent)
        : QGeoMapObjectPrivate(impl, parent, QGeoMapObject::PixmapType) {}

QGeoMapPixmapObjectPrivate::~QGeoMapPixmapObjectPrivate() {}

#include "moc_qgeomappixmapobject.cpp"

QTM_END_NAMESPACE

