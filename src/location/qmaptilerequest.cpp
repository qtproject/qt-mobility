/*
* ==============================================================================
*  Name        : QMapTile.cpp
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#include "qmaptilerequest.h"
#include "qmaptilereply.h"

namespace QLocation
{

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

}
