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
#include "qmaptilereply.h"

QTM_BEGIN_NAMESPACE

QMapTileRequest::QMapTileRequest(const MapVersion& mapVersion,
                                 const MapScheme& mapScheme,
                                 const MapResolution& mapResolution,
                                 const MapFormat& mapFormat)
        : cl(0), rw(0), ver(mapVersion), zoom(0), schm(mapScheme), res(mapResolution), frmt(mapFormat)
{
}

QMapTileRequest::QMapTileRequest(const QMapTileRequest& request)
        : cl(request.cl), rw(request.rw), ver(request.ver), zoom(request.zoom),
        schm(request.schm), res(request.res), frmt(request.frmt)
{
}

QMapTileRequest& QMapTileRequest::operator =(const QMapTileRequest& request)
{
    cl = request.cl;
    rw = request.rw;
    ver = request.ver;
    zoom = request.zoom;
    schm = request.schm;
    frmt = request.frmt;
    res = request.res;
    return *this;
}

void QMapTileRequest::setVersion(const MapVersion& version)
{
    ver = version;
}

void QMapTileRequest::setScheme(const MapScheme& scheme)
{
    schm = scheme;
}

void QMapTileRequest::setResolution(const MapResolution& resolution)
{
    res = resolution;
}

void QMapTileRequest::setFormat(const MapFormat& format)
{
    frmt = format;
}

quint16 QMapTileRequest::zoomLevel() const
{
    return zoom;
}

void QMapTileRequest::setZoomLevel(quint16 level)
{
    zoom = level;
}

//---------------------------------------------------------------
QMapTileReply::QMapTileReply(const QMapTileRequest& request)
        : req(request)
{
}

QTM_END_NAMESPACE
