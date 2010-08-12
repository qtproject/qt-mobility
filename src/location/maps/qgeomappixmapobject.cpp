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

QGeoMapPixmapObject::QGeoMapPixmapObject(QGeoMapObject *parent)
    : QGeoMapObject(new QGeoMapPixmapObjectPrivate(this, parent)) {}

QGeoMapPixmapObject::QGeoMapPixmapObject(const QGeoCoordinate &coordinate, const QPoint &offset, const QPixmap &icon, QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapPixmapObjectPrivate(this, parent))
{
    Q_D(QGeoMapPixmapObject);

    d->coordinate = coordinate;
    d->icon = icon;
    d->offset = offset;
}

QGeoMapPixmapObject::~QGeoMapPixmapObject()
{
}

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

QPixmap QGeoMapPixmapObject::icon() const
{
    Q_D(const QGeoMapPixmapObject);
    return d->icon;
}

void QGeoMapPixmapObject::setIcon(const QPixmap &icon)
{
    Q_D(QGeoMapPixmapObject);

    if (d->icon.isNull() && icon.isNull())
        return;

    if ((d->icon.isNull() && !icon.isNull())
            || (!d->icon.isNull() && icon.isNull())
            || (d->icon.toImage() != icon.toImage())) {
        d->icon = icon;
        objectUpdate();
        emit iconChanged(d->icon);
    }
}

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

