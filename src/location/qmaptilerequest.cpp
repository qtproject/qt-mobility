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

#include "qmaptilerequest.h"
#include "qmaptilerequest_p.h"
#include "qmaptilereply.h"

QTM_BEGIN_NAMESPACE

QMapTileRequest::QMapTileRequest()
        : d_ptr(new QMapTileRequestPrivate())
{
}

QMapTileRequest::QMapTileRequest(const MapVersion& mapVersion,
                                 const MapScheme& mapScheme,
                                 const MapResolution& mapResolution,
                                 const MapFormat& mapFormat)
        : d_ptr(new QMapTileRequestPrivate(mapVersion,mapScheme,mapResolution,mapFormat))
{
}

QMapTileRequest::QMapTileRequest(const QMapTileRequest& request)
        : d_ptr(new QMapTileRequestPrivate(request.d_ptr))
{
}

QMapTileRequest::~QMapTileRequest()
{
    Q_D(QMapTileRequest);
    delete d;
}

QMapTileRequest& QMapTileRequest::operator =(const QMapTileRequest& request)
{
    Q_D(QMapTileRequest);
    d->cl = request.col();
    d->rw = request.row();
    d->ver = request.version();
    d->zoom = request.zoomLevel();
    d->schm = request.scheme();
    d->frmt = request.format();
    d->res = request.resolution();
    return *this;
}

void QMapTileRequest::setVersion(const MapVersion& version)
{
    Q_D(QMapTileRequest);
    d->ver = version;
}

MapScheme QMapTileRequest::scheme() const
{
    Q_D(const QMapTileRequest);
    return d->schm;
}

void QMapTileRequest::setScheme(const MapScheme& scheme)
{
    Q_D(QMapTileRequest);
    d->schm = scheme;
}

MapResolution QMapTileRequest::resolution() const
{
    Q_D(const QMapTileRequest);
    return d->res;
}

void QMapTileRequest::setResolution(const MapResolution& resolution)
{
    Q_D(QMapTileRequest);
    d->res = resolution;
}

MapFormat QMapTileRequest::format() const
{
    Q_D(const QMapTileRequest);
    return d->frmt;
}

void QMapTileRequest::setFormat(const MapFormat& format)
{
    Q_D(QMapTileRequest);
    d->frmt = format;
}

quint16 QMapTileRequest::zoomLevel() const
{
    Q_D(const QMapTileRequest);
    return d->zoom;
}

void QMapTileRequest::setZoomLevel(quint16 level)
{
    Q_D(QMapTileRequest);
    d->zoom = level;
}

quint32 QMapTileRequest::col() const 
{
    Q_D(const QMapTileRequest);
    return d->cl;
}

void QMapTileRequest::setCol(quint32 c)
{
    Q_D(QMapTileRequest);
    d->cl = c;
}

quint32 QMapTileRequest::row() const
{
    Q_D(const QMapTileRequest);
    return d->rw;
}

void QMapTileRequest::setRow(quint32 r)
{
    Q_D(QMapTileRequest);
    d->rw = r;
}

MapVersion QMapTileRequest::version() const
{
    Q_D(const QMapTileRequest);
    return d->ver;
}

QMapTileRequestPrivate::QMapTileRequestPrivate()
        : cl(0), rw(0), ver(), zoom(0), schm(), res(), frmt()
{
}

QMapTileRequestPrivate::QMapTileRequestPrivate(const MapVersion& mapVersion,
                                 const MapScheme& mapScheme,
                                 const MapResolution& mapResolution,
                                 const MapFormat& mapFormat)
        : cl(0), rw(0), ver(mapVersion), zoom(0), schm(mapScheme), res(mapResolution), frmt(mapFormat)
{
}

QMapTileRequestPrivate::QMapTileRequestPrivate(const QMapTileRequestPrivate* request)
        : cl(request->cl), rw(request->rw), ver(request->ver), zoom(request->zoom),
        schm(request->schm), res(request->res), frmt(request->frmt)
{
}

//---------------------------------------------------------------
QMapTileReply::QMapTileReply(const QMapTileRequest& request)
        : req(request)
{
}

QTM_END_NAMESPACE
