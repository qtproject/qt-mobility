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

#include "qmappixmap.h"
#include "qmappixmap_p.h"

QTM_BEGIN_NAMESPACE

QMapPixmapPrivate::QMapPixmapPrivate()
    :pic(QPixmap()), geoTopLeft(QGeoCoordinateMaps()),
     mapTopLeft(QPointF())
{
}

/*!
    \class QMapPixmap
    \brief The QMapPixmap class represents a pixmap on a map.
    \ingroup location
*/

/*!
    Constructs the \a pixmap in the \a mapView.  It's top left corner is position according to \a topLeft
    corner.  The pixmap is displayed in the layer specified by \a layerIndex.
*/
QMapPixmap::QMapPixmap(const QMapView* mapView, const QGeoCoordinateMaps& topLeft, const QPixmap& pixmap, quint16 layerIndex)
    :QMapObject(*new QMapPixmapPrivate, mapView, QMapObject::PixmapObject, layerIndex)
{
    Q_D(QMapPixmap);
    d->geoTopLeft = topLeft;
    d->pic = pixmap;
}

QMapPixmap::QMapPixmap(QMapPixmapPrivate &dd, const QMapView* mapView, const QGeoCoordinateMaps& topLeft, const QPixmap& pixmap, quint16 layerIndex)
    :QMapObject(dd, mapView, QMapObject::PixmapObject, layerIndex)
{
    Q_D(QMapPixmap);
    d->geoTopLeft = topLeft;
    d->pic = pixmap;
}

/*!
    Returns the pixmap.
*/
QPixmap QMapPixmap::pixmap() const {
    Q_D(const QMapPixmap);
    return d->pic;
}

/*!
    Returns the top left corner (as a geo coordinate) of this pixmap.
*/
QGeoCoordinateMaps QMapPixmap::topLeft() const {
    Q_D(const QMapPixmap);
    return d->geoTopLeft;
}

QTM_END_NAMESPACE
