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

#include "qgeomaprectangleobject.h"
#include "qgeomaprectangleobject_p.h"
#include "qgeoboundingbox.h"

QTM_BEGIN_NAMESPACE

QGeoMapRectangleObject::QGeoMapRectangleObject(const QGeoBoundingBox &boundingBox, QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapRectangleObjectPrivate(this, parent))
{
    d_ptr->bounds = boundingBox;
}

QGeoMapRectangleObject::QGeoMapRectangleObject(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight, QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapRectangleObjectPrivate(this, parent))
{
    d_ptr->bounds = QGeoBoundingBox(topLeft, bottomRight);
}

QGeoMapRectangleObject::~QGeoMapRectangleObject()
{
}

QGeoBoundingBox QGeoMapRectangleObject::bounds() const
{
    Q_D(const QGeoMapRectangleObject);
    return d->bounds;
}

void QGeoMapRectangleObject::setBounds(const QGeoBoundingBox &bounds)
{
    Q_D(QGeoMapRectangleObject);
    d->bounds = bounds;
}

QPen QGeoMapRectangleObject::pen() const
{
    Q_D(const QGeoMapRectangleObject);
    return d->pen;
}

void QGeoMapRectangleObject::setPen(const QPen &pen)
{
    Q_D(QGeoMapRectangleObject);
    d->pen = pen;
}

QBrush QGeoMapRectangleObject::brush() const
{
    Q_D(const QGeoMapRectangleObject);
    return d->brush;
}

void QGeoMapRectangleObject::setBrush(const QBrush &brush)
{
    Q_D(QGeoMapRectangleObject);
    d->brush = brush;
}

/*******************************************************************************
*******************************************************************************/

QGeoMapRectangleObjectPrivate::QGeoMapRectangleObjectPrivate(QGeoMapObject *impl, QGeoMapObject *parent)
        : QGeoMapObjectPrivate(impl, parent, QGeoMapObject::RectangleType) {}

QGeoMapRectangleObjectPrivate::~QGeoMapRectangleObjectPrivate() {}

QTM_END_NAMESPACE

