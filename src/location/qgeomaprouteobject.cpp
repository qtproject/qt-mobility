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

#include "qgeomaprouteobject.h"
#include "qgeomaprouteobject_p.h"

#define DEFAULT_ROUTE_DETAIL_LEVEL 20

QTM_BEGIN_NAMESPACE

QGeoMapRouteObject::QGeoMapRouteObject(const QGeoRoute &route, QGeoMapObject *parent)
    : QGeoMapObject(new QGeoMapRouteObjectPrivate(this, parent))
{
    Q_D(QGeoMapRouteObject);
    d->route = route;
    d->boundingBox = route.bounds();
}

QGeoMapRouteObject::~QGeoMapRouteObject()
{
}

QPen QGeoMapRouteObject::pen() const
{
    Q_D(const QGeoMapRouteObject);

    return d->pen;
}

void QGeoMapRouteObject::setPen(const QPen &aPen)
{
    Q_D(QGeoMapRouteObject);

    d->pen = aPen;
}

QGeoRoute QGeoMapRouteObject::route() const
{
    Q_D(const QGeoMapRouteObject);

    return d->route;
}

void QGeoMapRouteObject::setDetailLevel(quint32 pixels)
{
    Q_D(QGeoMapRouteObject);

    d->detailLevel = pixels;
}

quint32 QGeoMapRouteObject::detailLevel() const
{
    Q_D(const QGeoMapRouteObject);

    return d->detailLevel;
}

/*******************************************************************************
*******************************************************************************/

QGeoMapRouteObjectPrivate::QGeoMapRouteObjectPrivate(QGeoMapObject *impl, QGeoMapObject *parent)
    : QGeoMapObjectPrivate(impl, parent)
{
    type = QGeoMapObject::GeoRouteType;
    detailLevel = DEFAULT_ROUTE_DETAIL_LEVEL;
}

QGeoMapRouteObjectPrivate::QGeoMapRouteObjectPrivate(const QGeoMapRouteObjectPrivate &other)
    : QGeoMapObjectPrivate(other), pen(other.pen), route(other.route), detailLevel(other.detailLevel)
{}

QGeoMapRouteObjectPrivate::~QGeoMapRouteObjectPrivate() {}

QGeoMapRouteObjectPrivate& QGeoMapRouteObjectPrivate::operator= (const QGeoMapRouteObjectPrivate &other)
{
    route = other.route;
    pen = other.pen;
    detailLevel = other.detailLevel;
    QGeoMapObjectPrivate::operator=(other);

    return *this;
}

QTM_END_NAMESPACE

