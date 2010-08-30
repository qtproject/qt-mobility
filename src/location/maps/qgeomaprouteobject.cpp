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

#include "qgeomaprouteobject.h"
#include "qgeomaprouteobject_p.h"

#define DEFAULT_ROUTE_DETAIL_LEVEL 20

QTM_BEGIN_NAMESPACE

QGeoMapRouteObject::QGeoMapRouteObject(const QGeoRoute &route, QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapRouteObjectPrivate(this, parent))
{
    Q_D(QGeoMapRouteObject);
    d->route = route;
}

QGeoMapRouteObject::~QGeoMapRouteObject()
{
}

QGeoRoute QGeoMapRouteObject::route() const
{
    Q_D(const QGeoMapRouteObject);
    return d->route;
}

void QGeoMapRouteObject::setRoute(const QGeoRoute &route)
{
    Q_D(QGeoMapRouteObject);
    //if (d->route != route) {
    d->route = route;
    objectUpdate();
    emit routeChanged(d->route);
    //}
}

QPen QGeoMapRouteObject::pen() const
{
    Q_D(const QGeoMapRouteObject);
    return d->pen;
}

void QGeoMapRouteObject::setPen(const QPen &pen)
{
    Q_D(QGeoMapRouteObject);
    if (d->pen != pen) {
        d->pen = pen;
        objectUpdate();
        emit penChanged(d->pen);
    }
}

quint32 QGeoMapRouteObject::detailLevel() const
{
    Q_D(const QGeoMapRouteObject);
    return d->detailLevel;
}

void QGeoMapRouteObject::setDetailLevel(quint32 detailLevel)
{
    Q_D(QGeoMapRouteObject);
    if (d->detailLevel != detailLevel) {
        d->detailLevel = detailLevel;
        objectUpdate();
        emit detailLevelChanged(d->detailLevel);
    }
}

/*******************************************************************************
*******************************************************************************/

QGeoMapRouteObjectPrivate::QGeoMapRouteObjectPrivate(QGeoMapObject *impl, QGeoMapObject *parent)
        : QGeoMapObjectPrivate(impl, parent, QGeoMapObject::RouteType)
{
    detailLevel = DEFAULT_ROUTE_DETAIL_LEVEL;
}

QGeoMapRouteObjectPrivate::~QGeoMapRouteObjectPrivate() {}

#include "moc_qgeomaprouteobject.cpp"

QTM_END_NAMESPACE

