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

#include "qgeotiledmaprequest.h"
#include "qgeotiledmaprequest_p.h"

#include "qgeotiledmapviewport.h"

QTM_BEGIN_NAMESPACE

QGeoTiledMapRequest::QGeoTiledMapRequest()
        : d_ptr(new QGeoTiledMapRequestPrivate()) {}

/*!
*/
QGeoTiledMapRequest::QGeoTiledMapRequest(QGeoTiledMapViewport *viewport, int row, int column, const QRectF &tileRect)
        : d_ptr(new QGeoTiledMapRequestPrivate())
{
    d_ptr->viewport = viewport;
    d_ptr->row = row;
    d_ptr->column = column;
    d_ptr->tileRect = tileRect;

    d_ptr->zoomLevel = viewport->zoomLevel();
    d_ptr->mapType = viewport->mapType();
    //d_ptr->imageFormat = viewport->imageFormat();
}

/*!
*/
QGeoTiledMapRequest::QGeoTiledMapRequest(const QGeoTiledMapRequest &other)
        : d_ptr(other.d_ptr) {}

/*!
*/
QGeoTiledMapRequest::~QGeoTiledMapRequest() {}

/*!
*/
QGeoTiledMapRequest& QGeoTiledMapRequest::operator= (const QGeoTiledMapRequest & other)
{
    d_ptr = other.d_ptr;

    return *this;
}

/*!
*/
QGeoTiledMapViewport* QGeoTiledMapRequest::viewport() const
{
    return d_ptr->viewport;
}

/*!
*/
QGeoMapWidget::MapType QGeoTiledMapRequest::mapType() const
{
    return d_ptr->mapType;
}

/*!
*/
int QGeoTiledMapRequest::zoomLevel() const
{
    return d_ptr->zoomLevel;
}

/*!
*/
int QGeoTiledMapRequest::row() const
{
    return d_ptr->row;
}

/*!
*/
int QGeoTiledMapRequest::column() const
{
    return d_ptr->column;
}

/*!
*/
QRectF QGeoTiledMapRequest::tileRect() const
{
    return d_ptr->tileRect;
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapRequestPrivate::QGeoTiledMapRequestPrivate()
        : QSharedData(),
        viewport(0) {}

QGeoTiledMapRequestPrivate::QGeoTiledMapRequestPrivate(const QGeoTiledMapRequestPrivate &other)
        : QSharedData(other),
        viewport(other.viewport),
        mapType(other.mapType),
        zoomLevel(other.zoomLevel),
        row(other.row),
        column(other.column),
        tileRect(other.tileRect) {}

QGeoTiledMapRequestPrivate::~QGeoTiledMapRequestPrivate() {}

QGeoTiledMapRequestPrivate& QGeoTiledMapRequestPrivate::operator= (const QGeoTiledMapRequestPrivate & other)
{
    viewport = other.viewport;
    mapType = other.mapType;
    zoomLevel = other.zoomLevel;
    row = other.row;
    column = other.column;
    tileRect = other.tileRect;

    return *this;
}

QTM_END_NAMESPACE

