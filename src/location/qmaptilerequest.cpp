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

QTM_BEGIN_NAMESPACE

/*!
    \class QMapTileRequeset
    \brief The QMapTileRequest class represents a request for a specific map tile.
    \ingroup location

    The maps that this class deals with are divided into 2^zoomLevel() columns
    and 2^zoomLevel() rows of tiles.
*/

/*!
    Constructor which sets some of the map data.
*/
QMapTileRequest::QMapTileRequest(const MapVersion& mapVersion,
                                 const MapScheme& mapScheme,
                                 const MapResolution& mapResolution,
                                 const MapFormat& mapFormat)
        : d_ptr(new QMapTileRequestPrivate())
{
    setVersion(mapVersion);
    setScheme(mapScheme);
    setResolution(mapResolution);
    setFormat(mapFormat);
}

/*!
    Default constructor
*/
QMapTileRequest::QMapTileRequest()
    : d_ptr(new QMapTileRequestPrivate())
{
}

QMapTileRequest::QMapTileRequest(const QMapTileRequest &mtr)
    : d_ptr(new QMapTileRequestPrivate(*(mtr.d_ptr)))
{
}

QMapTileRequest& QMapTileRequest::operator= (const QMapTileRequest &mtr)
{
    *d_ptr = *(mtr.d_ptr);
    return *this;
}

/*!
    Destructor
*/
QMapTileRequest::~QMapTileRequest()
{
    Q_D(QMapTileRequest);
    delete d;
}

/*!
    Returns the col index of the requested map tile.
*/
quint32 QMapTileRequest::col() const
{
    Q_D(const QMapTileRequest);
    return d->cl;
}

/*!
    Sets the col index of the requested map tile to \a c.

    Valid values are 0 ... 2^zoomLevel() - 1.
*/
void QMapTileRequest::setCol(quint32 c)
{
    Q_D(QMapTileRequest);
    d->cl = c;
}

/*!
    Returns the row index of the requested map tile.
*/
quint32 QMapTileRequest::row() const
{
    Q_D(const QMapTileRequest);
    return d->rw;
}
/*!
Sets the row index of the requested map tile to \a r.

Valid values are 0 ... 2^zoomLevel() - 1.
*/
void QMapTileRequest::setRow(quint32 r)
{
    Q_D(QMapTileRequest);
    d->rw = r;
}

/*!
    Returns the version of the requested map tile.
*/
MapVersion QMapTileRequest::version() const
{
    Q_D(const QMapTileRequest);
    return d->ver;
}

/*!
    Sets the version of the requested map tile to \a version.
*/
void QMapTileRequest::setVersion(const MapVersion& version)
{
    Q_D(QMapTileRequest);
    d->ver = version;
}

/*!
    Returns the resolution of the requested map tile.
*/
MapResolution QMapTileRequest::resolution() const
{
    Q_D(const QMapTileRequest);
    return d->res;
}

/*!
    Sets the resolution of the requested map tile to \a resolution.
*/
void QMapTileRequest::setResolution(const MapResolution& resolution)
{
    Q_D(QMapTileRequest);
    d->res = resolution;
}

/*!
    Returns the format of the requested map tile.
*/
MapFormat QMapTileRequest::format() const
{
    Q_D(const QMapTileRequest);
    return d->frmt;
}

/*!
    Sets the format of the requested map tile to \a format.
*/
void QMapTileRequest::setFormat(const MapFormat& format)
{
    Q_D(QMapTileRequest);
    d->frmt = format;
}

/*!
    Returns the scheme of the requested map tile.
*/
MapScheme QMapTileRequest::scheme() const
{
    Q_D(const QMapTileRequest);
    return d->schm;
}

/*!
    Sets the scheme of the requested map tile to \a scheme.
*/
void QMapTileRequest::setScheme(const MapScheme& scheme)
{
    Q_D(QMapTileRequest);
    d->schm = scheme;
}

/*!
    Returns the zoom level of the requested map tile.
*/
quint16 QMapTileRequest::zoomLevel() const
{
    Q_D(const QMapTileRequest);
    return d->zoom;
}

/*!
    Sets the zoom level of the requested map tile to \a level.
*/
void QMapTileRequest::setZoomLevel(quint16 level)
{
    Q_D(QMapTileRequest);
    d->zoom = level;
}

/*!
  Returns the request string for this request and the given \a host, \a token and \a referrer.
*/
QString QMapTileRequest::requestString(const QString &host, const QString &token, const QString &referrer) const
{
    Q_D(const QMapTileRequest);
    return d->requestString(host, token, referrer);
}

/******************************************************************************
  ****************************************************************************/

QMapTileRequestPrivate::QMapTileRequestPrivate()
    : cl(0), rw(0), zoom(0)
{}

QMapTileRequestPrivate::QMapTileRequestPrivate(const QMapTileRequestPrivate &mtrp)
    : cl(mtrp.cl),
    rw(mtrp.rw),
    ver(mtrp.ver),
    zoom(mtrp.zoom),
    schm(mtrp.schm),
    res(mtrp.res),
    frmt(mtrp.frmt)
{}

QMapTileRequestPrivate& QMapTileRequestPrivate::operator= (const QMapTileRequestPrivate &mtrp)
{
    cl = mtrp.cl;
    rw = mtrp.rw;
    ver = mtrp.ver;
    zoom = mtrp.zoom;
    schm = mtrp.schm;
    res = mtrp.res;
    frmt = mtrp.frmt;
    return *this;
}

QString QMapTileRequestPrivate::requestString(const QString &host, const QString &token, const QString &referrer) const
{
    QString request = "http://";
    request += host;
    request += "/maptiler/maptile/";
    request += ver.id;
    request += '/';
    request += schm.id;
    request += '/';
    request += QString::number(zoom);
    request += '/';
    request += QString::number(cl);
    request += '/';
    request += QString::number(rw);
    request += '/';
    request += res.id;
    request += '/';
    request += frmt.id;

    if (!token.isEmpty()) {
        request += "?token=";
        request += token;

        if (!referrer.isEmpty()) {
            request += "&referrer=";
            request += referrer;
        }
    } else if (!referrer.isEmpty()) {
        request += "?referrer=";
        request += referrer;
    }

    return request;
}

QTM_END_NAMESPACE
