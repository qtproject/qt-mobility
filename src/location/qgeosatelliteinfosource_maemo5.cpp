/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgeosatelliteinfosource_maemo5_p.h"
#include "liblocationwrapper.h"

QTM_BEGIN_NAMESPACE

#define MINIMUM_UPDATE_INTERVAL 1000

QGeoSatelliteInfoSourceMaemo::QGeoSatelliteInfoSourceMaemo(QObject *parent) 
    : QGeoSatelliteInfoSource(parent)
{
    client_id_ = -1;
    satelliteInited = false;
}

int QGeoSatelliteInfoSourceMaemo::init()
{
    if (satelliteInited)
        return 0;
    if (LiblocationWrapper::instance()->inited()) {
        QObject::connect(LiblocationWrapper::instance(), 
                         SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)),
                         this, 
                         SLOT(satellitesInView(const QList<QGeoSatelliteInfo> &)));
        QObject::connect(LiblocationWrapper::instance(), 
                         SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)),
                         this, 
                         SLOT(satellitesInUse(const QList<QGeoSatelliteInfo> &)));
        satelliteInited = true;
        return 0;
    }
    else {
        return -1;
    }
}

void QGeoSatelliteInfoSourceMaemo::setUpdateInterval(int msec)
{
    msec = (msec < MINIMUM_UPDATE_INTERVAL) ? MINIMUM_UPDATE_INTERVAL : msec;

    setUpdateInterval(LiblocationWrapper::instance()->setUpdateInterval(msec));
}

void QGeoSatelliteInfoSourceMaemo::startUpdates()
{
    if (satelliteInited)
        LiblocationWrapper::instance()->start();
}

void QGeoSatelliteInfoSourceMaemo::stopUpdates()
{
    if (satelliteInited)
        LiblocationWrapper::instance()->stop();
}

void QGeoSatelliteInfoSourceMaemo::requestUpdate(int timeout)
{
    if (satelliteInited)
        if (!LiblocationWrapper::instance()->requestUpdate(timeout))
            emit requestTimeout();
}

void QGeoSatelliteInfoSourceMaemo::satellitesInView(const QList<QGeoSatelliteInfo> &satellites)
{
    emit satellitesInViewUpdated(satellites);
}

void QGeoSatelliteInfoSourceMaemo::satellitesInUse(const QList<QGeoSatelliteInfo> &satellites)
{
    emit satellitesInUseUpdated(satellites);
}

#include "moc_qgeosatelliteinfosource_maemo5_p.cpp"
QTM_END_NAMESPACE

